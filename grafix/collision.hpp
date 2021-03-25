#include "polygon.hpp"
#include "vector2.hpp"
#include "utils.hpp"

#include <array>
#include <algorithm>

struct PartialCollisionData {
    double collisionDepth;
    Polygon* leftPoly;
    Polygon* rightPoly;
};

struct CollisionData {
    bool colliding;
    double collisionDepth;
    DVec2 collisionVector;
    DVec2 collisionPoint;
    Polygon* leftPoly;
    Polygon* rightPoly;
};

inline double getPolyCircleCollisionDepth(Polygon& poly, DVec2 cMid, double radius, DVec2 normal) {
    double minRightVal = Infinity;
    double maxLeftVal = -Infinity;

    // between goes from poly to circle
    DVec2 betweenVec = cMid - poly.mid;
    double between = betweenVec.dot(normal);
    if (between < 0) {
        // circle on the left
        maxLeftVal = cMid.dot(normal) + radius;
        minRightVal = poly.getMinVal(normal);
    }
    else {
        // poly on the left
        maxLeftVal = poly.getMaxVal(normal);
        minRightVal = cMid.dot(normal) - radius;
    }

    return maxLeftVal - minRightVal;
}

inline bool pointInPolygon(DVec2 &point, Polygon &polygon) {
	for (DVec2& normal: polygon.normals) {
		if (getPolyCircleCollisionDepth(polygon, point, 0, normal) < 0) {
			return false;
        }
    }
	return true;
}

inline bool compareAngles(const DVec2 & a, const DVec2 & b) {
    return a.getAngle() < b.getAngle();
}

DVec2 getPolygonMid(std::vector<DVec2> vertexPoints) {
	size_t n = vertexPoints.size();

    if (n == 1) {
		return vertexPoints[0];
    } else if (n == 2) {
        return (vertexPoints[0] + vertexPoints[1])/2;
    }

	DVec2 P0 = vertexPoints[0];
	double totalArea = 0;
	DVec2 midpoint(0, 0);
	for (size_t i = 1; i < n-1; i++) {
		DVec2 Pi = vertexPoints[i];
        DVec2 Pii = vertexPoints[i+1];

		double area = std::abs((Pi - P0).cross(Pii - P0));
		totalArea += area;
		midpoint += ((P0 + Pi + Pii)/3)*area;
    }

	return midpoint/totalArea;
}

inline DVec2 getCollisionPoint(Polygon* leftPoly, Polygon* rightPoly) {
	// get all collision polygon vertex points
	std::vector<DVec2> colypolyVertexPoints;

	// point in poly 
	for (DVec2& point: leftPoly->points) {
		if (pointInPolygon(point, *rightPoly)) {
			colypolyVertexPoints.push_back(point);
        }
    }

	for (DVec2& point: rightPoly->points) {
		if (pointInPolygon(point, *leftPoly)) {
			colypolyVertexPoints.push_back(point);
        }
    }

	// edges colliding
	for (DEdge& edge1: leftPoly->edges) {
		for (DEdge& edge2: rightPoly->edges) {
			DVec2 P1 = edge1.start;
			DVec2 r1 = edge1.end - edge1.start;
			DVec2 P2 = edge2.start;
			DVec2 r2 = edge2.end - edge2.start;

			double denominator = r1.cross(r2);
			if (denominator == 0) {
				continue;
            }

			double t1 = (r2.cross(P1) - r2.cross(P2))/denominator;
			double t2 = (r1.cross(P1) - r1.cross(P2))/denominator;

			if (t1 >= 0 && t2 >= 0 && t1 <= 1 && t2 <= 1) {
				colypolyVertexPoints.push_back(P1 + r1*t1);
            }
        }
    }

    if (colypolyVertexPoints.size() == 0) {
        // how does this happen?
        print("did something go wrong?");
        return {std::nan(""), std::nan("")};
    }

	// sort collision vertex points so the points are in a counter-clockwise ordering
	DVec2 pseudoMid(0, 0);
    for (size_t i = 0; i < colypolyVertexPoints.size(); i++) {
        pseudoMid += colypolyVertexPoints[i];
    }
    pseudoMid /= static_cast<double>(colypolyVertexPoints.size());
    for (size_t i = 0; i < colypolyVertexPoints.size(); i++) {
        colypolyVertexPoints[i] -= pseudoMid;
    }
	std::sort(colypolyVertexPoints.begin(), colypolyVertexPoints.end(), compareAngles);

	// the center of the collision polygon is the point where the collision occurred
	DVec2 collisionPoint = getPolygonMid(colypolyVertexPoints);
    collisionPoint += pseudoMid;

	return collisionPoint;
}

inline PartialCollisionData getCollisionDepth(Polygon& poly1, Polygon& poly2, DVec2 normal) {
    Polygon* leftPoly;
    Polygon* rightPoly;

    DVec2 betweenVec = poly2.mid - poly1.mid;
    double between = betweenVec.dot(normal);
    if (between < 0) {
        between *= -1;
        leftPoly = &poly2;
        rightPoly = &poly1;
    }
    else {
        leftPoly = &poly1;
        rightPoly = &poly2;
    }
    double maxLeftVal = leftPoly->getMaxVal(normal);
    double minRightVal = rightPoly->getMinVal(normal);

    double collisionDepth = maxLeftVal - minRightVal;

    return {collisionDepth, leftPoly, rightPoly};
}

inline CollisionData isColliding(Polygon& poly1, Polygon& poly2) {
    std::vector<DVec2> allNormals(poly1.normals);
    allNormals.insert(allNormals.end(), poly2.normals.begin(), poly2.normals.end());

    // get unique normals, normals that don't point in the same (or opposite) direction
    std::vector<DVec2> normals;
    for (DVec2& normal : allNormals) {
        bool duplicate = false;
        for (DVec2& uniqueNormal : normals) {
            if (std::abs(normal.cross(uniqueNormal)) < 1e-6) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            normals.push_back(normal);
        }
    }

    // collision vector points from rightPoly to leftPoly
    double minCollisionDepth = Infinity;
    DVec2 collisionVector;
    Polygon* finalLeftPoly;
    Polygon* finalRightPoly;
    for (DVec2& normal : normals) {
        PartialCollisionData data = getCollisionDepth(poly1, poly2, normal);
        double collisionDepth = data.collisionDepth;

        if (collisionDepth < 0) {
            return {false, -1, normal, {0, 0}, data.leftPoly, data.rightPoly};
        }
        else if (collisionDepth < minCollisionDepth) {
            minCollisionDepth = collisionDepth;
            collisionVector = normal;
            finalLeftPoly = data.leftPoly;
            finalRightPoly = data.rightPoly;
        }
    }
    DVec2 collisionPoint = getCollisionPoint(finalLeftPoly, finalRightPoly);
    if (std::isnan(collisionPoint.x)) {
        print("this worked");
        // i guess the polygons are not coliding after all
        return {false, -1, collisionVector, {0, 0}, finalLeftPoly, finalRightPoly};
    }

    return {true, minCollisionDepth, collisionVector, collisionPoint, finalLeftPoly, finalRightPoly};
}
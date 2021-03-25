#pragma once
#include "constants.hpp"
#include "vector2.hpp"
#include "hitbox.hpp"
#include "utils.hpp"

#include <vector>
#include <tuple>
#include <cmath>
#include <glad/glad.h>

struct DEdge {
    DVec2 start;
    DVec2 end;
};

class Polygon {
public:
    std::vector<DVec2> vertices;
    double density;
    GLcolor color;
    bool immovable;
    bool imrotatable;
    size_t degree;

    std::vector<DVec2> points;
    std::vector<DEdge> edges;
    std::vector<DVec2> normals;

    double area;
    double mass;
    double moofin;
    double radius;
    DVec2 mid;
    Hitbox hitbox;

    std::vector<GLfloat> vertexData;
    std::vector<GLuint> indexData;
    GLuint vbo;  // vertex buffer object

    DVec2 force = {0, 0};
    DVec2 acc = {0, 0};
    DVec2 vel = {0, 0};
    double tourqe = 0;
    double rotAcc = 0;
    double rotVel = 0;
    double rotation = 0;
    double cosθ = 1;
    double sinθ = 0;
                         
    Polygon(
            DVec2 pos, std::vector<DVec2> vertices,
            double density, GLcolor color={0,0,0,1},
            bool immovable=false, bool imrotatable=false
    ): vertices(vertices), density(density), color(color), immovable(immovable), imrotatable(imrotatable) {
        degree = vertices.size();
        points.resize(degree);
        edges.resize(degree);
        normals.resize(degree);

        setPoints(points, pos);
        setAreaAndMid(area, mid);
        mass = area * density;
        DVec2 dpos = mid - pos;
        if (dpos.getSquaredLength() > 1e-6) {
            print("grr");
            dpos.printSelf();
        }
        setEdges(edges);
        setNormals(normals);

        setMoofin(moofin);
        setHitbox(hitbox);
        setRadius(radius);

        vertexData.resize(degree*2);
        for (size_t i = 0; i < degree; i++) {
            vertexData[i*2] = static_cast<GLfloat>(vertices[i].x);
            vertexData[i*2+1] = static_cast<GLfloat>(vertices[i].y);
        }

        indexData.resize((degree-2)*3);
        for (size_t i = 0; i < degree-2; i++) {
            indexData[i*3] = static_cast<GLuint>(0);
            indexData[i*3+1] = static_cast<GLuint>(i+1);
            indexData[i*3+2] = static_cast<GLuint>(i+2);
        }

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size()*sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);
        glCheck();
    }

    void update(double dt) {
        move(dt);
        step(dt);
        setPoints(points);
        setEdges(edges);
        setNormals(normals);
        setHitbox(hitbox);
    }

    void draw() {
        // prepare to draw
        GLfloat fposx = static_cast<GLfloat>(mid.x);
        GLfloat fposy = static_cast<GLfloat>(mid.y);
        GLfloat fcosθ = static_cast<GLfloat>(cosθ);
        GLfloat fsinθ = static_cast<GLfloat>(sinθ);
        GLfloat transformationMatrix[9] = {
            fcosθ, -fsinθ, fposx,
            fsinθ,  fcosθ, fposy,
             0  ,   0  ,   1
        };
        glUniformMatrix3fv(1, 1, GL_TRUE, transformationMatrix);
        glCheck();

        glUniform4f(3, color.r, color.g, color.b, color.a);
        glCheck();

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
        glCheck();

        // draw
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexData.size()), GL_UNSIGNED_INT, indexData.data());
        glCheck();
    }

    virtual void move(double dt) {
    }

    void step(double dt) {
        if (!immovable) {
            acc = force / mass;
            vel += acc * dt;
            mid += vel * dt;
        }
        force.set(0, 0);

        if (!imrotatable) {
            rotAcc = tourqe / moofin;
            rotVel += rotAcc * dt;
            double newRotation = rotation + rotVel * dt;
            setRotation(newRotation);
        }
        tourqe = 0;
    }

    void setColor(GLcolor newColor) {
        color = newColor;
    }

    void setRotation(double newRotation) {
        rotation = newRotation;
        cosθ = std::cos(rotation);
        sinθ = std::sin(rotation);
    }
    
    void setPoints(std::vector<DVec2> &points, DVec2 &pos) {
        for (size_t i = 0; i < vertices.size(); i++) {
            points[i] = pos + vertices[i].getRotatedFast(cosθ, sinθ);
        }
    }

    void setPoints(std::vector<DVec2>& points) {
        for (size_t i = 0; i < vertices.size(); i++) {
            points[i] = mid + vertices[i].getRotatedFast(cosθ, sinθ);
        }
    }

    void setEdges(std::vector<DEdge> &edges) {
        for (size_t i = 0; i < points.size(); i++) {
            size_t ip = (i+1) % points.size();
            edges[i] = {points[i], points[ip]};
        }
    }

    void setNormals(std::vector<DVec2> &normals)  {
        for (size_t i = 0; i < edges.size(); i++) {
            DVec2 vec = (edges[i].end - edges[i].start).getNormalized();
            normals[i] = {vec.y, -vec.x};
        }
    }

    void setAreaAndMid(double &area, DVec2 &mid) {
        DVec2 P0 = points[0];

        double totalArea = 0;
        DVec2 midpoint(0, 0);
        for (size_t i = 1; i < points.size()-1; i++) {
            DVec2 Pi = points[i];
            DVec2 Pii = points[i+1];

            double subArea = std::abs((Pi - P0).cross(Pii - P0));
            totalArea += subArea;
            midpoint += ((P0 + Pi + Pii)/3)*subArea;
        }

        area = totalArea/2;
        mid = midpoint/totalArea;
    }

    void setMoofin(double &moofin) {
		double pseudoMoofin = 0;
		double totalArea = 0;
		for (size_t i = 0; i < points.size()-1; i++) {
			DVec2 vi = points[i] - mid;
            DVec2 vii = points[i+1] - mid;

			double area = std::abs(vii.cross(vi));
			double submoof = vi.getSquaredLength() + vi.dot(vii) + vii.getSquaredLength();

			totalArea += area;
			pseudoMoofin += area*submoof;
        }

		moofin = mass*(pseudoMoofin/totalArea)/6;
    }

    void setRadius(double& radius) {
        // not realy a radius, the average of the length of the vertices
        double sum = 0;
        for (size_t i = 0; i < degree; i++) {
            sum += vertices[i].getLength();
        }
        radius = sum/degree;
    }

    void setHitbox(Hitbox &hitbox) {
		double top = points[0].y;
		double bottom = points[0].y;
		double left = points[0].x;
		double right = points[0].x;
		for (size_t i = 1; i < points.size(); i++) {
			top = std::max(top, points[i].y);
			bottom = std::min(bottom, points[i].y);
			left = std::min(left, points[i].x);
			right = std::max(right, points[i].x);
        }
        hitbox.width = right-left;
        hitbox.height = top-bottom;
        hitbox.pos = {left, bottom};
    }

    double getMaxVal(DVec2 normal) {
        double maxVal = -Infinity;
        for (DVec2& point: points) {
            double val = point.dot(normal);
            maxVal = std::max(val, maxVal);
        }
        
        return maxVal;
    }

    double getMinVal(DVec2 normal) {
        double minVal = Infinity;
        for (DVec2& point : points) {
            double val = point.dot(normal);
            minVal = std::min(val, minVal);
        }
        return minVal;
    }

    double getLength(DVec2 normal) {
        return getMaxVal(normal) - getMinVal(normal);
    }
};

inline Polygon createRegularPolygon(DVec2 pos, int degree, float size, double density, GLcolor color, bool immovable=false, bool imrotatable=false) {
    std::vector<DVec2> vertices;
    vertices.resize(degree);
    double theta = 2*PI/degree;
    for (size_t i = 0; i < degree; i++) {
        double x = size*std::cos(i*theta);
        double y = size*std::sin(i*theta);
        vertices[i] = DVec2(x, y);
    }
    return Polygon(pos, vertices, density, color, immovable, imrotatable);
}

inline std::vector<DVec2> getSquareVertices(double width, double height) {
    double hw = width/2;
    double hh = height/2;
    DVec2 bl = DVec2(-hw, -hh);
    DVec2 br = DVec2(hw, -hh);
    DVec2 tr = DVec2(hw, hh);
    DVec2 tl = DVec2(-hw, hh);
    return {bl, br, tr, tl};
}

inline Polygon createRect(DVec2 pos, double width, double height, double density, GLcolor color, bool immovable=false, bool imrotatable=false) {
    return Polygon(pos, getSquareVertices(width, height), density, color, immovable, imrotatable);
}

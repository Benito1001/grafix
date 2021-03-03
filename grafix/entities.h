#pragma once
#include "polygon.h"
#include "player.h"
#include "utils.h"
#include "physics.h"
#include "vector2.h"

#include <vector>

void getBox(std::vector<Polygon>& entities, double contentScale) {
    double boxSize = 0.2;
    double boxLeft = -2 * contentScale;
    double boxRight = 2 * contentScale;
    double boxTop = contentScale;
    double boxBottom = -contentScale;
    entities.push_back(
        createRect({ 0, boxTop }, boxRight - boxLeft, boxSize, 1, { 0, 0, 0, 1 }, true, true)
    );
    entities.push_back(
        createRect({ 0, boxBottom }, boxRight - boxLeft, boxSize, 1, { 0, 0, 0, 1 }, true, true)
    );
    entities.push_back(
        createRect({ boxLeft, 0 }, boxSize, boxTop - boxBottom, 1, { 0, 0, 0, 1 }, true, true)
    );
    entities.push_back(
        createRect({ boxRight, 0 }, boxSize, boxTop - boxBottom, 1, { 0, 0, 0, 1 }, true, true)
    );
}

void getSlope(std::vector<Polygon>& entities, double contentScale) {
    double x0 = 0;
    double y0 = 2;
    double dx = 0.25;
    
    for (size_t i = 0; i < 8; i++) {
        double a = y0/(1 + y0*(x0 + dx));
        double b = y0/(1 + y0*x0);
        std::vector<DVec2> vertices = {
            {x0, 0},
            {x0 + dx, 0},
            {x0 + dx, a},
            {x0, b}
        };

        double denominator = 3*(a + b);
        double cy = (a*a + a*b + b*b)/denominator;
        double cx = x0 + dx*(2*a + b)/denominator;
        DVec2 mid(cx, cy);
        for (size_t i = 0; i < vertices.size(); i++) {
            vertices[i] -= mid;
        }

        entities.push_back(Polygon({x0 - 3, cy - 1}, vertices, 1, {1, 0, 1, 0}, true, true));
        x0 += dx;
    }
}

inline Player getPlayerAndEntities(std::vector<Polygon>& entities, double contentScale) {
    getSlope(entities, contentScale);
    Polygon hexagon = createRegularPolygon({ 2, -0.5 }, 5, 0.8, 1, { 0.1, 0.3, 0.7, 1 });
    entities.push_back(hexagon);
    getBox(entities, contentScale);
    
    GLcolor playerColor = { 0.4, 0.8, 0.2, 1 };
    return Player({ 0, 0 }, 0.8, 0.8, 1, playerColor);
}


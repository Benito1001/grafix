#pragma once
#include "polygon.hpp"
#include "utils.hpp"

#include <GLFW/glfw3.h>
#include <unordered_map>

class Player : public Polygon {
public:
    std::unordered_map<int, bool> keys;

    Player(
        DVec2 pos, double width, double height,
        double density, GLcolor color = {0, 0, 0, 1}
    ): Polygon(pos, getSquareVertices(width, height), density, color) {}

    void move(double dt) override  {
        double speedForce = 2*mass;
        if (keys[GLFW_KEY_W]) {
            force.y += speedForce;
        }
        if (keys[GLFW_KEY_S]) {
            force.y -= speedForce;
        }
        if (keys[GLFW_KEY_D]) {
            force.x += speedForce;
        }
        if (keys[GLFW_KEY_A]) {
            force.x -= speedForce;
        }
    }
};

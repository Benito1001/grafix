#pragma once
#include "vector2.hpp"
#include "utils.hpp"

class Hitbox {
public:
    DVec2 pos;
    double width;
    double height;
    Hitbox() = default;
    Hitbox(DVec2 pos, double width, double height): pos(pos), width(width), height(height) {}

    bool collides(Hitbox other) const {
		if (
			((pos.x + width > other.pos.x) && (pos.x < other.pos.x + other.width))
			&& ((pos.y + height > other.pos.y) && (pos.y < other.pos.y + other.height))
		) {
			return true;
        }
		return false;
    }

    void printSelf() const {
        print("(", pos.x, ",", pos.y, ") -", width, "x", height);
    }
};

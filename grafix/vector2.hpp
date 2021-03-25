#pragma once
#include <cmath>

template<typename T>
class Vector2 {
public:
    T x;
    T y;

    Vector2() = default;
    Vector2(T x, T y) {
        this->x = x;
        this->y = y;
    }

    Vector2<T> operator+(const Vector2<T> &other) const {
        return Vector2<T>(x + other.x, y + other.y);
    }
    Vector2<T> operator+(T adder) const {
        return Vector2<T>(x + adder, y + adder);
    }
    void operator+=(const Vector2<T> &other) {
        x += other.x;
        y += other.y;
    }
     void operator+=(T adder) {
        x += adder;
        y += adder;
    }

    Vector2<T> operator-(const Vector2<T> &other) const {
        return Vector2<T>(x - other.x, y - other.y);
    }
    Vector2<T> operator-(T subtracter) const {
        return Vector2<T>(x - subtracter, y - subtracter);
    }
    void operator-=(const Vector2<T> &other) {
        x -= other.x;
        y -= other.y;
    }
     void operator-=(T subtracter) {
        x -= subtracter;
        y -= subtracter;
    }

    Vector2<T> operator*(T multiplier) const {
        return Vector2<T>(x * multiplier, y * multiplier);
    }
    Vector2<T> operator*(Vector2<T> otherVec) const {
        // this is not realy a vector operation, however it is handy
        return Vector2<T>(x * otherVec.x, y * otherVec.y);
    }
    void operator*=(T multiplier) {
        x *= multiplier;
        y *= multiplier;
    }

    Vector2<T> operator/(T divisor) const {
        return Vector2<T>(x / divisor, y / divisor);
    }
    void operator/=(T divisor) {
        x /= divisor;
        y /= divisor;
    }

    Vector2<T> operator-() const {
		return Vector2<T>(-x, -y);
	}

    Vector2<T> getAbs() const {
        return Vector2<T>(std::abs(x), std::abs(y));
    }

    Vector2<T> getOrthogonal() const {
        return Vector2<T>(-y, x);
    }

    void normalize() {
        double length = getLength();
        x /= length;
        y /= length;
    }

    Vector2<T> getNormalized() const {
        double length = getLength();
        return *this/length;
    }


    T getSquaredLength() const {
        return x*x + y*y;
    }
    
    double getLength() const {
        return std::sqrt(static_cast<double>(getSquaredLength()));
    }

    Vector2<T> setLength(double length) const {
        return getNormalized()*length;
    }


    void rotate(double angle) {
        double cos = std::cos(angle);
        double sin = std::sin(angle);
        x = x*cos - y*sin;
        y = x*sin + y*cos;
    }

    Vector2<T> getRotated(double angle) const {
        double cos = std::cos(angle);
        double sin = std::sin(angle);
        return Vector2<T>(x*cos - y*sin, x*sin + y*cos);
    }

    Vector2<T> getRotatedFast(double cos, double sin) const {
        return Vector2<T>(x*cos - y*sin, x*sin + y*cos);
    }

    double getAngle() const {
        if (getSquaredLength() == 0) {
            return 0;
        }
        return std::atan2(y, x);
    }


    T cross(const Vector2<T> &other) const {
        return x*other.y - y*other.x;
    }

    T dot(const Vector2<T> &other) const {
        return x*other.x + y*other.y;
    }


    void set(T x, T y) {
        this->x = x;
        this->y = y;
    }

    Vector2<T> copy() const {
        return Vector2<T>(x, y);
    }

    /*std::string get_str() const {
        char buffer[64];
        std::sprintf(buffer, "[%g, %g]", x, y);
        return buffer;
    }*/

    void printSelf() const {
        std::cout << "[" << x << ", " << y << "]\n";
    }
};

template<typename T>
Vector2<T> operator*(int multiplier, Vector2<T> &vec) {
    return Vector2<T>(vec.x*multiplier, vec.y*multiplier);
}

template<typename T>
Vector2<T> operator*(double multiplier, Vector2<T> &vec) {
    return Vector2<T>(vec.x*multiplier, vec.y*multiplier);
}

using DVec2 = Vector2<double>;
using FVec2 = Vector2<float>;
using IVec2 = Vector2<int>; 

#pragma once

#include "spine/platform/hal.hpp"

namespace spn::structure {

/*
 * Euclidean XYZPoint (x, y, z)
 */

template<typename T>
struct XYZPoint {
    T x, y, z;

    XYZPoint() : x(0), y(0), z(0) {}
    XYZPoint(T x_value, T y_value, T z_value) : x(x_value), y(y_value), z(z_value) {}
    XYZPoint(const XYZPoint& other) { *this = other; }
    ~XYZPoint() = default;

    XYZPoint& operator=(const XYZPoint& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }
    XYZPoint operator+(const XYZPoint& other) const { return XYZPoint<T>(x + other.x, y + other.y, z + other.z); }
    XYZPoint operator-(const XYZPoint& other) const { return XYZPoint<T>(x - other.x, y - other.y, z - other.z); }
    XYZPoint operator*(const XYZPoint& other) const { return XYZPoint<T>(x * other.x, y * other.y, z * other.z); }
    XYZPoint operator/(const XYZPoint& other) const { return XYZPoint<T>(x / other.x, y / other.y, z / other.z); }
    XYZPoint operator%(const XYZPoint& other) const { return XYZPoint<T>(x % other.x, y % other.y, z % other.z); }
    bool operator==(const XYZPoint& other) const { return (x == other.x && y == other.y && z == other.z); }
    bool operator!=(const XYZPoint& other) const { return (x != other.x || y != other.y || z != other.z); }

    void Add(const XYZPoint& other) {
        x += other.x;
        y += other.y;
        z += other.z;
    }
    void Substract(const XYZPoint& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }
    void ScalarMultiplication(T s) {
        x *= s;
        y *= s;
        z *= s;
    }
    T Mag() const { return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); }
    void Normalize() {
        T mag = this->Mag();
        x /= mag;
        y /= mag;
        z /= mag;
    }
    XYZPoint Dot(const XYZPoint& other) const { return XYZPoint<T>(x * other.x, y * other.y, z * other.z); }
};

/*
 * Euclidean XYPoint (x, y)
 */

template<typename T>
struct XYPoint {
    T x, y;

    XYPoint() : x(0), y(0) {}
    XYPoint(T x_value, T y_value) : x(x_value), y(y_value) {}
    XYPoint(const XYPoint& other) { *this = other; }
    ~XYPoint() = default;

    XYPoint& operator=(const XYPoint& other) {
        x = other.x;
        y = other.y;
        return *this;
    }
    XYPoint operator+(const XYPoint& other) const { return XYPoint<T>(x + other.x, y + other.y); }
    XYPoint operator-(const XYPoint& other) const { return XYPoint<T>(x - other.x, y - other.y); }
    XYPoint operator*(const XYPoint& other) const { return XYPoint<T>(x * other.x, y * other.y); }
    XYPoint operator/(const XYPoint& other) const { return XYPoint<T>(x / other.x, y / other.y); }
    XYPoint operator%(const XYPoint& other) const { return XYPoint<T>(x % other.x, y % other.y); }
    bool operator==(const XYPoint& other) const { return (x == other.x && y == other.y); }
    bool operator!=(const XYPoint& other) const { return (x != other.x || y != other.y); }

    void Add(const XYPoint& other) {
        x += other.x;
        y += other.y;
    }
    void Substract(const XYPoint& other) {
        x -= other.x;
        y -= other.y;
    }
    void ScalarMultiplication(T s) {
        x *= s;
        y *= s;
    }
    T Mag() const { return sqrt(pow(x, 2) + pow(y, 2)); }
    void Normalize() {
        T mag = this->Mag();
        x /= mag;
        y /= mag;
    }
    XYPoint Dot(const XYPoint& other) const { return XYPoint<T>(x * other.x, y * other.y); }
};

} // namespace spn::structure

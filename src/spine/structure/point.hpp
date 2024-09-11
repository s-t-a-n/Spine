#pragma once

namespace spn::structure {

/*
 * Euclidean XYZPoint (x, y, z)
 */

template<typename T>
struct XYZPoint {
    T x, y, z;

    XYZPoint() : x(0), y(0), z(0) {}
    XYZPoint(T x_value, T y_value, T z_value) : x(x_value), y(y_value), z(z_value) {}

    XYZPoint operator+(const XYZPoint& other) const { return XYZPoint<T>(x + other.x, y + other.y, z + other.z); }
    XYZPoint operator-(const XYZPoint& other) const { return XYZPoint<T>(x - other.x, y - other.y, z - other.z); }
    XYZPoint operator*(const XYZPoint& other) const { return XYZPoint<T>(x * other.x, y * other.y, z * other.z); }
    XYZPoint operator/(const XYZPoint& other) const { return XYZPoint<T>(x / other.x, y / other.y, z / other.z); }
    XYZPoint operator%(const XYZPoint& other) const { return XYZPoint<T>(x % other.x, y % other.y, z % other.z); }
    bool operator==(const XYZPoint& other) const { return (x == other.x && y == other.y && z == other.z); }
    bool operator!=(const XYZPoint& other) const { return (x != other.x || y != other.y || z != other.z); }

    void add(const XYZPoint& other) {
        x += other.x;
        y += other.y;
        z += other.z;
    }
    void subtract(const XYZPoint& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }
    void scalar_multiplication(T s) {
        x *= s;
        y *= s;
        z *= s;
    }
    T mag() const { return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); }
    void normalize() {
        T mag = this->mag();
        x /= mag;
        y /= mag;
        z /= mag;
    }
    XYZPoint dot(const XYZPoint& other) const { return XYZPoint<T>(x * other.x, y * other.y, z * other.z); }
};

/*
 * Euclidean XYPoint (x, y)
 */

template<typename T>
struct XYPoint {
    T x, y;

    XYPoint() : x(0), y(0) {}
    XYPoint(T x_value, T y_value) : x(x_value), y(y_value) {}

    XYPoint operator+(const XYPoint& other) const { return XYPoint<T>(x + other.x, y + other.y); }
    XYPoint operator-(const XYPoint& other) const { return XYPoint<T>(x - other.x, y - other.y); }
    XYPoint operator*(const XYPoint& other) const { return XYPoint<T>(x * other.x, y * other.y); }
    XYPoint operator/(const XYPoint& other) const { return XYPoint<T>(x / other.x, y / other.y); }
    XYPoint operator%(const XYPoint& other) const { return XYPoint<T>(x % other.x, y % other.y); }
    bool operator==(const XYPoint& other) const { return (x == other.x && y == other.y); }
    bool operator!=(const XYPoint& other) const { return (x != other.x || y != other.y); }

    void add(const XYPoint& other) {
        x += other.x;
        y += other.y;
    }
    void subtract(const XYPoint& other) {
        x -= other.x;
        y -= other.y;
    }
    void scalar_multiplication(T s) {
        x *= s;
        y *= s;
    }
    T mag() const { return sqrt(pow(x, 2) + pow(y, 2)); }
    void normalize() {
        T mag = this->mag();
        x /= mag;
        y /= mag;
    }
    XYPoint dot(const XYPoint& other) const { return XYPoint<T>(x * other.x, y * other.y); }
};

} // namespace spn::structure

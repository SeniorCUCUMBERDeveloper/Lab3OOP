#ifndef POINT_HPP
#define POINT_HPP
#include <tuple>

template <typename T>
struct Point{
    T x, y, z;

    bool operator==(const Point<T>& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator<(const Point<T>& other) const {
        return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
    }

    Point(const Point<T>& other) : x(other.x), y(other.y), z(other.z) {}

    Point<T>& operator=(const Point<T>& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }

    Point(T x, T y, T z) : x(x), y(y), z(z) {}
    Point(){}
};

#endif
#ifndef POINT_HPP
#define POINT_HPP


template <typename T>
struct Point{
    T x, y, z;

    bool operator==(const Point<T>& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator<(const Point<T>& other) const {
        return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
    }

    Point(T x, T y, T z) : x(x), y(y), z(z) {}
    Point(){}
};

#endif
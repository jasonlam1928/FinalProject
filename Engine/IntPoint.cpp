#include "IntPoint.hpp"
#include "Point.hpp"  // for Point result in * and /

namespace Engine {
    IntPoint::IntPoint() : x(0), y(0) {}
    IntPoint::IntPoint(int x, int y) : x(x), y(y) {}

    bool IntPoint::operator==(const IntPoint& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool IntPoint::operator!=(const IntPoint& rhs) const {
        return !(*this == rhs);
    }

    bool IntPoint::operator<(const IntPoint& rhs) const {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    IntPoint IntPoint::operator+(const IntPoint& rhs) const {
        return IntPoint(x + rhs.x, y + rhs.y);
    }

    IntPoint IntPoint::operator-(const IntPoint& rhs) const {
        return IntPoint(x - rhs.x, y - rhs.y);
    }

    Point IntPoint::operator*(const float& rhs) const {
        return Point(x * rhs, y * rhs);
    }

    Point IntPoint::operator/(const float& rhs) const {
        return Point(x / rhs, y / rhs);
    }
}

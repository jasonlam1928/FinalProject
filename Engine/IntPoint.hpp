#ifndef INTPOINT_HPP
#define INTPOINT_HPP

#include <tuple>
#include <functional>
#include"Point.hpp"

namespace Engine {
    struct IntPoint {
        int x, y;
        IntPoint();
        IntPoint(int x, int y);

        bool operator==(const IntPoint& rhs) const;
        bool operator!=(const IntPoint& rhs) const;
        bool operator<(const IntPoint& rhs) const;

        IntPoint operator+(const IntPoint& rhs) const;
        IntPoint operator-(const IntPoint& rhs) const;

        // 以下回傳 Point（float 向量），需包含 Point.hpp
        Point operator*(const float& rhs) const;
        Point operator/(const float& rhs) const;
    };
}

namespace std {
    template <>
    struct hash<Engine::IntPoint> {
        size_t operator()(const Engine::IntPoint& p) const {
            return (hash<int>()(p.x) << 1) ^ hash<int>()(p.y);
        }
    };
}

#endif

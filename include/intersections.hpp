#pragma once
#include "geometry.hpp"
#include <cmath>
#include <optional>

namespace geometry::intersections {

/*
 * Класс для поиска пересечений между двумя фигурами
 *
 * Требуется организовать возможность нахождения пересечений только для следующих комбинаций фигур:
 *    - Line   & Line
 *    - Line   & Circle
 *    - Circle & Circle
 *
 * Для всех остальных требуется выбросить исключение std::logic_error
 */
class IntersectionVisitor {
public:// data
    static constexpr double eps = 1e-10;

public: // methods
    std::optional<Point2D> operator()(const Line &lhs, const Line &rhs) const {
        const auto lhs_dir = lhs.end - lhs.start;
        const auto rhs_dir = rhs.end - rhs.start;
        const auto delta = rhs.start - lhs.start;
        const auto denom = lhs_dir.Cross(rhs_dir);
        if (std::abs(denom) < eps) {
            return std::nullopt;
        }

        const auto lhs_t = delta.Cross(rhs_dir) / denom;
        const auto rhs_t = delta.Cross(lhs_dir) / denom;
        if (!(0. <= lhs_t && lhs_t <= 1.0 && 0. <= rhs_t && rhs_t <= 1.0)) {
            return std::nullopt;
        }

        return lhs.start + lhs_dir * lhs_t;
    }

    std::optional<Point2D> operator()(const Line &l, const Circle &r) const {
        const Point2D d = l.end - l.start;
        const Point2D f = l.start - r.Center();
        const double a = d.Dot(d);
        const double b = 2.0 * f.Dot(d);
        const double c = f.Dot(f) - r.radius * r.radius;
        const double discriminant = b * b - 4.0 * a * c;

        if (discriminant < -eps) {
            return std::nullopt;
        }
        if (std::abs(discriminant) < eps) {
            const double t = -b / (2.0 * a);
            if (0.0 <= t && t <= 1.0) {
                return l.start + d * t;
            }

            return std::nullopt;
        }

        const double sqrt_d = std::sqrt(discriminant);
        const double t1 = (-b - sqrt_d) / (2.0 * a);
        if (0.0 <= t1 && t1 <= 1.0) {
            return l.start + d * t1;
        }

        const double t2 = (-b + sqrt_d) / (2.0 * a);
        if (0.0 <= t2 && t2 <= 1.0) {
            return l.start + d * t2;
        }

        return std::nullopt;
    }

    std::optional<Point2D> operator()(const Circle &l, const Circle &r) const {
        const Point2D delta = r.Center() - l.Center();
        const double d2 = delta.Dot(delta);
        const double d = std::sqrt(d2);

        if (d < eps) {
            return Point2D{l.Center().x + l.radius, l.Center().y};
        }
        if (d > l.radius + r.radius + eps) {
            return std::nullopt;
        }
        if (d < std::abs(l.radius - r.radius) - eps) {
            return std::nullopt;
        }

        const double a = (l.radius * l.radius - r.radius * r.radius + d2) / (2.0 * d);
        const double h2 = l.radius * l.radius - a * a;

        if (h2 < -eps) {
            return std::nullopt;
        }

        const double h = std::sqrt(std::max(0., h2));
        const Point2D p = l.Center() + delta * (a / d);
        const Point2D perpendicular{-delta.y / d, delta.x / d};

        return p + perpendicular * h;
    }

    std::optional<Point2D> operator()(const auto &l, const auto &r) const {
        throw std::logic_error("Unsupported figures");
        return std::nullopt;
    }
};

inline std::optional<Point2D> GetIntersectPoint(const Shape &shape1, const Shape &shape2) {
    IntersectionVisitor visitor;
    return std::visit(visitor, shape1, shape2);
}

}  // namespace geometry::intersections
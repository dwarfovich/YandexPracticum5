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
public:
    std::optional<Point2D> operator()(const Line& lhs, const Line& rhs) const {
        static constexpr double eps = 1e-10;

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

    std::optional<Point2D> operator()(const Line &l, const Circle &r) const { return std::nullopt; }
    std::optional<Point2D> operator()(const Circle &l, const Circle &r) const { return std::nullopt; }

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
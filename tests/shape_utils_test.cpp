#include "shape_utils.hpp"
#include "geometry.hpp"

#include <gtest/gtest.h>

using namespace geometry;

/*
std::vector<std::pair<Shape, Shape>> FindAllCollisions(std::span<const Shape> shapes) {
    std::vector<std::pair<Shape, Shape>> collisions;
    for (auto i : std::views::iota(size_t{0}, shapes.size())) {
        auto colliding = std::views::iota(i + 1, shapes.size()) | std::views::filter([&](size_t j) {
                             return geometry::ShapesBoundingBoxesIntersect(shapes[i], shapes[j]);
                         });

        std::ranges::for_each(colliding, [&](size_t j) { collisions.emplace_back(shapes[i], shapes[j]); });
    }

    return collisions;
}

inline bool ShapesBoundingBoxesIntersect(const Shape &lhs, const Shape &rhs) noexcept {
    return BoundingBoxesIntersect(std::visit([](const auto &s) { return s.BoundBox(); }, lhs),
                                  std::visit([](const auto &s) { return s.BoundBox(); }, rhs));
}

*/
TEST(ShapeUtilsTest, SimpleCheck) { 
    geometry::Line line{{0, 1}, {2, 3}};
    geometry::Circle circle{{0, 1}, 5};
    ASSERT_TRUE(ShapesBoundingBoxesIntersect(line, circle));
}

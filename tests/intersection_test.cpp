#include <gtest/gtest.h>
#include "intersections.hpp"

using namespace geometry;
using namespace geometry::intersections;

TEST(IntersectionsTest, IntersectionCheck) { 
    geometry::Line line {{1, 1}, {2, 1}};
    geometry::Circle circle {{1, 1}, 1};
    IntersectionVisitor visitor;
    auto result = std::visit(visitor, Shape{line}, Shape{circle});
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), Point2D(2, 1));
}

TEST(IntersectionsTest, IntersectionCheck2) {
    geometry::Line line {{1, 1}, {2, 1}};
    geometry::Line line2 {{1, 1}, {2, 2}};
    IntersectionVisitor visitor;
    auto result = std::visit(visitor, Shape{line}, Shape{line2});
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), Point2D(1, 1));
}

TEST(IntersectionsTest, IntersectionCheck3) {
    geometry::Line line{{1, 1}, {2, 1}};
    geometry::Line line2{{0, 0}, {1, 1}};
    IntersectionVisitor visitor;
    auto result = std::visit(visitor, Shape{line}, Shape{line2});
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), Point2D(1, 1));
}

TEST(IntersectionsTest, IntersectionCheck4) {
    geometry::Circle c1{{1, 1}, 1};
    geometry::Circle c2{{3, 1}, 1};
    IntersectionVisitor visitor;
    auto result = std::visit(visitor, Shape{c1}, Shape{c2});
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), Point2D(2, 1));
}

TEST(IntersectionsTest, NotIntersectionCheck) {
    geometry::Circle c1{{1, 1}, 1};
    geometry::Circle c2{{3.1, 1}, 1};
    IntersectionVisitor visitor;
    auto result = std::visit(visitor, Shape{c1}, Shape{c2});
    ASSERT_FALSE(result.has_value());
}

TEST(IntersectionsTest, NotIntersectionCheck2) {
    geometry::Line line{{1, 1}, {2, 1}};
    geometry::Line line2{{1, 0}, {2, 0}};
    IntersectionVisitor visitor;
    auto result = std::visit(visitor, Shape{line}, Shape{line2});
    ASSERT_FALSE(result.has_value());
}

TEST(IntersectionsTest, NotIntersectionCheck3) {
    geometry::Line line{{1, 1}, {2, 1}};
    geometry::Circle circle{{1, 2.1}, 1};
    IntersectionVisitor visitor;
    auto result = std::visit(visitor, Shape{line}, Shape{circle});
    ASSERT_FALSE(result.has_value());
}
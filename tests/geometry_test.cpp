#include "geometry.hpp"
#include <gtest/gtest.h>

using namespace geometry;

TEST(GeometryTest, PointLine) {
    Line l{{0., 0.}, {0., 0.}};
    ASSERT_DOUBLE_EQ(l.Length(), 0.);
    ASSERT_EQ(l.Direction(), Point2D(0., 0.));
    ASSERT_EQ(l.BoundBox(), BoundingBox(0., 0., 0., 0.));
    ASSERT_DOUBLE_EQ(l.Height(), 0.);
    ASSERT_EQ(l.Center(), Point2D(0., 0.));
    const auto v = std::array<Point2D, 2>{Point2D{0., 0.}, Point2D{0., 0.}};
    ASSERT_EQ(l.Vertices(), v);
    const auto lines = Lines2D<2>{{0., 0.}, {0., 0.}};
    ASSERT_EQ(l.Lines(), lines);
}

TEST(GeometryTest, SimpleLine) {
    Line l{{0., 0.}, {1., 1.}};
    ASSERT_DOUBLE_EQ(l.Length(), std::sqrt(2));
    ASSERT_EQ(l.Direction(), Point2D(1. / l.Length(), 1. / l.Length()));
    ASSERT_EQ(l.BoundBox(), BoundingBox(0., 0., 1., 1.));
    ASSERT_DOUBLE_EQ(l.Height(), 1.);
    ASSERT_EQ(l.Center(), Point2D(0.5, 0.5));
    const auto v = std::array<Point2D, 2>{Point2D{0., 0.}, Point2D{1., 1.}};
    ASSERT_EQ(l.Vertices(), v);
    const auto lines = Lines2D<2>{{0., 1.}, {0., 1.}};
    ASSERT_EQ(l.Lines(), lines);
}

TEST(GeometryTest, TriangleTest) {
    Triangle t{{0., 0.}, {0., 1.}, {1., 0.}};
    ASSERT_DOUBLE_EQ(t.Area(), 0.5);
    ASSERT_EQ(t.BoundBox(), BoundingBox(0., 0., 1., 1.));
    ASSERT_DOUBLE_EQ(t.Height(), 1.);
    ASSERT_EQ(t.Center(), Point2D(1. / 3., 1. / 3.));
    const auto v = std::array<Point2D, 3>{Point2D{0., 0.}, Point2D{0., 1.}, Point2D{1., 0.}};
    ASSERT_EQ(t.Vertices(), v);
    const auto lines = Lines2D<4>{{0., 0., 1.0, 0.}, {0., 1., 0., 0.}};
    ASSERT_EQ(t.Lines(), lines);
}

TEST(GeometryTest, RectangleTest) {
    Rectangle r{{0., 0.}, 1., 1.};
    ASSERT_EQ(r.TopRight(), Point2D(1., 1.));
    ASSERT_EQ(r.BoundBox(), BoundingBox(0., 0., 1., 1.));
    ASSERT_DOUBLE_EQ(r.Height(), 1.);
    ASSERT_EQ(r.Center(), Point2D(0.5, 0.5));
    const auto v = std::array<Point2D, 4>{Point2D{0., 0.}, Point2D{1., 0.}, Point2D{1., 1.}, Point2D{0., 1.}};
    ASSERT_EQ(r.Vertices(), v);
    const auto lines = Lines2D<5>{{0., 0., 1., 1., 0.}, {0., 1., 1., 0., 0.}};
    ASSERT_EQ(r.Lines(), lines);
}

TEST(GeometryTest, CircleTest) {
    Circle c{{1., 1.}, 1.};
    ASSERT_EQ(c.BoundBox(), BoundingBox(0., 0., 2., 2.));
    ASSERT_DOUBLE_EQ(c.Height(), 2.);
    ASSERT_EQ(c.Center(), Point2D(1., 1.));
}

TEST(GeometryTest, IntersectionTest) {
    geometry::Line line{{1, 1}, {2, 1}};
    geometry::Circle circle{{1, 1}, 1};
    ASSERT_TRUE(ShapesBoundingBoxesIntersect(line, circle));
}

TEST(GeometryTest, NotIntersectionTest) {
    geometry::Line line{{1, 1}, {2, 1}};
    geometry::Circle circle{{10, 10}, 1};
    ASSERT_FALSE(ShapesBoundingBoxesIntersect(line, circle));
}

TEST(GeometryTest, IntersectionTest2) {
    geometry::Line line{{1, 1}, {2, 1}};
    geometry::Line line2{{1.5, 1.5}, {1.5, 0}};
    ASSERT_TRUE(ShapesBoundingBoxesIntersect(line, line2));
}

TEST(GeometryTest, NotIntersectionTest2) {
    geometry::Line line{{0, 0}, {1, 1}};
    geometry::Line line2{{2, 2}, {3, 3}};
    ASSERT_FALSE(ShapesBoundingBoxesIntersect(line, line2));
}

TEST(GeometryTest, IntersectionTest3) {
    geometry::Line line{{0, 0}, {1, 1}};
    geometry::Rectangle rect{{0.5, 0.5}, 2, 2};
    ASSERT_TRUE(ShapesBoundingBoxesIntersect(line, rect));
}

TEST(GeometryTest, NotIntersectionTest3) {
    geometry::Line line{{0, 0}, {1, 1}};
    geometry::Rectangle rect{{-1, -1}, -0.01, -0.01};
    ASSERT_FALSE(ShapesBoundingBoxesIntersect(line, rect));
}
#include "convex_hull.hpp"
#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "triangulation.hpp"
#include "visualization.hpp"

#include <algorithm>
#include <print>
#include <ranges>

using namespace geometry;

namespace rng = std::ranges;
namespace views = std::ranges::views;

void PrintAllIntersections(const Shape &shape, std::span<const Shape> others) {
    std::println("\n=== Intersections ===");

    /*
     * Используйте ranges чтобы оставить только фигуры,
     * поддерживающие возможность находить пересечения между собой
     *
     * Затем примените монадический интерфейс для обработки результатов:
     *     - Пересечение найдено в точке A между фигурами B и C
     *     - Фигуры B и C не пересекаются
     */
    auto shapesToCheck = others | views::filter([&shape](const Shape &s) {
                             return (std::holds_alternative<Line>(shape) && std::holds_alternative<Line>(s)) ||
                                    (std::holds_alternative<Line>(shape) && std::holds_alternative<Circle>(s)) ||
                                    (std::holds_alternative<Circle>(shape) && std::holds_alternative<Circle>(s));
                         });

    geometry::intersections::IntersectionVisitor visitor;
    std::ranges::for_each(shapesToCheck, [&](const Shape &other) {
        std::visit(visitor, shape, other)
            .transform([&](const Point2D &p) {
                std::println("Intersection found at {} between {} and {}", p, shape, other);
                return p;
            })
            .or_else([&] {
                std::println("{} and {} do not intersect", shape, other);
                return std::optional<Point2D>{};
            });
    });
}

void PrintDistancesFromPointToShapes(Point2D p, std::span<const Shape> shapes) {
    std::println("\n=== Distance from Point Test ===");

    /*
     * Используйте ranges чтобы выбрать любые 5 фигур из списка.
     * Затем найдите расстояния от заданной точки до всех выбранных фигур.
     * Выведите результат в формате "Расстояние от точки P до фигуры S равно D"
     */
    geometry::queries::PointToShapeDistanceVisitor visitor{p};
    auto selected_shapes = shapes | views::take(5);
    rng::for_each(selected_shapes, [&p, &visitor](const auto &shape) {
        const auto distance = std::visit(visitor, shape);
        std::print("Distance from point {} to figure {} is {}\n", p, shape, distance);
    });
}

void PerformShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Analysis ===");
    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Найти все пересечения между фигурами используя метод Bounding Box
     *     - Найти самую высокую фигуру (чья высота наибольшая)expected
     *     - Вывести расстояние между любыми двумя фигурами, которые поддерживают данную функциональность
     */

    geometry::intersections::IntersectionVisitor intersectionVisitor;
    for (auto &&[i, shape] : shapes | views::enumerate) {
        rng::for_each(shapes | std::views::drop(i), [&](const auto &other) {
            try {
                auto intersection = std::visit(intersectionVisitor, shapes[i], other);
                std::print("Shape {} intersects with shape {} at point {}\n", shapes[i], other, intersection.value());
            } catch (const std::exception &e) {
                std::print("Shape {} has no algorithm to find intersection with shape {}\n", shapes[i], other);
            }
        });
    }

    auto GetHeight = [](const auto &shape) { return std::visit([](const auto &s) { return s.Height(); }, shape); };
    auto iter = rng::max_element(shapes, {}, GetHeight);
    if (iter != shapes.end()) {
        std::print("Highest shape: {}, height = {}\n", *iter, GetHeight(*iter));
    } else {
        std::print("Couldn't find the tallest figure.\n");
    }

    geometry::queries::ShapeToShapeDistanceVisitor distance_visitor;
    for (auto &&[i, shape] : shapes | views::enumerate) {
        rng::for_each(shapes | std::views::drop(i), [&](const auto &other) {
            auto distance = std::visit(distance_visitor, shapes[i], other);
            if (distance.has_value()) {
                std::print("Distance from {} to {} is {}\n", shapes[i], other, distance.value());
            }
        });
    }
}

void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");
    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Вывести 3 любые фигуры, которые находятся выше 50.0
     *     - Вывести фигуры с наименьшей и с наибольшей высотами
     */

    // clang-format off
    auto highest_shapes = shapes
                          | views::filter([](const auto &shape) {
                              return std::visit([](const auto &s) { return s.BoundBox().max_y > 50.; }, shape);
                          })
                          | views::take(3);
    // clang-format on
    rng::for_each(highest_shapes, [](const auto &shape) { std::print("The {} is higher than 50.0", shape); });
}

int main() {
    std::vector<Point2D> ps = {{1, 2}, {3, 4}};
    std::print("{:new_line}", ps);
    std::print("\n");

    std::vector<Shape> shapes = utils::ParseShapes("circle 0 0 1.5; line 1 2 3 4; polygon 0 0 2 5; triangle 0 0 1 0 "
                                                   "0.5 1; polygon 0 0 1 2; badshape; circle 0 0 -1");

    std::println("Parsed {} shapes", shapes.size());

    geometry::Line line{{0, 1}, {2, 3}};
    geometry::Circle circle{{0, 1}, 5};
    geometry::Shape v1 = line;
    geometry::Shape v2 = circle;
    auto r = geometry::intersections::GetIntersectPoint(line, circle);
    std::print("Intersect result: {}\n\n", r.value_or(Point2D{0, 0}));

    // Выведите индекс каждой фигуры и её высоту
    std::print("Heights:\n");
    for (auto &&[i, shape] : shapes | std::views::enumerate) {
        std::print("{}: Shape {}, height = {}\n", i, shape,
                   std::visit([](const auto &s) { return s.Height(); }, shape));
    }

    //
    // Вызываем разработанные функции
    //
    // PrintAllIntersections(shapes[0], shapes);

    // PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);

    // PerformShapeAnalysis(shapes);

    // PerformExtraShapeAnalysis(shapes);

    //
    // Рисуем все фигуры
    //
    // Важно: после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 2ой график
    //
    geometry::visualization::Draw(shapes);

    //
    // Формируем список из вершин всех фигур
    //
    std::vector<Point2D> points;
    rng::for_each(shapes, [&points](const auto &shape) {
        std::visit(
            [&points](const auto &s) {
                auto &&vertices = s.Vertices();
                points.insert(points.end(), vertices.begin(), vertices.end());
            },
            shape);
    });

    //
    // Находим список точек, для построения выпуклой оболочки - convex hull - алгоритмом Грэхема
    // Создаём из них объект класса `Polygon` и добавляем его в список shapes
    // Рисуем все фигуры
    //

    auto hull = geometry::convex_hull::GrahamScan(points);
    if (hull.has_value()) {
        shapes.emplace_back(geometry::Polygon{std::move(hull.value())});
    }

    geometry::visualization::Draw(shapes);
    //
    // после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 3ий график
    //

    {
        std::vector<Point2D> points = {{0, 0}, {10, 0}, {5, 8}, {15, 5}, {2, 12}};
        auto result = geometry::triangulation::DelaunayTriangulation(points);
        if(result.has_value()){
            const auto& triangles = result.value();
            geometry::visualization::Draw(triangles);
        }
        //
        // Используйте список точек points или свой, чтобы
        // выполнить алгоритм триангуляции Делоне алгоритмом Боуэра-Ватсона
        //
        // После успешного завершения алгоритма - выведите результат для проверки
        // используя geometry::visualization::Draw
        //
    }
    return 0;
}
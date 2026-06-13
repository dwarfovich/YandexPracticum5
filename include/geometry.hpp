#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <format>
#include <numbers>
#include <ranges>
#include <variant>
#include <vector>

namespace geometry {

/*
 * Добавьте к методам класса Point2D и Lines2DDyn все необходимые аттрибуты и спецификаторы
 * Важно: Возвращаемый тип и принимаемые аргументы менять не нужно
 */
struct Point2D {
    double x, y;

    constexpr Point2D() noexcept : x(0), y(0) {}
    constexpr Point2D(double x, double y) noexcept : x(x), y(y) {}

    // Comparison
    [[nodiscard]] bool operator<(const Point2D &other) const noexcept { return x < other.x && y < other.y; }
    [[nodiscard]] bool operator==(const Point2D &other) const noexcept { return x == other.x && y == other.y; }

    // Binary math operators
    [[nodiscard]] Point2D operator+(const Point2D &other) const noexcept { return {x + other.x, y + other.y}; }
    [[nodiscard]] Point2D operator-(const Point2D &other) const noexcept { return {x - other.x, y - other.y}; }
    [[nodiscard]] Point2D operator*(double value) const noexcept { return {x * value, y * value}; }
    [[nodiscard]] Point2D operator/(double value) {
        return value == 0. ? throw std::runtime_error("Division by 0") : Point2D{x / value, y / value};
    }

    // Binary geometry operations
    [[nodiscard]] double Dot(const Point2D &other) const noexcept { return x * other.x + y * other.y; }
    [[nodiscard]] double Cross(const Point2D &other) const noexcept { return x * other.y - y * other.x; }
    [[nodiscard]] double Length() const noexcept { return std::sqrt(x * x + y * y); }
    [[nodiscard]] double DistanceTo(const Point2D &other) const noexcept { return (*this - other).Length(); }

    [[nodiscard]] Point2D Normalize() noexcept {
        const double len = Length();
        return len > 0 ? Point2D{x / len, y / len} : Point2D{0, 0};
    }
};

template <size_t N>
struct Lines2D {
    std::array<double, N> x;
    std::array<double, N> y;
};

struct Lines2DDyn {
    std::vector<double> x;
    std::vector<double> y;

    void Reserve(size_t n) {
        x.reserve(n);
        y.reserve(n);
    }
    void PushBack(Point2D p) {
        x.push_back(p.x);
        y.push_back(p.y);
    }
    void PushBack(double px, double py) {
        x.push_back(px);
        y.push_back(py);
    }
    [[noexcept]] Point2D Front() { return {x.front(), y.front()}; }
};

struct BoundingBox {
    double min_x, min_y, max_x, max_y;

    constexpr BoundingBox() noexcept : min_x(0), min_y(0), max_x(0), max_y(0) {}
    constexpr BoundingBox(double min_x, double min_y, double max_x, double max_y) noexcept
        : min_x(min_x), min_y(min_y), max_x(max_x), max_y(max_y) {}

    [[nodiscard]] constexpr bool Overlaps(const BoundingBox &other) const noexcept {
        return !(max_x < other.min_x || min_x > other.max_x || max_y < other.min_y || min_y > other.max_y);
    }

    [[nodiscard]] constexpr double Width() const noexcept { return max_x - min_x; }
    [[nodiscard]] constexpr double Height() const noexcept { return max_y - min_y; }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return {(min_x + max_x) / 2, (min_y + max_y) / 2}; }
};

struct Line {
    Point2D start, end;

    constexpr Line(Point2D start, Point2D end) noexcept : start(start), end(end) {}

    [[nodiscard]] constexpr double Length() noexcept { return start.DistanceTo(end); }
    [[nodiscard]] constexpr Point2D Direction() noexcept { return (end - start).Normalize(); }
    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept {
        return {std::min(start.x, end.x), std::min(start.y, end.y), std::max(start.x, end.x), std::max(start.y, end.y)};
    }
    [[nodiscard]] constexpr double Height() const noexcept { return std::max(start.y, end.y); }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return (start + end) / 2.0; }

    [[nodiscard]] constexpr std::array<Point2D, 2> Vertices() const noexcept {
        return {Point2D{start.x, start.y}, {end.x, end.y}};
    }
    [[nodiscard]] constexpr Lines2D<2> Lines() const noexcept { return {{start.x, end.x}, {start.y, end.y}}; }
};

struct Triangle {
    Point2D a, b, c;

    constexpr Triangle(Point2D a, Point2D b, Point2D c) noexcept : a(a), b(b), c(c) {}

    [[nodiscard]] constexpr double Area() const noexcept { return std::abs((b - a).Cross(c - a)) / 2.0; }
    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept {
        return {std::min({a.x, b.x, c.x}), std::min({a.y, b.y, c.y}), std::max({a.x, b.x, c.x}),
                std::max({a.y, b.y, c.y})};
    }
    [[nodiscard]] constexpr std::array<Point2D, 3> Vertices() const noexcept { return {a, b, c}; }
    [[nodiscard]] constexpr double Height() const noexcept { return std::max({a.y, b.y, c.y}); }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return (a + b + c) / 3.0; }

    [[nodiscard]] constexpr Lines2D<4> Lines() const noexcept { return {{a.x, b.x, c.x, a.x}, {a.y, b.y, c.y, a.y}}; }
};

struct Rectangle {
    Point2D bottom_left;
    double width, height;

    constexpr Rectangle(Point2D bottom_left, double width, double height) noexcept
        : bottom_left(bottom_left), width(width), height(height) {}

    [[nodiscard]] constexpr Point2D TopRight() const noexcept {
        return {bottom_left.x + width, bottom_left.y + height};
    }
    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept {
        return {bottom_left.x, bottom_left.y, bottom_left.x + width, bottom_left.y + height};
    }
    [[nodiscard]] constexpr std::array<Point2D, 4> Vertices() const noexcept {
        return {bottom_left,
                {bottom_left.x + width, bottom_left.y},
                {bottom_left.x + width, bottom_left.y + height},
                {bottom_left.x, bottom_left.y + height}};
    }
    [[nodiscard]] constexpr double Height() const noexcept { return bottom_left.y + height; }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return bottom_left + (Point2D{width, height} / 2.0); }

    [[nodiscard]] constexpr Lines2D<5> Lines() const noexcept {
        return {{bottom_left.x, bottom_left.x, bottom_left.x + width, bottom_left.x + width, bottom_left.x},
                {bottom_left.y, bottom_left.y + height, bottom_left.y + height, bottom_left.y, bottom_left.y}};
    }
};

struct RegularPolygon {
    Point2D center_p;
    double radius;
    int sides;

    constexpr RegularPolygon(Point2D center, double radius, int sides)
        : center_p(center), radius(radius), sides(sides) {}

    std::vector<Point2D> Vertices() const {
        std::vector<Point2D> points;
        points.reserve(sides);

        for (int i = 0; i < sides; ++i) {
            const double angle = 2 * std::numbers::pi * i / sides;
            points.emplace_back(center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle));
        }
        return points;
    }

    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept {
        return {center_p.x - radius, center_p.y - radius, center_p.x + radius, center_p.y + radius};
    }
    [[nodiscard]] constexpr double Height() const noexcept { return center_p.y + radius; }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return center_p; }

    [[nodiscard]] constexpr Lines2DDyn Lines() const {
        auto verts = Vertices();
        Lines2DDyn lines;
        lines.Reserve(verts.size() + 1);
        for (const auto &p : verts) {
            lines.PushBack(p);
        }
        lines.PushBack(lines.Front());
        return lines;
    }
};

struct Circle {
    Point2D center_p;
    double radius;

    constexpr Circle(Point2D center, double radius) noexcept : center_p(center), radius(radius) {}

    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept {
        return {center_p.x - radius, center_p.y - radius, center_p.x + radius, center_p.y + radius};
    }
    [[nodiscard]] constexpr double Height() const noexcept { return center_p.y + radius; }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return center_p; }

    [[nodiscard]] constexpr std::vector<Point2D> Vertices(size_t N = 30) const {
        std::vector<Point2D> points;
        points.reserve(N);

        for (auto i : std::ranges::views::iota(0u, N)) {
            const double angle = 2 * std::numbers::pi * i / N;
            points.emplace_back(center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle));
        }
        return points;
    }
    [[nodiscard]] constexpr Lines2DDyn Lines(size_t N = 100) const {
        Lines2DDyn lines;
        lines.Reserve(N + 1);
        for (auto i : std::ranges::views::iota(0u, N)) {
            double angle = 2 * std::numbers::pi * i / N;
            lines.PushBack(center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle));
        }
        lines.PushBack(lines.Front());
        return lines;
    }
};

class Polygon {
public:
    constexpr Polygon(std::vector<Point2D> points) noexcept : points_(std::move(points)) { CalculateBoundBox(); }

    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept { return bounding_box_; }
    [[nodiscard]] constexpr double Height() const noexcept {
        const auto box = BoundBox();
        return box.max_y - box.min_y;
    }

    [[nodiscard]] constexpr Point2D Center() const noexcept {
        const auto box = BoundBox();
        return Point2D{(box.min_x + box.max_x) / 2.0, (box.min_y + box.max_y) / 2.0};
    }

    [[nodiscard]] constexpr std::span<const Point2D> Vertices() const noexcept { return points_; }
    [[nodiscard]] constexpr Lines2DDyn Lines() const {
        Lines2DDyn lines;
        lines.Reserve(points_.size() + 1);
        for (const auto &p : points_) {
            lines.PushBack(p);
        }
        lines.PushBack(lines.Front());
        return lines;
    }

private:
    void CalculateBoundBox() {
        double min_x = points_[0].x, max_x = points_[0].x;
        double min_y = points_[0].y, max_y = points_[0].y;

        for (const auto &p : points_) {
            if (p.x < min_x)
                min_x = p.x;
            if (p.x > max_x)
                max_x = p.x;
            if (p.y < min_y)
                min_y = p.y;
            if (p.y > max_y)
                max_y = p.y;
        }

        bounding_box_ = BoundingBox{min_x, min_y, max_x, max_y};
    }

    std::vector<Point2D> points_;
    BoundingBox bounding_box_;
};

inline bool BoundingBoxesIntersect(const BoundingBox &lhs, const BoundingBox &rhs) noexcept {
    return lhs.min_x <= rhs.max_x && rhs.min_x <= lhs.max_x && lhs.min_y <= rhs.max_y && rhs.min_y <= lhs.max_y;
}

using Shape = std::variant<Line, Triangle, Rectangle, RegularPolygon, Circle, Polygon>;

inline bool ShapesBoundingBoxesIntersect(const Shape &lhs, const Shape &rhs) noexcept {
    return BoundingBoxesIntersect(std::visit([](const auto &s) { return s.BoundBox(); }, lhs),
                                  std::visit([](const auto &s) { return s.BoundBox(); }, rhs));
}

}  // namespace geometry

template <>
struct std::formatter<geometry::Point2D> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    constexpr auto format(const geometry::Point2D &p, FormatContext &ctx) const {
        return format_to(ctx.out(), "({:.2f}, {:.2f})", p.x, p.y);
    }
};

template <>
struct std::formatter<std::vector<geometry::Point2D>> {
    bool use_new_line = false;

    constexpr auto parse(std::format_parse_context &ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) {
            use_new_line = false;
        } else {
            auto specifier = std::string_view{it, ctx.end() - 1};
            if (specifier == "new_line") {
                use_new_line = true;
            }
        }
        return ctx.end() - 1;
    }

    template <typename FormatContext>
    auto format(const std::vector<geometry::Point2D> &v, FormatContext &ctx) const {
        auto out = ctx.out();
        // Standard loop may be simpler than ranges.
        for (std::size_t i = 0; i < v.size(); ++i) {
            out = use_new_line ? std::format_to(out, "\t{}", v[i]) : std::format_to(out, "{}", v[i]);
            if (i != v.size() - 1) {
                out = use_new_line ? std::format_to(out, "\n") : std::format_to(out, ", ");
            }
        }
        // for (const auto &[i, p] : v | std::views::enumerate) {
        //     use_new_line ? out = std::format_to(out, "\t{}", p) : out = std::format_to(out, "{}", p);
        //     if (i + 1 != v.size()) {
        //         out = use_new_line ? std::format_to(out, "\n") : std::format_to(out, ", ");
        //     }
        // }

        return out;
    }
};

template <>
struct std::formatter<geometry::Shape> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Shape &s, FormatContext &ctx) const {
        return std::visit([&ctx](const auto &shape) { return std::format_to(ctx.out(), "{}", shape); }, s);
    }
};

template <>
struct std::formatter<geometry::Line> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Line &l, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Line({}, {})", l.start, l.end);
    }
};

template <>
struct std::formatter<geometry::Circle> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Circle &c, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Circle(center={}, r={:.2f})", c.center_p, c.radius);
    }
};

template <>
struct std::formatter<geometry::Rectangle> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Rectangle &r, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Rectangle(bottom_left={}, w={:.2f}, h={:.2f})", r.bottom_left, r.width,
                              r.height);
    }
};

template <>
struct std::formatter<geometry::RegularPolygon> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::RegularPolygon &p, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "RegularPolygon(center={}, r={:.2f}, sides={})", p.center_p, p.radius,
                              p.sides);
    }
};
template <>
struct std::formatter<geometry::Triangle> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Triangle &t, FormatContext &ctx) const {
        return std::format_to(ctx.out(), "Triangle({}, {}, {})", t.a, t.b, t.c);
    }
};
template <>
struct std::formatter<geometry::Polygon> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const geometry::Polygon &poly, FormatContext &ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "Polygon[{} points]: [", poly.Vertices().size());

        for (const auto &p : poly.Vertices()) {
            out = std::format_to(out, "{} ", p);
        }

        return std::format_to(out, "]");
    }
};

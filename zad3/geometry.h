#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

class Vector;

class Position;

class Rectangle;

class Rectangles;

class Position {
private:
    int x_;
    int y_;

public:
    // konstruktory
    Position() = delete;

    Position(int x, int y);

    Position(const Position&) = default;

    explicit Position(const Vector &vec);

    // destruktory
    ~Position() = default;

    // gettery
    [[nodiscard]] int x() const;
    [[nodiscard]] int y() const;

    // metody
    [[nodiscard]] Position reflection() const;

    const static Position origin();

    // operatory
    Position &operator=(const Position &pos) = default;

    Position &operator=(const Vector &vec) = delete;

    Position &operator+=(const Vector& vector);
};


class Vector {
private:
    Position coords_;

public:
    // konstruktory
    Vector() = delete;

    Vector(int x, int y);

    Vector(const Vector&) = default;

    explicit Vector(const Position &pos);

    // destruktory
    ~Vector() = default;

    // gettery
    [[nodiscard]] int x() const;
    [[nodiscard]] int y() const;

    // metody
    [[nodiscard]] Vector reflection() const;

    // operatory
    Vector &operator=(const Vector &vec) = default;

    Vector &operator=(const Position &pos) = delete;

    Vector &operator+=(const Vector& vector);
};


class Rectangle {
private:
    Position bottomLeftCorner_;
    int width_;
    int height_;

public:
    // konstruktory
    Rectangle() = delete;

    Rectangle(int width, int height);

    Rectangle(int width, int height, const Position &pos);

    Rectangle(const Rectangle&) = default;

    Rectangle &operator=(const Rectangle&) = default;

    // destruktory
    ~Rectangle() = default;

    // gettery
    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;
    [[nodiscard]] Position pos() const;

    // metody
    [[nodiscard]] Rectangle reflection() const;

    [[nodiscard]] int area() const;

    friend Rectangle merge_all(const Rectangles &rects);

    // operatory
    Rectangle &operator+=(const Vector& vector);
};


class Rectangles {
private:
    std::vector<Rectangle> rectangles_;

public:
    // konstruktory
    Rectangles() = default;

    Rectangles(std::initializer_list <Rectangle> rect);

    Rectangles(const Rectangles&) = default;

    Rectangles &operator=(const Rectangles&) = default;

    // destruktory
    ~Rectangles() = default;

    // metody
    [[nodiscard]] size_t size() const;

    // operatory
    Rectangles &operator+=(const Vector& vector);

    const Rectangle &operator[](size_t position) const;

    Rectangle &operator[](size_t position);

    bool operator==(const Rectangles &rects) const;
};


// operatory non-member
bool operator==(const Position &a, const Position &b);

bool operator==(const Vector &a, const Vector &b);

bool operator==(const Rectangle &a, const Rectangle &b);

Position operator+(Position a, const Vector &b);

Position operator+(const Vector &a, Position b);

Vector operator+(Vector a, const Vector &b);

Rectangle operator+(Rectangle a, const Vector &b);

Rectangle operator+(const Vector &a, Rectangle b);

Rectangles operator+(Rectangles a, const Vector &b);

Rectangles operator+(const Vector &a, Rectangles b);


// operacje merge
Rectangle merge_horizontally(const Rectangle &rect1, const Rectangle &rect2);

Rectangle merge_vertically(const Rectangle &rect1, const Rectangle &rect2);

// friend of Rectangle
Rectangle merge_all(const Rectangles &rects);

#endif // GEOMETRY_H

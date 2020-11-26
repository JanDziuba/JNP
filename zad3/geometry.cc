#include "geometry.h"

#include <cassert>
#include <vector>

// Position

// konstruktory
Position::Position(int x, int y) : x_(x), y_(y) {}

Position::Position(const Vector &vec) : x_(vec.x()), y_(vec.y()) {}

// gettery
int Position::x() const {
    return x_;
}

int Position::y() const {
    return y_;
}

// metody
Position Position::reflection() const {
    return Position(y_, x_);
}

const Position Position::origin() {
    return Position(0, 0);
}

// operatory
Position &Position::operator+=(const Vector &vector) {
    x_ += vector.x();
    y_ += vector.y();
    return *this;
}


// Vector

// konstruktory
Vector::Vector(int x, int y) : coords_(x, y) {}

Vector::Vector(const Position &pos) : coords_(pos.x(), pos.y()) {}

// gettery
int Vector::x() const {
    return coords_.x();
}

int Vector::y() const {
    return coords_.y();
}

// metody
Vector Vector::reflection() const {
    return Vector(coords_.reflection());
}

// operatory
Vector &Vector::operator+=(const Vector &vector) {
    coords_ += vector;
    return *this;
}


// Rectangle

// konstruktory
Rectangle::Rectangle(int width, int height,
                     const Position &pos)
        : width_(width), height_(height), bottomLeftCorner_(pos) {
    assert(width > 0 && height > 0);
}

Rectangle::Rectangle(int width, int height)
        : width_(width), height_(height), bottomLeftCorner_(Position{0, 0}) {
    assert(width > 0 && height > 0);
}

// gettery
int Rectangle::width() const {
    return width_;
}

int Rectangle::height() const {
    return height_;
}

Position Rectangle::pos() const {
    return bottomLeftCorner_;
}

// metody
Rectangle Rectangle::reflection() const {
    return Rectangle(height_, width_, bottomLeftCorner_.reflection());
}

int Rectangle::area() const {
    return width_ * height_;
}

// operatory
Rectangle &Rectangle::operator+=(const Vector &vector) {
    bottomLeftCorner_ += vector;
    return *this;
}


// Rectangles

// konstruktory
Rectangles::Rectangles(std::initializer_list<Rectangle> rect) : rectangles_(
        rect) {}

// metody
size_t Rectangles::size() const {
    return rectangles_.size();
}

// operatory
Rectangles &Rectangles::operator+=(const Vector &vector) {
    for (auto itr = rectangles_.begin(); itr < rectangles_.end(); ++itr) {
        *itr += vector;
    }
    return *this;
}

const Rectangle &Rectangles::operator[](size_t position) const {
    assert(position < size());
    return rectangles_[position];
}

Rectangle &Rectangles::operator[](size_t position){
    assert(position < size());
    return rectangles_[position];
}

bool Rectangles::operator==(const Rectangles &rects) const {
    return rectangles_ == rects.rectangles_;
}


// operatory non-member
bool operator==(const Position &a, const Position &b) {
    if (a.x() == b.x() && a.y() == b.y()) {
        return true;
    } else {
        return false;
    }
}

bool operator==(const Vector &a, const Vector &b) {
    if (a.x() == b.x() && a.y() == b.y()) {
        return true;
    } else {
        return false;
    }
}

bool operator==(const Rectangle &a, const Rectangle &b) {
    if (a.pos() == b.pos() && a.width() == b.width() &&
        a.height() == b.height()) {
        return true;
    } else {
        return false;
    }
}

Position operator+(Position a, const Vector &b) {
    return a += b;
}

Position operator+(const Vector &a, Position b) {
    return b += a;
}

Vector operator+(Vector a, const Vector &b) {
    return a += b;
}

Rectangle operator+(Rectangle a, const Vector &b) {
    return a += b;
}

Rectangle operator+(const Vector &a, Rectangle b) {
    return b += a;
}

Rectangles operator+(Rectangles a, const Vector &b) {
    return a += b;
}

Rectangles operator+(const Vector &a, Rectangles b) {
    return b += a;
}


// operacje merge
bool can_merge_horizontally(const Rectangle &rect1, const Rectangle &rect2) {
    return rect1.width() == rect2.width() &&
           rect1.pos() + Vector(0, rect1.height()) == rect2.pos();
}

bool can_merge_vertically(const Rectangle &rect1, const Rectangle &rect2) {
    return rect1.height() == rect2.height() &&
           rect1.pos() + Vector(rect1.width(), 0) == rect2.pos();
}

Rectangle merge_horizontally(const Rectangle &rect1, const Rectangle &rect2) {
    assert(can_merge_horizontally(rect1, rect2));

    return Rectangle(rect1.width(), rect1.height() + rect2.height(),
                     rect1.pos());
}

Rectangle merge_vertically(const Rectangle &rect1, const Rectangle &rect2) {
    assert(can_merge_vertically(rect1, rect2));

    return Rectangle(rect1.width() + rect2.width(), rect1.height(),
                     rect1.pos());
}

// friend of Rectangle
Rectangle merge_all(const Rectangles &rects) {
    assert(rects.size() > 0);

    auto result = Rectangle(rects[0]);
    for (size_t index = 1; index < rects.size(); ++index) {
        if (can_merge_horizontally(result, rects[index])) {
            result.height_ += rects[index].height_;
        } else if (can_merge_vertically(result, rects[index])) {
            result.width_ += rects[index].width_;
        } else {
            assert(false);
        }
    }
    return result;
}

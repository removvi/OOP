#pragma once

#include <windows.h>

class Shape {
public:
    virtual ~Shape() = default;
    virtual void Draw(HDC dc) const = 0;
};

class PointShape final : public Shape {
public:
    explicit PointShape(POINT point);
    void Draw(HDC dc) const override;
private:
    POINT point_;
};

class LineShape final : public Shape {
public:
    LineShape(POINT first, POINT second);
    void Draw(HDC dc) const override;
private:
    POINT first_;
    POINT second_;
};

class RectangleShape final : public Shape {
public:
    RectangleShape(POINT first, POINT second);
    void Draw(HDC dc) const override;
private:
    POINT first_;
    POINT second_;
};

class EllipseShape final : public Shape {
public:
    EllipseShape(POINT center, POINT corner);
    void Draw(HDC dc) const override;
private:
    POINT center_;
    POINT corner_;
};

void DrawPreview(HDC dc, int type, POINT first, POINT second);

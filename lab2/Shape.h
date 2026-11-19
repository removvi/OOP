#pragma once

#include <windows.h>

class Shape {
public:
    virtual ~Shape() = default;
    virtual void Draw(HDC hdc) const = 0;
};

class PointShape : public Shape {
    POINT point;
public:
    explicit PointShape(POINT point);
    void Draw(HDC hdc) const override;
};

class LineShape : public Shape {
    POINT first;
    POINT second;
public:
    LineShape(POINT first, POINT second);
    void Draw(HDC hdc) const override;
};

class RectangleShape : public Shape {
    RECT rect;
public:
    RectangleShape(POINT center, POINT corner);
    void Draw(HDC hdc) const override;
};

class EllipseShape : public Shape {
    RECT rect;
public:
    EllipseShape(POINT first, POINT second);
    void Draw(HDC hdc) const override;
};

RECT MakeCornerRect(POINT first, POINT second);
RECT MakeCenterRect(POINT center, POINT corner);
void DrawRubberShape(HDC hdc, int type, POINT first, POINT second);

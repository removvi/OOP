#include "Shape.h"
#include <algorithm>
#include <cstdlib>

PointShape::PointShape(POINT point) : point(point) {}

void PointShape::Draw(HDC hdc) const {
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    Ellipse(hdc, point.x - 3, point.y - 3, point.x + 4, point.y + 4);
    SelectObject(hdc, oldBrush);
    DeleteObject(brush);
}

LineShape::LineShape(POINT first, POINT second) : first(first), second(second) {}

void LineShape::Draw(HDC hdc) const {
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    MoveToEx(hdc, first.x, first.y, nullptr);
    LineTo(hdc, second.x, second.y);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

RECT MakeCornerRect(POINT first, POINT second) {
    return {
        std::min(first.x, second.x),
        std::min(first.y, second.y),
        std::max(first.x, second.x),
        std::max(first.y, second.y)
    };
}

RECT MakeCenterRect(POINT center, POINT corner) {
    int radiusX = std::abs(corner.x - center.x);
    int radiusY = std::abs(corner.y - center.y);
    return {
        center.x - radiusX,
        center.y - radiusY,
        center.x + radiusX,
        center.y + radiusY
    };
}

RectangleShape::RectangleShape(POINT center, POINT corner) : rect(MakeCenterRect(center, corner)) {}

void RectangleShape::Draw(HDC hdc) const {
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH brush = CreateSolidBrush(RGB(128, 128, 128));
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

EllipseShape::EllipseShape(POINT first, POINT second) : rect(MakeCornerRect(first, second)) {}

void EllipseShape::Draw(HDC hdc) const {
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
    Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void DrawRubberShape(HDC hdc, int type, POINT first, POINT second) {
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

    if (type == 0) {
        Ellipse(hdc, first.x - 3, first.y - 3, first.x + 4, first.y + 4);
    } else if (type == 1) {
        MoveToEx(hdc, first.x, first.y, nullptr);
        LineTo(hdc, second.x, second.y);
    } else if (type == 2) {
        RECT rect = MakeCenterRect(first, second);
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    } else {
        RECT rect = MakeCornerRect(first, second);
        Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
    }

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

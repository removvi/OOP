#include "Shape.h"
#include <algorithm>
#include <cstdlib>

namespace {
constexpr COLORREF Ink = RGB(0, 0, 0);
constexpr COLORREF EllipseFill = RGB(128, 128, 128);
constexpr COLORREF Preview = RGB(0, 0, 255);

RECT CornerRect(POINT first, POINT second) {
    return {
        (std::min)(first.x, second.x),
        (std::min)(first.y, second.y),
        (std::max)(first.x, second.x),
        (std::max)(first.y, second.y)
    };
}

RECT CenterRect(POINT center, POINT corner) {
    int radiusX = std::abs(corner.x - center.x);
    int radiusY = std::abs(corner.y - center.y);
    return {
        center.x - radiusX,
        center.y - radiusY,
        center.x + radiusX,
        center.y + radiusY
    };
}
}

PointShape::PointShape(POINT point) : point_(point) {}

void PointShape::Draw(HDC dc) const {
    HBRUSH brush = CreateSolidBrush(Ink);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    Ellipse(dc, point_.x - 3, point_.y - 3, point_.x + 4, point_.y + 4);
    SelectObject(dc, oldBrush);
    DeleteObject(brush);
}

LineShape::LineShape(POINT first, POINT second) : first_(first), second_(second) {}

void LineShape::Draw(HDC dc) const {
    HPEN pen = CreatePen(PS_SOLID, 2, Ink);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    MoveToEx(dc, first_.x, first_.y, nullptr);
    LineTo(dc, second_.x, second_.y);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

RectangleShape::RectangleShape(POINT first, POINT second) : first_(first), second_(second) {}

void RectangleShape::Draw(HDC dc) const {
    RECT rect = CornerRect(first_, second_);
    HPEN pen = CreatePen(PS_SOLID, 2, Ink);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(HOLLOW_BRUSH));
    Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

EllipseShape::EllipseShape(POINT center, POINT corner) : center_(center), corner_(corner) {}

void EllipseShape::Draw(HDC dc) const {
    RECT rect = CenterRect(center_, corner_);
    HPEN pen = CreatePen(PS_SOLID, 2, Ink);
    HBRUSH brush = CreateSolidBrush(EllipseFill);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

void DrawPreview(HDC dc, int type, POINT first, POINT second) {
    HPEN pen = CreatePen(PS_SOLID, 1, Preview);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(HOLLOW_BRUSH));

    if (type == 0) {
        Ellipse(dc, first.x - 3, first.y - 3, first.x + 4, first.y + 4);
    } else if (type == 1) {
        MoveToEx(dc, first.x, first.y, nullptr);
        LineTo(dc, second.x, second.y);
    } else if (type == 2) {
        RECT rect = CornerRect(first, second);
        Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);
    } else {
        RECT rect = CenterRect(first, second);
        Ellipse(dc, rect.left, rect.top, rect.right, rect.bottom);
    }

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

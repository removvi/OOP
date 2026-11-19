#include <windows.h>
#include <windowsx.h>
#include "Shape.h"

constexpr int Capacity = 117;
constexpr int IdFileClear = 1001;
constexpr int IdFileExit = 1002;
constexpr int IdObjectPoint = 1101;
constexpr int IdObjectLine = 1102;
constexpr int IdObjectRectangle = 1103;
constexpr int IdObjectEllipse = 1104;
constexpr int IdHelpAbout = 1201;

Shape* pcshape[Capacity]{};
int shapeCount = 0;
int selectedType = 2;
bool isDragging = false;
POINT startPoint{};
POINT currentPoint{};
const wchar_t* objectNames[] = { L"Крапка", L"Лінія", L"Прямокутник", L"Еліпс" };

Shape* CreateShape(int type, POINT first, POINT second) {
    if (type == 0) return new PointShape(first);
    if (type == 1) return new LineShape(first, second);
    if (type == 2) return new RectangleShape(first, second);
    return new EllipseShape(first, second);
}

void ClearShapes() {
    for (int i = 0; i < shapeCount; ++i) {
        delete pcshape[i];
        pcshape[i] = nullptr;
    }
    shapeCount = 0;
}

void UpdateWindowTitle(HWND hwnd) {
    wchar_t title[160];
    wsprintfW(title, L"Lab2 — графічний редактор — об’єкт: %s — варіант 17", objectNames[selectedType]);
    SetWindowTextW(hwnd, title);
}

HMENU CreateMainMenu() {
    HMENU mainMenu = CreateMenu();
    HMENU fileMenu = CreatePopupMenu();
    HMENU objectMenu = CreatePopupMenu();
    HMENU helpMenu = CreatePopupMenu();

    AppendMenuW(fileMenu, MF_STRING, IdFileClear, L"Очистити полотно");
    AppendMenuW(fileMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(fileMenu, MF_STRING, IdFileExit, L"Вихід");

    AppendMenuW(objectMenu, MF_STRING, IdObjectPoint, L"Крапка");
    AppendMenuW(objectMenu, MF_STRING, IdObjectLine, L"Лінія");
    AppendMenuW(objectMenu, MF_STRING, IdObjectRectangle, L"Прямокутник");
    AppendMenuW(objectMenu, MF_STRING, IdObjectEllipse, L"Еліпс");

    AppendMenuW(helpMenu, MF_STRING, IdHelpAbout, L"Про програму");
    AppendMenuW(mainMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(fileMenu), L"Файл");
    AppendMenuW(mainMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(objectMenu), L"Об’єкти");
    AppendMenuW(mainMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(helpMenu), L"Довідка");
    return mainMenu;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_LBUTTONDOWN:
        startPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        currentPoint = startPoint;
        isDragging = true;
        SetCapture(hwnd);
        return 0;

    case WM_MOUSEMOVE:
        if (isDragging) {
            currentPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;

    case WM_LBUTTONUP:
        if (isDragging) {
            currentPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            if (shapeCount < Capacity) {
                pcshape[shapeCount++] = CreateShape(selectedType, startPoint, currentPoint);
            } else {
                MessageBoxW(hwnd, L"Досягнуто ліміт 117 об’єктів.", L"Lab2", MB_OK | MB_ICONWARNING);
            }
            isDragging = false;
            ReleaseCapture();
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IdFileClear:
            ClearShapes();
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        case IdFileExit:
            DestroyWindow(hwnd);
            return 0;
        case IdObjectPoint:
        case IdObjectLine:
        case IdObjectRectangle:
        case IdObjectEllipse:
            selectedType = LOWORD(wParam) - IdObjectPoint;
            UpdateWindowTitle(hwnd);
            return 0;
        case IdHelpAbout:
            MessageBoxW(hwnd,
                L"Лабораторна робота №2\nГрафічний редактор об’єктів\nВаріант 17",
                L"Про програму", MB_OK | MB_ICONINFORMATION);
            return 0;
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        HDC buffer = CreateCompatibleDC(hdc);
        HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
        HGDIOBJ oldBitmap = SelectObject(buffer, bitmap);
        FillRect(buffer, &clientRect, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
        for (int i = 0; i < shapeCount; ++i) {
            pcshape[i]->Draw(buffer);
        }
        if (isDragging) {
            DrawRubberShape(buffer, selectedType, startPoint, currentPoint);
        }
        BitBlt(hdc, 0, 0, width, height, buffer, 0, 0, SRCCOPY);
        SelectObject(buffer, oldBitmap);
        DeleteObject(bitmap);
        DeleteDC(buffer);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_DESTROY:
        ClearShapes();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int commandShow) {
    const wchar_t className[] = L"Lab2WindowClass";
    WNDCLASSW windowClass{};
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hCursor = LoadCursor(nullptr, IDC_CROSS);
    windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);

    RegisterClassW(&windowClass);
    HWND hwnd = CreateWindowExW(0, className,
        L"Lab2 — графічний редактор — об’єкт: Прямокутник — варіант 17",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 650,
        nullptr, CreateMainMenu(), instance, nullptr);
    if (!hwnd) return 0;

    ShowWindow(hwnd, commandShow);
    UpdateWindow(hwnd);

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return static_cast<int>(message.wParam);
}

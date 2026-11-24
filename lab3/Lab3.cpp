#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>
#include "Shape.h"

constexpr int Capacity = 118;
constexpr int IdClear = 1001;
constexpr int IdExit = 1002;
constexpr int IdAbout = 1003;
constexpr int IdPoint = 1100;
constexpr int IdLine = 1101;
constexpr int IdRectangle = 1102;
constexpr int IdEllipse = 1103;
constexpr int ToolbarId = 2001;

const wchar_t* ObjectNames[] = {
    L"Крапка", L"Лінія", L"Прямокутник", L"Еліпс"
};

const wchar_t* Tooltips[] = {
    L"Малювати крапку", L"Малювати лінію", L"Малювати прямокутник", L"Малювати еліпс"
};

class EditorApp {
public:
    explicit EditorApp(HINSTANCE instance) : instance_(instance), shapes_(new Shape*[Capacity]{}) {}
    ~EditorApp();
    bool Init(int show);

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnCreate(HWND hwnd);
    LRESULT OnCommand(HWND hwnd, WPARAM wParam);
    LRESULT OnNotify(NMHDR* header);
    LRESULT OnInitMenuPopup(HMENU menu);
    LRESULT OnMouseDown(HWND hwnd, LPARAM lParam);
    LRESULT OnMouseMove(HWND hwnd, LPARAM lParam);
    LRESULT OnMouseUp(HWND hwnd, LPARAM lParam);
    LRESULT OnPaint(HWND hwnd);
    LRESULT OnDestroy();
    void CreateToolbar(HWND hwnd);
    HBITMAP CreateToolBitmap(HWND hwnd, int type);
    void SelectTool(int type);
    void ClearShapes();
    Shape* CreateShape(POINT first, POINT second) const;

    HINSTANCE instance_{};
    HWND window_{};
    HWND toolbar_{};
    HMENU objectMenu_{};
    HIMAGELIST images_{};
    Shape** shapes_{};
    int shapeCount_{};
    int selectedType_{2};
    bool dragging_{};
    POINT firstPoint_{};
    POINT currentPoint_{};
};

EditorApp::~EditorApp() {
    ClearShapes();
    if (images_) ImageList_Destroy(images_);
    delete[] shapes_;
}

bool EditorApp::Init(int show) {
    INITCOMMONCONTROLSEX controls{ sizeof(controls), ICC_BAR_CLASSES };
    InitCommonControlsEx(&controls);

    WNDCLASSW windowClass{};
    windowClass.hInstance = instance_;
    windowClass.lpszClassName = L"Lab3EditorWindow";
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hCursor = LoadCursor(nullptr, IDC_CROSS);
    windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    if (!RegisterClassW(&windowClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;

    HMENU mainMenu = CreateMenu();
    HMENU fileMenu = CreatePopupMenu();
    objectMenu_ = CreatePopupMenu();
    HMENU helpMenu = CreatePopupMenu();
    AppendMenuW(fileMenu, MF_STRING, IdClear, L"Очистити полотно");
    AppendMenuW(fileMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(fileMenu, MF_STRING, IdExit, L"Вихід");
    AppendMenuW(objectMenu_, MF_STRING, IdPoint, ObjectNames[0]);
    AppendMenuW(objectMenu_, MF_STRING, IdLine, ObjectNames[1]);
    AppendMenuW(objectMenu_, MF_STRING, IdRectangle, ObjectNames[2]);
    AppendMenuW(objectMenu_, MF_STRING, IdEllipse, ObjectNames[3]);
    AppendMenuW(helpMenu, MF_STRING, IdAbout, L"Про програму");
    AppendMenuW(mainMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(fileMenu), L"Файл");
    AppendMenuW(mainMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(objectMenu_), L"Об’єкти");
    AppendMenuW(mainMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(helpMenu), L"Довідка");

    window_ = CreateWindowExW(0, windowClass.lpszClassName,
        L"Lab3 - графічний редактор - варіант 18",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 960, 680,
        nullptr, mainMenu, instance_, this);
    if (!window_) return false;

    ShowWindow(window_, show);
    UpdateWindow(window_);
    return true;
}

HBITMAP EditorApp::CreateToolBitmap(HWND hwnd, int type) {
    HDC screen = GetDC(hwnd);
    HDC memory = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, 24, 24);
    HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
    COLORREF mask = RGB(255, 0, 255);
    HBRUSH background = CreateSolidBrush(mask);
    RECT rect{ 0, 0, 24, 24 };
    FillRect(memory, &rect, background);
    DeleteObject(background);

    HPEN pen = CreatePen(PS_SOLID, 2, RGB(30, 60, 150));
    HGDIOBJ oldPen = SelectObject(memory, pen);
    HGDIOBJ oldBrush = SelectObject(memory, GetStockObject(HOLLOW_BRUSH));
    if (type == 0) {
        HBRUSH brush = CreateSolidBrush(RGB(30, 60, 150));
        HGDIOBJ previous = SelectObject(memory, brush);
        Ellipse(memory, 9, 9, 16, 16);
        SelectObject(memory, previous);
        DeleteObject(brush);
    } else if (type == 1) {
        MoveToEx(memory, 4, 19, nullptr);
        LineTo(memory, 20, 5);
    } else if (type == 2) {
        Rectangle(memory, 4, 5, 20, 19);
    } else {
        Ellipse(memory, 4, 5, 20, 19);
    }
    SelectObject(memory, oldBrush);
    SelectObject(memory, oldPen);
    DeleteObject(pen);
    SelectObject(memory, oldBitmap);
    DeleteDC(memory);
    ReleaseDC(hwnd, screen);
    return bitmap;
}

void EditorApp::CreateToolbar(HWND hwnd) {
    toolbar_ = CreateWindowExW(0, TOOLBARCLASSNAMEW, nullptr,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_TOP,
        0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ToolbarId), instance_, nullptr);
    SendMessageW(toolbar_, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    images_ = ImageList_Create(24, 24, ILC_COLOR32 | ILC_MASK, 4, 0);
    COLORREF mask = RGB(255, 0, 255);
    for (int type = 0; type < 4; ++type) {
        HBITMAP bitmap = CreateToolBitmap(hwnd, type);
        ImageList_AddMasked(images_, bitmap, mask);
        DeleteObject(bitmap);
    }
    SendMessageW(toolbar_, TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(images_));

    TBBUTTON buttons[4]{};
    for (int type = 0; type < 4; ++type) {
        buttons[type].iBitmap = type;
        buttons[type].idCommand = IdPoint + type;
        buttons[type].fsState = TBSTATE_ENABLED;
        buttons[type].fsStyle = BTNS_CHECKGROUP;
    }
    SendMessageW(toolbar_, TB_ADDBUTTONS, 4, reinterpret_cast<LPARAM>(buttons));
    SendMessageW(toolbar_, TB_CHECKBUTTON, IdRectangle, TRUE);
    SendMessageW(toolbar_, TB_AUTOSIZE, 0, 0);
}

void EditorApp::SelectTool(int type) {
    selectedType_ = (std::max)(0, (std::min)(type, 3));
    SendMessageW(toolbar_, TB_CHECKBUTTON, IdPoint + selectedType_, TRUE);
}

void EditorApp::ClearShapes() {
    for (int index = 0; index < shapeCount_; ++index) {
        delete shapes_[index];
        shapes_[index] = nullptr;
    }
    shapeCount_ = 0;
}

Shape* EditorApp::CreateShape(POINT first, POINT second) const {
    if (selectedType_ == 0) return new PointShape(first);
    if (selectedType_ == 1) return new LineShape(first, second);
    if (selectedType_ == 2) return new RectangleShape(first, second);
    return new EllipseShape(first, second);
}

LRESULT CALLBACK EditorApp::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    EditorApp* app = reinterpret_cast<EditorApp*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        auto create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        app = reinterpret_cast<EditorApp*>(create->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
    }
    return app ? app->HandleMessage(hwnd, message, wParam, lParam) : DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT EditorApp::OnCreate(HWND hwnd) {
    CreateToolbar(hwnd);
    return 0;
}

LRESULT EditorApp::OnCommand(HWND hwnd, WPARAM wParam) {
    int command = LOWORD(wParam);
    if (command >= IdPoint && command <= IdEllipse) {
        SelectTool(command - IdPoint);
        return 0;
    }
    if (command == IdClear) {
        ClearShapes();
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    if (command == IdExit) {
        DestroyWindow(hwnd);
        return 0;
    }
    if (command == IdAbout) {
        MessageBoxW(hwnd, L"Лабораторна робота №3\nГрафічний редактор об’єктів\nВаріант у журналі 17, для завдання 18", L"Про програму", MB_OK | MB_ICONINFORMATION);
        return 0;
    }
    return 0;
}

LRESULT EditorApp::OnNotify(NMHDR* header) {
    if (header->code == TTN_GETDISPINFOW) {
        auto info = reinterpret_cast<NMTTDISPINFOW*>(header);
        int index = static_cast<int>(header->idFrom) - IdPoint;
        if (index >= 0 && index < 4) {
            info->lpszText = const_cast<wchar_t*>(Tooltips[index]);
        }
    }
    return 0;
}

LRESULT EditorApp::OnInitMenuPopup(HMENU menu) {
    if (menu == objectMenu_) {
        CheckMenuRadioItem(objectMenu_, IdPoint, IdEllipse, IdPoint + selectedType_, MF_BYCOMMAND);
    }
    return 0;
}

LRESULT EditorApp::OnMouseDown(HWND hwnd, LPARAM lParam) {
    firstPoint_ = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    currentPoint_ = firstPoint_;
    dragging_ = true;
    SetCapture(hwnd);
    return 0;
}

LRESULT EditorApp::OnMouseMove(HWND hwnd, LPARAM lParam) {
    if (dragging_) {
        currentPoint_ = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        InvalidateRect(hwnd, nullptr, FALSE);
    }
    return 0;
}

LRESULT EditorApp::OnMouseUp(HWND hwnd, LPARAM lParam) {
    if (!dragging_) return 0;
    currentPoint_ = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    if (shapeCount_ < Capacity) {
        shapes_[shapeCount_++] = CreateShape(firstPoint_, currentPoint_);
    } else {
        MessageBoxW(hwnd, L"Досягнуто ліміт 118 об’єктів.", L"Lab3", MB_OK | MB_ICONWARNING);
    }
    dragging_ = false;
    ReleaseCapture();
    InvalidateRect(hwnd, nullptr, FALSE);
    return 0;
}

LRESULT EditorApp::OnPaint(HWND hwnd) {
    PAINTSTRUCT paint{};
    HDC dc = BeginPaint(hwnd, &paint);
    RECT client{};
    GetClientRect(hwnd, &client);
    int width = client.right - client.left;
    int height = client.bottom - client.top;
    HDC buffer = CreateCompatibleDC(dc);
    HBITMAP bitmap = CreateCompatibleBitmap(dc, width, height);
    HGDIOBJ oldBitmap = SelectObject(buffer, bitmap);
    FillRect(buffer, &client, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
    for (int index = 0; index < shapeCount_; ++index) {
        shapes_[index]->Draw(buffer);
    }
    if (dragging_) DrawPreview(buffer, selectedType_, firstPoint_, currentPoint_);
    BitBlt(dc, 0, 0, width, height, buffer, 0, 0, SRCCOPY);
    SelectObject(buffer, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(buffer);
    EndPaint(hwnd, &paint);
    return 0;
}

LRESULT EditorApp::OnDestroy() {
    ClearShapes();
    if (images_) {
        ImageList_Destroy(images_);
        images_ = nullptr;
    }
    PostQuitMessage(0);
    return 0;
}

LRESULT EditorApp::HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_CREATE) return OnCreate(hwnd);
    if (message == WM_COMMAND) return OnCommand(hwnd, wParam);
    if (message == WM_NOTIFY) return OnNotify(reinterpret_cast<NMHDR*>(lParam));
    if (message == WM_INITMENUPOPUP) return OnInitMenuPopup(reinterpret_cast<HMENU>(wParam));
    if (message == WM_LBUTTONDOWN) return OnMouseDown(hwnd, lParam);
    if (message == WM_MOUSEMOVE) return OnMouseMove(hwnd, lParam);
    if (message == WM_LBUTTONUP) return OnMouseUp(hwnd, lParam);
    if (message == WM_PAINT) return OnPaint(hwnd);
    if (message == WM_ERASEBKGND) return 1;
    if (message == WM_DESTROY) return OnDestroy();
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    EditorApp app(instance);
    if (!app.Init(show)) return 1;
    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return static_cast<int>(message.wParam);
}

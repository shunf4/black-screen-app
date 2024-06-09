//
// Created by spoil on 04/14/2024.
//

#include "WindowInitiator.hpp"

#include <algorithm>
#include <regex>
#include <utility>

#include "ColorHandler.hpp"

LRESULT CALLBACK HandleWindowMessages(HWND windowHandle, UINT messageType, WPARAM windowParameterValue, LPARAM messageData);

std::string WindowInitiator::m_color = "black";
bool WindowInitiator::disableKeyExit = false;

WindowInitiator::WindowInitiator(std::string color, const bool& disableKeyExit, int x, int y, int w, int h, int opacity) {
    WindowInitiator::disableKeyExit = disableKeyExit;

    std::ranges::transform(color, color.begin(),
                           [](const unsigned char stringCharacter) { return std::tolower(stringCharacter); });

    m_color = color == "black" ? std::move(color) :
              ColorHandler::colorMap.contains(color) ? ColorHandler::colorMap[color] : std::move(color);

    this->x = x;
    this->y = y;
    this->opacity = opacity;
    this->w = w;
    this->h = h;
    if (this->x == -1) { this->x = CW_USEDEFAULT; }
    if (this->y == -1) { if (this->x == CW_USEDEFAULT) { this->y = CW_USEDEFAULT; } else { this->y = 0; } }
}

void WindowInitiator::createWindow() {
    const WNDCLASS windowClass = {
        .lpfnWndProc = HandleWindowMessages,
        .hInstance = GetModuleHandle(nullptr),
        .lpszClassName = "BlackWindowClass"
    };
    RegisterClass(&windowClass);

    const auto windowHandle = CreateWindowEx(
        0,
        "BlackWindowClass",
        "Black Screen Application",
        // WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
        WS_POPUP | WS_VISIBLE | WS_SYSMENU,
        // WS_EX_APPWINDOW | WS_SYSMENU,
        x,
        y,
        w == -1 ? GetSystemMetrics(SM_CXSCREEN) : w,
        h == -1 ? GetSystemMetrics(SM_CYSCREEN) : h,
        nullptr,
        HMENU(),
        GetModuleHandle(nullptr),
        nullptr
    );

    LONG cur_style = GetWindowLong(windowHandle, GWL_EXSTYLE);
    SetWindowLong(windowHandle, GWL_EXSTYLE, cur_style | WS_EX_COMPOSITED | WS_EX_TRANSPARENT | WS_EX_LAYERED);
    SetWindowPos(windowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    SetLayeredWindowAttributes(windowHandle, 0, (unsigned char)(unsigned)this->opacity, LWA_ALPHA);

    ShowCursor(false);

    ShowWindow(windowHandle, SW_SHOW);
    UpdateWindow(windowHandle);

    MSG message;
    while (GetMessage(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT) {
            break;
        }
    }

    UnregisterClass("BlackWindowClass", GetModuleHandle(nullptr));
}

LRESULT CALLBACK HandleWindowMessages(const HWND windowHandle, const UINT messageType, const WPARAM windowParameterValue, const LPARAM messageData) { // NOLINT(*-misplaced-const)
    // if (messageType != 28 && messageType != 131 && messageType != 512 && messageType != 129 && messageType != 132 && messageType != 134 && messageType != 6 && messageType != 32 && messageType != 15) {
    // char a[1000];
    // _snprintf(a, 1000, "%d", messageType);
    // MessageBox(NULL, a, a, MB_OK);
    // }

    switch (messageType) {
        case WM_ENTERMENULOOP:
            ShowCursor(true);
            return 0;
        case WM_EXITMENULOOP:
            ShowCursor(false);
            return 0;
        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN: {
            if (!WindowInitiator::disableKeyExit) {
                PostQuitMessage(0);
            }
            return 0;
        }
        case WM_ERASEBKGND: {
            RECT clientRect;
            GetClientRect(windowHandle, &clientRect);
            HBRUSH colorBrush;

            if (WindowInitiator::m_color == "black") {
                colorBrush = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
            } else {
                if (std::regex_match(WindowInitiator::m_color, std::regex("^#([a-fA-F0-9]{6}|[a-fA-F0-9]{3})$"))) {
                    auto [red, green, blue] = ColorHandler::convertHextoRGB(WindowInitiator::m_color);
                    colorBrush = CreateSolidBrush(RGB(red, green, blue));
                } else {
                    const auto errorMessage = "Invalid color argument. Expected a hex color code or color name.";
                    MessageBox(nullptr, errorMessage, "Error - Black Screen Application", MB_ICONERROR | MB_OK);
                    throw std::invalid_argument(errorMessage);
                }
            }

            FillRect(reinterpret_cast<HDC>(windowParameterValue), &clientRect, colorBrush);

            if (WindowInitiator::m_color != "black") {
                DeleteObject(colorBrush);
            }

            return 1;
        }
        default:
            return DefWindowProc(windowHandle, messageType, windowParameterValue, messageData);
    }
}



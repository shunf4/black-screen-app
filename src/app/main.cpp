#include <iostream>
#include <map>

#include "WindowInitiator.hpp"

HHOOK g_hHook = NULL;
HHOOK g_hHookM = NULL;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && wParam == WM_SYSKEYDOWN)
    {
        KBDLLHOOKSTRUCT* pKeyboardHookStruct = (KBDLLHOOKSTRUCT*)lParam;

        if (pKeyboardHookStruct->vkCode == VK_F4 && (GetAsyncKeyState(VK_MENU) & 0x8000)
            || (pKeyboardHookStruct->vkCode == VK_TAB && (GetAsyncKeyState(VK_MENU) & 0x8000))
        )
        {
            return 1;
        }
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Lower down scroll speed
    static int x = 0;
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT* pMouseHookStruct = (MSLLHOOKSTRUCT*)lParam;

        if (wParam == WM_MOUSEWHEEL)
        {
            x = (x + 1) % 2;
            if (x == 1) {
                return CallNextHookEx(g_hHookM, nCode, wParam, lParam);
            } else {
                return 1;
            }
        }
    }

    // Call the next hook procedure
    return CallNextHookEx(g_hHookM, nCode, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // This, unfortunately, hooks ALL windows.
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    g_hHookM = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hInstance, 0);


    std::string backgroundColor = "black";
    std::string x = "-1";
    std::string y = "-1";
    std::string w = "-1";
    std::string h = "-1";
    std::string opacity = "255";
    auto shouldExitOnKeyPress = false;

    std::map<std::string, std::string*> argMap = {
        {"-c", &backgroundColor},
        {"--color", &backgroundColor},
        {"-x", &x},
        {"-y", &y},
        {"-w", &w},
        {"-h", &h},
        {"-o", &opacity},
        {"-dke", reinterpret_cast<std::string*>(&shouldExitOnKeyPress)},
        {"--disable-key-exit", reinterpret_cast<std::string*>(&shouldExitOnKeyPress)}
    };

    int argc;
    const auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    std::wstring argv1;
    if (argc <= 1 || (argv1 = argv[1] , (argv1 == L"-h" || argv1 == L"--help" || argv1 == L"/?"))) {
        MessageBox(NULL, "-dke/--disable-key-exit; -c/--color; -x pixel (negative possible; -1 default); -y pixel; -w pixel; -h pixel; -o opacity (0~255)", "BlackScreenApp Usage", MB_OK);
        goto usageExit;
    }

    for (auto i = 1; i < argc; ++i) {
        std::wstring currentArgW = argv[i];

        if (std::string currentArg(currentArgW.begin(), currentArgW.end()); argMap.contains(currentArg)) {
            if (currentArg == "-dke" || currentArg == "--disable-key-exit") {
                shouldExitOnKeyPress = true;
            } else {
                std::wstring nextArgW = argv[++i];
                *argMap[currentArg] = std::string(nextArgW.begin(), nextArgW.end());
            }
        }
    }

    LocalFree(argv);

    {
        // accurate pixels
        SetProcessDPIAware();
        WindowInitiator windowInitiator(backgroundColor, shouldExitOnKeyPress, stoi(x), stoi(y), stoi(w), stoi(h), stoi(opacity));
        windowInitiator.createWindow();
        UnhookWindowsHookEx(g_hHook);
        UnhookWindowsHookEx(g_hHookM);
        return 0;
    }

usageExit:
    LocalFree(argv);
    UnhookWindowsHookEx(g_hHook);
    UnhookWindowsHookEx(g_hHookM);
    return 0;
}

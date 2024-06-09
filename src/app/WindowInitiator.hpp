//
// Created by spoil on 04/14/2024.
//

#ifndef WINDOWINITIATOR_HPP
#define WINDOWINITIATOR_HPP

#include <string>
#include <windows.h>

class WindowInitiator {
public:
    static std::string m_color;
    static bool disableKeyExit;

    int x;
    int y;
    int w;
    int h;
    int opacity;

    explicit WindowInitiator(std::string color, const bool& disableKeyExit, int x, int y, int w, int h, int opacity);
    void createWindow();
};


#endif //WINDOWINITIATOR_HPP

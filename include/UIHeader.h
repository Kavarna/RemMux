#pragma once


#include "UIText.h"
#include "Common.h"

class UIHeader
{
public:
    UIHeader(int rows = 24, int cols = 80);
    ~UIHeader();

public:
    void render();

private:
    std::unique_ptr<UIText>         m_framerate;
    std::unique_ptr<UIText>         m_username;

    std::unique_ptr<UIText>         m_time;

    WINDOW*                         m_window;
};


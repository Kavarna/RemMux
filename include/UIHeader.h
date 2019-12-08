#pragma once


#include "UIText.h"
#include "Common.h"

class UIHeader
{
public:
    UIHeader(int rows = 24, int cols = 80, int instances = 9);
    ~UIHeader();

public:
    void render(unsigned int);
    void resize(int rows, int cols);

    void setActiveInstance(int index);

private:
    UIText                          m_framerate;
    UIText                          m_username;
    UIText                          m_time;

    std::vector<UIText>             m_instancesText;
    int                             m_activeInstance;

    WINDOW*                         m_window;
};


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

    void activateInstance(int index);
    void deleteInstance(int index);

private:
    void updateInstancesText(uint32_t, uint32_t);

private:
    UIText                          m_framerate;
    UIText                          m_username;
    UIText                          m_time;

    std::map<int, UIText>           m_instancesText;
    int                             m_activeInstance;    

    uint32_t                        m_rows;
    uint32_t                        m_cols;

    WINDOW*                         m_window;
};


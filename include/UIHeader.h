#pragma once


#include "IResizeable.h"

#include "UIText.h"
#include "Common.h"

class UIHeader : public IResizeable
{
public:
    UIHeader(int rows = 24, int cols = 80);
    ~UIHeader();

public:
    void render();

    virtual void resize(uint32_t rows, uint32_t cols) override;

private:
    std::unique_ptr<UIText>         m_framerate;
    std::unique_ptr<UIText>         m_username;

    std::unique_ptr<UIText>         m_clock;
    std::unique_ptr<UIText>         m_date;

    WINDOW*                         m_window;
};


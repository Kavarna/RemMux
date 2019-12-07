#include "UIHeader.h"


UIHeader::UIHeader(int rows, int cols)
{
    m_window = newwin(rows, cols - 1, 0, 0);

    m_framerate = std::make_unique<UIText>("60", Position{ 1,1 }, 2);

    char username[MAX_USERNAME_LENGTH];
    EVALUATE(getlogin_r(username, sizeof(username)), NULL, !=,
        "Unable to get username.");

    m_username = std::make_unique<UIText>(std::string(username),
            Position{ 1,3 }, MAX_USERNAME_LENGTH);
}

UIHeader::~UIHeader()
{
}

void UIHeader::render()
{
    box(m_window, 0, 0);

    wattr_on(m_window, COLOR_PAIR(COLOR_RED_BLACK_BACKGROUND),nullptr);
    m_framerate->render(m_window);
    wattr_off(m_window, COLOR_PAIR(COLOR_RED_BLACK_BACKGROUND),nullptr);
    
    wattr_on(m_window, COLOR_PAIR(COLOR_BLUE_BLACK_BACKGROUND),nullptr);
    m_username->render(m_window);
    wattr_off(m_window, COLOR_PAIR(COLOR_BLUE_BLACK_BACKGROUND),nullptr);

    wrefresh(m_window);
}

void UIHeader::resize(uint32_t rows, uint32_t cols)
{
    wclear(m_window);
    wresize(m_window, rows, cols);
}


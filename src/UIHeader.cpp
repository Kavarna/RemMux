#include "UIHeader.h"


std::string getCurrentTimeAsString()
{
    time_t rawtime;
    struct tm* timeinfo;
    char timeBuffer[MAX_DATE_LENGTH];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timeBuffer, 80, "%I:%M:%S %p %d/%m/%y", timeinfo);
    return timeBuffer;
}


UIHeader::UIHeader(int rows, int cols)
{
    m_framerate = std::make_unique<UIText>("60", Position{ 1,1 }, 2);

    char username[MAX_USERNAME_LENGTH];
    EVALUATE(getlogin_r(username, sizeof(username)), NULL, !=,
        "Unable to get username.");

    m_username = std::make_unique<UIText>(std::string(username),
            Position{ 1,3 }, MAX_USERNAME_LENGTH);



    m_time = std::make_unique<UIText>(getCurrentTimeAsString(),
            Position{1, cols - MAX_DATE_LENGTH - 1}, MAX_DATE_LENGTH);

    m_window = newwin(rows, cols - 1, 0, 0);
    
    wclear(m_window);
    wresize(m_window, rows, cols);
}

UIHeader::~UIHeader()
{
    if (m_window)
    {
        delwin(m_window);
    }
}

void UIHeader::render()
{
    // Update stuff
    m_time->setText(getCurrentTimeAsString());


    // Render stuff
    box(m_window, 0, 0);

    wattr_on(m_window, COLOR_PAIR(COLOR_RED_BLACK_BACKGROUND),nullptr);
    m_framerate->render(m_window);
    wattr_off(m_window, COLOR_PAIR(COLOR_RED_BLACK_BACKGROUND),nullptr);
    
    wattr_on(m_window, COLOR_PAIR(COLOR_BLUE_BLACK_BACKGROUND),nullptr);
    m_username->render(m_window);
    wattr_off(m_window, COLOR_PAIR(COLOR_BLUE_BLACK_BACKGROUND),nullptr);

    wattr_on(m_window, COLOR_PAIR(COLOR_GREEN_BLACK_BACKGROUND), nullptr);
    m_time->render(m_window);
    wattr_off(m_window, COLOR_PAIR(COLOR_GREEN_BLACK_BACKGROUND),nullptr);

    wrefresh(m_window);
}

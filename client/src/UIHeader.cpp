#include "UIHeader.h"
#include "Logger.h"


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


UIHeader::UIHeader(int rows, int cols, int instances)
{
    m_window = newwin(rows, cols - 1, 0, 0);
    resize(rows, cols);
}

UIHeader::~UIHeader()
{
    if (m_window)
    {
        delwin(m_window);
    }
}

void UIHeader::render(unsigned int framerate, int activeInstance)
{
    // Update stuff
    m_time.setText(getCurrentTimeAsString());
    
    if (framerate > 99)
    {
        framerate = 99;
    }
    m_framerate.setText(std::to_string(framerate));


    // Render stuff
    box(m_window, 0, 0);

    wattr_on(m_window, COLOR_PAIR(COLOR_RED_BLACK_BACKGROUND),nullptr);
    m_framerate.render(m_window);
    wattr_off(m_window, COLOR_PAIR(COLOR_RED_BLACK_BACKGROUND),nullptr);
    
    wattr_on(m_window, COLOR_PAIR(COLOR_BLUE_BLACK_BACKGROUND),nullptr);
    m_username.render(m_window);
    wattr_off(m_window, COLOR_PAIR(COLOR_BLUE_BLACK_BACKGROUND),nullptr);

    wattr_on(m_window, COLOR_PAIR(COLOR_GREEN_BLACK_BACKGROUND), nullptr);
    m_time.render(m_window);
    wattr_off(m_window, COLOR_PAIR(COLOR_GREEN_BLACK_BACKGROUND),nullptr);

    for (auto element : m_instancesText)
    {
        if (element.first == activeInstance)
        {
            wattr_on(m_window, COLOR_PAIR(COLOR_BLACK_LTGRAY_BACKGROUND), nullptr);
            element.second.render(m_window);
            wattr_off(m_window, COLOR_PAIR(COLOR_BLACK_LTGRAY_BACKGROUND), nullptr);
        }
        else
        {
            wattr_on(m_window, COLOR_PAIR(COLOR_BLACK_GRAY_BACKGROUND), nullptr);
            element.second.render(m_window);
            wattr_off(m_window, COLOR_PAIR(COLOR_BLACK_GRAY_BACKGROUND), nullptr);
        }
    }


    wrefresh(m_window);
}

void UIHeader::resize(int rows, int cols)
{
    m_rows = rows; m_cols = cols;

    m_framerate.setPosition({1,1});
    m_framerate.setMaxLength(MAX_FRAMERATE_LENGTH);

    char username[MAX_USERNAME_LENGTH];
    EVALUATE(getlogin_r(username, sizeof(username)), NULL, !=,
        "Unable to get username.");
    m_username.setMaxLength(MAX_USERNAME_LENGTH);
    m_username.setText(username);
    m_username.setPosition({1,3});
 
    m_time.setMaxLength(MAX_DATE_LENGTH);
    m_time.setText(getCurrentTimeAsString());
    m_time.setPosition({1, cols - MAX_DATE_LENGTH - 1});


    updateInstancesText(rows, cols);

    wclear(m_window);
    wresize(m_window, rows, cols);
}

void UIHeader::addInstance(int index)
{
    if (m_instancesText.find(index) == m_instancesText.end())
    {
        m_instancesText.insert({index, UIText{}});
    }
    updateInstancesText(m_rows, m_cols);
}

void UIHeader::deleteInstance(int index)
{
    m_instancesText.erase(index);
    updateInstancesText(m_rows, m_cols);
    wclear(m_window);
}

void UIHeader::updateInstancesText(uint32_t rows, uint32_t cols)
{
    int width = m_instancesText.size() * MAX_INSTANCE_NAME;
    int halfWidth = (int)std::floor(width / 2);
    int halfCols = (int)std::floor(cols / 2);

    unsigned int startcol = std::max(MAX_USERNAME_LENGTH + 4, halfCols - halfWidth);

    uint32_t index = 0;
    for (auto& element : m_instancesText)
    {
        element.second.setMaxLength(MAX_INSTANCE_NAME);
        element.second.setText(std::to_string(element.first) + ": bash");
        element.second.setPosition({1, (int)(startcol + MAX_INSTANCE_NAME * index)});
        index++;
    }
}

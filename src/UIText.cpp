#include "UIText.h"


UIText::UIText()
{
    create("", {0, 0}, 0);
}

UIText::UIText(const std::string& text, const Position& position, uint32_t maxLength)
{
    create(text, position, maxLength);
}

UIText::~UIText()
{
}

void UIText::create(const std::string& text, const Position& position, uint32_t maxLength)
{
    m_shouldRender  = true;
    m_textPosition  = position;
    m_text          = text;
    m_maxLength     = maxLength;
}

void UIText::render(WINDOW* win)
{
    mvwprintw(win, m_textPosition.row, m_textPosition.col, "%s", m_text.c_str());
    m_shouldRender  = false;
}

bool UIText::setText(std::string& text)
{
    bool result = true;
    if (text.size() > m_maxLength)
    {
        text.erase(text.begin() + m_maxLength, text.end());
        result = false;
    }
    m_text          = text;
    m_shouldRender  = true;
    return result;
}

const std::string& UIText::getText() const
{
    return m_text;
}

void UIText::setMaxLength(uint32_t maxLength)
{
    m_maxLength = maxLength;
}

uint32_t UIText::getMaxLength() const
{
    return m_maxLength;
}

void UIText::setPosition(const Position& position)
{
    m_textPosition = position;
    m_shouldRender = true;
}

const Position& UIText::getPosition() const
{
    return m_textPosition;
}

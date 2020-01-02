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
    m_textPosition  = position;
    m_text          = text;
    m_maxLength     = maxLength;
}

void UIText::render(WINDOW* win) const 
{
    mvwprintw(win, m_textPosition.row, m_textPosition.col, "%s", m_text.c_str());
}

void UIText::setText(const std::string& text)
{
    bool result = true;
    m_text = text;
    if (m_text.size() > m_maxLength)
    {
        m_text.erase(m_text.begin() + m_maxLength, m_text.end());
        result = false;
    }
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
}

const Position& UIText::getPosition() const
{
    return m_textPosition;
}

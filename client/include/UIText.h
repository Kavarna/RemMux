#pragma once


#include "UIComponent.h"
#include "Common.h"

class UIText : public UIComponent
{
public:
    UIText();
    UIText(const std::string& text, const Position& position, uint32_t maxLength);
    ~UIText();

public:
    virtual void render(WINDOW*) const override;

public:
    void setText(const std::string& text);
    const std::string& getText() const;
    void setMaxLength(uint32_t maxLength);
    uint32_t getMaxLength() const;
    void setPosition(const Position& position);
    const Position& getPosition() const;

private:
    void create(const std::string& text, const Position& position, uint32_t maxLength);

private:
    Position    m_textPosition;
    uint32_t    m_maxLength;
    std::string m_text;
};



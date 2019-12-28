#pragma once


#include "Common.h"


class Instance 
{

public:
    Instance(const Position& position,
             const Size& size,
             uint32_t rows, uint32_t cols);
    ~Instance();

public:
    void setLimits(std::shared_ptr<Instance> up,
                   std::shared_ptr<Instance> right,
                   std::shared_ptr<Instance> down,
                   std::shared_ptr<Instance> left);
    void render();

    void setFixed(bool fixed);

    void resize(uint32_t rows, uint32_t cols);

    void setShouldRender(bool shouldRender);

    void setPosition(const Position& pos);
    void setSize(const Size& size);

private:
    WINDOW* m_window;

    bool m_active;
    bool m_shouldRender;
    bool m_fixed;

    Size m_currentTerminalSize;

    Position m_currentInstancePosition;
    Size m_currentInstanceSize;

    std::shared_ptr<Instance> m_leftInstance = nullptr;
    std::shared_ptr<Instance> m_rightInstance = nullptr;
    std::shared_ptr<Instance> m_aboveInstance = nullptr;
    std::shared_ptr<Instance> m_belowInstance = nullptr;

};




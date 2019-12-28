#include "Instance.h"
#include "Logger.h"


Instance::Instance(const Position& position,
                   const Size& size,
                   uint32_t rows, uint32_t cols):
    m_currentInstancePosition(position),
    m_currentInstanceSize(size),
    m_currentTerminalSize(Size{(int)rows, (int)cols})
{
    m_window = newwin(size.rows, size.cols,
                position.row, position.col);
    m_active = true;
    m_shouldRender = true;
    m_fixed = false;
    resize(rows, cols);
}

Instance::~Instance()
{
    if (m_window)
    {
        delwin(m_window);
    }
}


void Instance::render()
{
    if (!m_shouldRender)
    {
        return;
    }
    if (m_active)
    {
        wattr_on(m_window, COLOR_PAIR(COLOR_GREEN_BLACK_BACKGROUND), nullptr);
        box(m_window, 0, 0);
        wattr_off(m_window, COLOR_PAIR(COLOR_GREEN_BLACK_BACKGROUND), nullptr);
    }
    else
    {
        box(m_window, 0, 0);
    }

    wrefresh(m_window);
}

void Instance::resize(uint32_t rows, uint32_t cols)
{
    m_currentTerminalSize = Size{(int)rows, (int)cols};
    if (m_fixed)
    {
        return;
    }
    if (m_leftInstance)
    {
        m_leftInstance->resize(rows, cols);
    }
    if (m_aboveInstance)
    {
        m_aboveInstance->resize(rows, cols);
    }

    auto convertFromOldToNew = [](int currentSize, int currentPoint, int newSize)-> int
    {
        return (newSize * currentPoint) / currentSize;
    };
    Logger::log("Resizing window from position = (",
        m_currentInstancePosition.row, ", ", m_currentInstancePosition.col,
        "); and size = (", m_currentInstanceSize.rows, ", " , m_currentInstanceSize.cols,
        "); to ");

    if (m_aboveInstance)
    {
        m_currentInstancePosition.row = m_aboveInstance->m_currentInstancePosition.row + 
                m_aboveInstance->m_currentInstanceSize.rows + 1;
    }
    else
    {
        m_currentInstancePosition.row = convertFromOldToNew(
                    m_currentTerminalSize.rows, m_currentInstancePosition.row, rows);
    }
    m_currentInstanceSize.rows = convertFromOldToNew(
                m_currentTerminalSize.rows, m_currentTerminalSize.rows, rows);

    if (m_leftInstance)
    {
        m_currentInstancePosition.col = m_aboveInstance->m_currentInstancePosition.col + 
                m_aboveInstance->m_currentInstanceSize.cols + 1;
    }
    else
    {
        m_currentInstancePosition.col = convertFromOldToNew(
                m_currentTerminalSize.cols, m_currentInstancePosition.col, cols);
    }

    m_currentInstanceSize.cols = convertFromOldToNew(
            m_currentTerminalSize.cols, m_currentTerminalSize.cols, cols);

    Logger::log("position = (",
        m_currentInstancePosition.row, ", ", m_currentInstancePosition.col,
        "); and size = (", m_currentInstanceSize.rows, ", " , m_currentInstanceSize.cols,
        "); \n");

    m_currentTerminalSize = { (int)rows, (int)cols };

    mvwin(m_window, m_currentInstancePosition.row, m_currentInstancePosition.col);
    wresize(m_window, m_currentInstanceSize.rows, m_currentInstanceSize.cols);
    wclear(m_window);
    wrefresh(m_window);

    if (m_rightInstance)
    {
        m_rightInstance->resize(rows, cols);
    }
    if (m_belowInstance)
    {
        m_belowInstance->resize(rows, cols);
    }
}

void Instance::setLimits(std::shared_ptr<Instance> up,
                         std::shared_ptr<Instance> right,
                         std::shared_ptr<Instance> down,
                         std::shared_ptr<Instance> left)
{
    m_aboveInstance = std::move(up);
    m_aboveInstance->m_belowInstance = std::make_shared<Instance>(*this);
    m_rightInstance = std::move(right);
    m_rightInstance->m_leftInstance = std::make_shared<Instance>(*this);    
    m_belowInstance = std::move(down);
    m_belowInstance->m_aboveInstance = std::make_shared<Instance>(*this);
    m_leftInstance = std::move(left);
    m_leftInstance->m_rightInstance = std::make_shared<Instance>(*this);
}

void Instance::setShouldRender(bool shouldRender)
{
    m_shouldRender = shouldRender;
}

void Instance::setFixed(bool fixed)
{
    m_fixed = fixed;
}

void Instance::setSize(const Size& size)
{
    m_currentInstanceSize = size;
}

void Instance::setPosition(const Position& pos)
{
    m_currentInstancePosition = pos;
}

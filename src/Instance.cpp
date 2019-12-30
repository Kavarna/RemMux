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
    m_startedRendering = false;
    m_startedResizing = false;
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
    m_startedRendering = true;

    if (m_aboveInstance && !m_aboveInstance->m_startedRendering)
    {
        m_aboveInstance->render();
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

    if (m_belowInstance && !m_belowInstance->m_startedRendering)
    {
        m_belowInstance->render();
    }

    m_startedRendering = false;
}

void Instance::resize(uint32_t rows, uint32_t cols)
{
    if (m_fixed)
    {
        m_currentTerminalSize = Size{(int)rows, (int)cols};
        return;
    }

    m_startedResizing = true;
    if (m_leftInstance)
    {
        if (!m_leftInstance->m_startedResizing)
        {
            m_leftInstance->resize(rows, cols);
        }
    }
    if (m_aboveInstance)
    {
        if (!m_aboveInstance->m_startedResizing)
        {
            m_aboveInstance->resize(rows, cols);
        }
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
                m_aboveInstance->m_currentInstanceSize.rows;
    }
    else
    {
        m_currentInstancePosition.row = convertFromOldToNew(
                    m_currentTerminalSize.rows, m_currentInstancePosition.row, rows);
    }
    m_currentInstanceSize.rows = m_rowsPercentage * rows;

    if (m_leftInstance)
    {
        m_currentInstancePosition.col = m_leftInstance->m_currentInstancePosition.col + 
                m_leftInstance->m_currentInstanceSize.cols;
    }
    else
    {
        m_currentInstancePosition.col = convertFromOldToNew(
                m_currentTerminalSize.cols, m_currentInstancePosition.col, cols);
    }

    m_currentInstanceSize.cols = m_colsPercentage * cols;

    Logger::log("position = (",
        m_currentInstancePosition.row, ", ", m_currentInstancePosition.col,
        "); and size = (", m_currentInstanceSize.rows, ", " , m_currentInstanceSize.cols,
        "); \n");

    updateWindow();

    if (m_rightInstance)
    {
        if (!m_rightInstance->m_startedResizing)
        {
            m_rightInstance->resize(rows, cols);
        }
    }
    if (m_belowInstance)
    {
        if (!m_belowInstance->m_startedResizing)
        {
            m_belowInstance->resize(rows, cols);
        }
    }
    m_currentTerminalSize = Size{(int)rows, (int)cols};
    m_startedResizing = false;
}

void Instance::setLimits(std::shared_ptr<Instance> up,
                         std::shared_ptr<Instance> right,
                         std::shared_ptr<Instance> down,
                         std::shared_ptr<Instance> left)
{
    m_aboveInstance = (up);
    m_aboveInstance->m_belowInstance = shared_from_this();
    m_rightInstance = (right);
    m_rightInstance->m_leftInstance = shared_from_this();
    m_belowInstance = (down);
    m_belowInstance->m_aboveInstance = shared_from_this();
    m_leftInstance = (left);
    m_leftInstance->m_rightInstance = shared_from_this();
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

std::shared_ptr<Instance> Instance::splitHorizontally()
{
    int initialRows = m_currentInstanceSize.rows;
    m_currentInstanceSize.rows = m_currentInstanceSize.rows / 2;
    m_rowsPercentage *= 0.5f;

    updateWindow();

    int newWindowRows = initialRows - m_currentInstanceSize.rows;
    int newWindowCols = m_currentInstanceSize.cols;
    int newWindowPositionRow = m_currentInstancePosition.row + m_currentInstanceSize.rows;
    int newWindowPositionCol = m_currentInstancePosition.col;
    std::shared_ptr<Instance> newInstance = std::make_shared<Instance>(
            Position{newWindowPositionRow, newWindowPositionCol},
            Size{newWindowRows, newWindowCols},
            m_currentTerminalSize.rows,
            m_currentTerminalSize.cols
            );
    newInstance->m_rowsPercentage = m_rowsPercentage;

    newInstance->setLimits(
        shared_from_this(),
        m_rightInstance,
        m_belowInstance,
        m_leftInstance
    );

    m_belowInstance = newInstance;
    
    newInstance->resize(m_currentTerminalSize.rows, m_currentTerminalSize.cols);

    return newInstance;
}

void Instance::updateWindow()
{
    mvwin(m_window, m_currentInstancePosition.row, m_currentInstancePosition.col);
    wresize(m_window, m_currentInstanceSize.rows, m_currentInstanceSize.cols);
    wclear(m_window);
    wrefresh(m_window);
}

void Instance::setActive(bool active)
{
    m_active = active;
}

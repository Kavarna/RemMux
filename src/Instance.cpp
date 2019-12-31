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
    if (m_leftInstance && !m_leftInstance->m_startedRendering)
    {
        m_leftInstance->render();
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
    if (m_rightInstance && !m_rightInstance->m_startedRendering)
    {
        m_rightInstance->render();
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
    if (m_aboveInstance)
    {
        rawUpdateLimit(m_aboveInstance, up);
    }
    else
    {
        m_aboveInstance = up;
    }
    if (up)
    {
        up->rawUpdateLimit(m_aboveInstance->m_belowInstance, shared_from_this());
    }


    if (m_rightInstance)
    {
        rawUpdateLimit(m_rightInstance, right);
    }
    else
    {
        m_rightInstance = right;
    }
    if (right)
    {
        right->rawUpdateLimit(m_rightInstance->m_leftInstance, shared_from_this());
    }


    if (m_belowInstance)
    {
        rawUpdateLimit(m_belowInstance, down);
    }
    else
    {
        m_belowInstance = down;
    }
    if (down)
    {
        down->rawUpdateLimit(m_aboveInstance->m_belowInstance, shared_from_this());
    }


    if (m_leftInstance)
    {
        rawUpdateLimit(m_leftInstance, left);
    }
    else
    {
        m_leftInstance = left;
    }
    if (left)
    {
        left->rawUpdateLimit(m_leftInstance->m_rightInstance, shared_from_this());
    }
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
    newInstance->m_colsPercentage = m_colsPercentage;

    newInstance->setLimits(
        shared_from_this(),
        m_rightInstance,
        m_belowInstance,
        m_leftInstance
    ); 

    newInstance->m_parent = shared_from_this();
    newInstance->m_splitType = Instance::SplitType::Horizontal;
    m_children.push_back(newInstance);


    rawUpdateLimit(m_belowInstance, newInstance);
    newInstance->m_belowInstance->updateAboveLimit(shared_from_this(), newInstance);
    newInstance->m_aboveInstance = shared_from_this();
    
    newInstance->resize(m_currentTerminalSize.rows, m_currentTerminalSize.cols);

    return newInstance;
}

std::shared_ptr<Instance> Instance::splitVertically()
{
    int initialCols = m_currentInstanceSize.cols;
    m_currentInstanceSize.cols = m_currentInstanceSize.cols / 2;
    m_colsPercentage *= 0.5f;

    updateWindow();

    int newWindowCols = initialCols - m_currentInstanceSize.cols;
    int newWindowRows = m_currentInstanceSize.rows;
    int newWindowPositionCol = m_currentInstancePosition.col + m_currentInstanceSize.cols;
    int newWindowPositionRow = m_currentInstancePosition.row;
    std::shared_ptr<Instance> newInstance = std::make_shared<Instance>(
            Position{newWindowPositionRow, newWindowPositionCol},
            Size{newWindowRows, newWindowCols},
            m_currentTerminalSize.rows,
            m_currentTerminalSize.cols
            );
    newInstance->m_colsPercentage = m_colsPercentage;
    newInstance->m_rowsPercentage = m_rowsPercentage;

    newInstance->setLimits(
        m_aboveInstance,
        m_rightInstance,
        m_belowInstance,
        shared_from_this()
    );

    newInstance->m_parent = shared_from_this();
    newInstance->m_splitType = Instance::SplitType::Vertical;
    m_children.push_back(newInstance);
    
    rawUpdateLimit(m_rightInstance, newInstance);
    newInstance->m_rightInstance->updateLeftLimit(shared_from_this(), newInstance);
    newInstance->m_leftInstance = shared_from_this();
    
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

std::shared_ptr<Instance> Instance::deleteWindow()
{
    return m_parent;
}

std::shared_ptr<Instance> Instance::getAboveWindow()
{
    if (m_aboveInstance && !m_aboveInstance->m_fixed)
    {
        return m_aboveInstance;
    }
    return nullptr;
}

std::shared_ptr<Instance> Instance::getBelowWindow()
{
    if (m_belowInstance && !m_belowInstance->m_fixed)
    {
        return m_belowInstance;
    }
    return nullptr;
}

std::shared_ptr<Instance> Instance::getLeftWindow()
{
    if (m_leftInstance && !m_leftInstance->m_fixed)
    {
        return m_leftInstance;
    }
    return nullptr;
}

std::shared_ptr<Instance> Instance::getRightWindow()
{
    if (m_rightInstance && !m_rightInstance->m_fixed)
    {
        return m_rightInstance;
    }
    return nullptr;
}

void Instance::updateLeftLimit(std::shared_ptr<Instance> oldLimit,
                               std::shared_ptr<Instance> newLimit)
{
    if (!m_beginLeftUpdate)
    {
        m_beginLeftUpdate = true;
        if (m_parent)
        {
            m_parent->updateLeftLimit(oldLimit, newLimit);
        }
        for (auto& child : m_children)
        {
            child->updateLeftLimit(oldLimit, newLimit);
        }
        if (m_leftInstance == oldLimit)
        {
            m_leftInstance = newLimit;
        }
        m_beginLeftUpdate = false;
    }
}

void Instance::updateRightLimit(std::shared_ptr<Instance> oldLimit,
                                std::shared_ptr<Instance> newLimit)
{
    if (!m_beginRightUpdate)
    {
        m_beginRightUpdate = true;
        if (m_parent)
        {
            m_parent->updateRightLimit(oldLimit, newLimit);
        }
        for (auto& child : m_children)
        {
            child->updateRightLimit(oldLimit, newLimit);
        }
        if (m_rightInstance == oldLimit)
        {
            m_rightInstance = newLimit;
        }
        m_beginRightUpdate = false;
    }
}

void Instance::updateAboveLimit(std::shared_ptr<Instance> oldLimit,
                                std::shared_ptr<Instance> newLimit)
{
    if (!m_beginAboveUpdate)
    {
        m_beginAboveUpdate = true;
        if (m_parent)
        {
            m_parent->updateAboveLimit(oldLimit, newLimit);
        }
        for (auto& child : m_children)
        {
            child->updateAboveLimit(oldLimit, newLimit);
        }
        if (m_aboveInstance == oldLimit)
        {
            m_aboveInstance = newLimit;
        }
        m_beginAboveUpdate = false;
    }
}

void Instance::updateBelowLimit(std::shared_ptr<Instance> oldLimit,
                                std::shared_ptr<Instance> newLimit)
{
    if (!m_beginBelowUpdate)
    {
        m_beginBelowUpdate = true;
        if (m_parent)
        {
            m_parent->updateBelowLimit(oldLimit, newLimit);
        }
        for (auto& child : m_children)
        {
            child->updateBelowLimit(oldLimit, newLimit);
        }
        if (m_belowInstance == oldLimit)
        {
            m_belowInstance = newLimit;
        }
        m_beginBelowUpdate = false;
    }
}

void Instance::rawUpdateLimit(std::shared_ptr<Instance> oldLimit,
                              std::shared_ptr<Instance> newLimit)
{
    if (!m_beginRawUpdate)
    {
        m_beginRawUpdate = true;

        updateBelowLimit(oldLimit, newLimit);
        updateAboveLimit(oldLimit, newLimit);
        updateLeftLimit(oldLimit, newLimit);
        updateRightLimit(oldLimit, newLimit);

        m_beginRawUpdate = false;
    }
}

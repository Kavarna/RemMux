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
    scrollok(m_window, true);
    idlok(m_window, true);
    m_active = true;
    m_shouldRender = true;
    m_fixed = false;
    m_startedRendering = false;
    m_startedResizing = false;

    m_cwd = g_startWorkingDirectory;
    printStartCommand();

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
    }
    updateWindow();
    wrefresh(m_window);

    if (m_active)
    {
        wattr_off(m_window, COLOR_PAIR(COLOR_GREEN_BLACK_BACKGROUND), nullptr);
    }

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
    m_aboveInstance = (up);
    m_aboveInstance->m_belowInstance = std::make_shared<Instance>(*this);
    m_rightInstance = (right);
    m_rightInstance->m_leftInstance = std::make_shared<Instance>(*this);    
    m_belowInstance = (down);
    m_belowInstance->m_aboveInstance = std::make_shared<Instance>(*this);
    m_leftInstance = (left);
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

    Logger::log("Split horizontal. Starting to iterate windows:\n");
    iterateWindowsBeginWithThis([&](std::shared_ptr<Instance> ptr)
    {
        if (ptr->m_aboveInstance == shared_from_this())
        {
            Logger::log(*ptr, "\n");
            ptr->m_aboveInstance = newInstance;
        }
    });

    newInstance->m_aboveInstance = shared_from_this();
    newInstance->m_rightInstance = m_rightInstance;
    newInstance->m_belowInstance = m_belowInstance;
    newInstance->m_leftInstance = m_leftInstance;
    m_belowInstance = newInstance;

    newInstance->m_parent = shared_from_this();
    newInstance->m_splitType = Instance::SplitType::Horizontal;
    m_children.push_back(newInstance);
    
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
    
    Logger::log("Split vertical. Starting to iterate windows:\n");
    iterateWindowsBeginWithThis([&](std::shared_ptr<Instance> ptr)
    {
        if (ptr->m_leftInstance == shared_from_this())
        {
            Logger::log(*ptr, "\n");
            ptr->m_leftInstance = newInstance;
        }
    });

    newInstance->m_aboveInstance = m_aboveInstance;
    newInstance->m_rightInstance = m_rightInstance;
    newInstance->m_belowInstance = m_belowInstance;
    newInstance->m_leftInstance = shared_from_this();

    m_rightInstance = newInstance;

    newInstance->m_parent = shared_from_this();
    newInstance->m_splitType = Instance::SplitType::Vertical;
    m_children.push_back(newInstance);

    newInstance->resize(m_currentTerminalSize.rows, m_currentTerminalSize.cols);

    return newInstance;
}

void Instance::updateWindow()
{
    mvwin(m_window, m_currentInstancePosition.row, m_currentInstancePosition.col);
    wresize(m_window, m_currentInstanceSize.rows, m_currentInstanceSize.cols);
    wclear(m_window);

    wmove(m_window, 0, 0);
    sendStringNotSave(m_cache);
    sendStringNotSave(m_command);

    wrefresh(m_window);
}

void Instance::setActive(bool active)
{
    m_active = active;
    if (m_active)
    {
        iterateWindowsBeginWithThis([](std::shared_ptr<Instance> wnd)
        {
            wnd->updateWindow();
        });
    }
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

void Instance::sendChar(char ch)
{
    sendCharNotSave(ch);

    if (ch == '\n')
    {
        sendStringAndSave(m_command + "\n");
        // Send and receive output
        executeCommand();
        printStartCommand();
        m_command = "";
    }
    else
    {
        m_command.push_back(ch);
    }
}

void Instance::sendStringAndSave(const std::string& str)
{
    for (const auto ch : str)
    {
        sendCharAndSave(ch);
    }
}

void Instance::printStartCommand()
{
    sendStringAndSave(m_cwd + std::string("->"));
}

void Instance::sendCharAndSave(char ch)
{
    waddch(m_window, ch);

    m_cache.push_back(ch);
    if (m_cache.size() > maxCacheSize)
    {
        m_cache.erase(m_cache.begin(), m_cache.begin() + 1);
    }
}

void Instance::sendCharNotSave(char ch)
{
    waddch(m_window, ch);
}

void Instance::sendStringNotSave(const std::string& str)
{
    for (const auto ch : str)
    {
        sendCharNotSave(ch);
    }
}

void Instance::removeLastChar()
{
    if (m_command.size() > 0)
    {
        m_command.pop_back();
    }
}

void Instance::executeCommand()
{
    m_available = false;

    //std::async(std::launch::async, [&]
    //{
        executeSpecificCommand(m_command);
        m_available = true;
    //});
}

void Instance::executeSpecificCommand(const std::string& m_command)
{
    if (m_command == "cd")
    {
        m_cwd = g_startWorkingDirectory;
    }
    else if (m_command.find("cd ") == 0)
    {
        uint32_t startDirPos = 0;
        for (uint32_t i = 2; i < m_command.size(); ++i)
        {
            if (m_command[i] != ' ')
            {
                startDirPos = i;
                break;
            }
        }
        if (startDirPos == 0)
        {
            m_cwd = g_startWorkingDirectory;
        }
        else
        {
            auto res = m_command.find_last_of("../");
            if (res == startDirPos + 2)
            {
                uint32_t pos = m_cwd.find_last_of('/');
                m_cwd.erase(m_cwd.begin() + pos, m_cwd.end());
            }
            else
            {
                // if [ -d "/hfasfsaome" ]; then echo "TRUE"; else echo "FALSE"; fi
                SendMessage(g_clientSocket, "if [ -d \"" +
                        std::string(m_command.c_str()+ startDirPos) +
                        "\" ]; then echo \"TRUE\"; else echo \"FALSE\"; fi");
                SendMessage(g_clientSocket, m_cwd);

                std::string result;
                ReadMessage(g_clientSocket, result);

                if (result.compare(0, strlen("TRUE"), "TRUE") == 0)
                {
                    if (m_command[startDirPos] == '/')
                    {
                        m_cwd = std::string(m_command.c_str() + startDirPos);
                    }
                    else
                    {
                        m_cwd = m_cwd + "/" + std::string(m_command.c_str() + startDirPos);
                    }
                }
                else
                {
                    sendStringAndSave("Invalid path\n");
                }
            }
            
        }
    }
    else
    {
        SendMessage(g_clientSocket, m_command);
        SendMessage(g_clientSocket, m_cwd);
        std::string response;
        ReadMessage(g_clientSocket, response);
        sendStringAndSave(response);
    }
    
}

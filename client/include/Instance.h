#pragma once


#include "Common.h"


class Instance :
    public std::enable_shared_from_this<Instance>
{
    enum class SplitType
    {
        Vertical,
        Horizontal,
        NoneOfAbove = 0
    };
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
    void setActive(bool active);

    std::shared_ptr<Instance> splitHorizontally();
    std::shared_ptr<Instance> splitVertically();
    // Returns the new window or nullptr
    // After using this method, this instance can be deleted (ie. .reset())
    std::shared_ptr<Instance> deleteWindow();

    std::shared_ptr<Instance> getAboveWindow();
    std::shared_ptr<Instance> getBelowWindow();
    std::shared_ptr<Instance> getLeftWindow();
    std::shared_ptr<Instance> getRightWindow();

    void updateWindow();

    template <typename function>
    void iterateWindowsBeginWithThis(function fct)
    {
        if (m_beginIterate)
            return;
        m_beginIterate = true;
        fct(shared_from_this());

        if (m_parent)
            m_parent->iterateWindowsBeginWithThis(fct);
        for (auto& child : m_children)
            child->iterateWindowsBeginWithThis(fct);

        m_beginIterate = false;
    }
    // friend std::ostream& operator << (std::ostream& stream, const Instance& inst)
    friend std::ostream& operator << (std::ostream& stream, const Instance& inst)
    {
        stream << "Window with position = (" <<
                inst.m_currentInstancePosition.row << ", " << inst.m_currentInstancePosition.col <<
                ") and size = (" << inst.m_currentInstanceSize.rows << ", " << inst.m_currentInstanceSize.cols <<
                "); Percentages = (" << inst.m_rowsPercentage << ", " << inst.m_colsPercentage << "); ";
        if (inst.m_fixed)
            stream << "fixed window; ";
        if (inst.m_shouldRender)
            stream << "should render window; ";
        if (inst.m_active)
            stream << "active window;";

        return stream;
    }

private:
    void updateLeftLimit(std::shared_ptr<Instance> oldLimit,
                         std::shared_ptr<Instance> newLimit);
    void updateRightLimit(std::shared_ptr<Instance> oldLimit,
                          std::shared_ptr<Instance> newLimit);
    void updateAboveLimit(std::shared_ptr<Instance> oldLimit,
                          std::shared_ptr<Instance> newLimit);
    void updateBelowLimit(std::shared_ptr<Instance> oldLimit,
                          std::shared_ptr<Instance> newLimit);
    void rawUpdateLimit(std::shared_ptr<Instance> oldLimit,
                        std::shared_ptr<Instance> newLimit);


private:
    WINDOW* m_window;

    bool m_active;
    bool m_shouldRender;
    bool m_fixed;

    Size m_currentTerminalSize;

    bool m_startedRendering;
    bool m_startedResizing;

    Position m_currentInstancePosition;
    Size m_currentInstanceSize;

    float m_rowsPercentage = 1.0f;
    float m_colsPercentage = 1.0f;

    std::shared_ptr<Instance> m_leftInstance = nullptr;
    std::shared_ptr<Instance> m_rightInstance = nullptr;
    std::shared_ptr<Instance> m_aboveInstance = nullptr;
    std::shared_ptr<Instance> m_belowInstance = nullptr;

    std::shared_ptr<Instance> m_parent = nullptr;
    SplitType m_splitType = SplitType::NoneOfAbove;

    std::vector<std::shared_ptr<Instance>> m_children;
    
    bool m_beginAboveUpdate = false;
    bool m_beginBelowUpdate = false;
    bool m_beginRightUpdate = false;
    bool m_beginLeftUpdate = false;
    bool m_beginRawUpdate = false;
    bool m_beginIterate = false;
};




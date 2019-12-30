#pragma once


#include "Common.h"
#include "UIHeader.h"
#include "Instance.h"

class RemMux
{

    static RemMux* m_instance;

    template <typename type>
    using Ptr = std::shared_ptr<type>;
    struct InstanceInfo 
    {
        std::shared_ptr<Instance>   m_instance;
        std::shared_ptr<Instance>   m_headerInstance;
        std::shared_ptr<Instance>   m_belowInstance;
        std::shared_ptr<Instance>   m_rightInstance;
        std::shared_ptr<Instance>   m_leftInstance;

        void resize(uint32_t rows, uint32_t cols);
        void render();
    };

private:
    RemMux();
    ~RemMux();

public:
    static RemMux* Get();
    static void Destroy();

private:
    static void signalHandler(int);

public:
    int run(int argc, const char *argv[]);

private:
    // init
    void parseArguments(int argc, const char *argv[]);
    void initCurses();
    void initColors();
    void initComponents();

    void updateLimits(InstanceInfo& info, uint32_t rows, uint32_t cols);

    // runtime
    void getUserInput();
    void resize();
    void present();

    // scheduling
    void schedule(uint32_t);
    bool isScheduled(uint32_t);
    void solve(uint32_t);

private:
    HighResolutionTimer         m_timer;
    float                       m_deltaTime;

    std::unique_ptr<UIHeader>   m_header;
    
    std::map<int, InstanceInfo> m_instances;
    int                         m_activeInstance;




    bool                        m_windowMode = false;

    uint64_t                    m_flags;                        

};



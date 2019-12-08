#pragma once


#include "Common.h"
#include "UIHeader.h"

class RemMux
{

    static RemMux* m_instance;


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

    uint64_t                    m_flags;                        

};



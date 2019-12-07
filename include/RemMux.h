#pragma once


#include "Common.h"
#include "IResizeable.h"
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
    void parseArguments(int argc, const char *argv[]);
    void initCurses();
    void initColors();
    void initComponents();

    void getUserInput();

    void resize();

    void present();

private:
    HighResolutionTimer         m_timer;
    float                       m_deltaTime;

};



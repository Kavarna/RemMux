#include "RemMux.h"


RemMux* RemMux::m_instance = nullptr;
RemMux* RemMux::Get()
{
    if (m_instance == nullptr)
    {
        m_instance = new RemMux;
    }
    return m_instance;
}
void RemMux::Destroy()
{
    delete m_instance;
}
void RemMux::signalHandler(int signal)
{
    if (signal == SIGWINCH)
    {
        RemMux::Get()->resize();
    }
}
void RemMux::resize()
{
    endwin();
    refresh();
    clear();
}



RemMux::RemMux()
{   
    initCurses();
    initColors();
    initComponents();
}


RemMux::~RemMux()
{
    endwin();
}


int RemMux::run(int argc, const char *argv[])
{
    parseArguments(argc, argv);

    while (true)
    {
        // Ensure framerate
        m_timer.waitSeconds(m_deltaTime - m_timer.timeSinceLastFrame());
        m_timer.update();

        getUserInput();
        present();
    }

}


void RemMux::parseArguments(int argc, const char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-framerate") == 0)
        {
            m_deltaTime = 2.0f / atoi(argv[i + 1]);
            ++i;
        }
    }
}


void RemMux::initCurses()
{
    initscr();

    // raw();
    keypad(stdscr, TRUE);
    noecho();
    nodelay(stdscr, TRUE);
}


void RemMux::initColors()
{
    start_color();

    init_color(COLOR_LTGRAY, 750, 750, 750);
    init_color(COLOR_GRAY, 500, 500, 500);

    init_pair(1, COLOR_RED,     COLOR_BLACK);
    init_pair(2, COLOR_GREEN,   COLOR_BLACK);
    init_pair(3, COLOR_BLUE,    COLOR_BLACK);
    init_pair(4, COLOR_WHITE,   COLOR_BLACK);
    init_pair(5, COLOR_BLACK,   COLOR_BLACK);


    init_pair(6, COLOR_RED,     COLOR_LTGRAY);
    init_pair(7, COLOR_GREEN,   COLOR_LTGRAY);
    init_pair(8, COLOR_BLUE,    COLOR_LTGRAY);
    init_pair(9, COLOR_WHITE,   COLOR_LTGRAY);
    init_pair(10, COLOR_BLACK,   COLOR_LTGRAY);

    init_pair(11, COLOR_RED,     COLOR_GRAY);
    init_pair(12, COLOR_GREEN,  COLOR_GRAY);
    init_pair(13, COLOR_BLUE,   COLOR_GRAY);
    init_pair(14, COLOR_WHITE,  COLOR_GRAY);
    init_pair(15, COLOR_BLACK,   COLOR_GRAY);
}


void RemMux::initComponents()
{
    signal(SIGWINCH, RemMux::signalHandler);
}

void RemMux::getUserInput()
{
    while (true)
    {
        int ch;
        ch = getch();
        if (ch == ERR)
            break;
        if (ch == '\n')
            break;
        mvwprintw(stdscr, 1, 0, "Pressed char = %c", ch);
    }
}

void RemMux::present()
{
    mvwprintw(stdscr, 0, 0, "Framerate = %d", m_timer.getPeriodCount());
}

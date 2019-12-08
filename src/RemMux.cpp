#include "RemMux.h"
#include "Logger.h"


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
        RemMux::Get()->schedule(FLAG_RESIZE);
        Logger::log("Resize scheduled\n");
    }
}
void RemMux::resize()
{
    endwin();
    refresh();
    clear();

    uint32_t rows, cols;
    getmaxyx(stdscr, rows, cols);
    Logger::log("Handling resize. New size = (", rows, ", ", cols, ")\n");
    solve(FLAG_RESIZE);

    m_header->resize(3, cols);
}



RemMux::RemMux()
{   
    initCurses();
    initColors();
    initComponents();
    resize();
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
    init_color(COLOR_GRAY, 450, 450, 450);

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

    m_header = std::make_unique<UIHeader>();
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
        for (int i = 1; i <= 9; ++i)
        {
            if (KEY_F(i) == ch)
            {
                m_header->setActiveInstance(i - 1);
                break;
            }
        }
        mvwprintw(stdscr, 3, 0, "Pressed char = %d;       ", ch);
    }
}

void RemMux::present()
{
    if (isScheduled(FLAG_RESIZE))
        resize();
    
    m_header->render(m_timer.getPeriodCount());
}

void RemMux::schedule(uint32_t flag)
{
    m_flags |= (1ul << flag);
}

bool RemMux::isScheduled(uint32_t flag)
{
    return m_flags & (1ul << flag);
}

void RemMux::solve(uint32_t flag)
{
    m_flags &= ~(1ul << flag); 
}

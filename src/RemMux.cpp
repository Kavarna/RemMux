#include "RemMux.h"
#include "Logger.h"



/* ~~~~~~~~Utilities~~~~~~~~ */

void RemMux::InstanceInfo::resize(uint32_t rows, uint32_t cols)
{
    m_headerInstance->resize(rows, cols);
    m_headerInstance->setPosition({0,0});
    m_headerInstance->setSize({(int)rows - 3, (int)cols});

    m_leftInstance->resize(rows, cols);
    m_leftInstance->setPosition({0, -3});
    m_leftInstance->setSize({3, (int)cols});

    m_rightInstance->resize(rows, cols);
    m_rightInstance->setPosition({0, (int) cols + 1});
    m_rightInstance->setSize({(int) rows, 2});

    m_belowInstance->resize(rows, cols);
    m_belowInstance->setPosition({(int)rows + 1, 0});
    m_belowInstance->setSize({2, (int) cols});

    m_instance->resize(rows - 3, cols);
}

void RemMux::InstanceInfo::render()
{
    m_instance->render();
}

/* ~~~~~~~~End utilities ~~~~~~~~ */

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
    for (auto& elem : m_instances)
    {
        elem.second.resize(rows, cols);
    }
}


RemMux::RemMux()
{
    Logger::log("App started.\n");
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
    init_pair(10, COLOR_BLACK,  COLOR_LTGRAY);

    init_pair(11, COLOR_RED,    COLOR_GRAY);
    init_pair(12, COLOR_GREEN,  COLOR_GRAY);
    init_pair(13, COLOR_BLUE,   COLOR_GRAY);
    init_pair(14, COLOR_WHITE,  COLOR_GRAY);
    init_pair(15, COLOR_BLACK,  COLOR_GRAY);
}


void RemMux::initComponents()
{
    signal(SIGWINCH, RemMux::signalHandler);

    uint32_t rows, cols;
    getmaxyx(stdscr, rows, cols);

    m_header = std::make_unique<UIHeader>(3, cols);

    InstanceInfo info;
    info.m_instance = std::make_unique<Instance>(Position{3, 0},
                                                Size{(int)rows - 3, (int)cols},
                                                rows, cols);
    updateLimits(info, rows, cols);
    m_instances[1] = std::move(info);
    m_header->addInstance(1);
                           
    m_activeInstance = 1;
}

void RemMux::updateLimits(RemMux::InstanceInfo& info, uint32_t rows, uint32_t cols)
{
    info.m_headerInstance = std::make_shared<Instance>(Position{0,0},
                            Size{(int)rows - 3, (int)cols},
                            rows, cols);
    info.m_headerInstance->setFixed(true);
    info.m_headerInstance->setShouldRender(false);

    info.m_leftInstance = std::make_shared<Instance>(Position{0, -3},
                            Size{(int)rows, 2},
                            rows, cols);
    info.m_leftInstance->setFixed(true);
    info.m_leftInstance->setShouldRender(false);

    info.m_rightInstance = std::make_shared<Instance>(Position{0, (int)cols + 1},
                            Size{(int)rows, 2},
                            rows, cols);
    info.m_rightInstance->setFixed(true);
    info.m_rightInstance->setShouldRender(false);

    info.m_belowInstance = std::make_shared<Instance>(Position{(int)rows + 1, 0},
                            Size{2, (int)cols},
                            rows, cols);
    info.m_belowInstance->setFixed(true);
    info.m_belowInstance->setShouldRender(false);
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
                m_activeInstance = i;
                m_header->addInstance(i);
                Logger::log("F", i, " pressed.\n");
                break;
            }
        }
        if (ch == CTRL('x'))
        { // Remove current instance
            Logger::log("CTRL + x pressed. Deleting instance", m_activeInstance, ". ");
            m_header->deleteInstance(m_activeInstance);
            for (const auto& elem : m_instances)
            {
                m_activeInstance = elem.first;
                break;
            }
            Logger::log("New active instance = ", m_activeInstance, ".\n");
            if (m_instances.size() == 0)
            {
                Logger::log("Application should close since there are no active instances.\n");
            }
        }
        else if (ch == CTRL('b'))
        { // Enter window mode
            Logger::log("CTRL + b pressed. Entering window mode.\n");
            m_windowMode = true;
        }
        else if (ch == '\"')
        {
            if (m_windowMode)
            {
                Logger::log("Window mode enabled and \" pressed. Splitting current horizontally window\n");
                m_windowMode = false;
            }
        }
        else if (ch == '%')
        {
            if (m_windowMode)
            {
                Logger::log("Window mode enabled and \% pressed. Splitting current vertically window\n");
                m_windowMode = false;
            }
        }
        mvwprintw(stdscr, 3, 0, "Pressed char = %d;       ", ch);
    }
}

void RemMux::present()
{
    if (isScheduled(FLAG_RESIZE))
        resize();
    
    m_header->render(m_timer.getPeriodCount(), m_activeInstance);
    m_instances[m_activeInstance].render();
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

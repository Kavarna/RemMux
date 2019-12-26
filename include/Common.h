#pragma once

// Unix
#include <curses.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

// C
#include <string.h>

// C++ std
#include <vector>
#include <string>
#include <cmath>
#include <map>

// Others
#include "HighResolutionTimer.h"


#define COLOR_LTGRAY                        8
#define COLOR_GRAY                          9

#define COLOR_RED_BLACK_BACKGROUND          1
#define COLOR_GREEN_BLACK_BACKGROUND        2
#define COLOR_BLUE_BLACK_BACKGROUND         3
#define COLOR_BLACK_BLACK_BACKGROUND        5
#define COLOR_WHITE_BLACK_BACKGROUND        4


#define COLOR_RED_LTGRAY_BACKGROUND         6
#define COLOR_GREEN_LTGRAY_BACKGROUND       7
#define COLOR_BLUE_LTGRAY_BACKGROUND        8
#define COLOR_WHITE_LTGRAY_BACKGROUND       9
#define COLOR_BLACK_LTGRAY_BACKGROUND       10

#define COLOR_RED_GRAY_BACKGROUND           11
#define COLOR_GREEN_GRAY_BACKGROUND         12
#define COLOR_BLUE_GRAY_BACKGROUND          13
#define COLOR_WHITE_GRAY_BACKGROUND         14
#define COLOR_BLACK_GRAY_BACKGROUND         15


#define MAX_USERNAME_LENGTH                 16
#define MAX_DATE_LENGTH                     20
#define MAX_INSTANCE_NAME                   8
#define MAX_FRAMERATE_LENGTH                2


#define FLAG_RESIZE                         0

#ifndef CTRL
#define CTRL(c) ((c) & 037)
#endif


#define THROW_ERROR(...) {char message[1024];\
sprintf(message, __VA_ARGS__);\
std::runtime_error error(&message[0]);\
throw error;}
#define EVALUATE(expression, expected_result, op, ...) {\
 auto value = (expression);\
if ( value op decltype(value)(expected_result) )\
    THROW_ERROR(__VA_ARGS__);\
}


struct Position
{
    int row;
    int col;
};

#ifndef ERRORHANDLER_HPP_
#define ERRORHANDLER_HPP_

#define MAX_STRLEN  1024

#define RESET       0
#define BRIGHT      1
#define DIM         2
#define UNDERLINE   3
#define BLINK       4
#define REVERSE     7
#define HIDDEN      8

#define BLACK       0
#define RED         1
#define GREEN       2
#define YELLOW      3
#define BLUE        4
#define MAGENTA     5
#define CYAN        6
#define WHITE       7

enum logType {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

void lprintf(logType t, const char* f, ...);

#endif // ERRORHANDLER_HPP_

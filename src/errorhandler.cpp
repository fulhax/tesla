#include "errorhandler.hpp"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int scolor(char *out, int attr, int fg, int bg = -1)
{
    if(bg != -1) {
        snprintf(out, MAX_STRLEN, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
    } else {
        snprintf(out, MAX_STRLEN, "%c[%d;%dm", 0x1B, attr, fg + 30);
    }

    return strlen(out);
}

int ccolor(char *out)
{
    snprintf(out, MAX_STRLEN, "%c[0m", 0x1B);
    return strlen(out);
}

void cfprintf(FILE *f, const char *fo, ...)
{
    char *in = new char[MAX_STRLEN];
    char *out = new char[MAX_STRLEN];
    int ao = 0;

    memset(in, 0, MAX_STRLEN);
    memset(out, 0, MAX_STRLEN);

    va_list args;
    va_start(args, fo);
    vsprintf(in, fo, args);
    va_end(args);

    for(char *at = in; *at != '\0'; at++) {
        if(*at == '^') {
            at++;

            #ifdef __linux__
            switch(*at) {
                case '0':
                    ao += ccolor(&out[ao]);
                    break;

                case 'd':
                    ao += scolor(&out[ao], DIM, BLACK);
                    break;

                case 'r':
                    ao += scolor(&out[ao], DIM, RED);
                    break;

                case 'g':
                    ao += scolor(&out[ao], DIM, GREEN);
                    break;

                case 'b':
                    ao += scolor(&out[ao], DIM, BLUE);
                    break;

                case 'y':
                    ao += scolor(&out[ao], DIM, YELLOW);
                    break;

                case 'm':
                    ao += scolor(&out[ao], DIM, MAGENTA);
                    break;

                case 'c':
                    ao += scolor(&out[ao], DIM, CYAN);
                    break;

                case 'w':
                    ao += scolor(&out[ao], DIM, WHITE);
                    break;
            }
            #endif
        } else {
            out[ao++] = *at;
        }
    }

    fprintf(f, "%s", out);
    delete [] in;
    delete [] out;
}

void lprintf(logType t, const char *f, ...)
{
    char output[MAX_STRLEN] = {0};
    va_list args;
    va_start(args, f);

    vsprintf(output, f, args);

    switch(t) {
        case LOG_INFO:
            cfprintf(stdout, "^0[^gINFO^0]    %s\n", output);
            break;

        case LOG_WARNING:
            cfprintf(stdout, "^0[^yWARNING^0] %s\n", output);
            break;

        case LOG_ERROR:
            cfprintf(stderr, "^0[^rERROR^0]   %s\n", output);
            break;
    }

    va_end(args);
}

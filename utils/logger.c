#include "logger.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

log_cfg_t log_cfg = {
    .flags = {
        .color = true,
        .datetime = false,
    },
    .level = LOG_TRACE,
};

typedef struct {
    uint8_t r, g, b;
} color_t;

const color_t log_colors[] = {
    {46, 196, 182},
    {253, 255, 252},
    {255, 159, 28},
    {231, 29, 54},
};

#define MAX_LEVEL_LEN 5

void log_stdout(int level, const char* fmt, ...) {
    if (level < log_cfg.level) {
        return;
    }

    va_list args;
    va_start(args, fmt);

    char level_str[16] = {0};
    switch (level) {
        case LOG_TRACE:   strcpy(level_str, "TRACE"); break;
        case LOG_ERROR:   strcpy(level_str, "ERROR"); break;
        case LOG_WARNING: strcpy(level_str, "WARN");  break;
        case LOG_INFO:    strcpy(level_str, "INFO");  break;
        default:          strcpy(level_str, "UNKNOWN"); break;
    }

    char message[4096] = {0};
    vsnprintf(message, sizeof(message), fmt, args);

    int padding = MAX_LEVEL_LEN - (int)strlen(level_str);

    if (log_cfg.flags.color) {
        // fprintf(stderr, "\033[%d;1m[%s]\033[0m%*s%s\n", 
        //         log_colors[level], level_str, padding + 1, "", message); // for standard ansi colors;
        // for rgb colors:
        fprintf(stderr, "\033[38;2;%d;%d;%dm[%s]\033[0m%*s%s\n", 
                log_colors[level].r, log_colors[level].g, log_colors[level].b, level_str, padding + 1, "", message);
    } else {
        fprintf(stderr, "[%s]%*s%s\n", level_str, padding + 1, "", message);
    }

    va_end(args);
}

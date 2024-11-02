#ifndef _HTTPC_LOGGER_H_
#define _HTTPC_LOGGER_H_

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum {
    LOG_TRACE,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
};

typedef struct {
    struct {
        uint16_t color : 1;
        uint16_t datetime : 1;
    } flags;
    int level;
} log_cfg_t;

extern log_cfg_t log_cfg;

void log_stdout(int level, const char* fmt, ...);
#define log_trace(fmt, ...) log_stdout(LOG_TRACE, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) log_stdout(LOG_INFO, fmt, ##__VA_ARGS__)
#define log_err(fmt, ...) log_stdout(LOG_ERROR, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) log_stdout(LOG_WARNING, fmt, ##__VA_ARGS__)
#define log_syserr(fmt, ...) log_stdout(LOG_ERROR, fmt ": %s (%d)", ##__VA_ARGS__, strerror(errno), errno)

#endif

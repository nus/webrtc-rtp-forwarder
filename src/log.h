#pragma once

#include <stdio.h>

enum LOG_LEVEL_E {
    LOG_LEVEL_NONE,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE,
};

void set_log_level(LOG_LEVEL_E level);
LOG_LEVEL_E get_log_level();

#define LOG_ERROR(FORMAT, ...)   do { if (get_log_level() >= LOG_LEVEL_ERROR)   fprintf(stderr, "LOG_ERROR   (" __FILE__ ") " FORMAT "\n", ## __VA_ARGS__); } while(0)
#define LOG_INFO(FORMAT,  ...)   do { if (get_log_level() >= LOG_LEVEL_INFO)    printf(         "LOG_INFO    (" __FILE__ ") " FORMAT "\n", ## __VA_ARGS__); } while(0)
#define LOG_DEBUG(FORMAT, ...)   do { if (get_log_level() >= LOG_LEVEL_DEBUG)   printf(         "LOG_DEBUG   (" __FILE__ ") " FORMAT "\n", ## __VA_ARGS__); } while(0)
#define LOG_VERBOSE(FORMAT, ...) do { if (get_log_level() >= LOG_LEVEL_VERBOSE) printf(         "LOG_VERBOSE (" __FILE__ ") " FORMAT "\n", ## __VA_ARGS__); } while(0)

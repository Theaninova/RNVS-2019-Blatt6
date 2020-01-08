#pragma once

#include "debug.h"

#define DEFAULT_STR_ARG(name, pos, default_value) \
    char* name; \
    if (argc <= pos + 1) { \
        WARN("No value specified for single parameter"); \
        name = default_value; \
    } else { \
        name = argv[pos + 1]; \
        LOG_STR(name); \
    }

#define STR_ARG(name, pos) \
    char* name; \
    if (argc < pos + 1) { \
        ERROR("input not correct. Please check again and retry"); \
        THROW(-1) \
    } else { \
        name = argv[pos + 1]; \
        LOG_STR(name); \
    }
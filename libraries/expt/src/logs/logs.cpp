/**
 * @file logs.c
 * @brief Implementation of logging functions.
 * 
 * @copyright 2024 MTA
 * @author 
 * Ing. Jiri Konecny
 */

#include "logs.hpp"

#ifndef DEBUG_VERBOSE_LEVEL
#define DEBUG_VERBOSE_LEVEL DEBUG_VERBOSE_IMPORTANT
#endif

std::string buildMessage(const char *format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return std::string(buffer);
}

#ifdef ARDUINO_H
    #include <Arduino.h>  ///< Include Arduino Serial functions
#elif defined(STDIO_H)
    #include <stdio.h>    ///< Include standard I/O functions
#endif

void logMessage(const char *format, ...) {
    va_list args;
    va_start(args, format);

    #ifdef ARDUINO_H
        if(!Serial)
        {
            initLogger();
        }
        // Create a buffer for formatted output
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer), format, args);
        Serial.println(buffer);  // Print via Arduino Serial
        Serial.flush();
    #elif defined(STDIO_H)
        vprintf(format, args);  // Print via standard console
    #endif
    va_end(args);
}

static int clampDebugVerboseLevel(int level) {
    if (level < DEBUG_VERBOSE_ERRORS) {
        return DEBUG_VERBOSE_ERRORS;
    }
    if (level > DEBUG_VERBOSE_ALL) {
        return DEBUG_VERBOSE_ALL;
    }
    return level;
}

static bool isDebugLevelEnabled(int level) {
#if ENABLE_DEBUG
    return clampDebugVerboseLevel(level) <= clampDebugVerboseLevel(DEBUG_VERBOSE_LEVEL);
#else
    (void)level;
    return false;
#endif
}

static void sanitizeDebugPayload(char *buffer) {
    if (!buffer) {
        return;
    }

    for (size_t i = 0; buffer[i] != '\0'; ++i) {
        if (buffer[i] == '?' || buffer[i] == '\r' || buffer[i] == '\n') {
            buffer[i] = ' ';
        }
    }
}

static void debugLogMessageVa(int level, const char *source, const char *reason, const char *format, va_list args) {
#if ENABLE_DEBUG
    if (!isDebugLevelEnabled(level)) {
        return;
    }

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format ? format : "", args);
    sanitizeDebugPayload(buffer);

    logMessage(
        "DEBUG: %s reason=%s source=%s",
        buffer,
        reason ? reason : "-",
        source ? source : "unknown");
#else
    (void)level;
    (void)source;
    (void)reason;
    (void)format;
    (void)args;
#endif
}

void debugLogMessage(int level, const char *source, const char *reason, const char *format, ...) {
    va_list args;
    va_start(args, format);
    debugLogMessageVa(level, source, reason, format, args);
    va_end(args);
}

void debugLogMessage(const char *source, const char *reason, const char *format, ...) {
    va_list args;
    va_start(args, format);
    debugLogMessageVa(DEBUG_VERBOSE_ALL, source, reason, format, args);
    va_end(args);
}

void initLogger(unsigned int baudrate, unsigned int timeout) {
    #ifdef ARDUINO_H
        Serial.begin(baudrate); // Initialize Serial for Arduino
        Serial.setTimeout(timeout); // Set timeout for Serial receive
    #elif defined(STDIO_H)
        // No initialization needed for standard console
        printf("Logger initialized for standard console...\n");
    #endif
}


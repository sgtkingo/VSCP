/**
 * @file config.h
 * @brief Configuration file for platform-specific settings.
 * 
 * This file defines macros to select the execution environment (Arduino or standard console).
 * Uncomment the desired macro to enable the respective environment.
 * 
 * @copyright 2025 MTA
 * @author Ing. Jiri Konecny
 */

#ifndef CONFIG_EXPT_H
#define CONFIG_EXPT_H

#ifdef __has_include
#if __has_include("../../engine/src/config.hpp")
#include "../../engine/src/config.hpp"
#endif
#endif

/// Uncomment to enable Arduino-based environments
#ifndef ARDUINO_H_ENV
#define ARDUINO_H_ENV
#endif

#ifdef ARDUINO_H_ENV
#define UART0_BAUDRATE 115200
#define UART0_TIMEOUT 100 // only for receive
#endif

/// Uncomment to enable standard console applications (PC/Linux)
//#define STDIO_H_ENV

// Uncomment to enable LVGL support
#define USE_LVGL
#define SPLASHER_TIMEOUT_MS 5000  // Default splash timeout in milliseconds

// Unified project debug logging switch. Exceptions are printed by their catch handlers.
#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG 1
#endif

// Debug verbosity:
// 1 = errors only, 2 = warnings and important operations, 3 = all debug details.
#ifndef DEBUG_VERBOSE_LEVEL
#define DEBUG_VERBOSE_LEVEL 2
#endif

// Uncomment to enable ESP32 platform
//#define ESP_PLATFORM

#endif // CONFIG_EXPT_H

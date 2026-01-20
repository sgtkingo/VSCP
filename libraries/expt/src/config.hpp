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

/// Uncomment to enable Arduino-based environments
#define ARDUINO_H 
#ifdef ARDUINO_H 
#define UART0_BAUDRATE 115200
#define UART0_TIMEOUT 100 // only for receive
#endif

/// Uncomment to enable standard console applications (PC/Linux)
//#define STDIO_H 

// Uncomment to enable LVGL support
//#define USE_LVGL
#define SPLASHER_TIMEOUT_MS 5000  // Default splash timeout in milliseconds

// Uncomment to enable ESP32 platform
//#define ESP_PLATFORM

#endif // CONFIG_EXPT_H
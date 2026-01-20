/**
 * @file config.hpp
 * @brief Configuration file for platform-specific settings.
 * 
 * This file defines macros to select the execution environment (Arduino or standard console).
 * Uncomment the desired macro to enable the respective environment.
 * 
 * @copyright 2025 MTA
 * @author Ing. Jiri Konecny
 */

#ifndef CONFIG_H
#define CONFIG_H

#define MAX_PROTOCOL_REQUEST_SIZE 1024 ///< Maximum size of a protocol request message

/// Uncomment to enable Arduino-based environments
#define ARDUINO_H 
#ifdef ARDUINO_H
#define UART1_PORT 0
#define UART1_BAUDRATE 115200
#define UART1_RX -1
#define UART1_TX -1
#define UART1_TIMEOUT 100
#endif
/// Set protocol verbosity level (0 = silent, 1 = errors, 2 = all)
#define PROTOCOL_VERBOSE 1
#define PROTOCOL_INIT_TIMEOUT 500

/// Uncomment to enable standard console applications (PC/Linux)
//#define STDIO_H 

///Set whatever the application should be a case sensitive
#define CASE_SENSITIVE true

#endif // CONFIG_H
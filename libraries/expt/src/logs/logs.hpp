/**
 * @file logs.h
 * @brief Header file for logging functions.
 * 
 * @copyright 2024 MTA
 * @author 
 * Ing. Jiri Konecny
 */

 #ifndef LOGS_H
 #define LOGS_H
 
 /*********************
 *      INCLUDES
 *********************/

 #include <string>
 #include <cstdarg>
 #include <cstdio>
 #include "../config.hpp"  ///< Configuration file inclusion


 /**
  * @brief Builds a formatted message string.
  * 
  * @param format The format string (like in printf).
  * @param ... Additional arguments for formatting.
  * @return std::string The formatted message.
  */
 std::string buildMessage(const char *format, ...);
 
 /**
  * @brief Logs a formatted message to the appropriate output (Serial for Arduino, stdout for PC).
  * 
  * This function supports variadic arguments similar to `printf()`.
  * 
    * @param format The format string (like in printf).
    * @param ... Additional arguments for formatting.
  */
 void logMessage(const char *format, ...);

/**
  * @brief Initializes the logger system.
  * 
  * This function sets up the logging environment, such as initializing Serial communication for Arduino.
  */
 void initLogger(unsigned int baudrate = UART0_BAUDRATE, unsigned int timeout = UART0_TIMEOUT);

 
 #endif // LOGS_H
 
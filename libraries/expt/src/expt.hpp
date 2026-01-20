/**************************************************************************
 * @file expt.hpp
 * @brief Exception and Error Handling Library for Virtual Sensors Protocol
 *
 * This header provides a comprehensive exception handling system for the VSCP
 * (Virtual Sensors Communication Protocol) library. It includes error codes,
 * exception classes, and logging functionality that works across different
 * platforms including Arduino, standard console applications, and LVGL-based
 * systems.
 *
 * Platform Support:
 * - Arduino: Define ARDUINO_H in config.hpp
 * - Console Applications: Define STDIO_H in config.hpp  
 * - LVGL GUI: Define USE_LVGL in config.hpp for splash messages
 *
 * @copyright 2025 MTA
 * @author Ing. Jiri Konecny
 **************************************************************************/

#ifndef EXPT_HPP
#define EXPT_HPP

/*********************
 *      DEFINES
 *********************/
// For Arduino need to be defined ARDUINO_H in config.hpp
// For standard console applications need to be defined STDIO_H in config.hpp
// For use of LVGL library for splash messages need to be defined USE_LVGL in config.hpp

/*********************
 *      INCLUDES
 *********************/
#include "config.hpp"  ///< Configuration file inclusion
#include "exceptions/error_codes.hpp"
#include "exceptions/exceptions.hpp" ///< For Exception class
#include "logs/logs.hpp"         ///< For logMessage function
#include "logs/splasher.hpp"     ///< For splashMessage function

#endif // EXPT_HPP

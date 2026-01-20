/**
 * @file error_codes.hpp
 * @brief Defines error codes used in exception handling and sensor operations.
 * 
 * This header provides a strongly typed enumeration for error codes that were previously defined
 * as preprocessor macros. These error codes are used throughout the application for configuration,
 * update, and value validation.
 * 
 * @copyright 2024 MTA
 * @author
 * Ing. Jiri Konecny
 */

#ifndef ERROR_CODES_HPP
#define ERROR_CODES_HPP

/**
 * @enum ErrorCode
 * @brief Enumeration of error codes for sensor and exception handling.
 *
 * - VALUE_ERROR: Invalid value error.
 * - VALUE_NOT_FOUND: Value not found.
 * - INVALID_VALUE: Invalid value.
 * - WARNING_CODE: Configuration error.
 * - ERROR_CODE: Operation error.
 * - CRITICAL_ERROR_CODE: Critical error, stop code.
 * - NOT_FOUND: Value not found.
 * - NOT_DEFINED_ERROR: Unknown error.
 */
enum class ErrorCode {
    VALUE_ERROR     = -1,  ///< Invalid value error.
    VALUE_NOT_FOUND =  0,  ///< Value not found.
    INVALID_VALUE,        ///< Invalid value.
    WARNING_CODE,  ///< Configuration error.
    ERROR_CODE,   ///< Operation error.
    CRITICAL_ERROR_CODE, ///< Critical error, stop code.
    NOT_FOUND,   ///< Value not found.  
    NOT_DEFINED_ERROR,  ///< Unknown error.
    TIMEOUT      ///< Timeout error.
};

#endif // ERROR_CODES_HPP

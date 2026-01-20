/**
 * @file exceptions.hpp
 * @brief Declaration of the Exception class for error handling.
 * 
 * This header file declares the Exception class, which encapsulates an error code,
 * message, and source information. It also supports nested (chained) exceptions via
 * an innerException pointer. The class provides a method to recursively print exception
 * details with indentation.
 * 
 * @copyright 2025 MTA
 * @author 
 * Ing. Jiri Konecny
 */

#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

/*********************
 *      INCLUDES
 *********************/

#include "error_codes.hpp"  ///< For error codes
#include "../logs/logs.hpp"   ///< For logMessage function
#include "../logs/splasher.hpp"   ///< For splashMessage function

#include <stdexcept>   ///< For std::exception

/**
 * @class Exception
 * @brief Represents an error/exception in the application.
 *
 * The Exception class stores an error code, a descriptive message, and the source where
 * the exception occurred. It also supports chaining of exceptions via the innerException
 * pointer. When an Exception object is destroyed, any chained exceptions are automatically
 * deleted.
 */
class Exception : public std::exception {
public:
    ErrorCode Code;               ///< Error code associated with the exception.
    std::string Name;       ///< Name of the exception type.
    std::string Message;    ///< Human-readable exception message.
    std::string Source;     ///< Origin of the exception (e.g., function or module name).
    Exception* innerException; ///< Pointer to a nested exception (if any).

    Exception(std::exception ex);
    Exception(Exception* ex);
    Exception(const std::string &source, Exception* inner = nullptr);
    Exception(const std::string &message, ErrorCode code = ErrorCode::NOT_DEFINED_ERROR, Exception* inner = nullptr);
    Exception(const std::string &source, const std::string &message, ErrorCode code = ErrorCode::NOT_DEFINED_ERROR, Exception* inner = nullptr);
    Exception(const std::string &name, const std::string &source, const std::string &message, ErrorCode code = ErrorCode::NOT_DEFINED_ERROR, Exception* inner = nullptr);
    
    virtual ~Exception();
    std::string flush(int level = 0) const;
    void print() const;
};

#endif // EXCEPTIONS_HPP

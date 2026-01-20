
// Define class Protocol for handling API communication protocol
// API protocol is URL-like with key-value pairs separated by '&' and '=', starting with '?' for request
// API response is in same format with key-value pairs
// All methods now return ResponseStatus struct containing status enum, error message, and parameters
//API methods:
/*
- init: handshake and ensure purpose of application, matches device db versions and api versions
req: ?type=INIT&app=APP_NAME&db=DB_VERSION&api=API_VERSION
res: ?status=1/0&error=Error Message
- update: request data update
req: ?type=UPDATE&id=UID
res: ?id=UID&status=1/0&param1=value1&param2=value2...
- config: send new configuration for sensor from HMI side to HW side
req: ?type=CONFIG&id=UID&param1=value1&param2=value2
res: ?id=UID&status=1/0&error=Error Message
- reset: reset the sensor
req: ?type=RESET&id=UID
res: ?id=UID&status=1/0&error=Error Message
- connect: connect sensor to pin(s)
req: ?type=CONNECT&id=UID&pins=PINS
res: ?id=UID&status=1/0&error=Error Message
- disconnect: disconnect sensor from pin
req: ?type=DISCONNECT&id=UID
res: ?id=UID&status=1/0&error=Error Message
*/
// Create class method for every API method
// For sending/receiving messages, use abstracted messenger interface - Messenger, user can connect to own implementation in .cpp file
// Messenger interface should be implemented for different platforms (Arduino, stdio, etc.)


/**
 * @file protocol.hpp
 * @brief Declaration of the Protocol class for handling API communication protocol.
 * 
 * This header defines the Protocol class that manages the communication protocol for sensor operations.
 * It includes methods for initializing, updating, configuring, resetting, connecting, and disconnecting sensors.
 * The protocol follows a URL-like format with key-value pairs for both requests and responses.
 * 
 * All methods return a ResponseStatus struct that contains:
 * - status: ResponseStatusEnum (OK/ERROR)  
 * - error: Error message string (if any)
 * - params: Additional response parameters as key-value pairs
 * 
 * Each method corresponds to a specific API request and response format.
 * The Protocol class interacts with the Messenger interface for communication.
 * Error handling is done through the ResponseStatus return value, not exceptions.
 */

#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "config.hpp"
#include "io/messenger.hpp"

#include <string>
#include <unordered_map>

/**
 * @enum ResponseStatusEnum
 * @brief Enumeration representing possible sensor response statuses.
 *
 * - OK: response indicates normal operation.
 * - ERROR: response indicates an error.
 */
enum class ResponseStatusEnum
{
    OK = 1,     ///< response is OK.
    ERROR = 0,  ///< response indicates an error.
};

/**
 * @struct ResponseStatus
 * @brief Structure representing the status of a sensor response.
 *
 * Contains the response status and an error message if applicable.
 */
struct ResponseStatus
{
    ResponseStatusEnum status;
    std::string error; ///< Last error message.
    std::unordered_map<std::string, std::string> params; ///< Additional parameters from response.
};

    
/**
 * @class Protocol
 * @brief Handles API communication protocol for sensor operations.
 * 
 * The Protocol class provides static methods for sensor communication using a URL-like format 
 * with key-value pairs. It provides methods for initializing connections, updating sensor data, 
 * configuring sensors, resetting sensors, and managing sensor connections to pins.
 * 
 * All methods return ResponseStatus struct instead of throwing exceptions, making error handling
 * predictable and consistent. The ResponseStatus contains the operation result, error messages,
 * and any additional response parameters.
 * 
 * API format: URL-like with key-value pairs separated by '&' and '=', starting with '?' for requests.
 * Response format: Same format with key-value pairs including status and error information.
 * Error handling: Done through ResponseStatus return values, not exceptions.
 */
class Protocol {
private:
    static const std::string API_VERSION; ///< API version constant
    static bool initialized;              ///< Protocol initialization status
    
    /**
     * @brief Parses a protocol message into key-value pairs.
     * 
     * @param message The protocol message to parse
     * @return std::unordered_map<std::string, std::string> Parsed key-value pairs
     */
    static std::unordered_map<std::string, std::string> parseMessage(const std::string& message, bool caseSensitive = CASE_SENSITIVE);
    
    /**
     * @brief Builds a protocol message from key-value pairs.
     * 
     * @param params Key-value pairs to build message from
     * @return std::string Formatted protocol message
     */
    static std::string buildMessage(const std::unordered_map<std::string, std::string>& params);

public:

    /**
     * @brief Initializes the protocol connection with default values (dummy/test mode).
     *
     * Base initialization method for testing and generic usage that doesn't require
     * platform-specific hooks. Uses default application information. Returns a dummy
     * successful response without actual communication.
     *
     * @return ResponseStatus Initialization response with status OK and success message
     */
    static ResponseStatus init_dummy();

    /**
     * @brief Initializes the protocol connection with default API version.
     *
     * Initialization method that uses default application name, version, and database version
     * but includes the current API version. Useful when only API compatibility needs
     * to be verified against the remote device.
     *
     * @return ResponseStatus Initialization response containing status (OK/ERROR), error message if any, and response parameters
     */
    static ResponseStatus init();

    /**
     * @brief Initializes the protocol connection with specified database version.
     *
     * Initialization method that uses default application name and version but allows
     * specifying the database version. Useful when database compatibility needs
     * to be verified against the remote device.
     *
     * @param db_version Database version to send in initialization request
     * @return ResponseStatus Initialization response containing status (OK/ERROR), error message if any, and response parameters
     */
    static ResponseStatus init(const std::string& db_version);

    /**
     * @brief Initializes the protocol connection with full handshake.
     *
     * Performs complete handshake to ensure application purpose, matches device database versions
     * and API versions with the remote device. Application info is sent once during initialization.
     *
     * Request format: ?type=INIT&app=APP_NAME&db=DB_VERSION&api=API_VERSION
     * Response format: ?status=1/0&error=Error Message
     *
     * @param app_name Application name identifier
     * @param db_version Database version to verify compatibility
     * @return ResponseStatus Initialization response containing status (OK/ERROR), error message if any, and response parameters
     */
    static ResponseStatus init(const std::string& app_name, const std::string& db_version);
    
    /**
     * @brief Requests data update for a specific sensor.
     *
     * Sends an update request to retrieve current sensor data and parameters from the remote device.
     * The response contains all current sensor parameters and values.
     *
     * Request format: ?type=UPDATE&id=UID
     * Response format: ?id=UID&status=1/0&param1=value1&param2=value2...
     *
     * @param uid Unique identifier of the sensor
     * @return ResponseStatus Update response containing status (OK/ERROR), error message if any, and updated sensor parameters
     */
    static ResponseStatus update(const std::string& uid);
    
    /**
     * @brief Sends new configuration parameters for sensor from HMI side to HW side.
     *
     * Transmits configuration parameters to update sensor settings on the remote device.
     * All configuration parameters are sent as key-value pairs in the request.
     *
     * Request format: ?type=CONFIG&id=UID&param1=value1&param2=value2
     * Response format: ?id=UID&status=1/0&error=Error Message
     *
     * @param uid Unique identifier of the sensor
     * @param config Configuration parameters map with key-value pairs to set
     * @return ResponseStatus Configuration response containing status (OK/ERROR), error message if any, and response parameters
     */
    static ResponseStatus config(const std::string& uid, const std::unordered_map<std::string, std::string>& config);
    
    /**
     * @brief Resets the specified sensor to default state.
     *
     * Sends a reset command to reinitialize the sensor to its default operational state.
     * This operation may clear sensor data and restore factory settings.
     *
     * Request format: ?type=RESET&id=UID
     * Response format: ?id=UID&status=1/0&error=Error Message
     *
     * @param uid Unique identifier of the sensor
     * @return ResponseStatus Reset response containing status (OK/ERROR), error message if any, and response parameters
     */
    static ResponseStatus reset(const std::string& uid);
    
    /**
     * @brief Connects sensor to specified hardware pin(s).
     *
     * Establishes a connection between the sensor and one or more hardware pins on the remote device.
     * Multiple pins can be specified for sensors that require multiple connections.
     *
     * Request format: ?type=CONNECT&id=UID&pins=PINS
     * where PINS is a string representing one or more pin numbers, e.g., "5" or "5,6,7"
     * Response format: ?id=UID&status=1/0&error=Error Message
     *
     * @param uid Unique identifier of the sensor
     * @param pins Pin number(s) as string - single pin "5" or multiple pins "5,6,7"
     * @return ResponseStatus Connection response containing status (OK/ERROR), error message if any, and response parameters
     */
    static ResponseStatus connect(const std::string& uid, const std::string& pins);
    
    /**
     * @brief Disconnects sensor from its currently assigned hardware pin(s).
     *
     * Removes the connection between the sensor and its hardware pins, effectively
     * disabling the sensor from reading/writing to hardware interfaces.
     *
     * Request format: ?type=DISCONNECT&id=UID
     * Response format: ?id=UID&status=1/0&error=Error Message
     *
     * @param uid Unique identifier of the sensor
     * @return ResponseStatus Disconnection response containing status (OK/ERROR), error message if any, and response parameters
     */
    static ResponseStatus disconnect(const std::string& uid);
    
    /**
     * @brief Checks if protocol is initialized.
     * 
     * @return bool True if protocol is initialized, false otherwise
     */
    static bool isInitialized();
    
    /**
     * @brief Gets the API version.
     * 
     * @return std::string API version
     */
    static std::string getApiVersion();
};

#endif // PROTOCOL_HPP
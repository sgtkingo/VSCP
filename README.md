# Virtual Sensors Communication Protocol (vscp) [v1.1.6]

This document describes the communication protocol used by the Virtual Sensors project for API communication between HMI (Human Machine Interface) and hardware components.

## Protocol Overview

The protocol uses a URL-like format with key-value pairs for both requests and responses. All messages start with `?` followed by parameters separated by `&` and key-value pairs separated by `=`.

**Format:** `?param1=value1&param2=value2&param3=value3`

## Protocol Class Usage

The Protocol class provides static methods for all API operations. No instantiation is required.

```cpp
#include "vscp.hpp"
//or #include <vscp.hpp> in Arduino IDE / PlatformIO

// Initialize the protocol
Protocol::init("MyApp", "1.0", "2.3");

// Use API methods
if (Protocol::isInitialized()) {
    //One time connect sensor to pin
    Protocol::connect("sensor123", 5);
}

//Update loop
while(1)
{
    auto sensorData = Protocol::update("sensor123");
    delay(100);
}
```

## API Methods

### 1. INIT - Protocol Initialization

Performs handshake to ensure application compatibility and version matching.

**Request:**
```
?type=INIT&app=APP_NAME&version=APP_VERSION&dbversion=DB_VERSION&api=API_VERSION
?type=INIT&dbversion=DB_VERSION&api=API_VERSION
?type=INIT&api=API_VERSION
?type=INIT
```

**Response:**
```
?status=1/0&error=Error Message
```

**Parameters:**
- `app`: Application name
- `version`: Application version
- `dbversion`: Database version 
- `api`: API version (managed internally as constant)

**Status Codes:**
- `1`: Success - Protocol initialized
- `0`: Failure - Check error message

**Example:**
```cpp
bool success = Protocol::init("VirtualSensors", "1.0", "2.3");
```

### 2. UPDATE - Request Sensor Data

Requests updated data from a specific sensor.

**Request:**
```
?type=UPDATE&id=UID
```

**Response:**
```
?id=UID&status=1/0&param1=value1&param2=value2&...
```

**Parameters:**
- `id`: Unique identifier of the sensor

**Returns:** Map of sensor parameters and values

**Example:**
```cpp
try {
    auto data = Protocol::update("temp_sensor_01");
    std::string temperature = data["temperature"];
    std::string humidity = data["humidity"];
} catch (const Exception& e) {
    // Handle error
}
```

### 3. CONFIG - Configure Sensor

Sends new configuration parameters from HMI to hardware.

**Request:**
```
?type=CONFIG&id=UID&param1=value1&param2=value2
```

**Response:**
```
?id=UID&status=1/0&error=Error Message
```

**Parameters:**
- `id`: Unique identifier of the sensor
- Additional parameters: Configuration key-value pairs

**Example:**
```cpp
std::unordered_map<std::string, std::string> config;
config["sample_rate"] = "1000";
config["threshold"] = "25.5";

bool success = Protocol::config("temp_sensor_01", config);
```

### 4. RESET - Reset Sensor

Resets the specified sensor to default state.

**Request:**
```
?type=RESET&id=UID
```

**Response:**
```
?id=UID&status=1/0
```

**Parameters:**
- `id`: Unique identifier of the sensor

**Example:**
```cpp
bool success = Protocol::reset("temp_sensor_01");
```

### 5. CONNECT - Connect Sensor to Pin

Connects a sensor to a specific hardware pin.

**Request:**
```
?type=CONNECT&id=UID&pin=PIN
```

**Response:**
```
?id=UID&status=1/0
```

**Parameters:**
- `id`: Unique identifier of the sensor
- `pin`: Hardware pin number

**Example:**
```cpp
bool success = Protocol::connect("temp_sensor_01", 5);
```

### 6. DISCONNECT - Disconnect Sensor

Disconnects a sensor from its current pin.

**Request:**
```
?type=DISCONNECT&id=UID
```

**Response:**
```
?id=UID&status=1/0
```

**Parameters:**
- `id`: Unique identifier of the sensor

**Example:**
```cpp
bool success = Protocol::disconnect("temp_sensor_01");
```

## Error Handling

All methods throw `Exception` objects on communication failures or protocol errors. Common error scenarios:

- **Protocol not initialized**: Call `Protocol::init()` first
- **Communication timeout**: Check messenger connection
- **UID mismatch**: Response UID doesn't match request UID
- **Invalid response format**: Malformed protocol message
- **Hardware error**: Sensor-specific error from hardware

```cpp
try {
    auto data = Protocol::update("sensor123");
} catch (const Exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
}
```

## Response Validation

The protocol automatically validates:

1. **UID Matching**: Response UID must match request UID
2. **Status Checking**: Status field indicates success/failure
3. **Format Validation**: Proper key-value pair structure

## Utility Methods

### Check Initialization Status
```cpp
bool isReady = Protocol::isInitialized();
```

### Get API Version
```cpp
std::string version = Protocol::getApiVersion(); // Returns "1.0"
```

## Integration

The Protocol class integrates with:

- **Messenger Interface**: Abstract communication layer
- **Parser Functions**: Message parsing utilities  
- **Exception System**: Unified error handling

## Thread Safety

⚠️ **Note**: The Protocol class uses static members and is not inherently thread-safe. Implement appropriate synchronization if using in multi-threaded environments.

## Example Communication Flow

```
1. HMI → HW: ?type=INIT&app=VirtualSensors&version=1.0&dbversion=2.3&api=1.0
2. HW → HMI: ?status=1

3. HMI → HW: ?type=CONNECT&id=temp_01&pin=5  
4. HW → HMI: ?id=temp_01&status=1

5. HMI → HW: ?type=UPDATE&id=temp_01
6. HW → HMI: ?id=temp_01&status=1&temperature=23.5&humidity=65.2

7. HMI → HW: ?type=CONFIG&id=temp_01&sample_rate=500
8. HW → HMI: ?id=temp_01&status=1
```

## Protocol Versions

- **Current API Version**: 1.0
- **Compatibility**: Backward compatible within major versions
- **Version Checking**: Automatic during initialization

---

*For implementation details, see the source files: `protocol.hpp` and `protocol.cpp` in directory `vscp`*
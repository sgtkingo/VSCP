# Virtual Sensors Communication Protocol (VSCP)

This repository contains the Virtual Sensors Communication Protocol library used
for request/response communication between an HMI/controller application and
virtual or physical sensor hardware.

- Library version: `1.5.0`
- Protocol API version: `1.4`
- Main include: `#include "vscp.hpp"`
- Implementation: `libraries/vscp/src/protocol.hpp` and `libraries/vscp/src/protocol.cpp`

This project uses the `VSCP` acronym for Virtual Sensors Communication Protocol.
It is a lightweight URL-like text protocol and is not the upstream Very Simple
Control Protocol event specification.

## Protocol Overview

VSCP messages use a query-string style wire format:

```text
?key=value&key2=value2
```

Rules:

- every request starts with `?`;
- parameters are separated by `&`;
- keys and values are separated by the first `=`;
- values are transferred as strings;
- responses contain `status=1` for success or `status=0` for failure;
- commands that target a device should return the same `id` as the request;
- values containing `&`, `=`, spaces, or special characters need an application
  level escaping/encoding convention.

The protocol is transport-agnostic. The bundled messenger layer can be adapted
to UART/Arduino, stdio, or another text-stream transport that preserves complete
messages.

## C++ API Model

`Protocol` is a static API. No instance is required.

All public command methods return `ResponseStatus`:

```cpp
struct ResponseStatus {
    ResponseStatusEnum status; // OK or ERROR
    std::string error;         // error message when status == ERROR
    std::unordered_map<std::string, std::string> params;
};
```

`params` contains additional response key/value pairs. For example, `UPDATE`
stores returned sensor values there.

Current public API methods:

```cpp
ResponseStatus Protocol::init_dummy();
ResponseStatus Protocol::init();
ResponseStatus Protocol::init(const std::string& db_version);
ResponseStatus Protocol::init(const std::string& app_name, const std::string& db_version);
ResponseStatus Protocol::connect(const std::string& uid, const std::string& pins);
ResponseStatus Protocol::disconnect(const std::string& uid);
ResponseStatus Protocol::update(const std::string& uid);
ResponseStatus Protocol::config(const std::string& uid, const std::unordered_map<std::string, std::string>& config);
ResponseStatus Protocol::control(const std::string& uid, const std::unordered_map<std::string, std::string>& control);
ResponseStatus Protocol::reset(const std::string& uid);

bool Protocol::isInitialized();
std::string Protocol::getApiVersion();
```

Basic usage:

```cpp
#include "vscp.hpp"

auto init = Protocol::init("VirtualSensors", "2.3");
if (init.status != ResponseStatusEnum::OK) {
    // init.error contains the failure reason
    return;
}

auto connected = Protocol::connect("temp_sensor_01", "5");
if (connected.status != ResponseStatusEnum::OK) {
    return;
}

auto update = Protocol::update("temp_sensor_01");
if (update.status == ResponseStatusEnum::OK) {
    auto temperature = update.params["temperature"];
    auto humidity = update.params["humidity"];
}
```

## Commands

| Command | Request | Success response | Purpose |
| --- | --- | --- | --- |
| `INIT` | `?type=INIT&app=<name>&db=<version>&api=1.4` | `?status=1` | Initialize protocol and check compatibility |
| `CONNECT` | `?type=CONNECT&id=<uid>&pins=<csv>` | `?id=<uid>&status=1` | Bind a device to one or more pins/channels |
| `DISCONNECT` | `?type=DISCONNECT&id=<uid>` | `?id=<uid>&status=1` | Remove the current device binding |
| `UPDATE` | `?type=UPDATE&id=<uid>` | `?id=<uid>&status=1&key=value...` | Read current readable sensor/device values |
| `CONFIG` | `?type=CONFIG&id=<uid>&key=value...` | `?id=<uid>&status=1` | Write configuration/profile values |
| `CONTROL` | `?type=CONTROL&id=<uid>&key=value...` | `?id=<uid>&status=1` | Write runtime output/control values |
| `RESET` | `?type=RESET&id=<uid>` | `?id=<uid>&status=1` | Reset a device or its runtime state |

## INIT

Initializes the messenger and verifies API/database compatibility with the
remote side.

Available overloads:

```cpp
ResponseStatus init_dummy();
ResponseStatus init();
ResponseStatus init(const std::string& db_version);
ResponseStatus init(const std::string& app_name, const std::string& db_version);
```

Generated requests:

```text
?type=INIT
?type=INIT&api=1.4
?type=INIT&db=DB_VERSION&api=1.4
?type=INIT&app=APP_NAME&db=DB_VERSION&api=1.4
```

Responses:

```text
?status=1
?status=0&error=API mismatch
```

Example:

```cpp
auto response = Protocol::init("VirtualSensors", "2.3");
if (response.status != ResponseStatusEnum::OK) {
    // response.error
}
```

## UPDATE

Requests current readable values from a device. Writable runtime values should
be changed with `CONTROL` and should normally not be returned by `UPDATE`.

Request:

```text
?type=UPDATE&id=UID
```

Response:

```text
?id=UID&status=1&temperature=23.5&humidity=65.2
```

Example:

```cpp
auto response = Protocol::update("temp_sensor_01");
if (response.status == ResponseStatusEnum::OK) {
    std::string temperature = response.params["temperature"];
}
```

## CONFIG

Writes configuration values. Use this for persistent settings or values that
change the device profile, not for live actuator output.

Request:

```text
?type=CONFIG&id=UID&sample_rate=1000&unit=C
```

Response:

```text
?id=UID&status=1
?id=UID&status=0&error=Invalid config value
```

Example:

```cpp
std::unordered_map<std::string, std::string> config;
config["sample_rate"] = "1000";
config["unit"] = "C";

auto response = Protocol::config("temp_sensor_01", config);
```

## CONTROL

Writes runtime control values to a device. Use this for actuator output or
writable live values, such as brightness, setpoints, motor speed, output level,
or enabled state. `CONTROL` is intentionally separate from `CONFIG`: it changes
the current runtime/output state, not the persistent device profile.

Request:

```text
?type=CONTROL&id=UID&brightness=80
?type=CONTROL&id=UID&set_point=32
```

Response:

```text
?id=UID&status=1
?id=UID&status=0&error=Value is not writable
```

Behavior expected from a device implementation:

- accept only values that the device model marks as writable/control values;
- reject read-only values with `status=0` and an `error`;
- acknowledge successful writes with the same `id` and `status=1`;
- apply value ranges, steps, and type validation in the device/application
  layer.

Example:

```cpp
std::unordered_map<std::string, std::string> control;
control["brightness"] = "80";

auto response = Protocol::control("led_01", control);
```

Hybrid device example:

```cpp
std::unordered_map<std::string, std::string> regulator;
regulator["set_point"] = "32";

auto response = Protocol::control("heater_01", regulator);
```

## RESET

Resets the specified device according to the remote implementation.

Request:

```text
?type=RESET&id=UID
```

Response:

```text
?id=UID&status=1
?id=UID&status=0&error=Reset failed
```

Example:

```cpp
auto response = Protocol::reset("temp_sensor_01");
```

## CONNECT

Connects a device to one or more hardware pins or logical channels.

Request:

```text
?type=CONNECT&id=UID&pins=5
?type=CONNECT&id=UID&pins=5,6,7
```

Response:

```text
?id=UID&status=1
?id=UID&status=0&error=Pin conflict
```

Example:

```cpp
auto response = Protocol::connect("temp_sensor_01", "5");
auto multi = Protocol::connect("heater_01", "3,5,6");
```

## DISCONNECT

Disconnects a device from its current pin/channel mapping.

Request:

```text
?type=DISCONNECT&id=UID
```

Response:

```text
?id=UID&status=1
?id=UID&status=0&error=Device not connected
```

Example:

```cpp
auto response = Protocol::disconnect("temp_sensor_01");
```

## Error Handling

Protocol command failures are reported through `ResponseStatus`, not by throwing
from the command methods themselves.

Common protocol-level errors:

- protocol was not initialized before a device command;
- empty `id`;
- missing or malformed `status`;
- response `id` does not match the request `id`;
- remote side returned `status=0`;
- API or database version mismatch;
- timeout or incomplete transport response.

Transport and messenger functions may still throw lower-level exceptions if the
underlying I/O fails.

Example:

```cpp
auto response = Protocol::update("sensor123");
if (response.status == ResponseStatusEnum::ERROR) {
    std::string reason = response.error;
}
```

## Validation Performed by the Library

The current implementation validates:

- the protocol is initialized before device commands;
- `uid`/`id` is not empty;
- response contains the same `id` as the request for device commands;
- response contains `status=1` before returning `OK`;
- response `error` is propagated when the remote side reports failure.

## Utility Methods

```cpp
bool ready = Protocol::isInitialized();
std::string api = Protocol::getApiVersion(); // "1.4"
```

## Example Communication Flow

```text
1. HMI -> HW: ?type=INIT&app=VirtualSensors&db=2.3&api=1.4
2. HW  -> HMI: ?status=1

3. HMI -> HW: ?type=CONNECT&id=temp_01&pins=5
4. HW  -> HMI: ?id=temp_01&status=1

5. HMI -> HW: ?type=UPDATE&id=temp_01
6. HW  -> HMI: ?id=temp_01&status=1&temperature=23.5&humidity=65.2

7. HMI -> HW: ?type=CONFIG&id=temp_01&sample_rate=500
8. HW  -> HMI: ?id=temp_01&status=1

9. HMI -> HW: ?type=CONTROL&id=led_01&brightness=80
10. HW -> HMI: ?id=led_01&status=1
```

## Configuration Notes

Protocol behavior is controlled from `libraries/vscp/src/config.hpp`.

Important defaults:

- `MAX_PROTOCOL_REQUEST_SIZE`: `1024`
- `PROTOCOL_VERBOSE`: `1`
- `PROTOCOL_INIT_TIMEOUT`: `500`
- `CASE_SENSITIVE`: `true`
- default API version in code: `1.4`

## Compatibility

Version `1.4` / library `1.5.0` documents the current protocol surface:

- command methods return `ResponseStatus` instead of returning raw maps/bools;
- `INIT` uses `db`, not `dbversion`;
- `CONNECT` uses `pins`, not `pin`, and accepts comma-separated values;
- `CONTROL` is a separate command for runtime write/control values;
- `getApiVersion()` returns `1.4`.

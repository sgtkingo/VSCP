//Put all files from libraries to the same folder as program
//#include <Arduiono.h>
#include <vscp.hpp>
#include <expt.hpp>

#define LOG_LEVEL 2

const String AGENT_NAME = "Agent ESP32"; // For ESP32
//const String AGENT_NAME = "Agent HMI"; // For HMI

void start() {
  // put your setup code here, to run once:
  sendMessageAsString("Hello from: " + AGENT_NAME);
}

void setup() {
  // put your setup code here, to run once:
  initLogger();

  //initMessenger(); //Use deafult settings
  //initMessenger(115200, SERIAL_8N1, 16, 17, 2); // For ESP32
  initMessenger(115200, SERIAL_8N1, 18, 17, 2); // For ESP32-S3
  //initMessenger(115200, SERIAL_8N1, -1, -1, 0); // For HMI

  start();
  logMessage("ESP32 Tester started");
}

String listen(const String filter = "") {
  // Reciever mode
  String input = receiveMessageAsString(LOG_LEVEL);
  int index = input.indexOf(filter);
  if (input.length() > 0) {
    // apply filter if provided
    if (!filter.isEmpty()) {
      if (index < 0) {
        if (LOG_LEVEL > 0)
          logMessage("Got messege but it does not match filter '%s': %s", filter.c_str(), input.c_str());
        return "";
      }
      else
      {
        input = input.substring(index);
      }
    }

    if (LOG_LEVEL > 0)
      logMessage("Received: %s", input.c_str());
    return input;
  }
  return "";
}

void reply(const String& msg) {
  // Transmitter mode
  sendMessageAsString(msg, LOG_LEVEL);
}

void loop() {
  // Reciever mode
  String input = listen("?type=INIT");
  delay(5);
  if (input.length() > 0) {
    reply("?status=1");
  }

  // Transmitter mode
  delay(100);
  //logMessage("Pooling...");
}
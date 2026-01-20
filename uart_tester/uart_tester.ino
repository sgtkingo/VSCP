//Put all files from libraries to the same folder as program
//#include <Arduiono.h>
#include <vscp.hpp>
#include <expt.hpp>

const String AGENT_NAME = "Agent ESP2";

void start() {
  // put your setup code here, to run once:
  sendMessage("Hello from: " + AGENT_NAME);
}

void setup() {
  // put your setup code here, to run once:
  initLogger();

  //initMessenger(); //Use deafult settings
  initMessenger(9600, SERIAL_8N1, 16, 17, 2); // For ESP32
  //initMessenger(115200, SERIAL_8N1, -1, -1, 0);

  start();
  logMessage("ESP32 Tester started");
}

String listen() {
  // Reciever mode
  String input = receiveMessageAsString();
  if (input.length() > 0) {
    logMessage("Received: %s", input.c_str());
    return input;
  }
  return "";
}

void reply(const String& msg) {
  // Transmitter mode
  String response = "Echo: "+ AGENT_NAME + ": " + msg;
  sendMessage(response);
  logMessage("Sent: %s", response.c_str());
}

void loop() {
  // Reciever mode
  String input = listen();
  if (input.length() > 0) {
    // Echo back the received message
    reply("here!");
  }

  // Transmitter mode
  //sendMessage("Hello from ESP32 UART tester!");
  delay(1000);
  logMessage("Pooling...");
}

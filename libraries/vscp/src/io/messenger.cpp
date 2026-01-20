/**
 * @file messenger.cpp
 * @brief Definition of the messenger interface and related global functions.
 * 
 * This header defines the global functions for message operations. It includes configuration
 * and exception handling support..
 * 
 * @copyright 2024 MTA
 * @author 
 * Ing. Jiri Konecny
 */
#include "messenger.hpp"

#ifdef ARDUINO_H
    #include <Arduino.h>  ///< Include Arduino 
    #include <HardwareSerial.h> ///< Include Arduino Serial functions

    HardwareSerial UART1_VIRTUAL(UART1_PORT);
    static bool uart1_initialized = false;

    String stripMessage(const String &input, bool trim = true) {
        String out = "";
        out.reserve(input.length());

        for (size_t i = 0; i < input.length(); i++) {
            char c = input[i];

            // tisknutelné ASCII = 32 až 126
            if (c >= 32 && c <= 126) {
                out += c;
            }
        }

        if (trim) {
            out.trim(); // Remove leading/trailing whitespace
        }
        
        return out;
    }

    void sendMessage(const char* message, int verbose, bool strip) {
        sendMessageAsString(String(message), verbose, strip);
    }

    void sendMessage(const std::string &message, int verbose, bool strip) {
        sendMessageAsString(String(message.c_str()), verbose, strip);
    }
    
    void sendMessageAsString(const String &message, int verbose, bool strip) {
        if(!uart1_initialized){
            initMessenger();
        }

        //strip message before sending
        String prepMessage = message;
        if (strip) 
            prepMessage = stripMessage(message, true);

        if (verbose >= 2) {
            Serial.print("[SEND] ");
            Serial.println(prepMessage);
        }

        UART1_VIRTUAL.println(prepMessage);
    }

    String receiveMessageAsString(int verbose, int timeout, bool strip) {
        String msg = ""; // static so it persists between calls

        if(!uart1_initialized){
            initMessenger();
        }

        msg = UART1_VIRTUAL.readStringUntil('\n');
        if (strip)
            msg = stripMessage(msg, true);

        if (msg.length()==0 && verbose>0) {
            Serial.println("[RECV] No message received (timeout?)");
        }

        if (verbose >= 2) {
            Serial.print("[RECV] ");
            Serial.println(msg);
        }

        return msg;
    }

    const char* receiveMessageAsChars(int verbose, int timeout, bool strip) {
        String msg = receiveMessageAsString(verbose, timeout, strip);
        return msg.c_str();
    }
    
    std::string receiveMessage(int verbose, int timeout, bool strip) {
        String msg = receiveMessageAsString(verbose, timeout, strip);
        return std::string(msg.c_str());
    }

    bool initMessenger(unsigned long baudrate, unsigned int mode, int rx, int tx, unsigned int port) {
        UART1_VIRTUAL.end(); // End if already initialized
        UART1_VIRTUAL = HardwareSerial(port);
        UART1_VIRTUAL.begin(baudrate, mode, rx, tx);
        UART1_VIRTUAL.setTimeout(UART1_TIMEOUT);
        return uart1_initialized = true;
    }

    bool initMessenger() {
        return initMessenger(UART1_BAUDRATE, SERIAL_8N1, UART1_RX, UART1_TX, UART1_PORT);
    }

#elif defined(STDIO_H)
    #include <stdio.h>    ///< Include standard I/O functions

    void sendMessage(const std::string &message) {
        printf("%s\n", message.c_str());
    }

    std::string receiveMessage(int timeout, int verbose) {
        char buffer[256];
        scanf("%s", buffer);
        return std::string(buffer);
    }

    bool initMessenger() {
        // No initialization needed for standard I/O
        return true;
    }

#else
    #error "No valid platform defined. Please define ARDUINO_H or STDIO_H in config.hpp"
    
#endif
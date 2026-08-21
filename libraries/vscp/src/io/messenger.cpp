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
#include <expt.hpp>

#if defined(ARDUINO_H_ENV) && defined(ARDUINO)
    #include <Arduino.h>  ///< Include Arduino 
    #include <HardwareSerial.h> ///< Include Arduino Serial functions

    HardwareSerial UART1_VIRTUAL(UART1_PORT);
    static bool uart1_initialized = false;
    static String receive_message_buffer;

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

        debugLogMessage("sendMessageAsString", "protocol io write", "%s", prepMessage.c_str());

        UART1_VIRTUAL.print('\n');
        UART1_VIRTUAL.println(prepMessage);
        UART1_VIRTUAL.flush();
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
            debugLogMessage("receiveMessageAsString", "protocol io read timeout", "timeout=%d", timeout);
        }

        debugLogMessage("receiveMessageAsString", "protocol io read", "%s", msg.c_str());

        return msg;
    }

    const char* receiveMessageAsChars(int verbose, int timeout, bool strip) {
        // Keep the storage alive after the function returns for C-string callers.
        receive_message_buffer = receiveMessageAsString(verbose, timeout, strip);
        return receive_message_buffer.c_str();
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
        debugLogMessage("initMessenger", "protocol io init", "baudrate=%lu port=%u rx=%d tx=%d timeout=%d", baudrate, port, rx, tx, UART1_TIMEOUT);
        return uart1_initialized = true;
    }

    bool initMessenger() {
        return initMessenger(UART1_BAUDRATE, SERIAL_8N1, UART1_RX, UART1_TX, UART1_PORT);
    }

#elif defined(STDIO_H_ENV)
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
    #error "No valid platform defined. Please define ARDUINO_H_ENV or STDIO_H_ENV in config.hpp"
    
#endif

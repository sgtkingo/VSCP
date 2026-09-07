import serial
import time
import random
import traceback

# Inicializace sériového portu
ser = serial.Serial(
    port='COM3',
    baudrate=115200,
    timeout=0.1
)

if not ser.is_open:
    print("COM3 port is not open, opening...")
    ser.open()

def send_message(message):
    print(f"Message sended to bus: {message}")
    ser.write(message.encode("utf-8"))

def is_firmware_log_line(line):
    upper_line = line.upper()
    return "DEBUG" in upper_line or "WARNING" in upper_line or "WARN:" in upper_line or "EXCEPTION" in upper_line

def find_vscp_request_start(line):
    return line.lower().find("?type=")

try:
    print("Emulator started, waiting for commands...")
    while True:
        res = ser.readline()
        if res:
            decoded = res.decode('utf-8', errors='ignore').strip()
            if is_firmware_log_line(decoded):
                request_index = find_vscp_request_start(decoded)
                if request_index == -1:
                    print(f"Firmware log: {decoded}")
                    continue

                log_line = decoded[:request_index].strip()
                if log_line:
                    print(f"Firmware log: {log_line}")
                decoded = decoded[request_index:].strip()

            print(f"Message received from bus: {decoded}")
            if "update" in decoded.lower():
                print("Update command received.")
                send_message("?id=0&status=1&temp=100&alarm=0")
            elif "config" in decoded.lower():
                print("Config command received.")
                send_message("?id=0&status=1")
            elif "connect" in decoded.lower():
                print("Connect command received.")
                send_message("?id=0&status=1")
            elif "disconnect" in decoded.lower():
                print("Disconnect command received.")
                send_message("?id=0&status=1")
        time.sleep(0.05)

except KeyboardInterrupt:
    print("Comm closing...")
except Exception as exc:
    print(f"EXCEPTION: com_reader failed reason={exc} source=com_reader")
    traceback.print_exc()

finally:
    ser.close()

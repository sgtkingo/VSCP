#!/usr/bin/env python3
"""
Virtual Sensors Protocol (VSCP) Emulator
========================================

Reactive client emulator that implements the complete VSCP communication protocol
as specified in vscp.hpp. Handles all protocol methods with dummy responses.

Protocol Methods:
- INIT: Handshake and version compatibility check
- UPDATE: Sensor data update requests
- CONFIG: Configuration changes from HMI to HW
- RESET: Sensor reset operations
- CONNECT: Connect sensor to specific pin
- DISCONNECT: Disconnect sensor from pin

Protocol Format: URL-like with key-value pairs
Request: ?type=METHOD&param1=value1&param2=value2
Response: ?status=1/0&param1=value1&error=message

Author: Generated for VSCP Protocol Testing
"""

import serial
import time
import random
import threading
import re
from urllib.parse import parse_qs, unquote
from typing import Dict, Any, Optional

class VSCPEmulator:
    """Virtual Sensors Communication Protocol Emulator"""
    
    def __init__(self, port='COM3', baudrate=115200, timeout=0.1):
        """Initialize the VSCP emulator"""
        self.API_VERSION = "1.2"
        self.DB_VERSION = "1.0.0"
        self.APP_NAME = "VSCP Emulator"
        self.APP_VERSION = "1.0.0"
        
        # Protocol state
        self.initialized = False
        self.connected_sensors = {}  # uid -> pin mapping
        self.sensor_configs = {}     # uid -> config dict
        
        # Serial connection
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.ser = None
        self.running = False
        
        # Dummy sensor data
        self.sensor_data = {
            "sensor_001": {"Temperature": 25.5, "Humidity": 60.2, "type": "DHT22"},
            "sensor_002": {"Distance": 150, "type": "Ultrasonic"},
            "sensor_003": {"Pressure": 1013.25, "Temperature": 22.1, "type": "BMP280"},
            "sensor_004": {"X": 45, "Y": 78, "Button": 0, "type": "Joystick"},
            "sensor_005": {"Lux": 350, "type": "Light"},
            "sensor_006": {"MagField": 12.5, "Detected": 0, "type": "Magnetic"},
            "imu_001": {
                "acm_x": -2.1, "acm_y": 0.8, "acm_z": 9.8,
                "gyr_x": 0.05, "gyr_y": -0.02, "gyr_z": 0.01,
                "type": "IMU"
            }
        }
        
    def connect_serial(self) -> bool:
        """Connect to serial port"""
        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=self.timeout
            )
            if not self.ser.is_open:
                self.ser.open()
            print(f"✓ Connected to {self.port} at {self.baudrate} baud")
            return True
        except Exception as e:
            print(f"✗ Failed to connect to {self.port}: {e}")
            return False
    
    def disconnect_serial(self):
        """Disconnect from serial port"""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("✓ Serial connection closed")
    
    def parse_message(self, message: str) -> Dict[str, str]:
        """Parse protocol message into key-value pairs"""
        params = {}
        
        # Remove leading '?' if present
        clean_message = message.strip()
        if clean_message.startswith('?'):
            clean_message = clean_message[1:]
        
        # Split by '&' and parse key=value pairs
        if clean_message:
            pairs = clean_message.split('&')
            for pair in pairs:
                if '=' in pair:
                    key, value = pair.split('=', 1)
                    params[key.strip()] = unquote(value.strip())
        
        return params
    
    def build_message(self, params: Dict[str, Any]) -> str:
        """Build protocol message from key-value pairs"""
        if not params:
            return "?status=0&error=No parameters"
        
        parts = []
        for key, value in params.items():
            parts.append(f"{key}={value}")
        
        return "?" + "&".join(parts)
    
    def handle_init(self, params: Dict[str, str]) -> str:
        """Handle INIT method - handshake and version check"""
        print(f"🔄 INIT request: {params}")
        
        # Extract parameters
        app = params.get('app', 'Unknown')
        dbversion = params.get('db', '0.0.0')
        api = params.get('api', '0.0.0')
        
        # Simulate version compatibility check
        response_params = {}
        
        if api == self.API_VERSION and dbversion == self.DB_VERSION:
            self.initialized = True
            response_params = {
                'status': '1',
                'message': f'Initialized with {app}'
            }
            print(f"✓ Initialization successful for {app}")
        else:
            response_params = {
                'status': '0',
                'error': f'Version mismatch - API:{api} (need {self.API_VERSION}), DB:{dbversion} (need {self.DB_VERSION})'
            }
            print(f"✗ Version mismatch: API {api}, DB {dbversion}")
        
        return self.build_message(response_params)
    
    def handle_update(self, params: Dict[str, str]) -> str:
        """Handle UPDATE method - return sensor data"""
        uid = params.get('id', '')
        print(f"📊 UPDATE request for sensor: {uid}")
        
        if not self.initialized:
            return self.build_message({'status': '0', 'error': 'Protocol not initialized'})
        
        if uid in self.sensor_data:
            # Get sensor data and add status
            sensor_info = self.sensor_data[uid].copy()
            
            # Add some random variation to make it realistic
            for key, value in sensor_info.items():
                if key != 'type' and isinstance(value, (int, float)):
                    if isinstance(value, int):
                        sensor_info[key] = value + random.randint(-2, 2)
                    else:
                        sensor_info[key] = round(value + random.uniform(-0.5, 0.5), 2)
            
            response_params = {'id': uid, 'status': '1'}
            response_params.update(sensor_info)
            
            print(f"✓ Sensor {uid} data: {sensor_info}")
        else:
            response_params = {
                'id': uid,
                'status': '0',
                'error': f'Sensor {uid} not found'
            }
            print(f"✗ Sensor {uid} not found")
        
        return self.build_message(response_params)
    
    def handle_config(self, params: Dict[str, str]) -> str:
        """Handle CONFIG method - configure sensor"""
        uid = params.get('id', '')
        print(f"⚙️  CONFIG request for sensor: {uid}")
        
        if not self.initialized:
            return self.build_message({'status': '0', 'error': 'Protocol not initialized'})
        
        # Extract configuration parameters (exclude 'type' and 'id')
        config_params = {k: v for k, v in params.items() if k not in ['type', 'id']}
        
        if uid:
            # Store configuration
            self.sensor_configs[uid] = config_params
            response_params = {
                'id': uid,
                'status': '1',
                'message': f'Configuration applied: {config_params}'
            }
            print(f"✓ Sensor {uid} configured: {config_params}")
        else:
            response_params = {
                'id': uid,
                'status': '0',
                'error': 'Invalid sensor ID'
            }
            print(f"✗ Invalid sensor ID: {uid}")
        
        return self.build_message(response_params)
    
    def handle_reset(self, params: Dict[str, str]) -> str:
        """Handle RESET method - reset sensor"""
        uid = params.get('id', '')
        print(f"🔄 RESET request for sensor: {uid}")
        
        if not self.initialized:
            return self.build_message({'status': '0', 'error': 'Protocol not initialized'})
        
        if uid in self.sensor_data or uid == 'all':
            # Reset sensor(s)
            if uid == 'all':
                self.sensor_configs.clear()
                self.connected_sensors.clear()
                print("✓ All sensors reset")
            else:
                self.sensor_configs.pop(uid, None)
                self.connected_sensors.pop(uid, None)
                print(f"✓ Sensor {uid} reset")
            
            response_params = {
                'id': uid,
                'status': '1'
            }
        else:
            response_params = {
                'id': uid,
                'status': '0',
                'error': f'Sensor {uid} not found'
            }
            print(f"✗ Sensor {uid} not found for reset")
        
        return self.build_message(response_params)
    
    def handle_connect(self, params: Dict[str, str]) -> str:
        """Handle CONNECT method - connect sensor to pin"""
        uid = params.get('id', '')
        pin = params.get('pin', '')
        print(f"🔌 CONNECT request: sensor {uid} to pin {pin}")
        
        if not self.initialized:
            return self.build_message({'status': '0', 'error': 'Protocol not initialized'})
        
        if uid and pin:
            try:
                pin_num = int(pin)
                # Check if pin is already used
                used_by = None
                for sensor_id, used_pin in self.connected_sensors.items():
                    if used_pin == pin_num:
                        used_by = sensor_id
                        break
                
                if used_by and used_by != uid:
                    response_params = {
                        'id': uid,
                        'status': '0',
                        'error': f'Pin {pin} already used by sensor {used_by}'
                    }
                    print(f"✗ Pin {pin} conflict: used by {used_by}")
                else:
                    self.connected_sensors[uid] = pin_num
                    response_params = {
                        'id': uid,
                        'status': '1',
                        'pin': pin
                    }
                    print(f"✓ Sensor {uid} connected to pin {pin}")
                    
            except ValueError:
                response_params = {
                    'id': uid,
                    'status': '0',
                    'error': f'Invalid pin number: {pin}'
                }
                print(f"✗ Invalid pin number: {pin}")
        else:
            response_params = {
                'id': uid,
                'status': '0',
                'error': 'Missing sensor ID or pin number'
            }
            print(f"✗ Missing parameters: uid={uid}, pin={pin}")
        
        return self.build_message(response_params)
    
    def handle_disconnect(self, params: Dict[str, str]) -> str:
        """Handle DISCONNECT method - disconnect sensor from pin"""
        uid = params.get('id', '')
        print(f"🔌 DISCONNECT request for sensor: {uid}")
        
        if not self.initialized:
            return self.build_message({'status': '0', 'error': 'Protocol not initialized'})
        
        if uid in self.connected_sensors:
            pin = self.connected_sensors.pop(uid)
            response_params = {
                'id': uid,
                'status': '1',
                'pin': str(pin)
            }
            print(f"✓ Sensor {uid} disconnected from pin {pin}")
        else:
            response_params = {
                'id': uid,
                'status': '0',
                'error': f'Sensor {uid} not connected'
            }
            print(f"✗ Sensor {uid} not connected")
        
        return self.build_message(response_params)
    
    def process_request(self, message: str) -> str:
        """Process incoming protocol request"""
        params = self.parse_message(message)
        request_type = params.get('type', '').upper()
        
        # Route to appropriate handler
        handlers = {
            'INIT': self.handle_init,
            'UPDATE': self.handle_update,
            'CONFIG': self.handle_config,
            'RESET': self.handle_reset,
            'CONNECT': self.handle_connect,
            'DISCONNECT': self.handle_disconnect
        }
        
        if request_type in handlers:
            return handlers[request_type](params)
        else:
            return self.build_message({
                'status': '0',
                'error': f'Unknown request type: {request_type}'
            })
    
    def listen_loop(self):
        """Main listening loop for incoming requests"""
        print("🎧 Listening for protocol requests...")
        buffer = ""
        
        while self.running:
            try:
                if self.ser and self.ser.in_waiting > 0:
                    data = self.ser.read(self.ser.in_waiting).decode('utf-8', errors='ignore')
                    buffer += data
                    
                    # Process complete messages (ending with newline or containing '?')
                    while '\n' in buffer or '?' in buffer:
                        if '\n' in buffer:
                            line, buffer = buffer.split('\n', 1)
                        else:
                            # If no newline but contains '?', process the whole buffer
                            line = buffer
                            buffer = ""
                        
                        line = line.strip()
                        if line and line.startswith('?'):
                            print(f"📨 Received: {line}")
                            response = self.process_request(line)
                            
                            # Send response
                            if response:
                                self.ser.write((response + '\n').encode('utf-8'))
                                print(f"📤 Sent: {response}")
                
                time.sleep(0.01)  # Small delay to prevent busy waiting
                
            except Exception as e:
                print(f"❌ Error in listen loop: {e}")
                time.sleep(0.1)
    
    def run(self):
        """Start the emulator"""
        print("🚀 Starting VSCP Emulator...")
        print(f"   API Version: {self.API_VERSION}")
        print(f"   DB Version: {self.DB_VERSION}")
        print(f"   Available sensors: {list(self.sensor_data.keys())}")
        
        if not self.connect_serial():
            return
        
        self.running = True
        
        # Start listening thread
        listen_thread = threading.Thread(target=self.listen_loop, daemon=True)
        listen_thread.start()
        
        try:
            print("\n💡 Emulator ready! Send protocol requests to test.")
            print("   Example: ?type=INIT&app=TestApp&version=1.0.0&dbversion=1.0.0&api=1.0")
            print("   Press Ctrl+C to stop\n")
            
            # Keep main thread alive
            while True:
                time.sleep(1)
                
        except KeyboardInterrupt:
            print("\n🛑 Shutting down emulator...")
        
        finally:
            self.running = False
            self.disconnect_serial()

def main():
    """Main entry point"""
    emulator = VSCPEmulator(port='COM5', baudrate=115200)
    emulator.run()

if __name__ == "__main__":
    main()

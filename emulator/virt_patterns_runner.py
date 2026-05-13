from engine.emulator import available_serial_ports
from engine.emulator_patterns import VSCPEmulator
import traceback

def main():
    """Main entry point"""
    
    # Get available COM ports
    available_ports = available_serial_ports()
    
    if available_ports:
        default_port = available_ports[0]
        print(f"Available COM ports: {', '.join(available_ports)}")
    else:
        default_port = 'COM8'
        print("No COM ports detected, using default COM8")
    
    port = input(f"Enter serial port (default: {default_port}): ").strip()
    if not port:
        port = default_port
    try:
        emulator = VSCPEmulator(port=port, baudrate=115200)
        emulator.run()
    except Exception as exc:
        print(f"EXCEPTION: virt_patterns_runner failed reason={exc} source=virt_patterns_runner")
        traceback.print_exc()

if __name__ == "__main__":
    main()

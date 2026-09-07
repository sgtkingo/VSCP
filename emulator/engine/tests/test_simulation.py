#!/usr/bin/env python3
"""
Test script to demonstrate the enhanced simulation capabilities
This script shows how the simulation generates realistic sensor data patterns
"""

import time
import matplotlib.pyplot as plt
from emulator.engine.emulator_patterns import VSCPEmulator

def test_simulation_patterns():
    """Test and visualize the enhanced simulation patterns"""
    
    # Create emulator instance (without serial connection for testing)
    emulator = VSCPEmulator()
    
    # Test data collection
    test_duration = 300  # 5 minutes
    sample_interval = 1  # 1 second
    samples = test_duration // sample_interval
    
    # Data storage for plotting
    time_points = []
    sensor_data = {
        "S00_temp": [],
        "S01_humi": [],
        "S02_Pressure": [],
        "S15_intensity": [],
        "imu_001_acm_x": [],
        "mic_001_dBFS": [],
        "cpu_temp_temp": []
    }
    
    print("🧪 Testing enhanced simulation patterns...")
    print(f"   Duration: {test_duration} seconds")
    print(f"   Samples: {samples}")
    
    start_time = time.time()
    
    for i in range(samples):
        current_time = time.time()
        elapsed = current_time - start_time
        time_points.append(elapsed)
        
        # Update scenario periodically during test
        if i % 60 == 0:  # Every 60 samples
            emulator.adjust_simulation_scenario()
            print(f"   📊 {elapsed:.0f}s - Mode: {emulator.simulation_mode}")
        
        # Sample various sensors
        for sensor_id, param in [
            ("S00", "temp"),
            ("S01", "humi"), 
            ("S02", "Pressure"),
            ("S15", "intensity"),
            ("imu_001", "acm_x"),
            ("mic_001", "dBFS"),
            ("cpu_temp", "temp")
        ]:
            if sensor_id in emulator.sensor_data and param in emulator.sensor_data[sensor_id]:
                config = emulator.sensor_data[sensor_id][param]
                if isinstance(config, dict) and 'base' in config:
                    value = emulator.simulate_realistic_value(sensor_id, param, config)
                    key = f"{sensor_id}_{param}"
                    sensor_data[key].append(value)
                else:
                    sensor_data[f"{sensor_id}_{param}"].append(config)
        
        time.sleep(0.1)  # Small delay to simulate real-time sampling
    
    print("✅ Simulation test completed!")
    
    # Create visualization
    fig, axes = plt.subplots(3, 3, figsize=(15, 12))
    fig.suptitle('Enhanced Sensor Simulation - Realistic Patterns', fontsize=16)
    
    plot_configs = [
        ("S00_temp", "Temperature (°C)", 0, 0, 'red'),
        ("S01_humi", "Humidity (%)", 0, 1, 'blue'),
        ("S02_Pressure", "Pressure (hPa)", 0, 2, 'green'),
        ("S15_intensity", "Light Intensity", 1, 0, 'orange'),
        ("imu_001_acm_x", "Acceleration X (m/s²)", 1, 1, 'purple'),
        ("mic_001_dBFS", "Audio Level (dBFS)", 1, 2, 'brown'),
        ("cpu_temp_temp", "CPU Temperature (°C)", 2, 0, 'pink')
    ]
    
    for sensor, title, row, col, color in plot_configs:
        if sensor in sensor_data and len(sensor_data[sensor]) > 0:
            axes[row, col].plot(time_points, sensor_data[sensor], color=color, linewidth=1)
            axes[row, col].set_title(title)
            axes[row, col].set_xlabel('Time (s)')
            axes[row, col].grid(True, alpha=0.3)
            axes[row, col].tick_params(labelsize=8)
    
    # Hide unused subplot
    axes[2, 1].axis('off')
    axes[2, 2].axis('off')
    
    plt.tight_layout()
    plt.savefig('simulation_patterns.png', dpi=150, bbox_inches='tight')
    print("📊 Visualization saved as 'simulation_patterns.png'")
    plt.show()

def demonstrate_scenario_effects():
    """Demonstrate how different scenarios affect sensor data"""
    
    emulator = VSCPEmulator()
    
    scenarios = ["normal", "high_activity", "thermal_event", "environmental", "vibration", "recovery"]
    sensor_id = "S00"
    param = "temp"
    config = emulator.sensor_data[sensor_id][param]
    
    print("🎭 Demonstrating scenario effects on temperature sensor...")
    
    for scenario in scenarios:
        emulator.simulation_mode = scenario
        values = []
        
        # Generate 20 samples for each scenario
        for _ in range(20):
            value = emulator.simulate_realistic_value(sensor_id, param, config)
            values.append(value)
        
        avg_val = sum(values) / len(values)
        min_val = min(values)
        max_val = max(values)
        
        print(f"   {scenario:15} | Avg: {avg_val:6.2f} | Range: {min_val:6.2f} - {max_val:6.2f}")

if __name__ == "__main__":
    print("🚀 Enhanced Simulation Test Suite")
    print("=" * 50)
    
    try:
        demonstrate_scenario_effects()
        print("\n" + "=" * 50)
        
        # Only run the full test if matplotlib is available
        try:
            import matplotlib.pyplot as plt
            test_simulation_patterns()
        except ImportError:
            print("📊 Matplotlib not available - skipping visualization test")
            print("   To see graphs: pip install matplotlib")
    
    except KeyboardInterrupt:
        print("\n🛑 Test interrupted by user")
    except Exception as e:
        print(f"❌ Test error: {e}")
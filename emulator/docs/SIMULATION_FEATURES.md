# Enhanced VSCP Emulator - Realistic Sensor Simulation

## Overview

The VSCP emulator has been significantly enhanced to provide realistic sensor data patterns that mimic real-world behavior. Instead of simple random variations, the emulator now generates sophisticated data patterns with:

## 🎯 New Simulation Features

### 1. **Multi-layered Data Generation**
- **Base Values**: Configurable baseline values for each sensor parameter
- **Oscillations**: Multiple frequency components (slow and fast cycles)
- **Trends**: Gradual drift over time with automatic bounds prevention
- **Environmental Patterns**: Sensor-specific realistic behaviors

### 2. **Peak and Dropout Events**
- **Gaussian Peaks**: Realistic spike patterns with natural rise/fall curves
- **Dropouts**: Sudden temporary value drops simulating sensor failures
- **Spike Countdown**: Randomized timing for natural peak distribution
- **Configurable Duration**: Variable peak and dropout lengths

### 3. **Environmental Modeling**
- **Temperature Sensors**: Daily temperature cycles + weather patterns
- **Humidity Sensors**: Inverse correlation with temperature
- **Pressure Sensors**: Barometric pressure variations with weather systems
- **Light Sensors**: Day/night cycles with cloud cover simulation
- **Audio Sensors**: Burst patterns and ambient noise levels

### 4. **Dynamic Scenarios**
The emulator cycles through different operational scenarios every 60 seconds:

#### Normal Mode
- Standard operation with baseline noise and patterns

#### High Activity Mode
- Increased sensor activity across all devices
- Higher noise levels and more frequent spikes
- Simulates busy operational periods

#### Thermal Event Mode
- Temperature-related sensors show elevated readings
- Affects: S00, S01, S02, cpu_temp sensors
- Simulates heating events or thermal stress

#### Environmental Disturbance Mode
- Pressure and light sensors affected
- Simulates weather disturbances or environmental changes
- Causes temporary dropouts and instability

#### Vibration Mode
- IMU and magnetic sensors show increased activity
- Simulates mechanical vibrations or magnetic interference
- High-frequency noise and frequent spikes

#### Recovery Mode
- Gradual return to normal after disturbances
- Reduced activity levels and stabilizing patterns

### 5. **Sensor-Specific Behaviors**

#### Binary Sensors (Button, Detected)
- Scenario-dependent activation rates
- 10% base activation, 30% during high activity

#### Bounded Sensors
- **Humidity**: Hard-limited to 0-100%
- **Pressure**: Realistic atmospheric range (900-1100 hPa)
- **Light**: Cannot go negative (absolute values)

#### Correlated Sensors
- Temperature and humidity show inverse relationship
- Related sensors respond to common environmental factors

## 🔧 Configuration Structure

Each sensor parameter now uses this enhanced configuration:

```python
"sensor_param": {
    "base": 25.5,      # Baseline value
    "range": 10,       # Maximum variation range
    "trend": 0.1,      # Long-term drift rate
    "noise": 0.5       # Noise level multiplier
}
```

## 📊 Data Patterns Generated

1. **Smooth Oscillations**: Natural wave patterns at multiple frequencies
2. **Random Walk**: Trending behavior with bounds checking
3. **Spike Events**: Realistic peak shapes with exponential decay
4. **Noise Layers**: Gaussian noise with occasional glitches
5. **Environmental Cycles**: Daily/seasonal patterns where appropriate
6. **Correlated Behavior**: Related sensors influence each other

## 🚀 Usage

The enhanced emulator works with the same VSCP protocol:

```
?type=UPDATE&id=S00
```

Returns realistic data like:
```
?id=S00&status=1&temp=26.34&alarm=61.23&type=DHT22
```

## 📈 Testing and Validation

Use the included `test_simulation.py` script to:
- Visualize simulation patterns over time
- Compare different scenario effects
- Validate realistic sensor behavior
- Generate test data for development

## 🎭 Benefits for Development

1. **Realistic Testing**: Applications can be tested with real-world data patterns
2. **Edge Case Coverage**: Automatic generation of peaks, dropouts, and anomalies
3. **Scenario Testing**: Different operational conditions simulated automatically
4. **Performance Validation**: Test how systems handle varying data loads
5. **Algorithm Development**: Train and test data processing algorithms with realistic inputs

## 🔍 Monitoring

The emulator provides runtime feedback:
- Current simulation mode displayed every 30 seconds
- Scenario transitions logged in real-time
- Individual sensor responses shown for each update request

This enhanced simulation provides a much more realistic testing environment for virtual sensor systems, allowing developers to validate their applications against real-world data patterns and edge cases.
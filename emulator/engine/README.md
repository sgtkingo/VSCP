# VSCP Emulator – Real HW Sensor Extensions

## Install
```bash
pip install sounddevice opencv-python psutil numpy wmi
```

## Run
```bash
cd vscp_emulator_real_sensors
python real_runner.py
```

Then send commands like `?type=UPDATE&id=mic_001` to retrieve live values.

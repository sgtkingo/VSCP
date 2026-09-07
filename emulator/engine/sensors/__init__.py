from .base import Sensor
from .microphone import MicrophoneSensor
from .camera import CameraSensor
from .cpu_temp import CpuTempSensor

def load_sensors():
    sensors = []
    # Create what you have available; constructors auto-disable when hw/lib is missing
    sensors.append(MicrophoneSensor(uid="mic_001"))
    sensors.append(CameraSensor(uid="cam_001", index=0))
    sensors.append(CpuTempSensor(uid="cpu_temp"))
    return [s for s in sensors if s.available]

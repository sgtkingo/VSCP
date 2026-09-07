import platform
from .base import Sensor

try:
    import psutil
    _HAVE_PSUTIL = True
except Exception:
    print("\t(!) psutil import failed")
    _HAVE_PSUTIL = False
    
try:
    import wmi
    _HAVE_WMI = True
except Exception:
    print("\t(!) wmi import failed")
    _HAVE_WMI = False

class CpuTempSensor(Sensor):
    kind = "Temperature"

    def __init__(self, uid: str):
        super().__init__(uid)
        self.min_period = 1.0
        self.available = _HAVE_PSUTIL or _HAVE_WMI

    def _read_temp(self):
        if _HAVE_PSUTIL:
            temps = getattr(psutil, 'sensors_temperatures', lambda: {})()
            if temps:
                print(f"\tDetected temperature sensors: {temps}")
                for key in ("coretemp", "k10temp", "acpitz"):
                    if key in temps and temps[key]:
                        return float(temps[key][0].current)
                for arr in temps.values():
                    if arr:
                        return float(arr[0].current)
            else:
                print("\t(!) psutil.sensors_temperatures() not available")
        if platform.system() == 'Windows':
            try:
                c = wmi.WMI(namespace='root\\wmi')
                sensors = c.MSAcpi_ThermalZoneTemperature()
                if sensors:
                    kelvin = sensors[0].CurrentTemperature / 10.0
                    return float(kelvin - 273.15)
            except Exception:
                print("\t(!)WMI import or access failed")
                pass
        return None

    def read(self):
        t = self._read_temp()
        if t is None:
            return {}
        return {"temp": round(t, 1)}

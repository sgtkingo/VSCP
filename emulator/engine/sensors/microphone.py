import numpy as np
from .base import Sensor

try:
    import sounddevice as sd
    _HAVE_SD = True
except Exception:
    print("MicrophoneSensor: sounddevice library not available, disabling microphone sensor.")
    _HAVE_SD = False

class MicrophoneSensor(Sensor):
    kind = "Acoustic"

    def __init__(self, uid: str, samplerate=16000, duration=0.1, device=None):
        super().__init__(uid)
        self.samplerate = samplerate
        self.duration = duration
        self.device = device
        self.min_period = max(0.2, duration)
        self.available = _HAVE_SD

    def _capture(self):
        frames = int(self.duration * self.samplerate)
        audio = sd.rec(frames, samplerate=self.samplerate, channels=1, dtype='float32', device=self.device, blocking=True)
        sd.wait()
        return audio.reshape(-1)

    @staticmethod
    def _rms_dbfs(x: np.ndarray, eps=1e-12):
        rms = float(np.sqrt(np.mean(np.square(x)) + eps))
        db = 20.0 * np.log10(rms + eps)  # dBFS
        return db

    def read(self):
        if not self.available:
            return {}
        audio = self._capture()
        peak = float(np.max(np.abs(audio)))
        dbfs = self._rms_dbfs(audio)
        zcr = float(((audio[:-1] * audio[1:]) < 0).mean())
        return {"dBFS": round(abs(dbfs), 2), "peak": round(peak, 3), "zcr": round(zcr, 3)}

import numpy as np
from .base import Sensor

try:
    import cv2
    _HAVE_CV = True
except Exception:
    _HAVE_CV = False

class CameraSensor(Sensor):
    kind = "Camera"

    def __init__(self, uid: str, index=0, width=320, height=240):
        super().__init__(uid)
        self.index = index
        self.size = (width, height)
        self.min_period = 0.3
        self.available = False
        if _HAVE_CV:
            cap = cv2.VideoCapture(index)
            if cap.isOpened():
                cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
                cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
                self.cap = cap
                self.available = True
            else:
                self.cap = None
        else:
            self.cap = None

    def __del__(self):
        try:
            if getattr(self, 'cap', None) is not None:
                self.cap.release()
        except Exception:
            pass

    @staticmethod
    def _lux_est(gray: np.ndarray) -> float:
        mean = gray.mean()
        return float(np.exp(mean / 40.0))  # heuristic proxy

    def read(self):
        if not self.available:
            return {}
        ok, frame = self.cap.read()
        if not ok:
            return {}
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        lux_est = self._lux_est(gray)
        bgr_mean = frame.reshape(-1, 3).mean(axis=0)
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        hist = np.bincount(hsv[...,0].reshape(-1), minlength=180)
        dom_hue = int(hist.argmax())
        return {
            "lux_est": round(lux_est, 1),
            "b_mean": round(float(bgr_mean[0]), 1),
            "g_mean": round(float(bgr_mean[1]), 1),
            "r_mean": round(float(bgr_mean[2]), 1),
            "dom_hue": dom_hue
        }

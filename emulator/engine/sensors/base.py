import time
from typing import Dict, Any

class Sensor:
    """Minimal interface for a polling sensor."""
    kind: str = "Generic"

    def __init__(self, uid: str):
        self.uid = uid
        self.available = True
        self._last = 0.0
        self.min_period = 0.2  # seconds

    def read(self) -> Dict[str, Any]:
        """Return dict with numeric fields. Must be fast and non-blocking."""
        now = time.time()
        if now - self._last < self.min_period:
            return {}
        self._last = now
        return {}

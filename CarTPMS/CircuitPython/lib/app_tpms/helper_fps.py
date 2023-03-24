"""Helper functions for the BridgeTek EVE's application
   Version: 1.0
   Last modified: 9 Jan 2023 - Tuan.Nguyen@Brtchip.com
"""
import time

class _evehp_fps_glob():
    _num_frame = 0
    _start_time_ms = 0
    _fps=0
    
def evehp_fps_start():
    _evehp_fps_glob._start_time_ms = time.monotonic_ns() / 1_000_000
    
def evehp_fps_addframe():
    _evehp_fps_glob._num_frame += 1

def evehp_fps_print(timeout = 1000):
    now_ms = time.monotonic_ns() / 1_000_000
    if now_ms - _evehp_fps_glob._start_time_ms < timeout:
        return
    
    duration_ms = now_ms - _evehp_fps_glob._start_time_ms
    _evehp_fps_glob._fps = 0
    if duration_ms != 0:
        _evehp_fps_glob._fps = _evehp_fps_glob._num_frame * 1000 / duration_ms
    
    _evehp_fps_glob._num_frame = 0
    _evehp_fps_glob._start_time_ms = now_ms
    
    print("fps = ", _evehp_fps_glob._fps)
    return _evehp_fps_glob._fps
    
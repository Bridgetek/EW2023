"""Helper functions for the BridgeTek EVE's application
   Version: 1.0
   Last modified: 9 Jan 2023 - Tuan.Nguyen@Brtchip.com
"""

import time
import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ...lib.brteve.brt_eve_bt817_8 import BrtEve


class evehp_Gesture():
    def __init__(self) -> None:
        self.trackTagTouched = 0
        self.trackRotateVal = 0
        self.trackLineVal = 0
        self.trackRotateAngel = 0

        self.tagPressed = 0
        self.tagReleased = 0
        self.isRelease = 0
        self.isTouch = 0
        self.numTap = 0
        self.isDoubleTap = 0
        self.isSwipe = 0
        self.touchX = 0
        self.touchY = 0
        self.scroll_y = 0
        self.scroll_x = 0


class _evehp_gesture_glob():
    """Internal data"""
    MIN_MOVE = 15
    SWIPE_LIMIT = 5
    _gesture = evehp_Gesture()
    _last_y = 0
    _last_x = 0
    # _last_is_swipe = 0
    _last_tag = 0
    _last_touch = 0
    # _count_swipe = 0
    # prev_time = 0
    _delay = 0
    _last_tap_time = 0
    _num_released_tag = 0
    _last_tag_released = 0

def evehp_gesture_topdown(limity = 50):
    """return the y of top swipe down"""
    # static variable
    if not hasattr(evehp_gesture_topdown, 'topdowny'):
            evehp_gesture_topdown.topdowny = 0

    if not _evehp_gesture_glob._gesture.isSwipe:
        return 0
    
    if _evehp_gesture_glob._gesture.touchY < _evehp_gesture_glob._last_y:
        evehp_gesture_topdown.topdowny -=1
    elif _evehp_gesture_glob._gesture.touchY > _evehp_gesture_glob._last_y:
        evehp_gesture_topdown.topdowny +=1

    evehp_gesture_topdown.topdowny %= limity
    return evehp_gesture_topdown.topdowny


def evehp_gesture_renew(eve: BrtEve):
    tag = eve.rd32(eve.REG_TOUCH_TAG) & 0xFF
    is_touch = eve.rd32(eve.REG_TOUCH_RAW_XY) != 0xFFFFFFFF
    touch_x = eve.rd16(eve.REG_TOUCH_SCREEN_XY + 2)
    touch_y = eve.rd16(eve.REG_TOUCH_SCREEN_XY + 4)
    track = eve.rd32(eve.REG_TRACKER)
    _evehp_gesture_glob._gesture.trackTagTouched = track & 0xFF
    _evehp_gesture_glob._gesture.trackRotateVal = track >> 16
    _evehp_gesture_glob._gesture.trackLineVal = track >> 16
    _evehp_gesture_glob._gesture.trackRotateAngel = (track >> 16) * 360 / 65535

    _evehp_gesture_glob._last_x = _evehp_gesture_glob._gesture.touchX
    _evehp_gesture_glob._gesture.touchX = touch_x

    _evehp_gesture_glob._last_y = _evehp_gesture_glob._gesture.touchY
    _evehp_gesture_glob._gesture.touchY = touch_y

    _evehp_gesture_glob._gesture.isTouch = is_touch
    _evehp_gesture_glob._gesture.isRelease = _is_release()
    _evehp_gesture_glob._gesture.tagPressed = tag
    
    released = _tag_release(tag)
    _evehp_gesture_glob._gesture.tagReleased = released
    _evehp_gesture_glob._gesture.isSwipe = _is_swipe()
    
    _evehp_gesture_glob._gesture.numTap = _num_tap(released)
    _evehp_gesture_glob._gesture.isDoubleTap = _is_double_tap()
    

    return _evehp_gesture_glob._gesture


def evehp_gesture_get():
    return _evehp_gesture_glob._gesture


def _is_release():
    touch = _evehp_gesture_glob._gesture.isTouch
    is_released = 0

    if (touch == 0 and _evehp_gesture_glob._last_touch != touch):
        is_released = _evehp_gesture_glob._last_touch
        _evehp_gesture_glob._last_touch = 0
    elif touch != 0:
        _evehp_gesture_glob._last_touch = touch
    return is_released

def _tag_release(new_tag):
    tag_released = 0

    if (new_tag == 0 and _evehp_gesture_glob._last_tag != new_tag):
        tag_released = _evehp_gesture_glob._last_tag
        _evehp_gesture_glob._last_tag = 0
    elif new_tag != 0:
        _evehp_gesture_glob._last_tag = new_tag
    return tag_released
        
def _num_tap(tag_released):
    THREADHOLD = 500
    now_ms = time.monotonic_ns() / 1000_000
    dur = now_ms - _evehp_gesture_glob._last_tap_time
    # print("tag_released", tag_released, _evehp_gesture_glob._num_released_tag, dur)
    
    if tag_released:
        _evehp_gesture_glob._last_tap_time = now_ms
    
    if tag_released:
        if tag_released == _evehp_gesture_glob._last_tag_released and dur < THREADHOLD:
            _evehp_gesture_glob._num_released_tag+=1
        else:
            _evehp_gesture_glob._last_tag_released = tag_released
            _evehp_gesture_glob._num_released_tag = 0
        
    return _evehp_gesture_glob._num_released_tag
    
def _is_double_tap():
    return True if _evehp_gesture_glob._gesture.numTap >= 2 else False
    
def _is_swipe():

    return False

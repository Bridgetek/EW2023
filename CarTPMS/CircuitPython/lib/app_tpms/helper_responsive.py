"""Helper functions for the BridgeTek EVE's application
   Version: 1.0
   Last modified: 9 Jan 2023 - Tuan.Nguyen@Brtchip.com

   Responsive: The purpose of this file to help developer design UI on 1 LCD and 
   scale it on every other LCD when runtime
"""

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ...lib.brteve.brt_eve_bt817_8 import BrtEve


class _evehp_responsive_glob:
    """internal data
    :param eve: BrtEve object
    :param dev_screen_w: Development screen width, the layout will be stretched on eve.lcd_width
    :param dev_screen_h: Development screen height, the layout will be stretched on eve.lcd_height
    :param app_x: Application X
    :param app_y: Application Y
    :param app_w: Application W
    :param app_h: Application H
    :return: none
    """

    # Default values
    dev_screen_w = 800
    dev_screen_h = 480
    run_screen_w = dev_screen_w
    run_screen_h = dev_screen_h
    lanscape = 0
    ratio_x = 1
    ratio_y = 1
    appw = dev_screen_w
    apph = dev_screen_h
    appx = 0
    appy = 0
    ratio_x = 1
    ratio_y = 1


def evehp_responsive_get():
    return _evehp_responsive_glob

def evehp_responsive_set_app_size(appx, appy, appw, apph):
    _evehp_responsive_glob.appx = appx
    _evehp_responsive_glob.appy = appy
    _evehp_responsive_glob.appw = appw
    _evehp_responsive_glob.apph = apph

def evehp_responsive_set_screen(dev_screen_w, dev_screen_h, run_screen_w, run_screen_h, appx = 0, appy = 0, appw = 0, apph = 0):
    _evehp_responsive_glob.dev_screen_w = dev_screen_w
    _evehp_responsive_glob.dev_screen_h = dev_screen_h
    _evehp_responsive_glob.run_screen_w = run_screen_w
    _evehp_responsive_glob.run_screen_h = run_screen_h
    if not appw:
        appw = run_screen_w
    if not apph:
        apph = run_screen_h
    evehp_responsive_set_app_size(appx, appy, appw, apph)
    
    _evehp_responsive_glob.ratio_x = 1.0*run_screen_w / dev_screen_w
    _evehp_responsive_glob.ratio_y = 1.0*run_screen_h / dev_screen_h
    print("ratio x", _evehp_responsive_glob.ratio_x , dev_screen_w, dev_screen_h, run_screen_w, run_screen_h)
    
def evehp_responsive_set_dev_screen(dev_screen_w, dev_screen_h):
    _evehp_responsive_glob.dev_screen_w = dev_screen_w
    _evehp_responsive_glob.dev_screen_h = dev_screen_h

    _evehp_responsive_glob.ratio_x = _evehp_responsive_glob.run_screen_w / \
        _evehp_responsive_glob.dev_screen_w
    _evehp_responsive_glob.ratio_y = _evehp_responsive_glob.run_screen_h / \
        _evehp_responsive_glob.dev_screen_h

def evehp_responsive_set_run_screen(run_screen_w, run_screen_h):
    _evehp_responsive_glob.run_screen_w = run_screen_w
    _evehp_responsive_glob.run_screen_h = run_screen_h

    _evehp_responsive_glob.ratio_x = _evehp_responsive_glob.run_screen_w / \
        _evehp_responsive_glob.dev_screen_w
    _evehp_responsive_glob.ratio_y = _evehp_responsive_glob.run_screen_h / \
        _evehp_responsive_glob.dev_screen_h


def evehp_responsive_flip(eve):
    w = _evehp_responsive_glob.run_screen_w
    h = _evehp_responsive_glob.run_screen_h
    _evehp_responsive_glob.run_screen_w = h
    _evehp_responsive_glob.run_screen_h = w

    a = _evehp_responsive_glob.ratio_x
    _evehp_responsive_glob.ratio_x = _evehp_responsive_glob.ratio_y
    _evehp_responsive_glob.ratio_y = a

    _evehp_responsive_glob.appw = _evehp_responsive_glob.appw * \
        _evehp_responsive_glob.ratio_x
    _evehp_responsive_glob.apph = _evehp_responsive_glob.apph * \
        _evehp_responsive_glob.ratio_y
    _evehp_responsive_glob.appx = _evehp_responsive_glob.appx * \
        _evehp_responsive_glob.ratio_x
    _evehp_responsive_glob.appy = _evehp_responsive_glob.appy * \
        _evehp_responsive_glob.ratio_y

    _evehp_responsive_glob.isFlipped = not _evehp_responsive_glob.isFlipped


def evehp_responsive_convert(size):
    return size * _evehp_responsive_glob.ratio_x


def evehp_responsive_convertx(x):
    return x * _evehp_responsive_glob.ratio_x


def evehp_responsive_converty(y):
    return y * _evehp_responsive_glob.ratio_y

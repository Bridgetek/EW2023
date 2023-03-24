"""Helper functions for the BridgeTek EVE's application
   Version: 1.0
   Last modified: 9 Jan 2023 - Tuan.Nguyen@Brtchip.com
"""

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from ...lib.brteve.brt_eve_bt817_8 import BrtEve

def evehp_calibrate_init(eve: BrtEve, a=None, b=None, c=None, d=None, e=None, f=None):
    if a != None and b != None and c != None and d != None and e != None and f != None:
        eve.wr32(eve.REG_TOUCH_TRANSFORM_A, a)
        eve.wr32(eve.REG_TOUCH_TRANSFORM_B, b)
        eve.wr32(eve.REG_TOUCH_TRANSFORM_C, c)
        eve.wr32(eve.REG_TOUCH_TRANSFORM_D, d)
        eve.wr32(eve.REG_TOUCH_TRANSFORM_E, e)
        eve.wr32(eve.REG_TOUCH_TRANSFORM_F, f)
    else:
        _evehp_calibrate_new(eve)    
    
def _evehp_calibrate_new(eve: BrtEve):
    """Private function, should not be called outside"""
    eve.cmd_dlstart()
    eve.ClearColorRGB(64, 64, 64)
    eve.Clear(1, 1, 1)
    eve.ColorRGB(0xff, 0xff, 0xff)
    eve.cmd_swap()
    eve.flush()

    eve.calibrate()
    eve.finish() # wait for calibration done

    eve.cmd_dlstart()
    eve.ClearColorRGB(64, 64, 64)
    eve.Clear(1, 1, 1)
    eve.ColorRGB(0xff, 0xff, 0xff)
    eve.cmd_swap()
    eve.flush()

    # Store calibration setting
    a = eve.rd32(eve.REG_TOUCH_TRANSFORM_A)
    b = eve.rd32(eve.REG_TOUCH_TRANSFORM_B)
    c = eve.rd32(eve.REG_TOUCH_TRANSFORM_C)
    d = eve.rd32(eve.REG_TOUCH_TRANSFORM_D)
    e = eve.rd32(eve.REG_TOUCH_TRANSFORM_E)
    f = eve.rd32(eve.REG_TOUCH_TRANSFORM_F)
    
    print("REG_TOUCH_TRANSFORM_A = ", a)
    print("REG_TOUCH_TRANSFORM_B = ", b)
    print("REG_TOUCH_TRANSFORM_C = ", c)
    print("REG_TOUCH_TRANSFORM_D = ", d)
    print("REG_TOUCH_TRANSFORM_E = ", e)
    print("REG_TOUCH_TRANSFORM_F = ", f)
import time

from .helper_gesture import *
from .helper_datetime import *
from .helper_responsive import *
from .app_tags import *
from .helper_datetime import *
from .helper_widgets import *

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve

# unified.blob                                                   : 0        : 4096   
# flash_no_anim_no_blob.bin                                      : 4096     : 1675264
# static_BG 2_nonsquare_864x480_COMPRESSED_RGBA_ASTC_4x4_KHR.raw : 1679360  : 414720 
# materials phase 1.anim.data                                    : 2094080  : 4606528
# materials phase 1.anim.object                                  : 6700608  : 576    
# materials phase 2.anim.data                                    : 6701184  : 4350528
# materials phase 2.anim.object                                  : 11051712 : 768    
_bg = 1679360
_materials_phase_1_anim_object = 6700608
_materials_phase_2_anim_object = 11051712
_FRAME_COUNT = 136

_ANIM_1 = 0
_ANIM_2 = 0

class app_ui_screensaver():
    def __init__(self, eve: BrtEve):
        self.eve = eve
        self.anim = _ANIM_1
        self.done = 0

    def event(self):
        eve=self.eve
        if evehp_gesture_get().isTouch:
            self.anim = _ANIM_2
            # eve.cmd_dlstart()
            # eve.ClearColorRGB(255, 255, 255)
            # eve.Clear(1, 1, 1)
            # eve.Display()
            # eve.cmd_swap()
            # eve.flush()

    def draw_anim(self, anim_obj_addr = 0, framecount = 0):
        eve = self.eve
        for frame in range(0, framecount):
            eve.cmd_dlstart()
            eve.ClearColorRGB(255, 255, 255)
            eve.Clear(1, 1, 1)

            #background
            eve.cmd_setbitmap(_bg, eve.ASTC_4x4, 862, 480)
            eve.TAG(tag_anim_bg)
            eve.Begin(eve.BITMAPS)
            eve.Vertex2f(0, 0)

            eve.cmd_animframe(0, 0, anim_obj_addr, frame)

            eve.Display()
            eve.cmd_swap()
            eve.flush()

            
    def draw(self):
        eve = self.eve
        eve.cmd_hsf(800)
        layout = evehp_responsive_get()

        self.event()
        if self.anim == _ANIM_1:
            self.draw_anim(_materials_phase_1_anim_object, _FRAME_COUNT)
        else: #if self.anim == _ANIM_2:
            self.done = 1
            
        return self.done

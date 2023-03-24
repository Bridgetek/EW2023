""" Embedded Word demo 2023: Car TPMS """
from .helper_calibrate import *
from .helper_datetime import *
from .helper_gesture import *
from .helper_image import *
from .helper_math import *
from .helper_responsive import *
from .helper_widgets import *
from .helper_fps import *
from .app_tags import *
from .app_image import *
from .app_ui_config import *

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_rp2040 import BrtEveRP2040
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve
    from lib.brteve.brt_eve_rp2040 import BrtEveRP2040

class color_val:
    """Set the wheels color green/yellow or red"""
    def __init__(self, min , min_red , min_yellow , min_green , max_green , max_yellow , max_red , max) -> None:
        self.min=1.0*min
        self.min_red=1.0*min_red
        self.min_yellow=1.0*min_yellow
        self.min_green=1.0*min_green
        self.max_green=1.0*max_green
        self.max_yellow=1.0*max_yellow
        self.max_red=1.0*max_red
        self.max=1.0*max
        
        self.red=[255, 0, 0]
        self.yellow=[255, 255, 0]
        self.green=[0, 255, 0]
        
    def change_unit(self, min , min_red , min_yellow , min_green , max_green , max_yellow , max_red , max):
        self.min=1.0*min
        self.min_red=1.0*min_red
        self.min_yellow=1.0*min_yellow
        self.min_green=1.0*min_green
        self.max_green=1.0*max_green
        self.max_yellow=1.0*max_yellow
        self.max_red=1.0*max_red
        self.max=1.0*max
        
    def color(self, val):
        if val <= self.min_red:
            return  self.red
        elif val>self.min_red and val <= self.min_yellow:
            return  self.yellow
        elif val>self.min_yellow and val <= self.min_green:
            return  self.green
        elif val>self.min_green and val <= self.max_green:
            return  self.green
        elif val>self.max_green and val <= self.max_yellow:
            return  self.yellow
        elif val>self.max_yellow:
            return  self.red
        
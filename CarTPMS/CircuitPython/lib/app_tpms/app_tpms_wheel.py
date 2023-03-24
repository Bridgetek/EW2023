""" Embedded Word demo 2023: Car TPMS """
import time
import random
import microcontroller

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
from .app_tpms_wheel_color import *

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_rp2040 import BrtEveRP2040
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve
    from lib.brteve.brt_eve_rp2040 import BrtEveRP2040

UNIT_PS_KPA = 0
UNIT_PS_BAR = 1
UNIT_PS_PSI = 2
UNIT_TEMP_C = 0
UNIT_TEMP_F = 1
UNIT_TEMP_OFF = 2
UNIT_DEPTH_MM = 0
UNIT_DEPTH_IN = 1

class wheel:
    def __init__(self, x = 0, y = 0, arrow=eveim_arrowl, font1 = 30, font2 = 28) -> None:
        self.random_id = 0
        
        self.val_ps = 32.0 # 0 to 50
        self.val_temp = 35.0
        self.val_depth = 8.0
        
        self.val_ps_psi=self.val_ps
        self.val_ps_bar=psi_bar(self.val_ps)
        self.val_ps_kpa=psi_kpa(self.val_ps)
        self.val_temp_c=self.val_temp
        self.val_temp_f=c_f(self.val_temp)
        self.val_depth_mm=self.val_depth
        self.val_depth_in=mm_inch(self.val_depth)

        self.text_ps = str(round(self.val_ps, 2))
        self.text_temp = str(round(self.val_temp, 2))
        self.text_depth = str(round(self.val_depth, 2))

        self.text_ps_unit = " PSI"
        self.text_temp_unit = "  C"
        self.text_depth_unit = " mm"

        self.im_ps_unit = eveim_PSI
        self.im_temp_unit = eveim_C1
        self.im_depth_unit = eveim_mm
        
        self.color_ps = [255, 255, 255]
        self.color_temp = [255, 255, 255]
        self.color_depth = [255, 255, 255]

        self.psi_color = color_val(0 ,20 ,24 ,28 ,35 ,40 ,50 ,60)
        self.kpa_color = color_val(psi_kpa(0) ,psi_kpa(20) ,psi_kpa(24) ,psi_kpa(28) ,psi_kpa(35) ,psi_kpa(40) ,psi_kpa(50) ,psi_kpa(60))
        self.bar_color = color_val(psi_bar(0), psi_bar(20), psi_bar(24), psi_bar(28), psi_bar(35), psi_bar(40), psi_bar(50), psi_bar(60))
        self.pressure_color = self.psi_color

        self.c_color = color_val(-20 ,-15 ,-5 ,0 ,76 ,80 ,85 ,100)
        self.f_color = color_val(c_f(-20), c_f(-5), c_f(5), c_f(20), c_f(76), c_f(80), c_f(85), c_f(100))
        self.temp_color = self.c_color

        self.mm_color = color_val(2 ,3 ,4 ,8 ,10 ,10 ,10 ,10)
        self.in_color = color_val(mm_inch(2), mm_inch(3), mm_inch(4), mm_inch(8), mm_inch(10), mm_inch(10), mm_inch(10), mm_inch(10))
        self.depth_color = self.mm_color
        
        
        self.im_arrow = arrow

        self.position_x_arrow = 0
        self.position_x_bar = 0
        self.position_x_point = 0

        self.position_x_pressure=0
        self.position_x_pressure_unit=0
        
        self.position_x_temp=0
        self.position_x_temp_unit=0

        self.position_x_depth=0
        self.position_x_depth_unit=0

        self.x=x
        self.y=y

        self.font1 = font1
        self.font2 = font2
        
        self.refresh()
        

    def change_pressure_unit(self, unit_crr, unit_next):
        self.change_unit_countdown = 20
        
        if unit_crr == UNIT_PS_KPA:
            self.val_ps_psi = psi_kpa(kpa=self.val_ps)
            self.val_ps_bar = psi_bar(psi=self.val_ps_psi)
            self.val_ps_kpa = self.val_ps
        elif unit_crr == UNIT_PS_BAR:
            self.val_ps_psi = psi_bar(bar=self.val_ps)
            self.val_ps_kpa = psi_kpa(psi=self.val_ps_psi)
            self.val_ps_bar = self.val_ps
        elif unit_crr == UNIT_PS_PSI:
            self.val_ps_bar = psi_bar(psi=self.val_ps)
            self.val_ps_kpa = psi_kpa(psi=self.val_ps)
            self.val_ps_psi = self.val_ps
        
        if unit_next == UNIT_PS_KPA:
            self.val_ps = self.val_ps_kpa
            self.text_ps_unit = " kPa"
            self.im_ps_unit = eveim_kPa
            self.pressure_color = self.kpa_color
        elif unit_next == UNIT_PS_BAR:
            self.val_ps = self.val_ps_bar
            self.text_ps_unit = " Bar"
            self.im_ps_unit = eveim_Bar
            self.pressure_color = self.bar_color
        elif unit_next == UNIT_PS_PSI:
            self.val_ps = self.val_ps_psi
            self.text_ps_unit = " PSI"
            self.im_ps_unit = eveim_PSI
            self.pressure_color = self.psi_color

        self.text_ps = str(round(self.val_ps, 1))
        self.refresh()
        
    def change_temp_unit(self, unit_crr, unit_next):
        self.change_unit_countdown = 20
        
        if unit_crr == UNIT_TEMP_C:
            self.val_temp_c = self.val_temp
            self.val_temp_f = c_f(c=self.val_temp)
        elif unit_crr == UNIT_TEMP_F:
            self.val_temp_c = c_f(f=self.val_temp)
            self.val_temp_f = self.val_temp
        
        if unit_next == UNIT_TEMP_C:
            self.val_temp = self.val_temp_c
            self.text_temp_unit = "  C"
            self.im_temp_unit = eveim_C1
            self.temp_color = self.c_color
        elif unit_next == UNIT_TEMP_F:
            self.val_temp = self.val_temp_f
            self.text_temp_unit = "  F"
            self.im_temp_unit = eveim_F1
            self.temp_color = self.f_color
        elif unit_next == UNIT_TEMP_OFF:
            self.text_temp_unit = "  OFF"
            self.im_temp_unit = eveim_Off
            
        self.text_temp = str(round(self.val_temp, 1))
        self.refresh()
                    
    def change_depth_unit(self, unit_crr, unit_next):
        self.change_unit_countdown = 20
        
        if unit_crr == UNIT_DEPTH_MM:
            self.val_depth_mm = self.val_depth
            self.val_depth_in = mm_inch(mm=self.val_depth)
        elif unit_crr == UNIT_DEPTH_IN:
            self.val_depth_mm = mm_inch(inch=self.val_depth)
            self.val_depth_in = self.val_depth
        
        if unit_next == UNIT_DEPTH_MM:
            self.val_depth = self.val_depth_mm
            self.text_depth_unit = " mm"
            self.im_depth_unit = eveim_mm
            self.depth_color = self.mm_color
        elif unit_next == UNIT_DEPTH_IN:
            self.val_depth = self.val_depth_in
            self.text_depth_unit = " inch"
            self.im_depth_unit = eveim_inch
            self.depth_color = self.in_color
            
        self.text_depth = str(round(self.val_depth, 1))
        self.refresh()
          
    def refresh(self):
        is_left = 1 if self.im_arrow ==eveim_arrowl else 0   
        point_size = 15
        bar_w = 15
        
        if is_left:
            w=eveim_arrowl.w
            self.position_x_arrow = self.x -w
            self.position_x_bar = self.x -w -bar_w -10 # 5 = padding
            self.position_x_point = self.x -w -point_size -15

            # value position is static
            x  = self.x - 144
            self.position_x_temp = x
            self.position_x_pressure = x
            self.position_x_depth = x
            
            # unit position is dynamic
            self.position_x_temp_unit = x
            self.position_x_pressure_unit = x
            self.position_x_depth_unit = x
        else:
            self.position_x_arrow = self.x
            w=eveim_arrowl.w + 5 # padding 5
            self.position_x_bar = self.x + w
            self.position_x_point = self.x + w + 13 + point_size/2 # 3= padding
            
            w+=20
            # value position is static
            x  = self.x + w + 10
            self.position_x_temp = x
            self.position_x_pressure = x
            self.position_x_depth = x + 10

            w+=15 + 5 # padding 5
            # unit position is dynamic
            self.position_x_temp_unit = self.x - w
            self.position_x_pressure_unit = self.x -w
            self.position_x_depth_unit = self.x -w- 35
                 
        self.color_ps = self.pressure_color.color(self.val_ps)
        self.color_temp = self.temp_color.color(self.val_temp)
        self.color_depth = self.depth_color.color(self.val_depth)

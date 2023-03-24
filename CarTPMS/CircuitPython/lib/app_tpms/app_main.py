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
from .app_tpms import *
from .app_tpms_wheel import wheel
from .app_tpms_wheel_color import color_val

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_rp2040 import BrtEveRP2040
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve
    from lib.brteve.brt_eve_rp2040 import BrtEveRP2040

DEBUG_ON = 0

class start():
    """Main application"""

    def __init__(self) -> None:
        global layout
        global c
        global appx
        global appy
        global appw
        global apph
        global eve 
        
        host = BrtEveRP2040()
        eve = BrtEve(host)
        eve.init(resolution="800x480_NoSquare")
        
        # non-square
        hsf_w = 862
        lcd_w = 800
        eve.lcd_width = hsf_w
        # evehp_responsive_set_screen(lcd_w, 480, lcd_w, 480)
        evehp_responsive_set_screen(lcd_w, 480, hsf_w, 480)
        eve.cmd_hsf(lcd_w)
        eve.calibrate() # Apply HSF require to re-calibrate
        
        layout = evehp_responsive_get()
        c = evehp_responsive_convert
        appx = layout.appx
        appy = layout.appy
        appw = layout.appw
        apph = layout.apph
        
        for i in eveims:
            i.offsetx = c(i.offsetx) 
            i.offsety = c(i.offsety) 

        if DEBUG_ON:
            image_on_ramg_from_eve_flash(eve)
        else:
            self.flashing()
            
            # copy into ramg
            image_on_ramg_from_eve_flash(eve)

        # configuration screen
        self.conf=ui_config(eve)
        
        # tpms obj
        self.tpms = tpms(eve, self.conf)
        
        if DEBUG_ON:
            self.conf.done = 1
            evehp_fps_start()
            
        is_changed_config = 0
        
        i=0
        while (1):
            if not self.conf.done:
                is_changed_config = 1
                self.start_display()
                self.conf.draw()
                self.end_display()
                continue
            
            if is_changed_config:
                evehp_datetime_now(confdata.hh, confdata.mm)
                self.tpms.backlight = confdata.light_level
                self.tpms.backlight_on = confdata.light_level
                self.tpms.backlight_next = confdata.light_level
                eve.wr32(eve.REG_PWM_DUTY, (int)(self.tpms.backlight))
                is_changed_config = 0
                
            if DEBUG_ON:
                evehp_fps_addframe()
                evehp_fps_print()
                
            self.start_display()
            self.tpms.draw()
            self.tpms.process_event()
            self.end_display()

    def start_display(self):
        eve.cmd_dlstart()
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear(1, 1, 1)

    def end_display(self):
        eve.Display()
        eve.cmd_swap()
        eve.flush()

    def init_message(self, msg, percent=None):
        eve.cmd_dlstart()
        eve.ClearColorRGB(51, 153, 255)
        eve.Clear(1, 1, 1)
        eve.VertexFormat(3)
        # self.drawing()
        eve.ColorRGB(255, 255, 255)
        eve.cmd_text(eve.lcd_width/2, c(10), 31, eve.OPT_CENTERX, ORG)
        eve.cmd_text(eve.lcd_width/2, c(70), 30, eve.OPT_CENTERX, PROGRAM)
        eve.cmd_text(eve.lcd_width/2, eve.lcd_height/4 + c(80), 27, eve.OPT_CENTER, msg)

        if percent:
            font = 29
            bar_range = 100

            opt = 0

            bar_w = (int)(eve.lcd_width * 8 / 10)
            bar_h = (int)(eve.lcd_height * 1 / 15)
            bar_x = (int)((eve.lcd_width - bar_w) / 2)
            bar_y = (int)((eve.lcd_height - bar_h) / 2)

            eve.ColorRGB(255, 255, 255)
            eve.cmd_progress(bar_x, bar_y, bar_w, bar_h, opt, percent, bar_range)

            text = str(percent) + '%'
            eve.ColorRGB(0, 200, 0)
            # eve.cmd_text(bar_x + bar_w / 2, bar_y + 3, font, opt, text)    
            
        eve.Display()
        eve.cmd_swap()
        eve.flush()
    
    def flashing(self):
        file='/lib/app_tpms/flash/flash.bin'
        question = "Write image from sdcard to EVE's connected flash?"
        yes = evehp_widget_dialog_yes_no(eve, org=ORG, program=PROGRAM, title="Preparing flash", question=question, timeout=120, default_tag=False) == True
        if yes == True:
            PROGESS_BAR_WRITE = 2
            progress = eve.storage.progress_bar_init(file, 0, PROGESS_BAR_WRITE)
            if progress == 0:
                return 0 # Error

            while 1:
                percent = eve.storage.progress_bar_write_chunk(progress)
                self.init_message("Copying images into EVE's connected flash", percent)

                if percent >= 100:
                    break

            progress.file_handler.close()

        eve.VertexFormat(4)

        # check flash
        eve.storage.flash_state(eve.FLASH_STATUS_FULL)
        if eve.FLASH_STATUS_FULL != eve.rd8(eve.REG_FLASH_STATUS):
            print("EVE error: Switch flash to fullmode failed")
            while 1:
                time.sleep(999)
        
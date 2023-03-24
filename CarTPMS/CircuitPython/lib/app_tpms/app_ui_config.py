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

_cout=1
_tag_lightlevel               = _cout ; _cout = _cout + 1
_tag_ui_clock_cancel          = _cout ; _cout = _cout + 1
_tag_ui_clock_set             = _cout ; _cout = _cout + 1
_tag_ui_alarm_mm              = _cout ; _cout = _cout + 1
_tag_ui_alarm_hh              = _cout ; _cout = _cout + 1
_tag_ui_alarm_rotate          = _cout ; _cout = _cout + 1

class confdata():
    light_level = 0x80
    hh = 0
    mm = 30
    
def backlight_set(eve, val, min_light = 10):
    light_set = min_light + (int)(val * (0x80 - min_light) / 0x80)
    eve.wr32(eve.REG_PWM_DUTY, light_set)
    
class ui_config():
    def __init__(self, eve: BrtEve):
        self.eve = eve

        self._tag_hh_selected  = 1
        self._tag_mm_selected  = 0

        confdata.hh, confdata.mm, _, _ = evehp_datetime_now_hh_mm_ss_ms()

        self.hh_clone=confdata.hh
        self.mm_clone=confdata.mm
        self.light_clone=confdata.light_level
        
        self.done = 0

    def event(self):
        evehp_gesture_renew(self.eve)
        # get new clock
        tag = evehp_gesture_get().trackTagTouched
        angel = evehp_gesture_get().trackRotateVal
        
        if tag == _tag_ui_alarm_rotate:
            hh = round(angel * 12 / 65535  + 6 ) % 12   # +6 because 0 degree = 6th hour
            mm = round(angel * 60 / 65535  + 30) % 60 # +30 because 0 degree = 30minute

            if  self._tag_hh_selected == 1:
                pm_time = 0
                if self.hh_clone == 11 and hh == 0:
                    pm_time = 12
                elif self.hh_clone == 23 and hh == 0:
                    pm_time = 0
                elif self.hh_clone >= 12:
                    pm_time = 12

                self.hh_clone = pm_time + hh
            elif self._tag_mm_selected == 1:
                self.mm_clone = mm

        elif tag == _tag_lightlevel:
            self.light_clone = (int)(evehp_gesture_get().trackLineVal * 0x80) / 65535
            backlight_set(self.eve, self.light_clone)
            
        tag = evehp_gesture_get().tagReleased
        if tag == _tag_ui_alarm_hh:
            self.hh_clone = confdata.hh
            self._tag_hh_selected  = 1
            self._tag_mm_selected  = 0
        elif tag == _tag_ui_alarm_mm:
            self.mm_clone = confdata.mm
            self._tag_mm_selected  = 1
            self._tag_hh_selected  = 0
        
            
        elif tag == _tag_ui_clock_set:
            confdata.hh =self.hh_clone
            confdata.mm =self.mm_clone
            confdata.light_level = self.light_clone
            backlight_set(self.eve, self.light_clone)
            self.done = 1

        elif tag == _tag_ui_clock_cancel:
            self.hh_clone =confdata.hh
            self.mm_clone =confdata.mm
            self.light_clone = confdata.light_level
            backlight_set(self.eve, self.light_clone)

    def draw(self):
        eve = self.eve
        layout = evehp_responsive_get()

        self.event()

        evehp_widget_box(eve, layout.appx, layout.appy, layout.appw, layout.apph, color=[0, 0, 0], rounded=1)
        
        hh = self.hh_clone
        mm = self.mm_clone

        eve.ColorRGB(255, 255, 255)
        title="Device's time"
        y = 20
        xcenter = layout.appx + layout.appw / 2
        eve.cmd_text(xcenter, y, 29, eve.OPT_CENTERX, title)

        panel_w=layout.appw 

        y += 120

        clocksize = round(min(layout.appw , layout.apph) / 6)

        # clock face
        eve.cmd_bgcolor(0x777777)
        eve.cmd_fgcolor(0xffff)
        eve.cmd_gradcolor(0xffffff)
        clockx = xcenter
        clocky = y
        clock_hh = hh
        clock_mm = mm

        eve.cmd_clock(clockx, clocky, clocksize, eve.OPT_NOSECS, clock_hh, clock_mm, 0, 0);
        # track the cmd_clock
        eve.cmd_track(clockx, clocky, 1, 1, _tag_ui_alarm_rotate)
        eve.ColorA(0)
        evehp_widget_point(eve, clockx, clocky, clocksize, -1, _tag_ui_alarm_rotate)
        eve.ColorA(255)

        eve.BlendFunc(eve.SRC_ALPHA, eve.ZERO) # remove gradient

        # time
        color_hh = str('0x777777')
        color_text = str('0xffffff')
        w = 60 # w atleast 60
        h = 30
        x = round(xcenter - w + 5)
        y += clocksize + 20

        # hh:mm
        # hh background
        evehp_widget_box(eve, x, y, w, h, 1, [0x77, 0x77, 0x77], _tag_ui_alarm_hh)
        # mm background
        x += w
        evehp_widget_box(eve, x, y, w, h, 1, [0x77, 0x77, 0x77], _tag_ui_alarm_mm)

        eve.BlendFunc(eve.SRC_ALPHA, eve.ONE_MINUS_SRC_ALPHA) # remove gradient

        # hh:mm text
        eve.ColorRGB(int('0x'+color_text[2:4], 16), int('0x'+color_text[4:6], 16), int('0x'+color_text[6:8], 16))
        eve.cmd_text(x, y + h/2, 30, eve.OPT_CENTER, ' : ')
        if self._tag_hh_selected == 1:
            eve.ColorRGB(0xFF, 0, 0)
        eve.Tag(_tag_ui_alarm_hh)
        eve.cmd_text(x - w/2, y + h/2, 30, eve.OPT_CENTER, evehp_math_zfill(str(hh), 2))

        if self._tag_mm_selected == 1:
            eve.ColorRGB(0xFF, 0, 0)
        else:
            eve.ColorRGB(int('0x'+color_text[2:4], 16), int('0x'+color_text[4:6], 16), int('0x'+color_text[6:8], 16))
        eve.Tag(_tag_ui_alarm_mm)
        eve.cmd_text(x + w/2, y + h/2, 30, eve.OPT_CENTER, evehp_math_zfill(str(mm), 2))
        
        eve.ColorRGB(0xff, 0xff, 0xff)
        w= layout.appw * 8 / 10
        h = 20
        x = layout.appx + layout.appw / 2
        y += 60
        eve.cmd_text(x, y, 30, eve.OPT_CENTERX, "Light level:")
        
        x = layout.appx + layout.appw / 2 - w/2
        y += 50
        eve.cmd_track(x, y, w, h, _tag_lightlevel)
        eve.TAG(_tag_lightlevel)
        eve.cmd_slider(x, y, w, h, 0, self.light_clone, 0x80)

        #save/cancel
        eve.ColorRGB(255, 255, 255)
        btn_w = 90
        btn_h = 30
        padding = 20
        
        btn_y = layout.appy + layout.apph - btn_h - 5
        btn_x1 = layout.appx + layout.appw - btn_w
        btn_x2 = btn_x1 - btn_w - padding
        eve.Tag(_tag_ui_clock_set)
        eve.cmd_button(btn_x1, btn_y, btn_w, btn_h, 28, 0, "Save")
        eve.Tag(_tag_ui_clock_cancel)
        eve.cmd_button(btn_x2, btn_y, btn_w, btn_h, 28, 0, "Clear")
        eve.ColorRGB(0xFF, 0xFF, 0xFF) #reset color
        
        return self.done

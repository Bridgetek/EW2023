""" Embedded Word demo 2023: Car Cliamte Control """
import time
import random

from .helper_calibrate import *
from .helper_datetime import *
from .helper_gesture import *
from .helper_image import *
from .helper_math import *
from .helper_responsive import *
from .helper_widgets import *
from .tags import *
from .image import *
from .ui_config import backlight_set, confdata, ui_config

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_rp2040 import BrtEveRP2040
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve
    from lib.brteve.brt_eve_rp2040 import BrtEveRP2040

ORG="Bridgetek"
PROGRAM="The Car Climate Control"

layout = evehp_responsive_get()
c = evehp_responsive_convert
appx = layout.appx
appy = layout.appy
appw = layout.appw
apph = layout.apph
TEXT_IMAGE_ON = 1
DEBUG_ON = 0
eve=None

class fps():
    def __init__(self) -> None:
        self.fps=0
        self.numframe = 0
        self.timestart_ms = 0
        self.timeprint_ms = 0
        
    def start(self):
        self.timestart_ms = time.monotonic_ns() / 1_000_000
        
    def add_1_frame(self):
        self.numframe+=1
        now_ms = time.monotonic_ns() / 1_000_000
        duration_ms = now_ms - self.timestart_ms
        if duration_ms != 0:
            self.fps = self.numframe * 1000 / duration_ms
    
    def print(self, timeout = 1000):
        now_ms = time.monotonic_ns() / 1_000_000
        if now_ms - self.timeprint_ms < timeout:
            return
        self.timeprint_ms = now_ms
        print("fps = ", self.fps)
        self.numframe = 0
        self.timestart_ms = now_ms

class climate():
    """Internal data structure"""
    # constants
    TEMP_MIN = 16
    TEMP_MAX = 32
    TEMP_MIN_F = evehp_math_c2f(16) + 1
    TEMP_MAX_F = evehp_math_c2f(32) + 1

    FAN_MAX = 5
    FAN_MIN = 0

    WIND_MIN = 1
    WIND_MAX = 3

    backlight_on = 0x80
    _BACKLIGHT_OFF = 0
    
    _SYNC_L = 0
    _SYNC_R = 1
    _SYNC_REAR = 2
    sync_from = _SYNC_L
    
    backlight = backlight_on
    backlight_next = backlight_on

    # variables
    temp_l = 16
    temp_r = 16
    temp_rear = 16

    # these value for lossness converting
    _temp_l_c = temp_l
    _temp_r_c = temp_r
    _temp_rear_c = temp_rear
    _temp_l_f = evehp_math_c2f(16) + 1
    _temp_r_f = evehp_math_c2f(16) + 1
    _temp_rear_f = evehp_math_c2f(16) + 1

    fan_l = 0
    fan_r = 0
    fan_rear = 0

    wind_l = [0, 0, 0]
    wind_r = [0, 0, 0]
    wind_rear = [0, 0, 0]

    enable_sync = False
    enable_front = True
    enable_rear = False
    enable_auto = False
    enable_onoff = True
    enable_freshair = False
    enable_ac = False
    enable_c = True
    enable_f = False

    timer_id_l = 1
    timer_id_r = 2
    timer_id_rear = 3

    time_wait_l = 1000
    time_wait_r = 1000
    time_wait_rear = 1000


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
            image_on_ramg_from_eve_flash(eve, 1024*1024)
            
        else:
            self.flashing()
            
            # copy into ramg
            image_on_ramg_from_eve_flash(eve)
            
            # set handler
            self.init_message("Preparing images")
            self.start_display()
            image_handler_init(eve)
            self.end_display()


        if DEBUG_ON:
            self.fps = fps()
            self.fps.start()
            climate.fan_l = 1
            climate.fan_r = 1
        
        # configuration screen
        self.conf=ui_config(eve)
        is_changed_config = 0
        
        # main loop
        while (1):
            if not self.conf.done:
                is_changed_config = 1
                self.start_display()
                self.conf.draw()
                self.end_display()
                continue
            
            if is_changed_config:
                evehp_datetime_now(confdata.hh, confdata.mm)
                climate.backlight = confdata.light_level
                climate.backlight_on = confdata.light_level
                climate.backlight_next = confdata.light_level
                eve.wr32(eve.REG_PWM_DUTY, (int)(climate.backlight))
                is_changed_config = 0
                
            if DEBUG_ON:
                self.fps.add_1_frame()
                self.fps.print()
            
            self.start_display()
            self.drawing()
            self.process_event()
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
        eve.VertexFormat(4)
    
    def flashing(self):
        file='/lib/app_climatecontrol/flash/flash.bin'
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

    def drawing(self):
        self.draw_demo()
    
    def draw_demo(self):
        self.draw_bg()
        self.draw_btns()
        self.draw_control_l()
        self.draw_control_r()
        self.draw_control_rear()
        self.draw_datetime()

    def draw_bg(self):
        """Draw background"""
        evehp_image_draw(eve, eveim_bg, tag=tag_bg)

    def draw_btns(self):
        """ Draw control buttons"""
        im = [
            [climate.enable_onoff                             , eveim_onoff_off , eveim_onoff , tag_onoff ],
            [climate.enable_onoff and climate.enable_sync     , eveim_sync_off  , eveim_sync  , tag_sync  ],
            [climate.enable_onoff and climate.enable_auto     , eveim_auto_off  , eveim_auto  , tag_auto  ],
            [climate.enable_onoff and climate.enable_front    , eveim_front_off , eveim_front , tag_front ],
            [climate.enable_onoff and climate.enable_rear     , eveim_rear_off  , eveim_rear  , tag_rear  ],
            [climate.enable_onoff and climate.enable_ac       , eveim_ac_off    , eveim_ac    , tag_ac    ],
            [climate.enable_onoff and climate.enable_freshair , eveim_air_off   , eveim_air   , tag_air   ],
            [climate.enable_onoff and climate.enable_c        , 0               , eveim_celsius    , tag_c     ],
            [climate.enable_onoff and climate.enable_f        , 0               , eveim_fahrenheit , tag_f     ],
        ]
        for i in im:
            condition = i[0]
            im_off = i[1]
            im_on = i[2]
            tag=i[3]
            # print("im", im_on.filepath, ", tag=", tag)
            if condition == True:
                evehp_image_draw(eve, im_on, tag=tag)
            elif im_off:
                evehp_image_draw(eve, im_off, tag=tag)
        
        # touch area
        eve.ColorA(0)
        evehp_widget_box(eve, 0, 430, 85, 50, tag=tag_c)
        evehp_widget_box(eve, 86, 430, 85, 50, tag=tag_f)
        eve.ColorA(255)

    def draw_control_l(self):
        """Draw left side fan, temp and air flow control buttons"""
        if climate.fan_l == 1: evehp_image_draw(eve, eveim_airflow_L1, rotate_angle=0, tag=tag_none)
        if climate.fan_l == 2: evehp_image_draw(eve, eveim_airflow_L2, rotate_angle=0, tag=tag_none)
        if climate.fan_l == 3: evehp_image_draw(eve, eveim_airflow_L3, rotate_angle=0, tag=tag_none)
        if climate.fan_l == 4: evehp_image_draw(eve, eveim_airflow_L4, rotate_angle=0, tag=tag_none)
        if climate.fan_l == 5: evehp_image_draw(eve, eveim_airflow_L5, rotate_angle=0, tag=tag_none)

        if climate.wind_l[0] == 1: evehp_image_draw(eve, eveim_air_l_1, tag=tag_air_flow1_front)
        if climate.wind_l[1] == 1: evehp_image_draw(eve, eveim_air_l_2, tag=tag_air_flow2_front)
        if climate.wind_l[2] == 1: evehp_image_draw(eve, eveim_air_l_3, tag=tag_air_flow3_front)
        if climate.wind_l[0] == 0: evehp_image_draw(eve, eveim_AL1_off, tag=tag_air_flow1_front)
        if climate.wind_l[1] == 0: evehp_image_draw(eve, eveim_AL2_off, tag=tag_air_flow2_front)
        if climate.wind_l[2] == 0: evehp_image_draw(eve, eveim_AL3_off, tag=tag_air_flow3_front)

        temp = climate.temp_l
        num = [eveim_0, eveim_1, eveim_2, eveim_3, eveim_4,
               eveim_5, eveim_6, eveim_7, eveim_8, eveim_9]
        num1_int = int(temp/10) % 10
        num2_int = int(temp % 10) % 10
        num1 = num[num1_int]
        num2 = num[num2_int]

        if num1_int > 0: evehp_image_draw(eve, num1, tag=tag_none)
        x = eveim_0.w
        evehp_image_draw(eve, num2, x, tag=tag_none)

        if climate.enable_c == True: evehp_image_draw(eve, eveim_C, tag=tag_none)
        if climate.enable_f == True: evehp_image_draw(eve, eveim_F, tag=tag_none)

        r = 80
        center = [appx + 101, appy + 156]
        point_l = [center[0] - r + r/4, center[1]]
        point_r = [center[0] + r - r/4, center[1]]
        point_t = [center[0], center[1] - r + r/4]
        point_b = [center[0], center[1] + r - r/4]
        eve.ColorA(0) # 0 when release
        evehp_widget_point(eve, point_l[0], point_l[1], radius=30, tag=tag_fan_decrease_driver)
        evehp_widget_point(eve, point_r[0], point_r[1], radius=30, tag=tag_fan_increase_driver)
        evehp_widget_point(eve, point_t[0], point_t[1], radius=30, tag=tag_temp_incr_driver)
        evehp_widget_point(eve, point_b[0], point_b[1], radius=30, tag=tag_temp_decr_driver)
        # evehp_widget_point(eve, center[0], center[1], radius=r, tag=tag_fan_decrease_driver) # debug: control left circle
        eve.ColorA(255)

    def draw_control_r(self):
        if climate.enable_rear: 
            return

        if climate.fan_r == 1: evehp_image_draw(eve, eveim_airflow_R1, rotate_angle=0, tag=tag_none)
        if climate.fan_r == 2: evehp_image_draw(eve, eveim_airflow_R2, rotate_angle=0, tag=tag_none)
        if climate.fan_r == 3: evehp_image_draw(eve, eveim_airflow_R3, rotate_angle=0, tag=tag_none)
        if climate.fan_r == 4: evehp_image_draw(eve, eveim_airflow_R4, rotate_angle=0, tag=tag_none)
        if climate.fan_r == 5: evehp_image_draw(eve, eveim_airflow_R5, rotate_angle=0, tag=tag_none)

        # evehp_image_draw(eve, eveim_air_r, tag=tag_none)
        if not climate.enable_sync:
            if climate.wind_r[0] == 1: evehp_image_draw(eve, eveim_air_r_1, tag=tag_air_flow1_passenger)
            if climate.wind_r[1] == 1: evehp_image_draw(eve, eveim_air_r_2, tag=tag_air_flow2_passenger)
            if climate.wind_r[2] == 1: evehp_image_draw(eve, eveim_air_r_3, tag=tag_air_flow3_passenger)
            if climate.wind_r[0] == 0: evehp_image_draw(eve, eveim_AR1_off, tag=tag_air_flow1_passenger)
            if climate.wind_r[1] == 0: evehp_image_draw(eve, eveim_AR2_off, tag=tag_air_flow2_passenger)
            if climate.wind_r[2] == 0: evehp_image_draw(eve, eveim_AR3_off, tag=tag_air_flow3_passenger)
        
        else: 
            evehp_image_draw(eve, eveim_R_disable, tag=tag_none)
            
            if climate.wind_r[0] == 1: evehp_image_draw(eve, eveim_air_r_1, tag=tag_none)
            if climate.wind_r[1] == 1: evehp_image_draw(eve, eveim_air_r_2, tag=tag_none)
            if climate.wind_r[2] == 1: evehp_image_draw(eve, eveim_air_r_3, tag=tag_none)
            if climate.wind_r[0] == 0: evehp_image_draw(eve, eveim_AR1_disable, tag=tag_none)
            if climate.wind_r[1] == 0: evehp_image_draw(eve, eveim_AR2_disable, tag=tag_none)
            if climate.wind_r[2] == 0: evehp_image_draw(eve, eveim_AR3_disable, tag=tag_none)
        
        # temperature number
        temp = climate.temp_r
        num = [eveim_0, eveim_1, eveim_2, eveim_3, eveim_4,
               eveim_5, eveim_6, eveim_7, eveim_8, eveim_9]
        num1_int = int(temp/10) % 10
        num2_int = int(temp % 10) % 10
        num1 = num[num1_int]
        num2 = num[num2_int]
        x = 660
        if num1_int > 0: evehp_image_draw(eve, num1, x, tag=tag_none)
        x += eveim_0.w
        evehp_image_draw(eve, num2, x, tag=tag_none)

        # F and C unit
        x = 660
        if climate.enable_c == True: evehp_image_draw(eve, eveim_C, x, tag=tag_none)
        if climate.enable_f == True: evehp_image_draw(eve, eveim_F, x, tag=tag_none)

        # Transparent button control
        r = 80
        center = [appx + x + 101, appy + 156]
        point_l = [center[0] - r + r/4, center[1]]
        point_r = [center[0] + r - r/4, center[1]]
        point_t = [center[0], center[1] - r + r/4]
        point_b = [center[0], center[1] + r - r/4]
        eve.ColorA(0) # 0 when release
        evehp_widget_point(eve, point_l[0], point_l[1], radius=30, tag=tag_fan_increase_passenger)
        evehp_widget_point(eve, point_r[0], point_r[1], radius=30, tag=tag_fan_decrease_passenger)
        evehp_widget_point(eve, point_t[0], point_t[1], radius=30, tag=tag_temp_incr_passenger)
        evehp_widget_point(eve, point_b[0], point_b[1], radius=30, tag=tag_temp_decr_passenger)
        # evehp_widget_point(eve, center[0], center[1], radius=r, tag=tag_fan_decrease_driver) # debug: control left circle
        eve.ColorA(255)

    def draw_control_rear(self):
        if not climate.enable_rear: 
            return

        if climate.fan_rear == 1: evehp_image_draw(eve, eveim_airflow_R1, tag=tag_none)
        if climate.fan_rear == 2: evehp_image_draw(eve, eveim_airflow_R2, tag=tag_none)
        if climate.fan_rear == 3: evehp_image_draw(eve, eveim_airflow_R3, tag=tag_none)
        if climate.fan_rear == 4: evehp_image_draw(eve, eveim_airflow_R4, tag=tag_none)
        if climate.fan_rear == 5: evehp_image_draw(eve, eveim_airflow_R5, tag=tag_none)

        # evehp_image_draw(eve, eveim_air_r, tag=tag_none)
        if not climate.enable_sync:
            if climate.wind_rear[0] == 1: evehp_image_draw(eve, eveim_air_r_1, tag=tag_air_flow1_passenger)
            if climate.wind_rear[1] == 1: evehp_image_draw(eve, eveim_air_r_2, tag=tag_air_flow2_passenger)
            if climate.wind_rear[2] == 1: evehp_image_draw(eve, eveim_air_r_3, tag=tag_air_flow3_passenger)
            if climate.wind_rear[0] == 0: evehp_image_draw(eve, eveim_AR1_off, tag=tag_air_flow1_passenger)
            if climate.wind_rear[1] == 0: evehp_image_draw(eve, eveim_AR2_off, tag=tag_air_flow2_passenger)
            if climate.wind_rear[2] == 0: evehp_image_draw(eve, eveim_AR3_off, tag=tag_air_flow3_passenger)
        
        else: 
            evehp_image_draw(eve, eveim_R_disable, tag=tag_none)

            if climate.wind_rear[0] == 1: evehp_image_draw(eve, eveim_air_r_1, tag=tag_none)
            if climate.wind_rear[1] == 1: evehp_image_draw(eve, eveim_air_r_2, tag=tag_none)
            if climate.wind_rear[2] == 1: evehp_image_draw(eve, eveim_air_r_3, tag=tag_none)
            if climate.wind_rear[0] == 0: evehp_image_draw(eve, eveim_AR1_disable, tag=tag_none)
            if climate.wind_rear[1] == 0: evehp_image_draw(eve, eveim_AR2_disable, tag=tag_none)
            if climate.wind_rear[2] == 0: evehp_image_draw(eve, eveim_AR3_disable, tag=tag_none)
        
        # temperature number
        temp = climate.temp_rear
        num = [eveim_0, eveim_1, eveim_2, eveim_3, eveim_4,
               eveim_5, eveim_6, eveim_7, eveim_8, eveim_9]
        num1_int = int(temp/10) % 10
        num2_int = int(temp % 10) % 10
        num1 = num[num1_int]
        num2 = num[num2_int]
        x = 660
        if num1_int > 0: evehp_image_draw(eve, num1, x, tag=tag_none)
        x += eveim_0.w
        evehp_image_draw(eve, num2, x, tag=tag_none)

        # F and C unit
        x = 660
        if climate.enable_c == True: evehp_image_draw(eve, eveim_C, x, tag=tag_none)
        if climate.enable_f == True: evehp_image_draw(eve, eveim_F, x, tag=tag_none)

        # Transparent button control
        r = 80
        center = [appx + x + 101, appy + 156]
        point_l = [center[0] - r + r/4, center[1]]
        point_r = [center[0] + r - r/4, center[1]]
        point_t = [center[0], center[1] - r + r/4]
        point_b = [center[0], center[1] + r - r/4]
        eve.ColorA(0) # 0 when release
        evehp_widget_point(eve, point_l[0], point_l[1], radius=30, tag=tag_fan_increase_passenger)
        evehp_widget_point(eve, point_r[0], point_r[1], radius=30, tag=tag_fan_decrease_passenger)
        evehp_widget_point(eve, point_t[0], point_t[1], radius=30, tag=tag_temp_incr_passenger)
        evehp_widget_point(eve, point_b[0], point_b[1], radius=30, tag=tag_temp_decr_passenger)
        # evehp_widget_point(eve, center[0], center[1], radius=r, tag=tag_fan_decrease_driver) # debug: control left circle
        eve.ColorA(255)


    def draw_datetime(self):
        eve.ColorRGB(255, 255, 255)
        x=appx + appw - 50
        y=appy + apph - 25
        eve.COLOR_A(0)
        evehp_widget_box(eve, x-50, y-10, 100, 40, tag_datetime)
        eve.COLOR_A(255)

        eve.Tag(tag_datetime)
        evehp_datetime_draw_hh_mm_apm(eve,x, y, 26)

    def process_event(self):
        gt = evehp_gesture_renew(eve)

        self.change_backlight()

        # print("gt.tagReleased:", gt.tagReleased) # debug

        # When climate is off, only on/off is controlable
        if climate.enable_onoff == False:
            # wake up when touch on screen
            if gt.tagReleased: 
                self.on_onoff_released()
            return
        
        if climate.enable_auto: self.auto_simulation()
        
        if gt.tagReleased == 0:
            return

        # if gt.numTap == 5:
        elif gt.tagReleased== tag_datetime:
            self.conf.done = 0
            
        elif gt.tagReleased == tag_onoff: 
            self.on_onoff_released()
            
        elif gt.tagReleased == tag_air: climate.enable_freshair = not climate.enable_freshair
        elif gt.tagReleased == tag_ac: climate.enable_ac = not climate.enable_ac
        elif gt.tagReleased == tag_rear: 
            self.on_rear_released()
        elif gt.tagReleased == tag_front: 
            self.on_front_released()
        elif gt.tagReleased == tag_auto: 
            self.on_auto_released()
        elif gt.tagReleased == tag_sync: 
            self.on_sync_released()
        elif gt.tagReleased == tag_c: 
            self.on_c_released()
        elif gt.tagReleased == tag_f: 
            self.on_f_released()
        
        elif gt.tagReleased == tag_fan_increase_driver: 
            self.on_fan_changed(l=True, up=True)
        elif gt.tagReleased == tag_fan_decrease_driver: 
            self.on_fan_changed(l=True, down=True)
        elif gt.tagReleased == tag_fan_increase_passenger: 
            self.on_fan_changed(r=True, up=True)
        elif gt.tagReleased == tag_fan_decrease_passenger: 
            self.on_fan_changed(r=True, down=True)
        
        elif gt.tagReleased == tag_air_flow1_front:
            self.on_wind_changed(l=True, btn_nth = 0)
        elif gt.tagReleased == tag_air_flow2_front: 
            self.on_wind_changed(l=True, btn_nth = 1)
        elif gt.tagReleased == tag_air_flow3_front:
            self.on_wind_changed(l=True, btn_nth = 2)
        elif gt.tagReleased == tag_air_flow1_passenger:
            self.on_wind_changed(r=True, btn_nth = 0)
        elif gt.tagReleased == tag_air_flow2_passenger:
            self.on_wind_changed(r=True, btn_nth = 1)
        elif gt.tagReleased == tag_air_flow3_passenger:
            self.on_wind_changed(r=True, btn_nth = 2)
        
        elif gt.tagReleased == tag_temp_incr_driver: 
            self.on_temp_changed(l=True, up=True)
        elif gt.tagReleased == tag_temp_decr_driver: 
            self.on_temp_changed(l=True, down=True)
        elif gt.tagReleased == tag_temp_incr_passenger: 
            self.on_temp_changed(r=True, up=True)            
        elif gt.tagReleased == tag_temp_decr_passenger: 
            self.on_temp_changed(r=True, down=True)


    def on_onoff_released(self):
        climate.enable_onoff = not climate.enable_onoff
        if climate.enable_onoff:
            climate.backlight_next = climate.backlight_on
        else:
            climate.backlight_next = climate._BACKLIGHT_OFF
        
    def on_fan_changed(self, l=False, r=False, up=False, down=False, rear=False):
        if l:
            climate.sync_from = climate._SYNC_L
            if up:
                climate.fan_l = min(climate.fan_l + 1, climate.FAN_MAX)
            else:
                climate.fan_l = max(climate.fan_l - 1, climate.FAN_MIN)            

            if climate.enable_sync:
                climate.fan_r = climate.fan_l
                climate.fan_rear = climate.fan_l
        elif r and (climate.enable_front or climate.enable_auto):
            climate.sync_from = climate._SYNC_R
            if up:
                climate.fan_r = min(climate.fan_r + 1, climate.FAN_MAX)
            else:
                climate.fan_r = max(climate.fan_r - 1, climate.FAN_MIN)
        elif r and (climate.enable_rear or climate.enable_auto) or rear:
            climate.sync_from = climate._SYNC_REAR
            if up:
                climate.fan_rear = min(climate.fan_rear + 1, climate.FAN_MAX)
            else:
                climate.fan_rear = max(climate.fan_rear - 1, climate.FAN_MIN)

    def on_temp_changed(self, l=False, r=False, up=False, down=False):
        _max = climate.TEMP_MAX if climate.enable_c else climate.TEMP_MAX_F
        _min = climate.TEMP_MIN if climate.enable_c else climate.TEMP_MIN_F
        if l:
            climate.sync_from = climate._SYNC_L
            if up:
                climate.temp_l = min(climate.temp_l + 1, _max)
            else:
                climate.temp_l = max(climate.temp_l - 1, _min) 

            if climate.enable_c:
                climate._temp_l_f = evehp_math_c2f(climate.temp_l)
                climate._temp_l_c = climate.temp_l
                climate._temp_l_f = max(min(climate._temp_l_f, climate.TEMP_MAX_F), climate.TEMP_MIN_F)
            elif climate.enable_f:
                climate._temp_l_c = evehp_math_f2c(climate.temp_l)
                climate._temp_l_f = climate.temp_l
                climate._temp_l_c = max(min(climate._temp_l_c, climate.TEMP_MAX), climate.TEMP_MIN)

            if climate.enable_sync:
                climate.temp_r = climate.temp_l
                climate.temp_rear = climate.temp_l

                climate._temp_r_f = climate._temp_l_f
                climate._temp_r_c = climate._temp_l_c

                climate._temp_rear_f = climate._temp_l_f
                climate._temp_rear_c = climate._temp_l_c

        elif r and (climate.enable_front or climate.enable_auto):
            climate.sync_from = climate._SYNC_R
            if up:
                climate.temp_r = min(climate.temp_r + 1, _max)
            else:
                climate.temp_r = max(climate.temp_r - 1, _min)
            
            if climate.enable_c:
                climate._temp_r_f = evehp_math_c2f(climate.temp_r)
                climate._temp_r_c = climate.temp_r
                climate._temp_r_f = max(min(climate._temp_r_f, climate.TEMP_MAX_F), climate.TEMP_MIN_F)
            if climate.enable_f:
                climate._temp_r_c = evehp_math_f2c(climate.temp_r)
                climate._temp_r_f = climate.temp_r
                climate._temp_r_c = max(min(climate._temp_r_c, climate.TEMP_MAX), climate.TEMP_MIN)

        elif r and (climate.enable_rear or climate.enable_auto):
            climate.sync_from = climate._SYNC_REAR
            if up:
                climate.temp_rear = min(climate.temp_rear + 1, _max)
            else:
                climate.temp_rear = max(climate.temp_rear - 1, _min)
            
            if climate.enable_c:
                climate._temp_rear_f = evehp_math_c2f(climate.temp_rear)
                climate._temp_rear_c = climate.temp_rear
                climate._temp_rear_f = max(min(climate._temp_rear_f, climate.TEMP_MAX_F), climate.TEMP_MIN_F)
            if climate.enable_f:
                climate._temp_rear_c = evehp_math_f2c(climate.temp_rear)
                climate._temp_rear_f = climate.temp_rear
                climate._temp_rear_c = max(min(climate._temp_rear_c, climate.TEMP_MAX), climate.TEMP_MIN)


    def on_wind_changed(self, l=False, r=False, btn_nth=0):
        if l:
            climate.sync_from = climate._SYNC_L
            climate.wind_l[btn_nth] = not climate.wind_l[btn_nth]
            
            if climate.enable_sync:
                climate.wind_r = climate.wind_l[:]
                climate.wind_rear = climate.wind_l[:]
            
        elif r and (climate.enable_front or climate.enable_auto):
            climate.sync_from = climate._SYNC_R
            climate.wind_r[btn_nth] = not climate.wind_r[btn_nth]

        elif r and (climate.enable_rear or climate.enable_auto):
            climate.sync_from = climate._SYNC_REAR
            climate.wind_rear[btn_nth] = not climate.wind_rear[btn_nth]
        

    def on_sync_released(self):
        climate.enable_sync = True
        climate.enable_auto = False
        climate.enable_front = False
        climate.enable_rear = False

        if climate.sync_from == climate._SYNC_L:
            climate.temp_r = climate.temp_l
            climate.temp_rear = climate.temp_l
            climate._temp_r_c = climate._temp_l_c
            climate._temp_r_f = climate._temp_l_f
            climate._temp_rear_c = climate._temp_l_c
            climate._temp_rear_f = climate._temp_l_f
            
            climate.fan_r = climate.fan_l
            climate.fan_rear = climate.fan_l
            climate.wind_r = climate.wind_l[:]
            climate.wind_rear = climate.wind_l[:]
            
        elif climate.sync_from == climate._SYNC_R:
            climate.temp_l = climate.temp_r
            climate.temp_rear = climate.temp_r
            climate._temp_l_c = climate._temp_r_c
            climate._temp_l_f = climate._temp_r_f
            climate._temp_rear_c = climate._temp_r_c
            climate._temp_rear_f = climate._temp_r_f
            
            climate.fan_l = climate.fan_r
            climate.fan_rear = climate.fan_r
            climate.wind_l = climate.wind_r[:]
            climate.wind_rear = climate.wind_r[:]
            
        elif climate.sync_from == climate._SYNC_REAR:
            climate.temp_l = climate.temp_rear
            climate.temp_r = climate.temp_rear
            climate._temp_l_c = climate._temp_rear_c
            climate._temp_l_f = climate._temp_rear_f
            climate._temp_r_c = climate._temp_rear_c
            climate._temp_r_f = climate._temp_rear_f
            
            climate.fan_l = climate.fan_rear
            climate.fan_r = climate.fan_rear
            climate.wind_l = climate.wind_rear[:]
            climate.wind_r = climate.wind_rear[:]
        
    def on_auto_released(self):
        climate.enable_auto = True
        climate.enable_sync = False
        climate.enable_front = False
        climate.enable_rear = False

    def on_front_released(self):
        climate.enable_front = True
        climate.enable_sync = False
        climate.enable_auto = False
        climate.enable_rear = False

    def on_rear_released(self):
        climate.enable_rear = True
        climate.enable_sync = False
        climate.enable_front = False
        climate.enable_auto = False

    def on_c_released(self):
        climate.enable_c = True
        climate.enable_f = False
        climate.temp_l = climate._temp_l_c
        climate.temp_r = climate._temp_r_c
        climate.temp_rear = climate._temp_rear_c
        
    def on_f_released(self):
        climate.enable_f = True
        climate.enable_c = False
        climate.temp_l = climate._temp_l_f
        climate.temp_r = climate._temp_r_f
        climate.temp_rear = climate._temp_rear_f

    def auto_simulation(self):
        """random increase or decrease fan speed in a random duration"""

        max_time = 3000
        min_time = 500
        if evehp_datetime_waituntil(climate.timer_id_l, climate.time_wait_l):
            climate.time_wait_l = evehp_math_random(min_time, max_time)
            is_up = 1 if climate.time_wait_l % 2 == 0 else 0
            self.on_fan_changed(l=True, up = is_up, down = not is_up)

        if evehp_datetime_waituntil(climate.timer_id_r, climate.time_wait_r):
            climate.time_wait_r = evehp_math_random(min_time, max_time)
            is_up = 1 if climate.time_wait_r % 2 == 0 else 0
            self.on_fan_changed(r=True, up = is_up, down = not is_up)

        if evehp_datetime_waituntil(climate.timer_id_rear, climate.time_wait_rear):
            climate.time_wait_rear = evehp_math_random(min_time, max_time)
            is_up = 1 if climate.time_wait_rear % 2 == 0 else 0
            self.on_fan_changed(rear=True, up = is_up, down = not is_up)
            
        a_random_number = climate.time_wait_l + climate.timer_id_r + climate.timer_id_rear
        climate.enable_freshair = 1 if a_random_number % 2 > 0 else 0

    def change_backlight(self):
        # print("light: ", climate.backlight, climate.backlight_next)
        if climate.backlight == climate.backlight_next:
            return
        
        diff = abs(climate.backlight - climate.backlight_next)
        step = 1 + (int)(diff / 10)
        if climate.backlight < climate.backlight_next:
            climate.backlight +=step
            if climate.backlight > climate.backlight_next:
                climate.backlight = climate.backlight_next
        else:
            climate.backlight -=step
            if climate.backlight < climate.backlight_next:
                climate.backlight = climate.backlight_next
        
        eve.wr32(eve.REG_PWM_DUTY, (int)(climate.backlight))

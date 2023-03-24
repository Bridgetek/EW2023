""" Embedded Word demo 2023: Car Cliamte Control """
import time
import random


from .helper_calibrate import *
from .helper_datetime import *
from .helper_gesture import *
from .helper_image import *
from .helper_math import *
from .helper_responsive import *
from .app_ui_config import *
from .helper_widgets import *
from .tags import *
from .image import *

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_rp2040 import BrtEveRP2040
else:
    from ..lib.brteve.brt_eve_bt817_8 import BrtEve
    from ..lib.brteve.brt_eve_rp2040 import BrtEveRP2040

layout = evehp_responsive_get()
c = evehp_responsive_convert
appx = layout.appx
appy = layout.appy
appw = layout.appw
apph = layout.apph
DEBUG_ON = 0

class _g():
    """Internal data structure"""
    # constants
    side_calib = 1.0 #-40 to 40
    side_x = 1.0 # will be re-calc
    side_y = 1.0
    side_angle = 1.0
    side_calib_next = 1.0
    class count_side_calib():
        calib05 = 0
        calib10 = 0
        calib20 = 0
        calib30 = 0
        calib40 = 0
        total = 0

    back_calib = 1.9 #-40 to 40
    back_x = 1.0
    back_y = 1.0
    back_angle = 1.0
    back_calib_next = 1.0
    class count_back_calib():
        calib05 = 0
        calib10 = 0
        calib20 = 0
        calib30 = 0
        calib40 = 0
        total = 0

    calib_up_rate = 5
    calib_down_rate = 1
    calib05_rate = 80 # % calibrate 0-5
    calib10_rate = 60 # % calibrate 5-10
    calib20_rate = 40 # % calibrate 10-20
    calib30_rate = 20 # % calibrate 20-30
    calib40_rate = 10 # % calibrate 30-40

    is_danger = False

    _MAX_CALIB = 40
    _MIN_CALIB = -40

    reset = True # reset flag
    
    page_nth = 0
    
    distance = 0
    p2_count=0
    speed = 50

eve=None
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
        eve.init(resolution="480x480")
        # eve.cmd_calibrate()

        if _g.page_nth == 0:
            done=1
            for im in eveims:
                self.init_message("Copying image (" + str(done) + "/" + str(len(eveims)) + ")")
                if not evehp_image_copy_to_ramg(eve, im, False):
                    self.init_message("Error copying image, please reset")
                    while 1:
                        time.sleep(1000)
                done+=1   

        # set development lcd
        evehp_responsive_set_screen(480, 480, eve.lcd_width, eve.lcd_height,)
        # set runtime lcd
        evehp_responsive_set_app_size(0, 0, 480, 480)


        layout = evehp_responsive_get()
        c = evehp_responsive_convert
        appx = layout.appx
        appy = layout.appy
        appw = layout.appw
        apph = layout.apph

        # configuration screen
        self.conf=ui_config(eve)
        while not self.conf.done:
            self.start_display()
            self.conf.draw()
            self.end_display()

        evehp_datetime_now(confdata.hh, confdata.mm)
        eve.wr32(eve.REG_PWM_DUTY, (int)(confdata.light_level))
        
        while (1):
            # continue
            self.start_display()
            self.drawing()
            self.process_event()
            self.end_display()


    def init_message(self, msg):
        eve.cmd_dlstart()
        eve.ClearColorRGB(51, 153, 255)
        eve.Clear(1, 1, 1)
        eve.VertexFormat(3)
        # self.drawing()
        eve.ColorRGB(255, 255, 255)
        eve.cmd_text(c(240), c(50), 31, eve.OPT_CENTER, "Bridgetek")
        eve.cmd_text(c(240), c(240), 30, eve.OPT_CENTER, "The Car Inclinometer demo")
        eve.ColorRGB(0, 51, 204)
        eve.cmd_text(c(240), c(280), 27, eve.OPT_CENTER, msg)
        self.end_display()

    def start_display(self):
        eve.cmd_dlstart()
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear(1, 1, 1)
        eve.VertexFormat(3)

    def drawing(self):
        if _g.page_nth == 0:
            self.draw_bg()
            self.draw_arrow_top() 
            self.draw_arrow_size()
            self.draw_car_side()
            self.draw_car_back()
        elif _g.page_nth == 1:  self.draw_page2()
        elif _g.page_nth == 2: self.draw_page3()
        else : self.draw_page4()

    def end_display(self):
        eve.Display()
        eve.cmd_swap()
        eve.finish()

    def draw_bg(self):
        eve.ColorRGB(0, 0, 0)
        eve.Begin(eve.POINTS)
        eve.PointSize(800)
        eve.Vertex2f(0, 0)

        eve.ColorRGB(255, 255, 255)
        evehp_image_draw(eve, eveim_bg, msg_on=False, tag=tag_bg)
        
        _g.is_danger = False
        if abs(_g.back_calib) >= 30 or abs(_g.side_calib) >= 30:
            _g.is_danger = True
            evehp_image_draw(eve, eveim_DANGER__, msg_on=False, tag=tag_bg)

    def draw_car_side(self):
        im=eveim_Jeep_Side_view
        
        if abs(_g.side_calib) >= 30:
            eve.ColorRGB(248,89,89)
            
        angle_draw= (_g.side_angle - 270) % 360
        angle_pos = (360 - _g.side_angle + 90) % 360

        RADIUS = 100 # const
        x = 240 + RADIUS * math.sin(math.pi * 2 * angle_pos / 360) - im.w/2
        y = 240 + RADIUS * math.cos(math.pi * 2 * angle_pos / 360) - im.h/2
        evehp_image_draw(eve, im, x, y, rotate_angle=angle_draw)

        txt = evehp_math_zfill(str(int(abs(_g.side_calib))), 2)
        eve.cmd_text(40, c(240 - 70), 30, 0, txt)
        if _g.side_calib < 0:
            eve.cmd_text(20, c(240 - 72), 30, 0, "-")

        eve.ColorRGB(255, 255, 255)
        
    def draw_car_back(self):
        pass
        im=eveim_Jeep_Back_view
        
        if abs(_g.back_calib) >= 30:
            eve.ColorRGB(248,89,89)

        angle_draw=(_g.back_angle - 270 - 180) % 360
        angle_pos = 360 - _g.back_angle + 90
        RADIUS = 95 # const
        x = 240 + RADIUS * math.sin(math.pi * 2 * angle_pos / 360) - im.w/2
        y = 240 + RADIUS * math.cos(math.pi * 2 * angle_pos / 360) - im.h/2
        evehp_image_draw(eve, im, x, y, rotate_angle=angle_draw)

        txt = evehp_math_zfill(str(int(abs(_g.back_calib))), 2)
        eve.cmd_text(40, c(240 + 25), 30, 0, txt)
        if _g.back_calib < 0:
            eve.cmd_text(20, c(240 + 25), 30, 0, "-")

        eve.ColorRGB(255, 255, 255)

    def draw_arrow_top(self):
        pass
        evehp_image_draw(eve, eveim_arrow2, _g.side_x, _g.side_y, rotate_angle=_g.side_angle)

    def draw_arrow_size(self):
        pass
        evehp_image_draw(eve, eveim_arrow2, _g.back_x, _g.back_y, rotate_angle=_g.back_angle)

    def next_calib(self, cur_calib, calib_count):
        calib_count.total +=1
        
        rd = random.randint(0, 100)
        is_calib05 = True if rd < _g.calib05_rate else False
        is_calib10 = True if rd < _g.calib10_rate else False
        is_calib20 = True if rd < _g.calib20_rate else False
        is_calib30 = True if rd < _g.calib30_rate else False
        is_calib40 = True if rd < _g.calib40_rate else False
        is_negative = 1 if random.randint(0, 100) < 50 else -1
        
        rate_05 = calib_count.calib05 * 100 / calib_count.total
        rate_10 = calib_count.calib10 * 100 / calib_count.total
        rate_20 = calib_count.calib20 * 100 / calib_count.total
        rate_30 = calib_count.calib30 * 100 / calib_count.total
        rate_40 = calib_count.calib40 * 100 / calib_count.total
                
        if rate_05 > _g.calib05_rate: is_calib05  = 0
        if rate_10 > _g.calib10_rate: is_calib10  = 0
        if rate_20 > _g.calib20_rate: is_calib20  = 0
        if rate_30 > _g.calib30_rate: is_calib30  = 0
        if rate_40 > _g.calib40_rate: is_calib40  = 0

        # if cur_calib >= 0  and 5  >= cur_calib: is_calib05  = 0
        if cur_calib >= 6  and 10 >= cur_calib: is_calib10  = 0
        if cur_calib >= 11 and 20 >= cur_calib: is_calib20  = 0
        if cur_calib >= 21 and 30 >= cur_calib: is_calib30  = 0
        if cur_calib >= 31 and 40 >= cur_calib: is_calib40  = 0
        
        # print("rate: ", rate_05 ,rate_10 ,rate_20 ,rate_30 ,rate_40)
        # print("is__: ", is_calib05 ,is_calib10 ,is_calib20 ,is_calib30 ,is_calib40)

        if is_calib40:
            calib_count.calib40 +=1
            return is_negative * 35 + random.randint(_g.calib_down_rate, _g.calib_up_rate)

        elif is_calib30:
            calib_count.calib30 +=1
            return is_negative *25 + random.randint(_g.calib_down_rate, _g.calib_up_rate)
        
        elif is_calib20:
            calib_count.calib20 +=1
            return is_negative *15 + random.randint(_g.calib_down_rate, _g.calib_up_rate)
        
        elif is_calib10:
            calib_count.calib10 +=1
            return is_negative *5 + random.randint(_g.calib_down_rate, _g.calib_up_rate)
            
        elif is_calib05:
            calib_count.calib05 +=1
            return is_negative *3 + random.randint(_g.calib_down_rate, _g.calib_up_rate)
        else: # is_calib05:
            calib_count.calib05 +=1
            return is_negative *3 + random.randint(_g.calib_down_rate, _g.calib_up_rate)
          
    def renew_side_calib(self):
        if _g.side_calib == _g.side_calib_next:
            _g.side_calib_next = self.next_calib(_g.side_calib_next, _g.count_side_calib)
            
        up_down = random.randint(_g.calib_down_rate, _g.calib_up_rate)
        
        t=1.0 * up_down / 3
        
        if _g.side_calib > _g.side_calib_next:
            if _g.side_calib - t < _g.side_calib_next:
                _g.side_calib = _g.side_calib_next
            else:
                _g.side_calib -= 1.0*t
        else:
            if _g.side_calib + t > _g.side_calib_next:
                _g.side_calib = _g.side_calib_next
            else:
                _g.side_calib += 1.0*t
                
        # vibration
        vb=random.randint(_g.calib_down_rate, _g.calib_up_rate) - (_g.calib_up_rate - _g.calib_down_rate)
        vb /= 8
        draw_calib = _g.side_calib + vb
        
        # test calib
        if DEBUG_ON:
            up_down = 0
            if _g.side_calib == 40: _g.side_calib = 30
            elif _g.side_calib == 30: _g.side_calib = 20
            elif _g.side_calib == 20: _g.side_calib = 10
            elif _g.side_calib == 10: _g.side_calib = 0
            elif _g.side_calib == 0: _g.side_calib = -10
            elif _g.side_calib == -10: _g.side_calib = -20
            elif _g.side_calib == -20: _g.side_calib = -30
            elif _g.side_calib == -30: _g.side_calib = -40
            elif _g.side_calib == -40: _g.side_calib = 40

        calib = _g._MAX_CALIB - draw_calib # transform -40 to +40 -> 0 to 80

        MAX_VAL = _g._MAX_CALIB - _g._MIN_CALIB # const, 0->80  <=>-40 to 40 
        MAX_ANGLE  = 80 # const, 0->71  <=> -40 to 40 as designed on UI
        RADIUS = 140 # const
        ANGLE_OFFSET = 220

        angel_per_val_x10 = MAX_ANGLE * 10 / MAX_VAL
        angle = (ANGLE_OFFSET-angel_per_val_x10 * calib / 10) % 360

        #revert clock wise
        angle = 360 - angle
        _g.side_x = 240 + RADIUS * math.sin(math.pi * 2 * angle / 360) - 8
        _g.side_y = 240 + RADIUS * math.cos(math.pi * 2 * angle / 360) - 14
        _g.side_angle = (360 - (angle - 90)) % 360
        
    def renew_back_calib(self):
        if _g.back_calib == _g.back_calib_next:
            _g.back_calib_next = self.next_calib(_g.back_calib_next, _g.count_back_calib)
            
        up_down = random.randint(_g.calib_down_rate, _g.calib_up_rate)
        
        t=1.0 * up_down / 3
        
        if _g.back_calib > _g.back_calib_next:
            if _g.back_calib - t < _g.back_calib_next:
                _g.back_calib = _g.back_calib_next
            else:
                _g.back_calib -= 1.0*t
        else:
            if _g.back_calib + t > _g.back_calib_next:
                _g.back_calib = _g.back_calib_next
            else:
                _g.back_calib += 1.0*t
                
        # vibration
        vb=random.randint(_g.calib_down_rate, _g.calib_up_rate) - (_g.calib_up_rate - _g.calib_down_rate)
        vb /= 8
        draw_calib = _g.back_calib + vb
        
        # test calib
        if DEBUG_ON:
            up_down = 0
            if _g.back_calib == 40: _g.back_calib = 30
            elif _g.back_calib == 30: _g.back_calib = 20
            elif _g.back_calib == 20: _g.back_calib = 10
            elif _g.back_calib == 10: _g.back_calib = 0
            elif _g.back_calib == 0: _g.back_calib = -10
            elif _g.back_calib == -10: _g.back_calib = -20
            elif _g.back_calib == -20: _g.back_calib = -30
            elif _g.back_calib == -30: _g.back_calib = -40
            elif _g.back_calib == -40: _g.back_calib = 40

        calib = _g._MAX_CALIB - draw_calib # transform -40 to +40 -> 0 to 80

        MAX_VAL = _g._MAX_CALIB - _g._MIN_CALIB # const, 0->80  <=>-40 to 40 
        MAX_ANGLE  = 80 # const, 0->71  <=> -40 to 40 as designed on UI
        RADIUS = 140 # const
        ANGLE_OFFSET = 40

        angel_per_val_x10 = MAX_ANGLE * 10 / MAX_VAL
        angle = (angel_per_val_x10 * calib / 10 - ANGLE_OFFSET) % 360

        #revert clock wise
        angle = 360 - angle
        _g.back_x = 240 + RADIUS * math.sin(math.pi * 2 * angle / 360) - 8
        _g.back_y = 240 + RADIUS * math.cos(math.pi * 2 * angle / 360) - 14
        _g.back_angle = (360 - (angle - 90)) % 360

    def draw_page2(self):
        dt = evehp_datetime_yyyy_mt_dd_hh_mm_ss_ms_weekday_weeks_weekl_mons_monl()
        
        eve.ColorRGB(0, 0, 0)
        evehp_widget_box(eve, 0, -2, eve.lcd_width,eve.lcd_height + 4)
        eve.ColorRGB(16,33,51)
        h = eve.lcd_height * 8 / 10
        y = eve.lcd_height / 2 - h/2
        evehp_widget_box(eve, 0, y, eve.lcd_width,h)

        cx = c(240)
        cy = c(240)
        eve.ColorRGB(255, 255, 255)

        # date
        y = c(60)
        text = str(dt[3]) +":"+ str(dt[4]) +":"+ str(dt[5])
        eve.cmd_text(cx, y, 31, eve.OPT_CENTERX, text)
        
        # clock
        y+=60
        text =  str(dt[9]) +"   "+str(dt[2]) +"-"+ str(dt[1]) +"-"+ str(dt[0])
        eve.cmd_text(cx, y, 28, eve.OPT_CENTERX, text)        

        y += 50
        eve.ColorRGB(153, 51, 0)
        evehp_widget_box(eve, 0, y, eve.lcd_width, 2)
        eve.ColorRGB(255, 255, 255)

        # info
        y+=60

        _g.p2_count +=1
        if _g.p2_count > 10:
            _g.p2_count = 0
            
            _g.distance += random.randint(1, 10) / 20
            _g.speed += (random.randint(5, 10) - 5)
            if _g.speed < 10 or _g.speed > 90:
                _g.speed = 50

            if _g.distance > 100000:
                _g.distance = 0

        text =  "Speed: " + str(_g.speed) + " mph"
        eve.cmd_text(cx - 88, y, 29, 0, text)     

        y+=50
        text =  "Distance: " + str(int(_g.distance)) + " miles"
        eve.cmd_text(cx-105, y, 29, 0, text)     

        y+=50
        text =  "Axis side: " + str(int(_g.side_calib)) + " degree"
        eve.cmd_text(cx-115, y, 29, 0, text)

        y+=50
        text =  "Axis back: " + str(int(_g.back_calib)) + " degree"
        eve.cmd_text(cx-115, y, 29, 0, text)
           


    def draw_page3(self): pass
    def draw_page4(self): pass
    
    def process_event(self):
        self.renew_side_calib()
        self.renew_back_calib()

        g = evehp_gesture_renew(eve)
        if g.isRelease:
            _g.page_nth += 1
            _g.page_nth %= 2

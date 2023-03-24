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
from .app_tpms_wheel import *
from .app_tpms_wheel_color import color_val
from .app_digit_image import *

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_rp2040 import BrtEveRP2040
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve
    from lib.brteve.brt_eve_rp2040 import BrtEveRP2040


ORG="Bridgetek"
PROGRAM="The Car Tyre Pressure Monitoring System"

layout = evehp_responsive_get()
c = evehp_responsive_convert
appx = layout.appx
appy = layout.appy
appw = layout.appw
apph = layout.apph
eve=None

class tpms():
    """Main application"""

    def __init__(self, brteve, conf) -> None:
        global layout
        global c
        global appx
        global appy
        global appw
        global apph
        global eve 

        layout = evehp_responsive_get()
        c = evehp_responsive_convert
        appx = layout.appx
        appy = layout.appy
        appw = layout.appw
        apph = layout.apph
        eve = brteve
        
        self.conf = conf
        
        self.topl = wheel(0, 0)
        self.topr = wheel(0, 0, arrow=eveim_arrowr)
        self.botl = wheel(0, 0)
        self.botr = wheel(0, 0, arrow=eveim_arrowr)
        
        [i.refresh() for i in self.wheels()]
        
        
        self._PAGE_1=0
        self._PAGE_2=1
        self.page_nth = self._PAGE_1
        
        self.change_unit_countdown = 0
        
        self.random_id = 0
        
        self.text_p1_unit_pressure = "PSI"
        self.text_p1_unit_temp = "C"
        self.text_depth_unit = "mm"

        self.unit_pressure=UNIT_PS_PSI
        self.unit_temp=UNIT_TEMP_OFF
        self.unit_depth=UNIT_DEPTH_MM
        
        self._MENU_OFF = 0
        self._MENU_PRESSURE = 1
        self._MENU_TEMP = 2
        self._MENU_DEPTH = 3
        
        self.menu_on = self._MENU_OFF

        self.backlight_on = 0x80
        self._BACKLIGHT_OFF = 5
        
        self.backlight = self.backlight_on
        self.backlight_next = self.backlight_on
                
                
    def wheels(self):
        return [self.topl, self.topr, self.botr, self.botl]
    
    def draw(self):
        self.draw_bg()
        
        #date time
        hh, mm, ss, ms, ampm = evehp_datetime_now_hh_mm_ss_ms_ampm()
        hh = evehp_math_zfill(hh, 2)
        mm = evehp_math_zfill(mm, 2)
        ss = evehp_math_zfill(ss, 2)
        ms = evehp_math_zfill(ms, 3)
        x = appx + appw - 100
        y = appy + apph - 27
        eve.cmd_text(x, y, 26, 0, hh + ":" + mm + ":" + ss + "  " + ampm)
        eve.ColorA(0)
        evehp_widget_box(eve, x-5, y-10, 100, 40, tag= tag_datetime)
        eve.ColorA(255)
        eve.TAG(tag_none)
        
        w = 136
        h = 152
        x = appx + appw/2 - w/2
        y = appy + 152
        self.draw_wheels(x, y, self.topl)
        self.draw_wheels(x+w, y, self.topr)
        self.draw_wheels(x, y+h, self.botl)
        self.draw_wheels(x+w, y+h, self.botr)

    def draw_bg(self):
        eve.ColorRGB(255, 255, 255)
        x = appx
        y = appy
        
        # background
        evehp_image_draw(eve, eveim_bg,x, y, tag=tag_bg)
        # indicator
        y = appy + apph - 40
        if self.page_nth == self._PAGE_1:
            evehp_image_draw(eve, eveim_indicator, x+102, y, tag=tag_bg)

        # right button tap
        w = 104
        h = 40
        imx = appx + appw - 150
        imy = appy + apph - 100
        eve.ColorA(0)
        evehp_widget_box(eve, imx, imy, 150, 50, tag=tag_page)
        eve.ColorA(255)
        
        # self.draw_menu()
        self.draw_menu_img()
    
    def draw_menu(self):
        x = appx + 104/2
        y = appy + apph - 20
        
        # menu
        font = 21
        if self.page_nth == self._PAGE_1:
            txt=["Pressure\n(kPa)", "Pressure\n(Bar)", "Pressure\n(PSI)"]
            eve.cmd_text(x, y, font, eve.OPT_CENTER, txt[self.unit_pressure])
            
            x += 104
            txt=["Temperature\n(  C)", "Temperature\n(  F)"]
            eve.cmd_text(x, y, font, eve.OPT_CENTER, txt[self.unit_temp])
            eve.cmd_text(x-5, y, 20, 0, "o")
                        
            x = appx
            y = appy + apph - 40
            w = 104
            h = 40
            eve.ColorA(0)
            evehp_widget_box(eve, x, y, w, h, rounded=6, tag=tag_menu_pressure)
            evehp_widget_box(eve, x+w, y, w, h, rounded=6, tag=tag_menu_temp)
            eve.ColorA(255)
            
        else:
            txt=["Depth\n (mm)", "Depth\n(inch)"]
            eve.cmd_text(x, y, font, eve.OPT_CENTER, txt[self.unit_depth])
            
            x = appx
            y = appy + apph - 40
            w = 104
            h = 40
            eve.ColorA(0)
            evehp_widget_box(eve, x, y, w, h, rounded=6, tag=tag_menu_depth)
            eve.ColorA(255)
                
        w = 104
        h = 34
        x = appx
        y = appy + apph - 40
        if self.menu_on == self._MENU_PRESSURE:
            txt=["kPa", "Bar", "PSI"]
            tags=[tag_menu_pressure_kpa, tag_menu_pressure_bar, tag_menu_pressure_psi]
            self.draw_submenu_lv1(x, y, w, h, txt, self.unit_pressure, tags)
        elif self.menu_on == self._MENU_DEPTH:
            txt=["mm", "inch"]
            tags=[tag_menu_depth_mm, tag_menu_depth_in]
            self.draw_submenu_lv1(x, y, w, h, txt, self.unit_depth, tags)
        elif self.menu_on == self._MENU_TEMP:
            x = appx + 104
            # txt=["C", "F"]
            # tags=[tag_menu_temp_c, tag_menu_temp_f]
            # self.draw_submenu_lv1(x, y, w, h, txt, self.unit_temp, tags)
            
            img=[eveim_C, eveim_F]
            tags=[tag_menu_temp_c, tag_menu_temp_f]
            self.draw_submenu_lv1_img(x, y, w, h, img, self.unit_temp, tags)

    def draw_submenu_lv1(self,x, y, w, h, txt_list, selected_nth, tags):
        num = len(txt_list)
        bh = h*num
        evehp_widget_box(eve, x, y-bh, w, bh, color=[0x17,0x1F,0x30])
        
        # green color theselecting unit
        imy= [y - (i+1)*h for i in range (num)]
        evehp_widget_box(eve, x, imy[selected_nth], w, h, rounded=6, color=[0x28, 0x78, 0xD3])
        eve.ColorRGB(255, 255, 255)
        
        font = 22
        for i in range (num):
            txt = txt_list[i]
            ix = x + w/2
            iy = imy[i] + h/2
            eve.cmd_text(ix, iy, font, eve.OPT_CENTER, txt)
        
        eve.ColorA(0)
        for i in range (num):
            evehp_widget_box(eve, x, imy[i], w, h, rounded=6, tag=tags[i])
        eve.ColorA(255)
        
    def draw_menu_img(self):
        x = appx
        y = appy + apph - 40
        
        # menu
        padding_top = 6
        adjust_x = 1
        y += padding_top
        if self.page_nth == self._PAGE_1:
            if self.unit_pressure == UNIT_PS_PSI:
                im = eveim_Pressure__PSI_
                ix = 104/2 - im.w/2 + adjust_x
                evehp_image_draw(eve, im, x + ix, y, tag=tag_menu_pressure)
            elif self.unit_pressure == UNIT_PS_BAR:
                im = eveim_Pressure__BAR_
                ix = 104/2 - im.w/2 + adjust_x
                evehp_image_draw(eve, im, x + ix, y, tag=tag_menu_pressure)
            elif self.unit_pressure == UNIT_PS_KPA:
                im = eveim_Pressure__KPA_
                ix = 104/2 - im.w/2 + adjust_x
                evehp_image_draw(eve, im, x + ix, y, tag=tag_menu_pressure)
            
            imx = x + 104
            if self.unit_temp == UNIT_TEMP_C:
                im = eveim_Temperature__C_
                ix = 104/2 - im.w/2 + adjust_x
                evehp_image_draw(eve, im, imx + ix, y, tag=tag_menu_temp)
            elif self.unit_temp == UNIT_TEMP_F:
                im = eveim_Temperature__F_
                ix = 104/2 - im.w/2 + adjust_x
                evehp_image_draw(eve, im, imx + ix, y, tag=tag_menu_temp)
            elif self.unit_temp == UNIT_TEMP_OFF:
                im = eveim_Temperature__Off_
                ix = 104/2 - im.w/2 + adjust_x
                evehp_image_draw(eve, im, imx + ix, y, tag=tag_menu_temp)
            
            y -=padding_top
            eve.ColorA(0)
            w = 104
            h = 40
            evehp_widget_box(eve, x, y, w, h, rounded=6, tag=tag_menu_pressure)
            evehp_widget_box(eve, x+w, y, w, h, rounded=6, tag=tag_menu_temp)
            eve.ColorA(255)
            
        else:
            if self.unit_depth == UNIT_DEPTH_MM:
                im = eveim_Depth__mm_
                ix = 104/2 - im.w/2 + adjust_x
                evehp_image_draw(eve, im, x + ix, y, tag=tag_menu_depth)
            elif self.unit_depth == UNIT_DEPTH_IN:
                im = eveim_Depth__inch_
                ix = 104/2 - im.w/2 + adjust_x
                evehp_image_draw(eve, im, x + ix, y, tag=tag_menu_depth)

            y -=padding_top
            eve.ColorA(0)
            w = 104
            h = 40
            evehp_widget_box(eve, x, y, w, h, rounded=6, tag=tag_menu_depth)
            eve.ColorA(255)
                
        w = 104
        h = 34
        x = appx
        y = appy + apph - 40
        if self.menu_on == self._MENU_PRESSURE:
            img=[eveim_kPa_menu, eveim_Bar_menu, eveim_PSI_menu]
            tags=[tag_menu_pressure_kpa, tag_menu_pressure_bar, tag_menu_pressure_psi]
            self.draw_submenu_lv1_img(x, y, w, h, img, self.unit_pressure, tags)
        elif self.menu_on == self._MENU_DEPTH:
            img=[eveim_mm_menu, eveim_inch_menu]
            tags=[tag_menu_depth_mm, tag_menu_depth_in]
            self.draw_submenu_lv1_img(x, y, w, h, img, self.unit_depth, tags)
        elif self.menu_on == self._MENU_TEMP:
            x = appx + 104
            img=[eveim_C, eveim_F, eveim_Off]
            tags=[tag_menu_temp_c, tag_menu_temp_f, tag_menu_temp_off]
            self.draw_submenu_lv1_img(x, y, w, h, img, self.unit_temp, tags)

    def draw_submenu_lv1_img(self,x, y, w, h, ims:evehp_image, selected_nth, tags):
        num = len(ims)
        bh = h*num
        evehp_widget_box(eve, x, y-bh, w, bh, color=[0x17,0x1F,0x30])
        
        # green color theselecting unit
        imy= [y - (i+1)*h for i in range (num)]
        evehp_widget_box(eve, x, imy[selected_nth], w, h, rounded=6, color=[0x28, 0x78, 0xD3])
        eve.ColorRGB(255, 255, 255)
        
        for i in range (num):
            im = ims[i]
            ix = w/2 - im.w/2
            iy = h/2 - im.h/2
            evehp_image_draw(eve, im, x + ix, imy[i] + iy, tag=tags[i])
        
        eve.ColorA(0)
        for i in range (num):
            evehp_widget_box(eve, x, imy[i], w, h, rounded=6, tag=tags[i])
        eve.ColorA(255)
        
    def draw_wheels(self, x, y, tp:wheel):
        if self.page_nth == self._PAGE_1:
            im=eveim_Tyre_Pressure1
            imx = appx + appw/2 - im.w/2
            imy = appy + 50
            evehp_image_draw(eve, im, imx, imy)
            
            imx = appx + appw - 130
            imy = appy + apph - 87
            evehp_image_draw(eve, eveim_Tyre_Tread_Depth, imx, imy, tag=tag_page)
            
            return self.draw_wheels_page1(x, y, tp)
        else:
            im=eveim_Tyre_Tread_Depth1
            imx = appx + appw/2 - im.w/2
            imy = appy + 50
            evehp_image_draw(eve, im, imx, imy)
            
            imx = appx + appw - 130
            imy = appy + apph - 87
            evehp_image_draw(eve, eveim_Tyre_Pressure, imx, imy, tag=tag_page)
            
            return self.draw_wheels_page2(x, y, tp)
            
    def draw_wheels_page2(self, x, y, tp:wheel):
        w = 15
        h = 44
        font1=30
        font2=22
        
        pc = tp.val_depth / tp.depth_color.max_red
        pc15 = pc* 10
        point_size = 15
        radius = point_size - pc15
        # print("depth radius", radius)
        
        # ball
        imx = x + tp.position_x_point
        imy = y + 25
        #border point
        evehp_widget_point(eve, imx, imy, point_size, color=[tp.color_depth[0], tp.color_depth[1], tp.color_depth[2]])
        #inner point
        evehp_widget_point(eve, imx, imy, radius, color=[50,87,146])
        eve.ColorRGB(tp.color_depth[0], tp.color_depth[1], tp.color_depth[2])
        
        # depth
        imy = y + 16
        imy_unit = -5
        if self.unit_depth == UNIT_DEPTH_IN:
            imy_unit = 0
        
            
        if tp.im_arrow == eveim_arrowr:
            digit_x = x + tp.position_x_depth + 10
            imx = draw_digits(eve, digit_x, imy, eveim_digits_big, text=tp.text_depth)
            evehp_image_draw(eve, tp.im_depth_unit, imx + 10, imy-imy_unit)
        else:            
            imx = x + tp.position_x_point - point_size - 12 - tp.im_depth_unit.w
            evehp_image_draw(eve, tp.im_depth_unit, imx, imy - imy_unit)
            draw_digits(eve, imx - 10, imy, eveim_digits_big, text=tp.text_depth, draw_from_right = True)

        # arrow
        imx = x + tp.position_x_arrow - 2
        imy = y + 17
        evehp_image_draw(eve, tp.im_arrow, imx, imy)
        
        eve.ColorRGB(255, 255, 255)
        
    def draw_wheels_page1(self, x, y, tp:wheel):
        """Draw information beside a wheel

        Args:
            x (int): wheel position x
            y (int): wheel position y
            tp (wheel): wheel object
        """        
        
        w = 15
        h = 44
        font1=30
        font2=22
        
        # box
        imx = x + tp.position_x_bar
        imy = y + 5
        border=8
        pc = min(tp.val_ps / tp.pressure_color.max_red, 1)
        # print("pc", pc)
        imh = pc * (h-border/2)

        # box
        evehp_widget_box(eve, imx+border/2 - 3, imy + border + (41-imh), w-border+5, imh-border, 
            rounded=6, color=[tp.color_ps[0], tp.color_ps[1], tp.color_ps[2]])
        # box border
        eve.ColorRGB(255, 255, 255)
        evehp_image_draw(eve, eveim_zpressure_bar, imx, imy, tag_none)
        
        # pressure
        eve.ColorRGB(tp.color_ps[0], tp.color_ps[1], tp.color_ps[2])
        imy = y + 9
        if tp.im_arrow == eveim_arrowr:
            x_last_digit = draw_digits(eve, x + tp.position_x_pressure, imy, eveim_digits_big, text=tp.text_ps)
            imx = x_last_digit
            evehp_image_draw(eve, tp.im_ps_unit, imx + 10, imy)
        else:
            imx = x - 86
            evehp_image_draw(eve, tp.im_ps_unit, imx, imy)
            imx -= 10
            x_last_digit = draw_digits(eve, imx, imy, eveim_digits_big, text=tp.text_ps, draw_from_right = True)
            
        # eve.cmd_text(x + tp.position_x_pressure      , y     , tp.font1, 0, tp.text_ps)
        # eve.cmd_text(imx , imy     , tp.font1, 0, tp.text_ps_unit)
        
        if self.unit_temp != UNIT_TEMP_OFF:
            # temp
            is_left = 1 if tp.im_arrow == eveim_arrowl else 0   
            imy = y + 37
            eve.ColorRGB(tp.color_temp[0], tp.color_temp[1], tp.color_temp[2])
            if is_left:
                imx = draw_digits(eve, x_last_digit, imy, eveim_digits_small, text=tp.text_temp)
                evehp_image_draw(eve, tp.im_temp_unit, imx + 10, imy)
                # eve.cmd_text(x + tp.position_x_temp      , imy, tp.font2, 0, tp.text_temp)
                # eve.cmd_text(x + tp.position_x_temp_unit , imy, tp.font2, 0, tp.text_temp_unit)
                # eve.cmd_text(x + tp.position_x_temp_unit, imy, 26   , 0, "o")
            else:
                imx = x_last_digit + 10 + tp.im_ps_unit.w - tp.im_temp_unit.w
                evehp_image_draw(eve, tp.im_temp_unit, imx, imy)
                imx = draw_digits(eve, imx - 10, imy, eveim_digits_small, text=tp.text_temp, draw_from_right=1)
            
            eve.ColorRGB(255, 255, 255)
        
        # arrow
        imx = x + tp.position_x_arrow - 2
        imy = y + 17
        evehp_image_draw(eve, tp.im_arrow, imx, imy)

        eve.ColorRGB(255, 255, 255)

    def process_event(self):
        # print(".", end='')
        # return
        g = evehp_gesture_renew(eve)
            
        if g.tagReleased == tag_menu_pressure_kpa: 
            [i.change_pressure_unit(self.unit_pressure, UNIT_PS_KPA) for i in self.wheels()]
            self.menu_on = self._MENU_OFF
            self.unit_pressure = UNIT_PS_KPA
        elif g.tagReleased == tag_menu_pressure_bar: 
            [i.change_pressure_unit(self.unit_pressure, UNIT_PS_BAR) for i in self.wheels()]
            self.menu_on = self._MENU_OFF
            self.unit_pressure = UNIT_PS_BAR
        elif g.tagReleased == tag_menu_pressure_psi: 
            [i.change_pressure_unit(self.unit_pressure, UNIT_PS_PSI) for i in self.wheels()]
            self.menu_on = self._MENU_OFF
            self.unit_pressure = UNIT_PS_PSI
            
        elif g.tagReleased == tag_menu_temp_c: 
            [i.change_temp_unit(self.unit_temp, UNIT_TEMP_C) for i in self.wheels()]
            self.unit_temp = UNIT_TEMP_C
            self.menu_on = self._MENU_OFF
        elif g.tagReleased == tag_menu_temp_f: 
            [i.change_temp_unit(self.unit_temp, UNIT_TEMP_F) for i in self.wheels()]
            self.menu_on = self._MENU_OFF
            self.unit_temp = UNIT_TEMP_F
        elif g.tagReleased == tag_menu_temp_off: 
            [i.change_temp_unit(self.unit_temp, UNIT_TEMP_OFF) for i in self.wheels()]
            self.menu_on = self._MENU_OFF
            self.unit_temp = UNIT_TEMP_OFF
            
        elif g.tagReleased == tag_menu_depth_mm: 
            [i.change_depth_unit(self.unit_depth, UNIT_DEPTH_MM) for i in self.wheels()]
            self.menu_on = self._MENU_OFF
            self.unit_depth = UNIT_DEPTH_MM
        elif g.tagReleased == tag_menu_depth_in: 
            [i.change_depth_unit(self.unit_depth, UNIT_DEPTH_IN) for i in self.wheels()]
            self.menu_on = self._MENU_OFF
            self.unit_depth = UNIT_DEPTH_IN
            
        elif g.tagReleased == tag_menu_pressure:
            self.menu_on = self._MENU_PRESSURE
        elif g.tagReleased == tag_menu_temp: 
            self.menu_on = self._MENU_TEMP
        elif g.tagReleased == tag_menu_depth: 
            self.menu_on = self._MENU_DEPTH
        elif g.tagReleased == tag_datetime: 
            self.conf.done = 0
            
        elif g.tagReleased == tag_page:
            self.page_nth +=1
            self.page_nth %= 2 
            self.menu_on = self._MENU_OFF
            
        self.random_id+=1
        self.random_id %=4
        self.next_random(self.wheels()[self.random_id])
        
        # if g.isDoubleTap:
        #     # reset values
        #     for i in self.wheels():
        #         i.val_ps = i.pressure_color.min_green
        #         i.val_temp = i.temp_color.min_green
        #         i.val_depth = i.depth_color.min_green

    def next_random(self, tp: wheel):
        if self.change_unit_countdown > 0:
            self.change_unit_countdown -=1
            return
        
        tp.random_id+=1
        tp.random_id %=3
        # print("cpu temp", microcontroller.cpu.temperature) # use cpu temp for a random?
        rd = random.randint(-1000, 1000)
        rd_val = 1.0*rd / 1000
        
        if tp.random_id == 0:
            if self.unit_pressure == UNIT_PS_BAR:
                rd_val /=10
                
            tp.val_ps += rd_val
            tp.val_ps = max(min(tp.val_ps, tp.pressure_color.max), tp.pressure_color.min)
            tp.text_ps = str(round(tp.val_ps, 1))
        elif tp.random_id == 1:
            if self.unit_temp == UNIT_TEMP_F:
                rd_val /=2
                
            tp.val_temp += rd_val
            tp.val_temp = max(min(tp.val_temp, tp.temp_color.max), tp.temp_color.min)
            tp.text_temp = str(round(tp.val_temp, 1))
        elif tp.random_id == 2:
            if self.unit_depth == UNIT_DEPTH_IN:
                rd_val /= 25.4
            
            tp.val_depth += rd_val
            tp.val_depth = max(min(tp.val_depth, tp.depth_color.max), tp.depth_color.min)
            
            if self.unit_depth == UNIT_DEPTH_IN:
                tp.text_depth = str(round(tp.val_depth, 2))
            else:
                tp.text_depth = str(round(tp.val_depth, 1))

        # print(".", end='')
        tp.refresh()
        
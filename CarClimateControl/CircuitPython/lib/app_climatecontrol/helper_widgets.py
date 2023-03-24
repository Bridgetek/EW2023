"""Helper functions for the BridgeTek EVE's application
   Version: 1.0
   Last modified: 9 Jan 2023 - Tuan.Nguyen@Brtchip.com
"""

import time
import math
import sys
from .helper_gesture import *

if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve


class _evehp_responsive_glob:
    """internal data"""


def _color_tag(eve: BrtEve, color, tag):
    if (tag):
        eve.Tag(tag)
    if color != -1:
        eve.ColorRGB(color[0], color[1], color[2])


def evehp_widget_message(eve: BrtEve, text):
    print("EVE message: ", text)
    w = eve.lcd_width
    h = eve.lcd_height
    x = w/2
    y = h/2
    opt = eve.OPT_CENTER #| eve.OPT_FILL

    eve.cmd_dlstart()
    eve.ClearColorRGB(255, 255, 255)
    eve.Clear(1, 1, 1)
    eve.VertexFormat(3)

    eve.ColorRGB(0, 0, 0)
    # eve.cmd_fillwidth(w *9 / 10) # enable this on BT881 will faulty
    eve.cmd_text(x, y, 30, opt, text)
    eve.Display()
    eve.cmd_swap()
    eve.finish()

def evehp_widget_track_rotary(eve:BrtEve, topx, topy, diameter, tag, transparent=0):
    eve.ColorA(transparent)
    eve.cmd_track(topx, topy, 1, 1, tag)
    eve.Tag(tag)
    eve.Begin(eve.POINTS)
    eve.PointSize(diameter)
    eve.Vertex2f(topx, topy)
    # restore transparent
    if transparent != 255:
        eve.ColorA(255)

def evehp_widget_non_square_scan(eve: BrtEve, start_hfs_w, lcd_w = 800):
    eve_w = eve.lcd_width
    eve_h = eve.lcd_height
    scanw = start_hfs_w
    while 1:
        
        eve.wr32(eve.REG_HSIZE, scanw)
        eve.cmd_hsf(lcd_w)
        scanw +=1
        
        eve.cmd_dlstart()
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear(1, 1, 1)
        eve.ColorRGB(0, 0, 0)
        
        eve.ColorRGB(0, 0, 0)
        evehp_widget_point(eve, eve_w/2, eve_h/2, eve_w/2 if eve_w < eve_h else eve_h/2)
        eve.ColorRGB(255, 0, 0)
        evehp_widget_box(eve, eve_w/2, 0, 1, eve_h)
        evehp_widget_box(eve, 0, eve_h/2, eve_w, 1)
        
        
        text="Non-square Testing with cmd_hfs(" + str(scanw) + ")"
        print(text)
        eve.ColorRGB(255, 0, 0)
        eve.cmd_fillwidth(eve_w *9 / 10)
        eve.cmd_text(eve_w/2, eve_h/2, 30, eve.OPT_CENTER | eve.OPT_FILL, text)
        
        eve.Display()
        eve.cmd_swap()
        eve.flush()
        eve.finish()
        time.sleep(.5)

def evehp_widget_non_square(eve: BrtEve, physical_w, physical_h, evew = 0, test=0):
    eve_w = eve.lcd_width
    if evew:
        eve_w = evew
    eve_h = eve.lcd_height
    logical_w = int(eve_h * physical_w / physical_h)

    # Configure panel
    eve.wr32(eve.REG_HSIZE, (int)(logical_w))
    eve.cmd_hsf(eve_w)

    if test:
        eve.cmd_dlstart()
        eve.ClearColorRGB(255, 255, 255)
        eve.Clear(1, 1, 1)
        eve.ColorRGB(0, 0, 0)
        
        eve.ColorRGB(0, 0, 0)
        evehp_widget_point(eve, eve_w/2, eve_h/2, eve_w/2 if eve_w < eve_h else eve_h/2)
        eve.ColorRGB(255, 0, 0)
        evehp_widget_box(eve, eve_w/2, 0, 1, eve_h)
        evehp_widget_box(eve, 0, eve_h/2, eve_w, 1)
        
        
        text="Non-square Testing with cmd_hfs(" + str(logical_w) + ")" + str(eve_w)
        print(text)
        eve.ColorRGB(255, 0, 0)
        eve.cmd_fillwidth(eve_w *9 / 10)
        eve.cmd_text(eve_w/2, eve_h/2, 30, eve.OPT_CENTER | eve.OPT_FILL, text)
        
        eve.Display()
        eve.cmd_swap()
        eve.flush()
        eve.finish()
        while 1: time.sleep(1000)
            
def evehp_widget_point(eve: BrtEve, x, y, radius, color=-1, tag=0):
    if radius < 1:
        return
    _color_tag(eve, color, tag)
    eve.Begin(eve.POINTS)
    eve.PointSize(radius * 2)
    eve.Vertex2f(x, y)


def evehp_widget_box(eve: BrtEve, x, y, w, h, 
    rounded=2, color=-1, tag=0, text=None, font=30, otp=0, tcolor=None):
    if w < 1 or h < 1:
        return
    _color_tag(eve, color, tag)
    eve.Begin(eve.RECTS)
    eve.LineWidth(rounded)
    eve.Vertex2f(x + rounded / 2, y + rounded / 2)
    eve.Vertex2f(x + w - rounded / 2, y + h - rounded / 2)

    if text != None:
        tx = x + w/2
        ty = y + h/2
        totp = eve.OPT_CENTER
        if tcolor != None:
            eve.ColorRGB(tcolor[0], tcolor[1], tcolor[2])

        eve.cmd_text(tx, ty, font, otp | totp, text)


def evehp_widget_meter(eve: BrtEve, x, y, radius, color=-1, tag=0):
    color_yellow = [0xF5, 0x90, 0x1F]
    color_white = [255, 255, 255]

    c1 = color_white
    c2 = color_yellow
    degree = 1
    _color_tag(eve, color, tag)

    eve.StencilFunc(eve.NEVER, 0x00, 0x00)
    eve.StencilOp(eve.INCR, eve.INCR)
    eve.Begin(eve.POINTS)
    eve.PointSize(80 * 2)
    eve.Vertex2f(x, y)

    eve.StencilFunc(eve.NOTEQUAL, 0x01, 0x01)
    eve.PointSize(90 * 2)
    eve.Vertex2f(x, y)

    eve.StencilFunc(eve.EQUAL, 0x01, 0x01)
    eve.StencilOp(eve.KEEP, eve.KEEP)

    eve.ColorRGB(c2[0], c2[1], c2[2])
    eve.PointSize(10 * 2)

    r = 85
    for num in range(0, degree, 2):
        num = math.radians(num)
        posX = x + (r) * math.cos(num)
        posY = y + (r) * math.sin(num)
        eve.Vertex2f(posX, posY)
    degree += 2
    if degree > 360:
        degree = 1
        if c1 == color_white:
            c1 = color_yellow
            c2 = color_white
        else:
            c1 = color_white
            c2 = color_yellow


def evehp_widget_dialog_yes_no(eve: BrtEve, title="Question", question="Yes/No?", org=None, program=None,
                          timeout=120, default_tag=1, x=0, y=0, w=0, h=0, lcd_w=0, lcd_h=0, tag_yes=1, tag_no=2):
    # lcd_w and lcd_h may different with eve.lcd_w and eve.lcd_h, when cmd_setrotate is used
    if lcd_w == 0 or lcd_h == 0:  # w and h are not set
        lcd_w = eve.lcd_width
        lcd_h = eve.lcd_height

    # calculate dialog size and position
    border = 2  # pixel
    base_lcd_w = 800
    base_lcd_h = 480
    dialog_w = 400
    dialog_h = 200
    dialog_x = (base_lcd_w - dialog_w) / 2
    dialog_y = (base_lcd_h - dialog_h) / 2
    htop = 50  # widget header height
    hbottom = 50  # widget footer height
    font = 20

    btn_h = 30
    btn_w = 80

    ratio = base_lcd_w * 100 / lcd_w

    if (ratio != 100):
        dialog_w = dialog_w * 100 / ratio
        dialog_h = dialog_h * 100 / ratio
        btn_w = btn_w * 100 / ratio
        btn_h = btn_h * 100 / ratio
        dialog_x = (lcd_w - dialog_w) / 2
        dialog_y = (lcd_h - dialog_h) / 2
        htop = htop * 100 / ratio
        hbottom = hbottom * 100 / ratio

        ratio += (100 - ratio) / 2
        font = round(font * 100 / ratio)
        if font > 31:
            font = 31
        if font < 16:
            font = 16
        if font == 17 or font == 19:  # 17 and 19 are special unicode font
            font = 18

    if x != 0 or y != 0:  # x and y are set
        dialog_x = x
        dialog_y = y
    if w != 0 or h != 0:  # w and h are set
        dialog_w = w
        dialog_h = h

    time_start = time.monotonic_ns() / 1000_000_000
    while 1:
        time_end = time.monotonic_ns() / 1000_000_000
        duration = time_end - time_start
        if duration > timeout:
            return default_tag == tag_yes

        eve.cmd_dlstart()
        # eve.ClearColorRGB(255, 255, 255)
        eve.ClearColorRGB(51, 153, 255)
        eve.Clear(1, 1, 1)
        eve.VertexFormat(3)
        eve.ColorRGB(255, 255, 255)

        # / fade the whole LCD
        # eve.ColorRGB(0x3F, 0x3F, 0x3F)
        # eve.ColorRGB(51, 153, 255)
        # eve.Begin(eve.RECTS)
        # eve.Vertex2f(0, 0)
        # eve.Vertex2f(lcd_w, lcd_h)

        if org: eve.cmd_text(eve.lcd_width / 2, 10, 31, eve.OPT_CENTERX, org)
        if program: eve.cmd_text(eve.lcd_width / 2, 70, 30, eve.OPT_CENTERX, program)

        # / diag border
        eve.ColorRGB(0xE1, 0xE1, 0xE1)
        eve.Begin(eve.RECTS)
        eve.Vertex2f(dialog_x, dialog_y)
        eve.Vertex2f(dialog_x + dialog_w, dialog_y + dialog_h)

        # / center part
        eve.ColorRGB(0xFF, 0xFF, 0xFF)
        eve.Begin(eve.RECTS)
        eve.Vertex2f(dialog_x + border, dialog_y + border)
        eve.Vertex2f(dialog_x + dialog_w - border,
                     dialog_y + dialog_h - border)

        # / top part
        eve.ColorRGB(0x46, 0x50, 0x52)
        eve.Begin(eve.RECTS)
        eve.Vertex2f(dialog_x + border, dialog_y + border)
        eve.Vertex2f(dialog_x + dialog_w - border, dialog_y + htop)

        # / bottom part
        eve.ColorRGB(0xF3, 0xF3, 0xF3)
        eve.Begin(eve.RECTS)
        eve.Vertex2f(dialog_x + border, dialog_y + dialog_h - hbottom + border)
        eve.Vertex2f(dialog_x + dialog_w - border,
                     dialog_y + dialog_h - border)

        # / diag message
        eve.ColorRGB(0xFF, 0xFF, 0xFF)
        eve.cmd_text(dialog_x + border + 10, dialog_y + border + 10,
                     font, 0, title + ' (' + str(timeout - round(duration)) + 's)')

        eve.ColorRGB(0x78, 0x78, 0x78)
        eve.cmd_fillwidth(dialog_w - border - 30)
        eve.cmd_text(dialog_x + border + 30, dialog_y +
                     dialog_h / 2 - 20, font, eve.OPT_FILL, question)

        # / diag button yes/no
        if default_tag == True:
            eve.ColorRGB(0xFF, 0x00, 0x00)
        else:
            eve.ColorRGB(0xFF, 0xFF, 0xFF)
        btn_margin = 25
        eve.Tag(tag_yes)
        eve.cmd_button(dialog_x + dialog_w / 2 - btn_w - btn_margin,
                       dialog_y + dialog_h - hbottom + border +
                       (hbottom - btn_h) / 2, btn_w, btn_h,
                       font, 0, "Yes")

        if default_tag == False:
            eve.ColorRGB(0xFF, 0x00, 0x00)
        else:
            eve.ColorRGB(0xFF, 0xFF, 0xFF)
        eve.Tag(tag_no)
        eve.cmd_button(dialog_x + dialog_w / 2 + btn_margin,
                       dialog_y + dialog_h - hbottom + border +
                       (hbottom - btn_h) / 2, btn_w, btn_h,
                       font, 0, "No")

        eve.Display()
        eve.cmd_swap()
        eve.finish()

        tag_released = evehp_gesture_renew(eve).tagReleased

        if tag_released == tag_yes:
            return True
        if tag_released == tag_no:
            return False

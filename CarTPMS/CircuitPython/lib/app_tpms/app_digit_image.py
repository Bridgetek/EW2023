import time

from .helper_gesture import *
from .helper_datetime import *
from .helper_responsive import *
from .app_tags import *
from .helper_datetime import *
from .helper_widgets import *
from .helper_image import *

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve

def draw_digits(eve, x, y, image:evehp_image, val = None, text = '', tag=0, draw_from_right = 0):
    """image = list of evehp_image of char: 0123456789., return last position drawn"""
    valid = "0123456789."
    if val:
        text = str(val)
    elif text == '':
        return
    numdot = ord(".") - ord("0")
    
    if draw_from_right:
        # text[::-1] -> NotImplementedError: only slices with step=1 (aka None) are supported
        for i in range(len(text)):
            char = text[len(text) - i -1]
            if char not in valid:
                continue
            num = ord(char) - ord("0")
            img_id = num
            y_dot = 0
            if num == numdot:
                img_id = len(valid) - 1
                y_dot = image[0].h - image[img_id].h
                
            x -= image[img_id].w
            evehp_image_draw(eve, image[img_id], x, y + y_dot, tag)
        
        return x
    
    for char in text:
        if char not in valid:
            continue
        num = ord(char) - ord("0")
        img_id = num
        y_dot = 0
        if num == numdot:
            img_id = len(valid) - 1
            y_dot = image[0].h - image[img_id].h
        evehp_image_draw(eve, image[img_id], x, y + y_dot, tag)
        x += image[img_id].w
    return x
            

    

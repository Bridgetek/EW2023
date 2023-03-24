"""Helper functions for the BridgeTek EVE's application
   Version: 1.0
   Last modified: 9 Jan 2023 - Tuan.Nguyen@Brtchip.com
"""

import time
import sys
import random

if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_storage.brt_eve_storage import align_to
else:
    from ...lib.brteve.brt_eve_bt817_8 import BrtEve
    from ...lib.brteve.brt_eve_storage.brt_eve_storage import align_to


class _evehp_gesture_glob:
    '''internal data'''
    _last_random = 0


def evehp_math_random(minv, maxv):
    num = minv - 1
    while num < minv or num == _evehp_gesture_glob._last_random:
        num = time.monotonic_ns()
        nums = str(num)
        num3 = nums[len(nums) - 3:]

        for i in num3:
            num += (int)(i)

        num = num % maxv

    _evehp_gesture_glob._last_random = num
    return num


def evehp_math_zfill(s, num):
    s = str(s)
    strlen = len(s)
    for i in range(num - strlen):
        s = '0' + s
    return s


def evehp_math_flash_address_roundup(address):
    address = int(address)
    address = evehp_math_roundup_to_nearest(address, 4)
    address = align_to(address, 256)
    return address

def evehp_math_flash_address_convert(x: int):
    FLASH_ADDRESS = (0x800000)
    return int(FLASH_ADDRESS | int(x / 32))


def evehp_math_roundup_to_nearest(n, m):
    return (n + m - 1) // m * m

def evehp_math_c2f(celsius):
    fahrenheit=(celsius * 1.8)+32
    return fahrenheit
    
def evehp_math_f2c(fahrenheit):
    celsius = (fahrenheit - 32)/1.8
    return celsius

def evehp_math_textw(font, txt):
    class font_w_table:
        def __init__(self, font, l, s) -> None:
            self.font = font
            self.size_long = l
            self.size_short = s

    _FONTW ={
        16: font_w_table(16, 7, 3),
        17: font_w_table(17, 7, 3),
        18: font_w_table(18, 8, 3),
        19: font_w_table(19, 8, 3),
        20: font_w_table(20, 5, 1),
        21: font_w_table(21, 7, 1),
        22: font_w_table(22, 8, 1),
        23: font_w_table(23, 9, 2),
        24: font_w_table(24, 11, 2),
        25: font_w_table(25, 15, 3),
        26: font_w_table(26, 6, 2),
        27: font_w_table(27, 7, 2),
        28: font_w_table(28, 9, 2),
        29: font_w_table(29, 11, 3),
        30: font_w_table(30, 14, 3),
        31: font_w_table(31, 19, 4),
    }

    short_char="1ilj;:',.`- 1I;:',.`- "
    list_short_char = {c : txt.count(c) for c in short_char}

    txt_len = len(txt)
    return txt_len * _FONTW[font].size_long

    num_short_char = sum(list_short_char.values())
    num_long_char = (txt_len - num_short_char)

    char_w_long =  num_long_char * _FONTW[font].size_long
    char_w_short = num_short_char * _FONTW[font].size_short

    return char_w_long + char_w_short

def rand_arround(rand_id=None, minv=0, maxv=100, middle=50, min_step=0, max_step=1, refresh=0):
    maxv=maxv
    minv=minv
    min_step=min_step
    max_step=max_step
    
    class rand_arround_dat():
        def __init__(self, minv=0, maxv=100, middle=None, min_step=0, max_step=1) -> None:
            if middle: self.val = middle
            else: self.val = (maxv - minv) / 2
            
            self.last_val=0
            self.maxv=maxv
            self.minv=minv
            self.min_step=min_step
            self.max_step=max_step
            
            self.count_up = 0        
            self.count_down = 0        
            self.count_total = 0        
            self.ratio = 30
    
    if rand_id == None or refresh:
        rand_id = rand_arround_dat(minv, maxv, middle, min_step, max_step)
    
    r=random.randint(0, 100)
    
    is_up = 1 if r > 50 else -1
    
    if rand_id.count_total > 0:
        if rand_id.count_up *100 / rand_id.count_total > rand_id.ratio:
            is_up = -1 
        if rand_id.count_down *100 / rand_id.count_total > rand_id.ratio:
            is_up = 1
    
    range = rand_id.max_step - rand_id.min_step
    step = 1.0 * is_up * r * range / 100
    rand_id.val += step
    rand_id.val = max(min(rand_id.val, rand_id.maxv), rand_id.minv)
    
    return rand_id
    
        
    
        
    
    
    
    
    
    
    
    
    
    
    
    
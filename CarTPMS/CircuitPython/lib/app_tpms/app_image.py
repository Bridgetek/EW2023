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
from .app_tags import *

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_rp2040 import BrtEveRP2040
else:
    from ..lib.brteve.brt_eve_bt817_8 import BrtEve
    from ..lib.brteve.brt_eve_rp2040 import BrtEveRP2040

imdir="app/images/4444/"

eveim_0                =evehp_image(0, 0, 9    ,14   , "0_9x14_ARGB4.raw"                      , 4096   ,  256   , layout=BrtEve.ARGB4)
eveim_0big             =evehp_image(0, 0, 12   ,19   , "0big_12x19_ARGB4.raw"                  , 4352   ,  512   , layout=BrtEve.ARGB4)
eveim_1                =evehp_image(0, 0, 6    ,13   , "1_6x13_ARGB4.raw"                      , 4864   ,  192   , layout=BrtEve.ARGB4)
eveim_1big             =evehp_image(0, 0, 9    ,18   , "1big_9x18_ARGB4.raw"                   , 5056   ,  384   , layout=BrtEve.ARGB4)
eveim_2                =evehp_image(0, 0, 10   ,13   , "2_10x13_ARGB4.raw"                     , 5440   ,  320   , layout=BrtEve.ARGB4)
eveim_2big             =evehp_image(0, 0, 13   ,18   , "2big_13x18_ARGB4.raw"                  , 5760   ,  512   , layout=BrtEve.ARGB4)
eveim_3                =evehp_image(0, 0, 9    ,14   , "3_9x14_ARGB4.raw"                      , 6272   ,  256   , layout=BrtEve.ARGB4)
eveim_3big             =evehp_image(0, 0, 13   ,19   , "3big_13x19_ARGB4.raw"                  , 6528   ,  512   , layout=BrtEve.ARGB4)
eveim_4                =evehp_image(0, 0, 10   ,13   , "4_10x13_ARGB4.raw"                     , 7040   ,  320   , layout=BrtEve.ARGB4)
eveim_4big             =evehp_image(0, 0, 14   ,18   , "4big_14x18_ARGB4.raw"                  , 7360   ,  512   , layout=BrtEve.ARGB4)
eveim_5                =evehp_image(0, 0, 9    ,14   , "5_9x14_ARGB4.raw"                      , 7872   ,  256   , layout=BrtEve.ARGB4)
eveim_5big             =evehp_image(0, 0, 12   ,19   , "5big_12x19_ARGB4.raw"                  , 8128   ,  512   , layout=BrtEve.ARGB4)
eveim_6                =evehp_image(0, 0, 9    ,14   , "6_9x14_ARGB4.raw"                      , 8640   ,  256   , layout=BrtEve.ARGB4)
eveim_6big             =evehp_image(0, 0, 12   ,19   , "6big_12x19_ARGB4.raw"                  , 8896   ,  512   , layout=BrtEve.ARGB4)
eveim_7                =evehp_image(0, 0, 10   ,13   , "7_10x13_ARGB4.raw"                     , 9408   ,  320   , layout=BrtEve.ARGB4)
eveim_7big             =evehp_image(0, 0, 13   ,18   , "7big_13x18_ARGB4.raw"                  , 9728   ,  512   , layout=BrtEve.ARGB4)
eveim_8                =evehp_image(0, 0, 10   ,14   , "8_10x14_ARGB4.raw"                     , 10240  ,  320   , layout=BrtEve.ARGB4)
eveim_8big             =evehp_image(0, 0, 12   ,19   , "8big_12x19_ARGB4.raw"                  , 10560  ,  512   , layout=BrtEve.ARGB4)
eveim_9                =evehp_image(0, 0, 9    ,14   , "9_9x14_ARGB4.raw"                      , 11072  ,  256   , layout=BrtEve.ARGB4)
eveim_9big             =evehp_image(0, 0, 12   ,19   , "9big_12x19_ARGB4.raw"                  , 11328  ,  512   , layout=BrtEve.ARGB4)
eveim_arrowl           =evehp_image(0, 0, 12   ,17   , "arrowl_12x17_ARGB4.raw"                , 11840  ,  448   , layout=BrtEve.ARGB4)
eveim_arrowr           =evehp_image(0, 0, 13   ,17   , "arrowr_13x17_ARGB4.raw"                , 12288  ,  448   , layout=BrtEve.ARGB4)
eveim_Bar              =evehp_image(0, 0, 36   ,19   , "Bar_36x19_ARGB4.raw"                   , 12736  ,  1408  , layout=BrtEve.ARGB4)
eveim_Bar_menu         =evehp_image(0, 0, 20   ,11   , "Bar_menu_20x11_ARGB4.raw"              , 14144  ,  448   , layout=BrtEve.ARGB4)
eveim_C                =evehp_image(0, 0, 13   ,12   , "C_13x12_ARGB4.raw"                     , 14592  ,  320   , layout=BrtEve.ARGB4)
eveim_C1               =evehp_image(0, 0, 17   ,14   , "C1_17x14_ARGB4.raw"                    , 14912  ,  512   , layout=BrtEve.ARGB4)
eveim_Depth__inch_     =evehp_image(0, 0, 35   ,31   , "Depth (inch)_35x31_ARGB4.raw"          , 15424  ,  2176  , layout=BrtEve.ARGB4)
eveim_Depth__mm_       =evehp_image(0, 0, 35   ,31   , "Depth (mm)_35x31_ARGB4.raw"            , 17600  ,  2176  , layout=BrtEve.ARGB4)
eveim_dot2             =evehp_image(0, 0, 3    ,3    , "dot2_3x3_ARGB4.raw"                    , 19776  ,  64    , layout=BrtEve.ARGB4)
eveim_dotbig           =evehp_image(0, 0, 5    ,5    , "dotbig_5x5_ARGB4.raw"                  , 19840  ,  64    , layout=BrtEve.ARGB4)
eveim_F                =evehp_image(0, 0, 13   ,11   , "F_13x11_ARGB4.raw"                     , 19904  ,  320   , layout=BrtEve.ARGB4)
eveim_F1               =evehp_image(0, 0, 16   ,13   , "F1_16x13_ARGB4.raw"                    , 20224  ,  448   , layout=BrtEve.ARGB4)
eveim_inch             =evehp_image(0, 0, 44   ,19   , "inch_44x19_ARGB4.raw"                  , 20672  ,  1728  , layout=BrtEve.ARGB4)
eveim_inch_menu        =evehp_image(0, 0, 24   ,12   , "inch_menu_24x12_ARGB4.raw"             , 22400  ,  576   , layout=BrtEve.ARGB4)
eveim_indicator        =evehp_image(0, 0, 2    ,40   , "indicator_2x40_ARGB4.raw"              , 22976  ,  192   , layout=BrtEve.ARGB4)
eveim_kPa              =evehp_image(0, 0, 39   ,20   , "kPa_39x20_ARGB4.raw"                   , 23168  ,  1600  , layout=BrtEve.ARGB4)
eveim_kPa_menu         =evehp_image(0, 0, 22   ,12   , "kPa_menu_22x12_ARGB4.raw"              , 24768  ,  576   , layout=BrtEve.ARGB4)
eveim_mm               =evehp_image(0, 0, 40   ,13   , "mm_40x13_ARGB4.raw"                    , 25344  ,  1088  , layout=BrtEve.ARGB4)
eveim_mm_menu          =evehp_image(0, 0, 24   ,8    , "mm_menu_24x8_ARGB4.raw"                , 26432  ,  384   , layout=BrtEve.ARGB4)
eveim_Off              =evehp_image(0, 0, 20   ,12   , "Off_20x12_ARGB4.raw"                   , 26816  ,  512   , layout=BrtEve.ARGB4)
eveim_Pressure__BAR_   =evehp_image(0, 0, 54   ,30   , "Pressure (BAR)_54x30_ARGB4.raw"        , 27328  ,  3264  , layout=BrtEve.ARGB4)
eveim_Pressure__KPA_   =evehp_image(0, 0, 54   ,30   , "Pressure (KPA)_54x30_ARGB4.raw"        , 30592  ,  3264  , layout=BrtEve.ARGB4)
eveim_Pressure__PSI_   =evehp_image(0, 0, 54   ,30   , "Pressure (PSI)_54x30_ARGB4.raw"        , 33856  ,  3264  , layout=BrtEve.ARGB4)
eveim_PSI              =evehp_image(0, 0, 35   ,19   , "PSI_35x19_ARGB4.raw"                   , 37120  ,  1344  , layout=BrtEve.ARGB4)
eveim_PSI_menu         =evehp_image(0, 0, 19   ,12   , "PSI_menu_19x12_ARGB4.raw"              , 38464  ,  512   , layout=BrtEve.ARGB4)
eveim_Temperature__C_  =evehp_image(0, 0, 78   ,30   , "Temperature (C)_78x30_ARGB4.raw"       , 38976  ,  4736  , layout=BrtEve.ARGB4)
eveim_Temperature__F_  =evehp_image(0, 0, 78   ,30   , "Temperature (F)_78x30_ARGB4.raw"       , 43712  ,  4736  , layout=BrtEve.ARGB4)
eveim_Temperature__Off_=evehp_image(0, 0, 78   ,30   , "Temperature (Off)_78x30_ARGB4.raw"     , 48448  ,  4736  , layout=BrtEve.ARGB4)
eveim_Tyre_Pressure    =evehp_image(0, 0, 58   ,30   , "Tyre Pressure_58x30_ARGB4.raw"         , 53184  ,  3520  , layout=BrtEve.ARGB4)
eveim_Tyre_Pressure1   =evehp_image(0, 0, 170  ,26   , "Tyre Pressure1_170x26_ARGB4.raw"       , 56704  ,  8896  , layout=BrtEve.ARGB4)
eveim_Tyre_Tread_Depth =evehp_image(0, 0, 70   ,34   , "Tyre Tread Depth_70x34_ARGB4.raw"      , 65600  ,  4800  , layout=BrtEve.ARGB4)
eveim_Tyre_Tread_Depth1=evehp_image(0, 0, 209  ,27   , "Tyre Tread Depth1_209x27_ARGB4.raw"    , 70400  , 11328  , layout=BrtEve.ARGB4)
eveim_zpressure_bar    =evehp_image(0, 0, 15   ,42   , "zpressure_bar_15x42_ARGB4.raw"         , 81728  , 1280   , layout=BrtEve.ARGB4)
eveim_zpressure_bar_p4 =evehp_image(0, 0, 15   ,42   , "zpressure_bar_15x42_ARGB4.raw"         , 83008  , 640+40 , layout=BrtEve.PALETTED4444, paletted=evehpi_pallete(83648, 83008, 64, 640 ))
eveim_bg1              =evehp_image(0, 0, 862  ,480  , "zzbg_862x480_RGB565.raw"               , 83712  , 827520 , layout=BrtEve.RGB565)
eveim_bg2              =evehp_image(0, 0, 800  ,480  , "zzbg_test_800x480_RGB565.raw"          , 911232 , 768000 , layout=BrtEve.RGB565)
eveim_bg = eveim_bg1

eveims = [
    eveim_Off                   ,
    eveim_Temperature__Off_     ,
    eveim_0                     ,
    eveim_0big                  ,
    eveim_1                     ,
    eveim_1big                  ,
    eveim_2                     ,
    eveim_2big                  ,
    eveim_3                     ,
    eveim_3big                  ,
    eveim_4                     ,
    eveim_4big                  ,
    eveim_5                     ,
    eveim_5big                  ,
    eveim_6                     ,
    eveim_6big                  ,
    eveim_7                     ,
    eveim_7big                  ,
    eveim_8                     ,
    eveim_8big                  ,
    eveim_9                     ,
    eveim_9big                  ,
    eveim_arrowl                ,
    eveim_arrowr                ,
    eveim_Bar                   ,
    eveim_Bar_menu              ,
    eveim_C                     ,
    eveim_C1                    ,
    eveim_Depth__inch_          ,
    eveim_Depth__mm_            ,
    eveim_dot2                  ,
    eveim_dotbig                ,
    eveim_F                     ,
    eveim_F1                    ,
    eveim_inch                  ,
    eveim_inch_menu             ,
    eveim_indicator             ,
    eveim_kPa                   ,
    eveim_kPa_menu              ,
    eveim_mm                    ,
    eveim_mm_menu               ,
    eveim_Pressure__BAR_        ,
    eveim_Pressure__KPA_        ,
    eveim_Pressure__PSI_        ,
    eveim_PSI                   ,
    eveim_PSI_menu              ,
    eveim_Temperature__C_       ,
    eveim_Temperature__F_       ,
    eveim_Tyre_Pressure         ,
    eveim_Tyre_Pressure1        ,
    eveim_Tyre_Tread_Depth      ,
    eveim_Tyre_Tread_Depth1     ,
    eveim_zpressure_bar         ,
    eveim_zpressure_bar_p4      ,
    eveim_bg                    ,
]

eveim_digits_big = [
    eveim_0big,
    eveim_1big,
    eveim_2big,
    eveim_3big,
    eveim_4big,
    eveim_5big,
    eveim_6big,
    eveim_7big,
    eveim_8big,
    eveim_9big,
    eveim_dotbig,
]

eveim_digits_small = [
    eveim_0,
    eveim_1,
    eveim_2,
    eveim_3,
    eveim_4,
    eveim_5,
    eveim_6,
    eveim_7,
    eveim_8,
    eveim_9,
    eveim_dot2,
]

#copy as much as possible image into ramg 
def image_on_flash_init(eve):
    evehp_image_copy_array_to_flash(eve, eveims)

def image_on_ramg_init(eve):
    evehp_image_copy_array_to_ramg(eve, eveims, msg_on=False)

def image_handler_init(eve):
    return
    handler = 0
    for im in eveims:
        im.handler_set = handler
        evehp_image_draw(eve, im, quiet=1)
        
        handler+=1
        if handler > 14:
            im.handler_set = 0
            return
        im.handler_draw = handler
            
def image_on_ramg_from_eve_flash(eve):
    eve.storage.flash_state(eve.FLASH_STATUS_FULL)
    
    # palette lut must in ramg
    ramg = 0
    for im in eveims:
        if im.paletted and (ramg + im.paletted.size_pallete) < eve.RAM_G_SIZE:
            eve.cmd_flashread(ramg, im.paletted.addr_pallete,im.paletted.size_pallete)
            print("image paletted lut copied into ramg: ", im.filepath, "at ", ramg)
            
            im.paletted.ramg_addr_pallete = ramg
            ramg+=im.paletted.size_pallete

        eve.finish()
    
    # palette index can stored in ramg or flash
    for im in eveims:
        if im.paletted and (ramg + im.paletted.size_index) < eve.RAM_G_SIZE:
            eve.cmd_flashread(ramg, im.paletted.addr_index,im.paletted.size_index)
            print("image paletted index copied into ramg: ", im.filepath, "at ", ramg)
            im.paletted.ramg_addr_index = ramg
            im.exist_on_ramg = True
            ramg+=im.paletted.size_index
        elif (ramg + im.size) < eve.RAM_G_SIZE:
            eve.cmd_flashread(ramg, im.addr_flash, im.size)
            print("image copied into ramg: ", im.filepath, "at ", ramg)
            
            im.addr_ramg = ramg
            im.exist_on_ramg = True
            ramg+=im.size
        else:
            return
        eve.finish()
        

def flashing(eve, file):
    question = "Write image from sdcard to EVE's connected flash?"
    yes = evehp_widget_dialog_yes_no(eve, title="Preparing flash", question=question, timeout=120, default_tag=False) == True
    if yes == True:
        eve.storage.write_flash_with_progressbar(file, 0)

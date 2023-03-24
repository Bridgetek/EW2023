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

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_rp2040 import BrtEveRP2040
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve
    from lib.brteve.brt_eve_rp2040 import BrtEveRP2040
    

eveim_0                        =evehp_image(64, 129    , 26   , 45   , "0_26x45_PALETTED4444"                        , paletted=evehpi_pallete(5312    , 4096    , 64      , 1216    ), layout = BrtEve.PALETTED4444)
eveim_1                        =evehp_image(64, 129    , 14   , 43   , "1_14x43_PALETTED4444"                        , paletted=evehpi_pallete(6016    , 5376    , 64      , 640     ), layout = BrtEve.PALETTED4444)
eveim_2                        =evehp_image(64, 129    , 28   , 44   , "2_28x44_PALETTED4444"                        , paletted=evehpi_pallete(7360    , 6080    , 64      , 1280    ), layout = BrtEve.PALETTED4444)
eveim_3                        =evehp_image(64, 129    , 28   , 45   , "3_28x45_PALETTED4444"                        , paletted=evehpi_pallete(8704    , 7424    , 64      , 1280    ), layout = BrtEve.PALETTED4444)
eveim_4                        =evehp_image(64, 129    , 31   , 43   , "4_31x43_PALETTED4444"                        , paletted=evehpi_pallete(10112   , 8768    , 64      , 1344    ), layout = BrtEve.PALETTED4444)
eveim_5                        =evehp_image(64, 129    , 26   , 44   , "5_26x44_PALETTED4444"                        , paletted=evehpi_pallete(11328   , 10176   , 64      , 1152    ), layout = BrtEve.PALETTED4444)
eveim_6                        =evehp_image(64, 129    , 27   , 45   , "6_27x45_PALETTED4444"                        , paletted=evehpi_pallete(12608   , 11392   , 64      , 1216    ), layout = BrtEve.PALETTED4444)
eveim_7                        =evehp_image(64, 129    , 29   , 43   , "7_29x43_PALETTED4444"                        , paletted=evehpi_pallete(13952   , 12672   , 64      , 1280    ), layout = BrtEve.PALETTED4444)
eveim_8                        =evehp_image(64, 129    , 28   , 45   , "8_28x45_PALETTED4444"                        , paletted=evehpi_pallete(15296   , 14016   , 64      , 1280    ), layout = BrtEve.PALETTED4444)
eveim_9                        =evehp_image(64, 129    , 27   , 45   , "9_27x45_PALETTED4444"                        , paletted=evehpi_pallete(16576   , 15360   , 64      , 1216    ), layout = BrtEve.PALETTED4444)
eveim_ac                       =evehp_image(327, 352   , 72   , 73   , "ac_72x73_PALETTED4444"                       , paletted=evehpi_pallete(21952   , 16640   , 512     , 5312    ), layout = BrtEve.PALETTED4444)
eveim_ac_off                   =evehp_image(327, 352   , 72   , 73   , "ac_off_72x73_PALETTED4444"                   , paletted=evehpi_pallete(27776   , 22464   , 512     , 5312    ), layout = BrtEve.PALETTED4444)
eveim_air                      =evehp_image(404, 351   , 72   , 73   , "air_72x73_PALETTED4444"                      , paletted=evehpi_pallete(33600   , 28288   , 512     , 5312    ), layout = BrtEve.PALETTED4444)
eveim_air_l_1                  =evehp_image(228, 185   , 45   , 45   , "air_l_1_45x45_PALETTED4444"                  , paletted=evehpi_pallete(36160   , 34112   , 512     , 2048    ), layout = BrtEve.PALETTED4444)
eveim_air_l_2                  =evehp_image(178, 260   , 46   , 46   , "air_l_2_46x46_PALETTED4444"                  , paletted=evehpi_pallete(38848   , 36672   , 448     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_air_l_3                  =evehp_image( 96, 294   , 45   , 45   , "air_l_3_45x45_PALETTED4444"                  , paletted=evehpi_pallete(41344   , 39296   , 512     , 2048    ), layout = BrtEve.PALETTED4444)
eveim_air_l                    =evehp_image( 97, 185   , 177  , 153  , "air_l_177x153_PALETTED4444"                  , paletted=evehpi_pallete(68992   , 41856   , 320     , 27136   ), layout = BrtEve.PALETTED4444)
eveim_air_off                  =evehp_image(404, 351   , 72   , 73   , "air_off_72x73_PALETTED4444"                  , paletted=evehpi_pallete(74624   , 69312   , 512     , 5312    ), layout = BrtEve.PALETTED4444)
eveim_air_r_1                  =evehp_image(533, 185   , 46   , 46   , "air_r_1_46x46_PALETTED4444"                  , paletted=evehpi_pallete(77312   , 75136   , 448     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_air_r_2                  =evehp_image(575, 267   , 46   , 46   , "air_r_2_46x46_PALETTED4444"                  , paletted=evehpi_pallete(79936   , 77760   , 512     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_air_r_3                  =evehp_image(654 + 5, 291   , 46   , 46   , "air_r_3_46x46_PALETTED4444"                  , paletted=evehpi_pallete(82624   , 80448   , 448     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_air_r                    =evehp_image(519, 145   , 195  , 232  , "air_r_195x232_PALETTED4444"                  , paletted=evehpi_pallete(128320  , 83072   , 320     , 45248   ), layout = BrtEve.PALETTED4444)
eveim_AL1_off                  =evehp_image(228, 185   , 45   , 45   , "AL1_off_45x45_PALETTED4444"                  , paletted=evehpi_pallete(130688  , 128640  , 384     , 2048    ), layout = BrtEve.PALETTED4444)
eveim_AL2_off                  =evehp_image(178, 260   , 46   , 46   , "AL2_off_46x46_PALETTED4444"                  , paletted=evehpi_pallete(133248  , 131072  , 384     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_AL3_off                  =evehp_image( 96, 294   , 45   , 45   , "AL3_off_45x45_PALETTED4444"                  , paletted=evehpi_pallete(135680  , 133632  , 384     , 2048    ), layout = BrtEve.PALETTED4444)
eveim_AR1_disable              =evehp_image(533, 185   , 46   , 46   , "AR1_disable_46x46_PALETTED4444"              , paletted=evehpi_pallete(138240  , 136064  , 384     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_AR1_off                  =evehp_image(533, 185   , 46   , 46   , "AR1_off_46x46_PALETTED4444"                  , paletted=evehpi_pallete(140800  , 138624  , 384     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_AR2_disable              =evehp_image(575, 267   , 46   , 47   , "AR2_disable_46x47_PALETTED4444"              , paletted=evehpi_pallete(143360  , 141184  , 384     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_AR2_off                  =evehp_image(575, 267   , 46   , 47   , "AR2_off_46x47_PALETTED4444"                  , paletted=evehpi_pallete(145920  , 143744  , 448     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_AR3_disable              =evehp_image(654 + 5, 291   , 47   , 46   , "AR3_disable_47x46_PALETTED4444"              , paletted=evehpi_pallete(148544  , 146368  , 384     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_AR3_off                  =evehp_image(654 + 5, 291   , 47   , 46   , "AR3_off_47x46_PALETTED4444"                  , paletted=evehpi_pallete(151104  , 148928  , 384     , 2176    ), layout = BrtEve.PALETTED4444)
eveim_auto                     =evehp_image(364, 175   , 72   , 73   , "auto_72x73_PALETTED4444"                     , paletted=evehpi_pallete(156800  , 151488  , 512     , 5312    ), layout = BrtEve.PALETTED4444)
eveim_auto_off                 =evehp_image(364, 175   , 72   , 73   , "auto_off_72x73_PALETTED4444"                 , paletted=evehpi_pallete(162624  , 157312  , 512     , 5312    ), layout = BrtEve.PALETTED4444)
eveim_C                        =evehp_image(129, 133   , 13   , 12   , "C_13x12_PALETTED4444"                        , paletted=evehpi_pallete(163328  , 163136  , 64      , 192     ), layout = BrtEve.PALETTED4444)
eveim_celsius                  =evehp_image(2  , 408   , 78   , 40   , "celsius_78x40_PALETTED4444"                  , paletted=evehpi_pallete(166528  , 163392  , 448     , 3136    ), layout = BrtEve.PALETTED4444)
eveim_F                        =evehp_image(129, 133   , 13   , 11   , "F_13x11_PALETTED4444"                        , paletted=evehpi_pallete(167168  , 166976  , 64      , 192     ), layout = BrtEve.PALETTED4444)
eveim_fahrenheit               =evehp_image( 82, 408   , 78   , 40   , "fahrenheit_78x40_PALETTED4444"               , paletted=evehpi_pallete(170368  , 167232  , 384     , 3136    ), layout = BrtEve.PALETTED4444)
eveim_fan_higher               =evehp_image( 19,  75   , 38   , 55   , "fan-higher_38x55_PALETTED4444"               , paletted=evehpi_pallete(172864  , 170752  , 192     , 2112    ), layout = BrtEve.PALETTED4444)
eveim_fan_lower                =evehp_image( 19,  75   , 38   , 55   , "fan-lower_38x55_PALETTED4444"                , paletted=evehpi_pallete(175168  , 173056  , 192     , 2112    ), layout = BrtEve.PALETTED4444)
eveim_front                    =evehp_image(327, 273   , 72   , 73   , "front_72x73_PALETTED4444"                    , paletted=evehpi_pallete(180672  , 175360  , 512     , 5312    ), layout = BrtEve.PALETTED4444)
eveim_front_off                =evehp_image(327, 273   , 72   , 73   , "front_off_72x73_PALETTED4444"                , paletted=evehpi_pallete(186496  , 181184  , 512     , 5312    ), layout = BrtEve.PALETTED4444)
eveim_onoff                    =evehp_image(369,  26   , 62   , 62   , "onoff_62x62_PALETTED4444"                    , paletted=evehpi_pallete(190912  , 187008  , 512     , 3904    ), layout = BrtEve.PALETTED4444)
eveim_onoff_off                =evehp_image(369,  26   , 62   , 62   , "onoff_off_62x62_PALETTED4444"                , paletted=evehpi_pallete(195328  , 191424  , 512     , 3904    ), layout = BrtEve.PALETTED4444)
eveim_rear                     =evehp_image(403, 273  , 72   , 73   , "rear_72x73_PALETTED4444"                     , paletted=evehpi_pallete(201152  , 195840  , 512     , 5312    ), layout = BrtEve.PALETTED4444)
eveim_rear_off                 =evehp_image(403, 273  , 73   , 73   , "rear_off_73x73_PALETTED4444"                 , paletted=evehpi_pallete(207040  , 201664  , 512     , 5376    ), layout = BrtEve.PALETTED4444)
eveim_sync                     =evehp_image(369, 102  , 62   , 62   , "sync_62x62_PALETTED4444"                     , paletted=evehpi_pallete(211456  , 207552  , 512     , 3904    ), layout = BrtEve.PALETTED4444)
eveim_sync_off                 =evehp_image(369, 102  , 62   , 63   , "sync_off_62x63_PALETTED4444"                 , paletted=evehpi_pallete(215936  , 211968  , 512     , 3968    ), layout = BrtEve.PALETTED4444)
eveim_temp_higher              =evehp_image( 19,  75  , 55   , 38   , "temp-higher_55x38_PALETTED4444"              , paletted=evehpi_pallete(218560  , 216448  , 128     , 2112    ), layout = BrtEve.PALETTED4444)
eveim_temp_lower               =evehp_image( 19,  75  , 55   , 38   , "temp-lower_55x38_PALETTED4444"               , paletted=evehpi_pallete(220800  , 218688  , 128     , 2112    ), layout = BrtEve.PALETTED4444)
eveim_airflow_L1               =evehp_image( 15, 90   , 295  , 280  , "airflow-L1_295x280_COMPRESSED_RGBA_ASTC_5x5_KHR.raw "          , 220928  , 52864 , layout = BrtEve.ASTC_5x5)
eveim_airflow_L2               =evehp_image( 15, 90   , 295  , 280  , "airflow-L2_295x280_COMPRESSED_RGBA_ASTC_5x5_KHR.raw "          , 273792  , 52864 , layout = BrtEve.ASTC_5x5)
eveim_airflow_L3               =evehp_image( 15, 90   , 300  , 290  , "airflow-L3_300x290_COMPRESSED_RGBA_ASTC_5x5_KHR.raw "          , 326656  , 55680 , layout = BrtEve.ASTC_5x5)
eveim_airflow_L4               =evehp_image( 15, 90   , 320  , 305  , "airflow-L4_320x305_COMPRESSED_RGBA_ASTC_5x5_KHR.raw "          , 382336  , 62464 , layout = BrtEve.ASTC_5x5)
eveim_airflow_L5               =evehp_image( 15, 90   , 320  , 305  , "airflow-L5_320x305_COMPRESSED_RGBA_ASTC_5x5_KHR.raw "          , 444800  , 62464 , layout = BrtEve.ASTC_5x5)
eveim_airflow_R1               =evehp_image(508, 92   , 295  , 280  , "airflow-R1_295x280_COMPRESSED_RGBA_ASTC_5x5_KHR.raw "          , 507264  , 52864 , layout = BrtEve.ASTC_5x5)
eveim_airflow_R2               =evehp_image(508, 92   , 295  , 280  , "airflow-R2_295x280_COMPRESSED_RGBA_ASTC_5x5_KHR.raw "          , 560128  , 52864 , layout = BrtEve.ASTC_5x5)
eveim_airflow_R3               =evehp_image(508, 92   , 300  , 290  , "airflow-R3_300x290_COMPRESSED_RGBA_ASTC_5x5_KHR.raw "          , 612992  , 55680 , layout = BrtEve.ASTC_5x5)
eveim_airflow_R4               =evehp_image(490, 92   , 320  , 300  , "airflow-R4_320x300_COMPRESSED_RGBA_ASTC_5x5_KHR.raw "          , 668672  , 61440 , layout = BrtEve.ASTC_5x5)
eveim_airflow_R5               =evehp_image(490, 92   , 320  , 300  , "airflow-R5_320x300_COMPRESSED_RGBA_ASTC_5x5_KHR.raw "          , 730112  , 61440 , layout = BrtEve.ASTC_5x5)
eveim_bg                       =evehp_image(  0,   0  , 862  , 480  , "bg_non_square_862x480_864x480_COMPRESSED_RGBA_ASTC_6x5_KHR.raw", 791552  , 221184, layout = BrtEve.ASTC_6x5)
eveim_R_disable                =evehp_image(626,  63  , 180  , 175  , "R_disable_180x175_COMPRESSED_RGBA_ASTC_6x5_KHR.raw            ", 1012736 , 16832 , layout = BrtEve.ASTC_6x5)


eveims = [
    eveim_0                                     ,
    eveim_1                                     ,
    eveim_2                                     ,
    eveim_3                                     ,
    eveim_4                                     ,
    eveim_5                                     ,
    eveim_6                                     ,
    eveim_7                                     ,
    eveim_8                                     ,
    eveim_9                                     ,
    eveim_ac                                    ,
    eveim_ac_off                                ,
    eveim_air                                   ,
    eveim_air_l_1                               ,
    eveim_air_l_2                               ,
    eveim_air_l_3                               ,
    eveim_air_l                                 ,
    eveim_air_off                               ,
    eveim_air_r_1                               ,
    eveim_air_r_2                               ,
    eveim_air_r_3                               ,
    eveim_air_r                                 ,
    eveim_airflow_L1                            ,
    eveim_airflow_L2                            ,
    eveim_airflow_L3                            ,
    eveim_airflow_L4                            ,
    eveim_airflow_L5                            ,
    eveim_airflow_R1                            ,
    eveim_airflow_R2                            ,
    eveim_airflow_R3                            ,
    eveim_airflow_R4                            ,
    eveim_airflow_R5                            ,
    eveim_AL1_off                               ,
    eveim_AL2_off                               ,
    eveim_AL3_off                               ,
    eveim_AR1_disable                           ,
    eveim_AR1_off                               ,
    eveim_AR2_disable                           ,
    eveim_AR2_off                               ,
    eveim_AR3_disable                           ,
    eveim_AR3_off                               ,
    eveim_auto                                  ,
    eveim_auto_off                              ,
    eveim_C                                     ,
    eveim_celsius                               ,
    eveim_F                                     ,
    eveim_fahrenheit                            ,
    eveim_fan_higher                            ,
    eveim_fan_lower                             ,
    eveim_front                                 ,
    eveim_front_off                             ,
    eveim_onoff                                 ,
    eveim_onoff_off                             ,
    eveim_R_disable                             ,
    eveim_rear                                  ,
    eveim_rear_off                              ,
    eveim_sync                                  ,
    eveim_sync_off                              ,
    eveim_temp_higher                           ,
    eveim_temp_lower                            ,
    eveim_bg                                    ,
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
            
           
def image_on_ramg_from_eve_flash(eve, flash_size=0, skip_blob4096kb = 4096):
    eve.storage.flash_state(eve.FLASH_STATUS_FULL)
    
    if flash_size:
        print("Copying one-time-only from flash into ramg")
        if skip_blob4096kb:
            eve.cmd_flashread(0, 4096, flash_size)
        
    # palette lut must in ramg
    ramg = 0
    for im in eveims:
        if im.paletted and (ramg + im.paletted.size_pallete) < eve.RAM_G_SIZE:
            if flash_size:
                im.paletted.ramg_addr_pallete = im.paletted.addr_pallete -skip_blob4096kb
                print("image paletted lut address on ramg: ", im.filepath, "at ", im.paletted.ramg_addr_pallete)
            else:
                eve.cmd_flashread(ramg, im.paletted.addr_pallete,im.paletted.size_pallete)
                print("image paletted lut copied into ramg: ", im.filepath, "at ", ramg)
                im.paletted.ramg_addr_pallete = ramg
                ramg+=im.paletted.size_pallete

        eve.finish()
    
    # palette index can stored in ramg or flash
    for im in eveims:
        if im.paletted and (ramg + im.paletted.size_index) < eve.RAM_G_SIZE:
            if flash_size:
                im.paletted.ramg_addr_pallete = im.paletted.addr_index-skip_blob4096kb
                print("image paletted lut address on ramg: ", im.filepath, "at ", im.paletted.ramg_addr_pallete)
            else:
                eve.cmd_flashread(ramg, im.paletted.addr_index,im.paletted.size_index)
                print("image paletted index copied into ramg: ", im.filepath, "at ", ramg)
                im.paletted.ramg_addr_index = ramg
                ramg+=im.paletted.size_index
            
            im.exist_on_ramg = True
                
        elif (ramg + im.size) < eve.RAM_G_SIZE:
            if flash_size:
                im.addr_ramg = im.addr_flash-skip_blob4096kb
                print("image address on ramg: ", im.filepath, "at ", im.addr_ramg)
            else:
                eve.cmd_flashread(ramg, im.addr_flash, im.size)
                print("image copied into ramg: ", im.filepath, "at ", ramg)
                im.addr_ramg = ramg
                ramg+=im.size
            
            im.exist_on_ramg = True
        else:
            return
        eve.finish()
        

def flashing(eve, file):
    question = "Write image from sdcard to EVE's connected flash?"
    yes = evehp_widget_dialog_yes_no(eve, title="Preparing flash", question=question, timeout=120, default_tag=False) == True
    if yes == True:
        eve.storage.write_flash_with_progressbar(file, 0)

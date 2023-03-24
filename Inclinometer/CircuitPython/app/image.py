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
    from ..lib.brteve.brt_eve_bt817_8 import BrtEve
    from ..lib.brteve.brt_eve_rp2040 import BrtEveRP2040

imdir="app/images/4444/"
eveim_arrow2           =evehp_image(0  , 0  , 17 , 28 , imdir + "arrow2_17x28_PALETTED4444", paletted=evehpi_pallete(0, 0, 0, 0), layout = BrtEve.PALETTED4444)
eveim_bg               =evehp_image(-5 , -6 , 490, 490, imdir + "bg_490x490_PALETTED4444", paletted=evehpi_pallete(0, 0, 0, 0), layout = BrtEve.PALETTED4444)
eveim_DANGER__         =evehp_image(173, 223, 131, 23 , imdir + "DANGER!!_131x23_PALETTED4444", paletted=evehpi_pallete(0, 0, 0, 0), layout = BrtEve.PALETTED4444)
eveim_Jeep_Back_view   =evehp_image(0  , 0  , 89 , 84 , imdir + "Jeep Back view_89x84_PALETTED4444", paletted=evehpi_pallete(0, 0, 0, 0), layout = BrtEve.PALETTED4444)
eveim_Jeep_Side_view   =evehp_image(0  , 0  , 133, 63 , imdir + "Jeep Side view_133x63_PALETTED4444", paletted=evehpi_pallete(0, 0, 0, 0), layout = BrtEve.PALETTED4444)

eveims = [
    eveim_arrow2          ,
    eveim_bg             ,
    eveim_DANGER__       ,
    eveim_Jeep_Back_view ,
    eveim_Jeep_Side_view ,
]
#copy as much as possible image into ramg 
def image_on_flash_init(eve):
    evehp_image_copy_array_to_flash(eve, eveims)

def image_on_ramg_init(eve):
    evehp_image_copy_array_to_ramg(eve, eveims, msg_on=False)

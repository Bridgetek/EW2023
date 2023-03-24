"""Helper functions for the BridgeTek EVE's application
   Version: 1.0
   Last modified: 14 Feb 2023 - Tuan.Nguyen@Brtchip.com
                  Add palleted image

   Usage:
    1. draw directly
        im =evehp_image(...)
        evehp_image_draw(im)

    2. copy then draw later
        im = [evehp_image(...), evehp_image(...)]
        evehp_image_copy_array_to_flash(im)
        evehp_image_copy_array_to_ramg(im)
        evehp_image_draw(im)
        
"""

from .helper_widgets import *
from .helper_math import *

import sys
if sys.implementation.name == "circuitpython":
    from brteve.brt_eve_bt817_8 import BrtEve
    from brteve.brt_eve_storage.brt_eve_storage import align_to
else:
    from lib.brteve.brt_eve_bt817_8 import BrtEve
    from lib.brteve.brt_eve_storage.brt_eve_storage import align_to

class evehpi_pallete():
    def __init__(self, addr_lut, addr_index, size_lut, size_index):
        self.addr_pallete = addr_lut
        self.addr_index = addr_index
        self.size_pallete = size_lut
        self.size_index = size_index

        self.ramg_addr_pallete = 0
        self.ramg_addr_index = 0
        self.ramg_size_pallete = size_lut
        self.ramg_size_index = size_index


class evehp_image():
    def __init__(self, offsetx, offsety, w, h, path, 
                 flash_addr=0, size=0, layout=BrtEve.ASTC_4x4, 
                 tag=None, paletted:evehpi_pallete=None, handler_set = 0, handler_draw = 0) -> None:
        self.w = w
        self.h = h
        self.offsetx = offsetx
        self.offsety = offsety
        self.filepath = path
        self.addr_ramg = 0
        self.addr_flash = flash_addr
        self.bitmaplayout = layout
        self.handler_set = handler_set
        self.handler_draw = handler_draw
        self.tag = tag
        self.paletted = paletted
        self.size = size

        self.exist_on_ramg = False
        self.exist_on_flash = False

        if flash_addr > 4096:
            self.exist_on_ramg = False
            self.exist_on_flash = True
        # print("self img: ",self.filepath ,  self.bitmaplayout, self.w, self.h, self.addr_flash)

class _evehp_gesture_image():
    """Internal data"""
    flash_top_addr = 4096
    ramg_top_addr = 0


def evehp_image_draw(eve: BrtEve, img: evehp_image, x=0, y=0, tag=None, scale_ratio=1, 
                     default_ramg=True, msg_on=False, addr=None, rotate_angle=0, 
                     handler_set=None, handler_draw=None, quiet=None):
    # print("name", img.filepath, "where", "flash" if img.exist_on_flash else "ramg")

    if img.exist_on_ramg:
        return _asset_draw_ramg(eve, img, x, y, tag, scale_ratio, rotate_angle, handler_set, handler_draw, quiet)
    elif img.exist_on_flash:
        return _asset_draw_flash(eve, img, x, y, tag, scale_ratio)
    elif default_ramg:  # copy to ramg and draw
        evehp_image_copy_to_ramg(eve, img, msg_on, addr)
        return _asset_draw_ramg(eve, img, x, y, tag, scale_ratio, rotate_angle)
    else:  # copy to flash and draw
        evehp_image_copy_to_flash(eve, img, msg_on, addr)
        return _asset_draw_flash(eve, img, x, y, tag, scale_ratio)


def evehp_image_copy_array_to_flash(eve: BrtEve, img: evehp_image, msg_on=True, addr=None):
    im: evehp_image
    for im in img:
        evehp_image_copy_to_flash(eve, im, msg_on, addr)


def evehp_image_copy_array_to_ramg(eve: BrtEve, img: evehp_image, msg_on=True, addr=None):
    im: evehp_image
    for im in img:
        if not evehp_image_copy_to_ramg(eve, im, msg_on, addr):
            return

def evehp_image_copy_to_flash(eve: BrtEve, img: evehp_image, msg_on=True, addr=None):
    address = _evehp_gesture_image.flash_top_addr

    if addr != None and addr >= 4096:
        address = addr

    if msg_on == True:
        evehp_widget_message(
            eve, "Copying " + img.filepath + " to EVE's FLASH...")

    address = evehp_math_flash_address_roundup(address)

    if img.paletted:
        img.paletted.addr_index = address
        address += eve.storage.write_flash_via_fifo(img.filepath+"_index.raw", address, False)

        address = evehp_math_flash_address_roundup(address)
        
        img.paletted.addr_pallete = address
        address += eve.storage.write_flash_via_fifo(img.filepath+"_lut.raw", address, False)

    else:
        img.addr_flash = address
        address += eve.storage.write_flash_via_fifo(img.filepath, address, False)

    address = evehp_math_flash_address_roundup(address)
    _evehp_gesture_image.flash_top_addr = address

    img.exist_on_flash = 1
    img.exist_on_ramg = 0

def evehp_image_copy_to_ramg(eve: BrtEve, img: evehp_image, msg_on=True, addr=None):
    address = _evehp_gesture_image.ramg_top_addr
    if addr != None:
        address = addr

    if msg_on == True:
        filename = img.filepath
        l =len(filename) 
        if l > 32:
            filename = filename[0:25]+"..."+filename[l-4:l]
        evehp_widget_message(
            eve, "Copying " + filename + " to EVE's RAM_G...")
        
    address = evehp_math_roundup_to_nearest(address, 4)
    if address > eve.RAM_G_SIZE:
        evehp_widget_message(eve, "Ignored: image exceeded RAM_G size")
        return False
    
    if img.paletted:
        img.paletted.addr_index = address
        address = eve.write_file(address, img.filepath +"_index.raw")

        address = evehp_math_roundup_to_nearest(address, 4)
        if address > eve.RAM_G_SIZE:
            evehp_widget_message(eve, "Ignored: image exceeded RAM_G size")
            return False
        
        img.paletted.addr_pallete = address
        address = eve.write_file(address, img.filepath +"_lut.raw")
    else:
        img.addr_ramg = address
        address = eve.write_file(address, img.filepath)
        if address > eve.RAM_G_SIZE:
            evehp_widget_message(eve, "Ignored: image exceeded RAM_G size")
            return False

    _evehp_gesture_image.ramg_top_addr = address

    img.exist_on_ramg = 1
    img.exist_on_flash = 0
    return True # success

def _asset_scale(eve: BrtEve, ratio, img):
    eve.BitmapSize(eve.BILINEAR, eve.BORDER, eve.BORDER, img.w * 2, img.h * 2)
    eve.cmd_loadidentity()
    # eve.cmd_translate(65536 * img.w, 65536 * img.h)
    eve.cmd_scale(ratio * 65536, ratio * 65536)
    # eve.cmd_translate(-65536 * img.w, -65536 * img.h)
    eve.cmd_setmatrix()

    
def _asset_draw_flash(eve: BrtEve, img: evehp_image, x=0, y=0, tag=None, scale_ratio=1):
    if img.bitmaplayout == eve.PALETTED8:
        print("Error: Paletted8 is not support from flash")
        return
    elif img.paletted:
        eve.PaletteSource(img.paletted.addr_pallete)
        eve.cmd_setbitmap(img.paletted.addr_index, img.bitmaplayout, img.w, img.h)
    else:
        addr=evehp_math_flash_address_convert(img.addr_flash)
        eve.cmd_setbitmap(addr, img.bitmaplayout, img.w, img.h)
        # print("flash img: ",img.filepath ,  img.bitmaplayout, img.w, img.h, img.addr_flash, addr)

    if scale_ratio != 1:
        eve.SaveContext()
        _asset_scale(eve, scale_ratio, img)

    if tag and tag > 0 and tag < 255:
        eve.Tag(tag)
    eve.Begin(eve.BITMAPS)
    eve.Vertex2f(img.offsetx * scale_ratio+x, img.offsety * scale_ratio+y)

    if scale_ratio != 1:
        eve.RestoreContext()

def _asset_draw_ramg(eve: BrtEve, img: evehp_image, x=0, y=0, tag=None, 
                     scale_ratio=1, rotate_angle=0, 
                     handler_set=0, handler_draw=0, quiet=0):
    _MAX_ANGLE       =  360
    _CIRCLE_MAX      =  65536
    mrootx = 0
    mrooty = 0
    translate = 0
    lw = 0
    lh = 0
    imx = img.offsetx * scale_ratio + x
    imy = img.offsety * scale_ratio + y
    
    eve.SaveContext()

    if not handler_draw:
        handler_draw = img.handler_draw
    if not handler_set:
        handler_set = img.handler_set
        
    # set bitmap
    if handler_set and not handler_draw:
        print("image set handler: ", img.filepath, handler_set)
        eve.BITMAP_HANDLE(handler_set)
    
    if handler_draw:
        eve.BITMAP_HANDLE(handler_draw)
        # print("image reuser handler: ", img.filepath)
    elif img.bitmaplayout == eve.PALETTED8:
        bitmap_source = img.paletted.ramg_addr_index

        stride = img.w
        height= img.h
        width = img.w
        # print("Drawing image", img.filepath, "with paletted8, index=", bitmap_source, "lut=", pal_source)
        
        eve.BITMAP_SOURCE(bitmap_source)
        eve.BITMAP_LAYOUT(eve.PALETTED8, stride & 0x3ff, height & 0x1ff)
        eve.BITMAP_LAYOUT_H(stride >> 10, height >> 9)
        eve.BITMAP_SIZE(eve.NEAREST, eve.BORDER, eve.BORDER, width & 0x1ff, height & 0x1ff)
        eve.BITMAP_SIZE_H(width >> 9, height >> 9)

    elif img.paletted:
        eve.PaletteSource(img.paletted.ramg_addr_pallete)
        eve.cmd_setbitmap(img.paletted.ramg_addr_index, img.bitmaplayout, img.w, img.h)
        # print("img:", img.filepath, "paletted source: ", img.paletted.ramg_addr_pallete, img.paletted.ramg_addr_index, img.bitmaplayout, img.w, img.h)
    else:
        eve.cmd_setbitmap(img.addr_ramg, img.bitmaplayout, img.w, img.h)
        # print("ramg img.bitmaplayout", img.bitmaplayout, img.w, img.h, img.addr_ramg)


    if tag and tag > 0 and tag < 255:
        eve.Tag(tag)

    if scale_ratio != 1:
        _asset_scale(eve, scale_ratio, img)

    if rotate_angle != 0:
        #painting frame size 4x image
        _PAINT_SIZE = 8
        translate = img.w if img.w > img.h else img.h
        lw = translate * _PAINT_SIZE
        lh = translate * _PAINT_SIZE

        # center of painting frame
        mrootx = img.w / 2
        mrooty = img.h / 2

        # painting frame
        eve.BitmapSize(eve.BILINEAR, eve.BORDER, eve.BORDER, lw, lh)
        eve.BitmapSizeH(lw >> 8, lh >> 8) 

        eve.cmd_loadidentity()
        # move image to the center of painting frame
        eve.cmd_translate((img.w) * _CIRCLE_MAX, (img.h) * _CIRCLE_MAX)
        eve.cmd_rotate(rotate_angle * _CIRCLE_MAX / _MAX_ANGLE) 
        eve.cmd_translate(-(mrootx) * _CIRCLE_MAX, -(mrooty) * _CIRCLE_MAX)
        eve.cmd_setmatrix()

        # finale vertex move image back to top-left 
        imx = img.offsetx * scale_ratio + x-mrootx
        imy = img.offsety * scale_ratio + y-mrooty
        # print("img x,t: ", imx, imy) 

    if quiet:
        return
    
    # vertex
    eve.Begin(eve.BITMAPS)
    if img.bitmaplayout == eve.PALETTED8:
        pal_source = img.paletted.ramg_addr_pallete
        
        eve.BLEND_FUNC(eve.ONE, eve.ZERO)
        eve.COLOR_MASK(0, 0, 0, 1)
        eve.PALETTE_SOURCE(pal_source + 3)
        eve.VERTEX2F(imx, imy)

        eve.BLEND_FUNC(eve.DST_ALPHA, eve.ONE_MINUS_DST_ALPHA)
        eve.COLOR_MASK(1, 0, 0, 0)
        eve.PALETTE_SOURCE(pal_source + 2)
        eve.VERTEX2F(imx, imy)

        eve.COLOR_MASK(0, 1, 0, 0)
        eve.PALETTE_SOURCE(pal_source + 1)
        eve.VERTEX2F(imx, imy)

        eve.COLOR_MASK(0, 0, 1, 0)
        eve.PALETTE_SOURCE(pal_source + 0)
        eve.VERTEX2F(imx, imy)
    else:
        eve.Vertex2f(imx, imy)
    eve.END()
    eve.RESTORE_CONTEXT()
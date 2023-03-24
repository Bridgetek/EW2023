"""The Raspberry Pi Pico is a low-cost, high-performance microcontroller
   board with flexible digital interfaces"""
import os
import board
import time
import busio
import digitalio
import sdcardio # pylint: disable=import-error
import storage # pylint: disable=import-error

def spilock(func):
    """ Delegate function, to lock and unlocki SPI"""
    def wrapper(*args):
        spi = args[0].spi_eve
        while not spi.try_lock():
            pass
        ret = func(*args)
        spi.unlock()
        return ret
    return wrapper

def pin(pin_num):
    """Init a pin"""
    ret = digitalio.DigitalInOut(pin_num)
    ret.direction = digitalio.Direction.OUTPUT
    ret.value = True
    return ret

class BrtEveRP2040():
    """ Host platform RP2040 to control EVE, this class initialize,
    and set up SPI connection on RP2040, also set up the SDcard

    A host platform class must have below APIs:
     - transfer()
     - write_ili9488()
     - write_ili9488_cmd()
     - write_ili9488_data()
     - spi_sdcard -- SPI object of SDcard interface
    """

    def __init__(self, 
                 eve_spi_sck = board.GP2 , 
                 eve_spi_mosi = board.GP3, 
                 eve_spi_miso = board.GP4):
        mach = os.uname().machine # pylint: disable=no-member
        if mach != 'Raspberry Pi Pico with rp2040':
            return
        
        #SPI for Eve
        self.spi_eve = busio.SPI(eve_spi_sck, MOSI=eve_spi_mosi, MISO=eve_spi_miso)
        self.pin_eve_cs = pin(board.GP5) #cs of SPI for Eve
        self.pin_eve_pdn = pin(board.GP7) #power down pin of Eve

        self.pin_ili9488_cs = pin(board.GP9) #CSX pin of ILI9488
        self.pin_ili9488_dcx = pin(board.GP8) #D/CX pin of ILI9488

        # SD card connecting to RP2040
        # self.setup_sd() # disable when using ST7701S

    def setup_sd(self,
                  pin_sck = board.GP10,
                  pin_cs = board.GP13,
                  pin_mosi = board.GP11,
                  pin_miso = board.GP12,
                  ):
        """ Setup sdcard"""
        #SPI for SD card
        self.pin_cs_sdcard = pin_cs #cs of SPI for SD card
        self.spi_sdcard = busio.SPI(pin_sck, MOSI=pin_mosi, MISO=pin_miso)
        try:
            self.sdcard = sdcardio.SDCard(self.spi_sdcard, self.pin_cs_sdcard)
        except OSError:
            return False
        
        self.vfs = storage.VfsFat(self.sdcard)
        storage.mount(self.vfs, "/sd")
        return True

    @spilock
    def _setup_spi(self):
        """ Setup SPI interface"""
        self.spi_eve.configure(baudrate=30000000, phase=0, polarity=0)

    @spilock
    def transfer(self, write_data, bytes_to_read = 0):
        """ Transfer data via SPI"""
        self.pin_eve_cs.value = False
        self.spi_eve.write(write_data)
        read_buffer = None
        if bytes_to_read != 0:
            read_buffer = bytearray(bytes_to_read)
            self.spi_eve.readinto(read_buffer)
        self.pin_eve_cs.value = True
        return read_buffer

    def write_ili9488(self,cmd,data):
        """ Write command and data to ili9488 LCD"""
        self.write_ili9488_cmd(cmd)
        self.write_ili9488_data(data)

    @spilock
    def write_ili9488_cmd(self, cmd):
        """ Write command to ili9488 LCD"""
        self.pin_ili9488_cs.value = False
        self.pin_ili9488_dcx.value = False

        self.spi_eve.write(cmd)
        self.pin_ili9488_cs.value = True

    @spilock
    def write_ili9488_data(self, data):
        """ Write data to ili9488 LCD"""
        self.pin_ili9488_cs.value = False
        self.pin_ili9488_dcx.value = True

        self.spi_eve.write(data)
        self.pin_ili9488_cs.value = True

    def send_ST7701S(self, data, pin_cs, pin_sda, pin_sck, is_command):
            pin_cs.value = False
            time.sleep(0.0001)
            if is_command:
                pin_sda.value =False
            else:
                pin_sda.value =True

            pin_sck.value = False
            time.sleep(0.0001)
            pin_sck.value = True
            for i in range(8):
                time.sleep(0.0001)
                pin_sck.value=False
                if (data&0x80):
                    pin_sda.value =True
                else:
                    pin_sda.value =False
                data = data <<1
                time.sleep(0.0001)
                pin_sck.value = True
            time.sleep(0.0001)
            pin_sck.value = False
            time.sleep(0.0001)
            pin_cs.value = True
            
    def send_ST7701S_cmd(self, data, pin_cs, pin_sda, pin_sck):
        return self.send_ST7701S(data, pin_cs, pin_sda, pin_sck, True)
    
    def send_ST7701S_arg(self, data, pin_cs, pin_sda, pin_sck):
        return self.send_ST7701S(data, pin_cs, pin_sda, pin_sck, False)

    def ST7701_set(self,
        pin_cs = board.GP17,
        pin_sck = board.GP10,
        pin_sda = board.GP11,
        pin_reset  = board.GP22
        ):
        pin_cs = pin(pin_cs)
        pin_sck = pin(pin_sck)
        pin_sda = pin(pin_sda)
        pin_reset  = pin(pin_reset)

        time.sleep(0.02)
        pin_cs.value = True
        pin_sda.value = True
        pin_sck.value = False
        time.sleep(0.02)

        pin_reset.value = True
        time.sleep(0.02)
        pin_reset.value = False
        time.sleep(0.02)
        pin_reset.value = True
        time.sleep(0.02)

        self.send_ST7701S_cmd(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x77, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x01, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x10, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xC0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x3B, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xC1, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x0B, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x02, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xC2, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x07, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x02, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xCC, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x10, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xCD, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x08, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xB0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x11, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x16, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x0E, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x11, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x06, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x05, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x09, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x08, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x21, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x06, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x13, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x10, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x29, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x31, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x18, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xB1, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x11, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x16, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x0E, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x11, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x07, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x05, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x09, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x09, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x21, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x05, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x13, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x11, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x2A, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x31, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x18, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x77, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x01, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x11, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xB0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x6D, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xB1, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x37, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xB2, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x81, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xB3, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x80, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xB5, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x43, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xB7, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x85, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xB8, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x20, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xC1, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x78, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xC2, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x78, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xC3, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x8C, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xD0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x88, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xE0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x02, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xE1, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x03, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x04, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x20, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x20, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xE2, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xE3, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x11, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xE4, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x22, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xE5, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x05, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xEC, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x07, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xEE, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xE6, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x11, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xE7, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x22, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xE8, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x06, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xED, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x08, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xEF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xEB, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x40, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x40, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xED, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xBA, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x0A, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xBF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x45, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x54, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xFB, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xA0, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xAB, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xEF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x10, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x0D, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x04, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x08, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x3F, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x1F, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x77, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x01, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x13, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xEF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x08, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0xFF, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x77, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x01, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0x36, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x00, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_cmd(0x3A, pin_cs, pin_sda, pin_sck)
        self.send_ST7701S_arg(0x66, pin_cs, pin_sda, pin_sck)
        time.sleep(0.1)
        self.send_ST7701S_cmd(0x11,pin_cs, pin_sda, pin_sck)
        time.sleep(0.1)     
        self.send_ST7701S_cmd(0x29, pin_cs, pin_sda, pin_sck)

        # pin_cs.deinit()
        # pin_sck.deinit()
        # pin_sda.deinit()
        # pin_reset.deinit()



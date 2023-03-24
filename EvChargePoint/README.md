# EvChargePoint Demo 

## Configuration:
- BT817
- LCD 1280x800
- MM2040EV (Pico RP2040 mcu)
- No need SDcard

Build: (On Developer command promt for VS 2019)
```
cd EveDemoApps
mkdir build && cd build
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040 -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=EVE_DISPLAY_WXGA ..
nmake EvChargePoint
```

## Build instructions
### Raspberry Pi Pico
Requires the Pico toolchain https://github.com/ndabas/pico-setup-windows to be installed.

The following steps will build for Raspberry Pi Pico.
Replace `X:\source\EveDemoApps` with your EveDemoApps repository path.

Launch the *Developer Command Prompt for VS*
```sh
cd X:\source\EveDemoApps
mkdir build
cd build
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040 -DEVE_APPS_GRAPHICS=[EVE graphics] ..
nmake [Project name]

# [EVE graphics] can be EVE or module name, such as BT817, BT815, VM800B35A_BK ...
# [Project name] is the folder name of a subfolder inside demo_application
# example: nmake CarDashBoard
```

Display resolution is set via `EVE_APPS_DISPLAY`, example: cmake -G "NMake Makefiles"  -DEVE_APPS_DISPLAY=WXGA
By default, WVGA is set

EVE graphics is set via `EVE_APPS_GRAPHICS`, example: cmake -G "NMake Makefiles"  -DEVE_APPS_GRAPHICS=BT815
By default, MULTI is set

### MSVC
Way 1: Open .sln inside each example, press F5 to build and run

Way 2: Use Cmake:
```sh
#Launch the Developer Command Prompt for VS
cd X:\source\EveDemoApps
mkdir build
cd build
cmake -G "NMake Makefiles" -DEVE_APPS_PLATFORM=FT4222 -DEVE_APPS_GRAPHICS=[EVE graphics] ..
nmake [Project name]

# [EVE graphics] can be EVE or module name, such as BT817, BT815, VM800B35A_BK ...
# [Project name] is the folder name of a subfolder inside demo_application
# example: nmake CarDashBoard
```

# Run

## Connections

| RP2040 | EVE | UART | SD |
| --- | --- | --- | --- |
| GP0 (TX) | | RX (debug) | |
| GP1 (RX) | | TX (optional) | |
| GP2 (SPI0 SCK) | SCK | | |
| GP3 (SPI0 MOSI) | MOSI | | |
| GP4 (SPI0 MISO) | MISO | | |
| GP5 (GPIO) | CS | | |
| GP6 (GPIO) | INT | | |
| GP7 (GPIO) | PWD | | |
| 5V | 5V | | |
| GND | GND | | |

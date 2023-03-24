## INTRODUCTION
   This is Car DashBoard demo for EW2023.
   The Car DashBoard demo has 2 UIs, user can switch UI with a button (on MM2040EV, the button GPIO27 and 3.3v)
    
   It support callist replay mode.
   It's written in C language.

## SUPPORTED PLATFORM
   

- Host platform:
        
   1. Microsoft Visual C++ platform with Emulator
   2. MM2040EV (RP2040)

- EVE platform: BT817       
- LCD size: Ultra Stretched Panel (1440x540)

## SETUP AND RUN
- CONNECT HARDWARE
    - Microsoft Visual C++ platform with Emulator
          - This setup uses window PC only
            
    - MM2040EV
          - Attach MM2040EV to the BT817 board via QSPI port
            - Connect a button to MM2040EV:         
          - One button's wire connect to ADC1 port (GPIO27, CN7 port 2nd)
          - Another button's wire connect to 3.3v via an 10k ohm resistor (GPIO27, CN7 port 1st)
        
## BUILD AND RUN

- Pre condition

     1. Flashing:
            - User is required to flash the Test/Flash/BT81X_Flash_CarDashBoard.bin once before run this demo
            - To do that, set macro FLASH_INIT_STATUP = 1 in EW2023_CarDashBoard.c, build and run it.
     2. SDcard
            - If running on embedded board (MM2040EV, FT9XX...), user need to prepare an SDcard: Copy the Test folder to an SDcard, the location on SDcard is: /EveApps/EW2023_CarDashBoard/Test
            - Put the SDcard to MM2040EV
            
- Microsoft Visual C++ platform with Emulator
            - Open project in Project\Msvc_Emulator with Microsoft Visual C++
            - Build (Ctrl + B)
            - Run (F5)
                        
- MM2040EV
            - Build: Check build/build.cmd
            - Run demo: 

     1. Connect an Window PC to MM2040EV with a USB cable
     2. Hold button "BOOTSEL" while pressing reset button on MM2040EV
                   or: Hold button "BOOTSEL" while power on the board MM2040EV
     3. An USB drive will appear on Window PC, copy the UF2 into that USB
     4. The demo should start after that

## CONFIGURATION INSTRUCTIONS

   To change the host and EVE platform, use below macro:
    
   1. Host platform: Defined with ESD_TARGET_PLATFORM, such as:
        - Window host: EVE_PLATFORM_FT4222, EVE_PLATFORM_MPSSE
        - FT9XX host: MM900EV1A, MM900EV1B...
    
   2. EVE platform: Defined with ESD_TARGET_GRAPHICS, such as:
        EVE_GRAPHICS_BT815, EVE_GRAPHICS_BT816, EVE_GRAPHICS_BT817, 
        EVE_GRAPHICS_BT818...
    
 Please see common\eve_hal\EVE_Config.h. for more macro.
            
                 
                                   【END】
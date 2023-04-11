
## INTRODUCTION
This is a demo for an Car Climate Control using BridgeTek [IDM2040-7A](https://brtchip.com/product/idm2040-7a/) module


## HARDWARE 
IDM2040-7A module

## STARTUP
- Start IDM2040 with adafruit-circuitpython-raspberry_pi_pico-en_US-7.3.0.uf2

  Follow this guideline: https://learn.adafruit.com/intro-to-rp2040-pio-with-circuitpython/installing-circuitpython


- Copy source code in Python into CircuitPython drive:
    + CircuitPython/lib
    + CircuitPython/app
    + CircuitPython/code.py
- Demo should start after that

## Functionalities

1. Change clock time:
    Tap the time to open clock configuration page
    Tap on the watch to change te hour and minute
    Tap save to return demo page
    
2.  Control the car climate:
    
    - Fan speed
    - Air distribution
    - Temperature
    - Fresh air
    - Auto fan speed and fresh air
    - Display Celsius anf Fahrenheit
    - Air conditioner on and off
    - Sync control: When enable, the Front and Rear will have the same parameter
    - On and off
    
    The Car climate has 3 zone: Front driver, Front passenger and Rear:
    
    - When Rear is enabled, Front passenger control is disabled
    - When Front is enabled, Rear control is disabled
    
    
    
    
                                   【END】

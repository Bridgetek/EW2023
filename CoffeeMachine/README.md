## INTRODUCTION
BridgeTek's Embedded World 2023 demo: Coffee Machine using BridgeTek's IDM2040-43A with Arduino IDE


## HARDWARE 
- IDM2040-43A with:
    - RPi2040 mcu
    - EVE IC BT883
    - LCD 4.3inch 480x272
- Datasheet: https://brtchip.com/wp-content/uploads/2023/11/DS_IDM2040-43A.pdf

## STARTUP
- Copy EVE's flash file to an SDcard:
```sh
    $cp CoffeeMachine\SDcard\e23cfm.bin F:\
```
- Plug SDcard into IDM2040-43A board
- Start Arduino IDE, open the CoffeeMachine/EW2023_CoffeeMachine/EW2023_CoffeeMachine.ino
- Click "Upload" button to start demo.
- Demo should start after that

 ## Asset list
Below is asset mapping of e23cfm.bin:
```
    unified.blob                                         : 0      : 4096  
    americano_118x86_PALETTED4444_index.raw              : 4096   : 10176 
    americano_118x86_PALETTED4444_lut.raw                : 14272  : 512   
    bg_480x272_PALETTED4444_index.raw                    : 14784  : 130560
    bg_480x272_PALETTED4444_lut.raw                      : 145344 : 320   
    Brewing in Progress..._231x24_PALETTED4444_index.raw : 145664 : 5568  
    Brewing in Progress..._231x24_PALETTED4444_lut.raw   : 151232 : 512   
    brewing_97x106_PALETTED4444_index.raw                : 151744 : 10304 
    brewing_97x106_PALETTED4444_lut.raw                  : 162048 : 512   
    btn selected big_60x30_PALETTED4444_index.raw        : 162560 : 1856  
    btn selected big_60x30_PALETTED4444_lut.raw          : 164416 : 448   
    btn selected_50x30_PALETTED4444_index.raw            : 164864 : 1536  
    btn selected_50x30_PALETTED4444_lut.raw              : 166400 : 512   
    btn unselected big_50x20_PALETTED4444_index.raw      : 166912 : 1024  
    btn unselected big_50x20_PALETTED4444_lut.raw        : 167936 : 320   
    btn unselected_40x20_PALETTED4444_index.raw          : 168256 : 832   
    btn unselected_40x20_PALETTED4444_lut.raw            : 169088 : 256   
    btn-brew_122x40_PALETTED4444_index.raw               : 169344 : 4928  
    btn-brew_122x40_PALETTED4444_lut.raw                 : 174272 : 512   
    btn-left_38x46_PALETTED4444_index.raw                : 174784 : 1792  
    btn-left_38x46_PALETTED4444_lut.raw                  : 176576 : 512   
    btn-right_38x46_PALETTED4444_index.raw               : 177088 : 1792  
    btn-right_38x46_PALETTED4444_lut.raw                 : 178880 : 512   
    btn-start_122x40_PALETTED4444_index.raw              : 179392 : 4928  
    btn-start_122x40_PALETTED4444_lut.raw                : 184320 : 512   
    cappuchino_118x86_PALETTED4444_index.raw             : 184832 : 10176 
    cappuchino_118x86_PALETTED4444_lut.raw               : 195008 : 512   
    Coffee Size_58x9_PALETTED4444_index.raw              : 195520 : 576   
    Coffee Size_58x9_PALETTED4444_lut.raw                : 196096 : 128   
    Coffee Strength_83x12_PALETTED4444_index.raw         : 196224 : 1024  
    Coffee Strength_83x12_PALETTED4444_lut.raw           : 197248 : 128   
    Dark Coffee Roast_170x15_PALETTED4444_index.raw      : 197376 : 2560  
    Dark Coffee Roast_170x15_PALETTED4444_lut.raw        : 199936 : 512   
    Enjoy your coffee!_191x23_PALETTED4444_index.raw     : 200448 : 4416  
    Enjoy your coffee!_191x23_PALETTED4444_lut.raw       : 204864 : 512   
    enjoy__coffee_93x68_PALETTED4444_index.raw           : 205376 : 6336  
    enjoy__coffee_93x68_PALETTED4444_lut.raw             : 211712 : 512   
    Good Morning!_175x25_PALETTED4444_index.raw          : 212224 : 4416  
    Good Morning!_175x25_PALETTED4444_lut.raw            : 216640 : 512   
    L_6x7_PALETTED4444_index.raw                         : 217152 : 64    
    L_6x7_PALETTED4444_lut.raw                           : 217216 : 64    
    Light Coffee Roast_174x21_PALETTED4444_index.raw     : 217280 : 3712  
    Light Coffee Roast_174x21_PALETTED4444_lut.raw       : 220992 : 512   
    M_8x7_PALETTED4444_index.raw                         : 221504 : 64    
    M_8x7_PALETTED4444_lut.raw                           : 221568 : 128   
    Medium Coffee Roast_201x15_PALETTED4444_index.raw    : 221696 : 3072  
    Medium Coffee Roast_201x15_PALETTED4444_lut.raw      : 224768 : 512   
    Milk Type_52x12_PALETTED4444_index.raw               : 225280 : 640   
    Milk Type_52x12_PALETTED4444_lut.raw                 : 225920 : 128   
    Milk_23x9_PALETTED4444_index.raw                     : 226048 : 256   
    Milk_23x9_PALETTED4444_lut.raw                       : 226304 : 64    
    mocha_118x86_PALETTED4444_index.raw                  : 226368 : 10176 
    mocha_118x86_PALETTED4444_lut.raw                    : 236544 : 512   
    Oat_15x7_PALETTED4444_index.raw                      : 237056 : 128   
    Oat_15x7_PALETTED4444_lut.raw                        : 237184 : 192   
    S_5x7_PALETTED4444_index.raw                         : 237376 : 64    
    S_5x7_PALETTED4444_lut.raw                           : 237440 : 64    
    Skimmed_40x7_PALETTED4444_index.raw                  : 237504 : 320   
    Skimmed_40x7_PALETTED4444_lut.raw                    : 237824 : 384   
    Soy_16x10_PALETTED4444_index.raw                     : 238208 : 192   
    Soy_16x10_PALETTED4444_lut.raw                       : 238400 : 192   
    steam_50x38_PALETTED4444_index.raw                   : 238592 : 1920  
    steam_50x38_PALETTED4444_lut.raw                     : 240512 : 512   
    strength 1_168x8_PALETTED4444_index.raw              : 241024 : 1344  
    strength 1_168x8_PALETTED4444_lut.raw                : 242368 : 512   
    strength 2_168x8_PALETTED4444_index.raw              : 242880 : 1344  
    strength 2_168x8_PALETTED4444_lut.raw                : 244224 : 512   
    strength 3_168x8_PALETTED4444_index.raw              : 244736 : 1344  
    strength 3_168x8_PALETTED4444_lut.raw                : 246080 : 512   
    strength 4_168x8_PALETTED4444_index.raw              : 246592 : 1344  
    strength 4_168x8_PALETTED4444_lut.raw                : 247936 : 512   
    Sweetness Level_84x9_PALETTED4444_index.raw          : 248448 : 768   
    Sweetness Level_84x9_PALETTED4444_lut.raw            : 249216 : 128   
    warning_11x9_PALETTED4444_index.raw                  : 249344 : 128   
    warning_11x9_PALETTED4444_lut.raw                    : 249472 : 64    
    water_9x12_PALETTED4444_index.raw                    : 249536 : 128   
    water_9x12_PALETTED4444_lut.raw                      : 249664 : 192   
    Whole_26x7_PALETTED4444_index.raw                    : 249856 : 192   
    Whole_26x7_PALETTED4444_lut.raw                      : 250048 : 256   
```

                       【END】

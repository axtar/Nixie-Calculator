# Read before you start
#### This repository contains some information and ideas for building a nixie calculator and is provided *as-is* for educational, experimental, and hobbyist purposes only. However, this repository is only intended for suitably qualified electronics engineers. The project has not been tested for compliance with electrical, safety, or electromagnetic standards (e.g., CE, FCC). 

##
<img align="left" width="70" src="../Images/warning_sign_small.png" />

#### Nixie tubes and associated circuitry operate at potentially dangerous voltages. If you choose to build or use the calculator, you are doing so at your own risk.

##
**Please read all documents in [Docs](../Docs) before deciding whether to build the calculator.**

## Known issues
Currently no known issues<br>
~~The recently added nixie dimming function has been disabled in versions IN-12A and IN-12B due to flickering of the LED lighting.~~ fixed :white_check_mark:

## Documentation guide

[Assembling the PCBs](Assembling%20the%20PCBs.pdf)

Not a guide but some important information about assembling the PCBs

[Assembling the calculator - 7-seg LED - version](Assembling%20the%20calculator%20-%207-seg%20LED%20-%20version.pdf)

Step-by-step assembly guide for the 7-seg LED calculator version

[Assembling the calculator - IN-12 - version](Assembling%20the%20calculator%20-%20IN-12%20-%20version.pdf)

Step-by-step assembly guide for the IN-12 calculator version

[Assembling the calculator - IN-16 - B-5870 - IN-17 - versions](Assembling%20the%20calculator%20-%20IN-16%20-%20B-5870%20-%20IN-17%20-%20versions.pdf)

Step-by-step assembly guide for the IN-16 - B-5870 - IN-17 - calculator versions

[Assembling the peripherals module](Assembling%20the%20peripherals%20module.pdf)

Step-by-step assembly guide for the optional peripherals-module

[Flashing the bootloader and the firmware](Flashing%20the%20bootloader%20and%20the%20firmware.pdf)

Step-by-step guide for burning the bootloader of the keyboard MCU as well as flashing the keyboard and controller firmware

[Gerber and EasyEDA project files](Gerber%20and%20EasyEDA%20project%20files.pdf)

Some information about the PCB files

[High Voltage Power Supply](High%20Voltage%20Power%20Supply.pdf)

Recommendations for the nixie power supply

[Keyboard layout and keycaps](Keyboard%20layout%20and%20keycaps.pdf)

Shows the RPN and ALG keyboard layouts and contains information on how to obtain the custom keycaps

[Mounting the nixies](Mounting%20the%20%20nixies.pdf)

Step-by-step guide for mounting and soldering the nixie tubes

[Nixie Calculator User Manual (Algebraic)](Nixie%20Calculator%20User%20Manual%20(Algebraic).pdf)

Complete user manual for the algebraic version of the calculator

[Nixie Calculator User Manual (RPN)](Nixie%20Calculator%20User%20Manual%20(RPN).pdf)

Complete user manual for the RPN version of the calculator

[PCB List](PCB%20List.pdf)

Contains a list of PCBs required for the different versions of the calculator

[Printing the case](Printing%20the%20case.pdf)

Contains a list of 3D printed parts required for the different versions of the calculator as well as information on printing and post-processing the parts

[How to build the keyboard firmware](/Firmware/Keyboard/README_HowToBuild.md)

Short guide on how to build the keyboard firmware

[How to build the controller firmware](/Firmware/Controller/README_HowToBuild.md)

Short guide on how to build the controller firmware

## How to approach this project
Consider building the 7-segment LED version first. If you later decide to build a nixie version you can reuse the keyboard and the controller.


I recommend following this order:
1. Read the documentation (and use it to implement the next steps).
2. Build the keyboard firmware.
3. Build the controller firmware.
4. Assemble the controller board and flash the firmware. 
5. Assemble the keyboard, burn the bootloader and flash the firmware.
6. Assemble the display board(s).
7. If building a nixie version use a commercial nixie power supply (recommended) or assemble the HV PSU board (experimental).
8. Complete the wiring of the device.
9. Test the device.
10. Print the case and complete the assembly of the calculator.


## AI
I recently started using AI in this project because I wanted to add some additional web-based features to the calculator, and I don't enjoy web design.

## Web server and OTA (over-the-air) firmware update
The calculator can be configured and used without a network connection. If you don't like network and web-based features in a retro project you can just change the defines to
``` C++
#define WEB_SUPPORT false
#define OTA_SUPPORT false
```
in the Config.h file and the network-based stuff will be ignored during compilation. If you want to use the the web interface but you don't like the OTA update feature, set the defines to
``` C++
#define WEB_SUPPORT true
#define OTA_SUPPORT false
```

## Internet connection
Since the time can be set manually and synchronized with the browser time or a GPS module, I will not implement an internet connection for NTP synchronization. I may be wrong, but securely managing passwords for third-party access points requires, in addition to firmware and NVS encryption (including fuse burning), a secure browser connection with valid certificates to avoid browser warnings. I don't feel like dealing with all that stuff.
## Device mirroring
The device can be mirrored using a browser. Device mirroring is WebSocket based and is bidirectional as there is also a virtual keyboard. Actions on the calculator are reflected in the browser and vice versa. Please note that the updates sent to the browser are throttled to avoid potential WebSocket flooding, for example, in stopwatch mode. 

## About the experimental arbitrary-precision arithmetic
The arbitrary-precision arithmetic used in the calculator is based on the ratpak library. It’s experimental because I had to make changes to run the code on an ESP32 and fix some memory leaks, rounding problems and faulty formatting routines.

Calculations with very big numbers can be slow. For example, calculating the square root of 9e+9999 with a precision of 32 digits takes about 7 seconds. For calculations in the more common range between 1e+99 and 1e-99, the result is usually displayed immediately. One exception is the calculation of non-integer factorials where a gamma function approximation is used.

The precision of the calculations and the internal registers has been set to 32 as a compromise between accuracy and performance.

The decision to use arbitrary-precision arithmetic was motivated by the fun of learning something new and by the poor precision of the 64-bit floating-point arithmetic. Just an example: if you calculate 1.0000000000001 ^ 999 using the C/C++ pow function you will get 1.00000000009982 instead of 1.00000000009990.

## Precision of timer, stopwatch and clock

The precision of the timer and stopwatch depends on the internal MCU oscillator. It is possible to connect the (probably) more precise 32kHz clock signal of the RTC chip to the ESP32, but I have no plans to implement this change in the firmware. If no GPS module is connected, the clock depends on the accuracy of the RTC chip.

## Nixies

To extend the lifespan of the nixie tubes, it’s recommended to use the auto-off function or to connect the peripherals module for presence detection (PIR). It’s also recommended to use the cathode poisoning prevention function. This is especially important for the IN-15A symbol nixies which only display + and -. The firmware allows dimming of the nixies using a 20 kHz PWM signal. I don't know if this affects their lifespan.

## Firmware development tools

I use VS Code and the ESP-IDF extension with arduino-esp32 as a component to develop the firmware for the controller (ESP32). For the keyboard firmware (ATmega328P) I use the Arduino IDE. 


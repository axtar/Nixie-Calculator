## Nixie calculator controller firmware
### Version: 0.9.4
Status:    Beta<br/>
Date:      September 3, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 248000
#### Changes
- The SSID can now be changed using the web interface
---
### Version: 0.9.3
Status:    Beta<br/>
Date:      September 3, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 252000
#### Comments
LED flickering was fixed by twisting the GND wire with the CTL wire (LED cable) and the GND wire with the BLANK wire (DRIVER cable)
#### Changes
- Re-enabled nixie dimming for IN-12A and B versions
---
### Version: 0.9.2
Status:    Beta<br/>
Date:      September 2, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 252000
#### Comments
Erase flash before applying this version
#### Changes
- Added OTA firmware update to the web interface
---
### Version: 0.9.1
Status:    Beta<br/>
Date:      August 30, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 248500
#### Changes
- Replaced std::bind with lambdas
#### Fixes
- Fixed busy calc animations not displaying on calculator page
---
### Version: 0.9.0
Status:    Beta<br/>
Date:      August 29, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 248500
#### Comments
There is currently nothing else on the "roadmap"
#### Changes
- Added AI generated Wi-Fi access point password change page
- Added a "reset password to default" shortcut in menu mode (the shortcut is restricted to the physical keyboard)
- Added AI generated page to sync the calculator time with the browser time
- The shortcut [F]+[ENT] / [F]+[=] to start/stop the AP now also works in clock mode
- The shortcut [F]+[STO] / [F]+[MS] now commits all the temporarely changed settings
- Show heap memory shorcut changed to [F]+[0]
- Added a factory reset option by pressing the button at the back for 3 seconds during startup
- Added a new setting to correct the temperature provided by the external sensor
- Added a new setting to correct for potential time drift of the RTC chip.
---
### Version: 0.3.8
Status:    Beta<br/>
Date:      August 27, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 248500
#### Fixes
- Fixed the status page always showing temperatures in degrees Celsius
- Fixed the calculator display always showing the board temperature in degrees Celsius
- Fixed empty registers on the calculator page after leaving and returning to it
- Fixed stale LED lighting when entering menu mode via the web interface
- Fixed some typos in the comments
---
### Version: 0.3.7
Status:    Beta<br/>
Date:      August 26, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 248500
#### Changes
- Added new setting "trigcolorchange" that triggers a color change event for RGB random modes in clock mode
- Added new settings "dimbrightness", "dimstarttime" and "dimduration" for time driven brightness change
---
### Version: 0.3.6
Status:    Beta<br/>
Date:      August 26, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 249000
#### Changes
- Added percentage of "High Voltage On Time" in the status page
#### Fixes
- Timezone rules are now reloaded after a configuration change
---
### Version: 0.3.5
Status:    Beta<br/>
Date:      August 25, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 249000
#### Changes
- Removed external dependencies by replacing libraries with native drivers and additional code
---
### Version: 0.3.4
Status:    Beta<br/>
Date:      August 15, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 246000
#### Changes
- Disabled nixie dimming for IN-12A/B display versions due to LED flickering
- Digits on the virtual display are now more visible
---
### Version: 0.3.3
Status:    Beta<br/>
Date:      August 15, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 246000
#### Changes
- Added "High Voltage On Time" to the status page
- Removed "Calculator Time" from the status page
- For the LED 7-seg version, the virtual display now renders the digits as 7-segment displays
#### Known issues
- PWM nixie dimming is causing LED flickering in the IN-12A/B display versions. Next version will disable PWM dimming for these display types. 
---
### Version: 0.3.2
Status:    Beta<br/>
Date:      August 12, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 246000
#### Changes
- Added PWM nixie dimming. Setting the brightness to 15 disables PWM
---
### Version: 0.3.1
Status:    Beta<br/>
Date:      August 11, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 246000
#### Changes
- Added AI generated "Calculator" page for device mirroring
- Removed the "Registers" page
- Enhanced performance when showing registers
- Added "apautostart" setting. If set to on, the WiFi access point will be enabled at startup
---
### Version: 0.2.2
Status:    Beta<br/>
Date:      August 8, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 246560
#### Changes
- Added AI generated status web page
- Changed display format for dual time clock mode
#### Fixes
- more understandable names for the rgb mode enums
---
### Version: 0.2.1
Status:    Beta<br/>
Date:      August 8, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 246608
#### Fixes
- Fixed crash when stopping the web server
---
### Version: 0.2.0
Status:    Beta<br/>
Date:      August 6, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 246588
#### Changes
- Added AI generated configuration web page
#### Fixes
- Fixed typo in Settings.hpp
---
### Version: 0.1.11
Status:    Beta<br/>
Date:      August 6, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 246628
#### Comments
- Last AI-free version
#### Changes
- CALC_TYPE definition in Config.h has been replaced with RPN_MODE
- WEBSOCKET_SUPPORT definition in Config.h has been replaced with WEB_SUPPORT
#### Fixes
- Fixed duplicate settings name
---
### Version: 0.1.10
Status:    Beta<br/>
Date:      July 27, 2026<br/>
ESP-IDF:   v5.5.5<br/>
Arduino:   v3.3.11<br>
Free Heap: 246628
#### Changes
- Added new setting for blinking behavior during time, timer and menu value input
---
### Version: 0.1.9
Status:    Beta<br/>
Date:      July 26, 2026<br/>
ESP-IDF:   v5.5.4<br/>
Arduino:   v3.3.10<br>
Free Heap: 245428
#### Changes
- Added new setting for the calculator input direction. With this setting you can define
  if the input direction is from right to left, left to right or left to right
  with leading zeros. This setting also affects the output alignment.
- Added shortcut [F]+[9] in calculator mode to temporarily change the input direction
- [F] + [STO] (RPN) / [F] + [MS] (ALG) in calculator mode now also stores the input direction
---
### Version: 0.1.8
Status:    Beta<br/>
Date:      July 20, 2026<br/>
ESP-IDF:   v5.5.4<br/>
Arduino:   v3.3.10<br>
Free Heap: 245408
#### Changes
- Added shortcut [F]+[.] in clock mode to display the board temperature
---
### Version: 0.1.7
Status:    Beta<br/>
Date:      July 19, 2026<br/>
ESP-IDF:   v5.5.4<br/>
Arduino:   v3.3.10<br>
Free Heap: 245408
#### Changes
- Added board temperature monitoring
---
### Version: 0.1.6
Status:    Beta<br/>
Date:      April 25, 2026<br/>
ESP-IDF:   v5.5.4<br/>
Arduino:   v3.3.8<br>
Free Heap: 245888
#### Changes
- Added dual clock mode
- Added new settings for the second timezone
- Added new setting for the second timezone color
- Added new setting for the time separator mode
---
### Version: 0.1.5
Status:    Beta<br/>
Date:      April 13, 2026<br/>
ESP-IDF:   v5.5.4<br/>
Arduino:   v3.3.8<br>
Free Heap: 246968
#### Changes
- Added shortcut to display uptime
- Added shortcut to trim the X register to the displayed value
- Changed some existing shortcuts (see Table of Shortcuts in the manual)
---
### Version: 0.1.4
Status:    Beta<br/>
Date:      February 3, 2026<br/>
ESP-IDF:   v5.5.2<br/>
Arduino:   v3.3.6<br>
Free Heap: 248092
#### Fixes
- fixed an issue introduced with a recent change in the original ratpak that could lead to incorrect results for lograt 
---
### Version: 0.1.3
Status:    Beta<br/>
Date:      January 24, 2026<br/>
ESP-IDF:   v5.5.2<br/>
Arduino:   v3.3.6<br>
Free Heap: 246564
#### Changes
- added yyddmm date format
---
### Version: 0.1.2
Status:  Beta<br/>
Date:    December 26, 2025<br/>
ESP-IDF: v5.5.1<br/>
Arduino: v3.3.3
#### Changes
- added a fix for the √(2.25) - 1.5 rounding issue that was recently merged into the original ratpak code 
- removed my fix for this issue 
---
### Version: 0.1.1
Status:  Beta<br/>
Date:    August 16, 2025<br/>
ESP-IDF: v5.5.1<br/>
Arduino: v3.3.2
#### Changes
- moved access point definitions for SSID and password to Config.h 
- added [F]+[CLS] (RPN) / [F]+[AC] (ALG) shortcut to leave menu mode without storing the changes
---
### Version: 0.1.0
Status:  Beta<br/>
Date:    August 5, 2025
#### Changes
- added INT operation
- added [F]+[00] shortcut in calculator mode to show firmware versions
---
### Version: 0.0.32
Status:  Alpha<br/>
Date:    July 28, 2025
#### Changes
- Updated to arduino-esp32 v3.3.0 / ESP-IDF v5.5.0
- added support for B-5870 nixie display
---
### Version: 0.0.31
Status:  Alpha<br/>
Date:    July 22, 2025
#### Changes
- added support for IN-12 nixie display
---
### Version: 0.0.30
Status:  Alpha<br/>
Date:    July 9, 2025
#### Changes
- added support for IN-16 nixie display
---
### Version: 0.0.29
Status:  Alpha<br/>
Date:    June 26, 2025
#### Changes
- added support for 7-seg LED display
---
### Version: 0.0.28
Status:  Alpha<br/>
Date:    June 22, 2025
#### Changes
- added support for algebraic mode
---
### Version: 0.0.27
Status:  Alpha<br/>
Date:    June 21, 2025
#### Changes
- preparation for algebraic mode support
---
### Version: 0.0.26
Status:  Alpha<br/>
Date:    June 14, 2025
#### Comments
- first published version of the revised firmware
#### Known issues
- RPN support only
- IN-17 display support only
---

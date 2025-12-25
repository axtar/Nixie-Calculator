## Nixie calculator controller firmware
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
- first published version
#### Known issues
- RPN support only
- IN-17 display support only
---

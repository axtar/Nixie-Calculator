## How to build the controller firmware
### With VS Code and ESP-IDF extension
- Configure ESP-IDF extension<br/>
Click "Configure ESP-IDF extension" and select EXPRESS<br/>
Set download server to github<br/>
Select the ESP-IDF version used for the latest firmware version (see [Release Notes](RELEASE_NOTES.md)) <br/>
Click install
- Download the calculator repository and unzip it into your project directory
- Follow [Dependencies](README_Dependencies.md) to add libraries
- Open the Software/Firmware/Controller folder in VS Code
- In include/config.h select your display type, enable or disable WebSocket support and define the calculator type
- Build project

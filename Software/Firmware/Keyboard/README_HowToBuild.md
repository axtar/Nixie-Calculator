## How to build the keyboard firmware
### With VS Code and PlatformIO extension:
- Download the calculator repository and unzip it into your project directory
- Open the Software/Firmware/Keyboard folder in VS Code
- Build the project
### With Arduino IDE:
- Download the calculator repository and unzip it into your project directory
- Create a new sketch
- In the editor, delete the content of the created sketch
- Paste the content of Software/Firmware/Keyboard/src/main.cpp to the sketch
- Add the "Keypad" library using the library manager or from https://github.com/Chris--A/Keypad
- Select the "Arduino Uno" board
- Verify/Compile the sketch

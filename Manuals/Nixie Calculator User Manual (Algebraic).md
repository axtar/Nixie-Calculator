# Nixie Calculator User Manual (Algebraic Version)

*Draft (generated from pdf) - Version 0.9 — September 4, 2026*

## Contents

- [General information](#general-information)
- [Power supply](#power-supply)
- [Power on / Power off](#power-on--power-off)
- [The \[F\] key](#the-f-key)
- [The \[↑\] key](#the--key)
- [Calculator mode](#calculator-mode)
- [Clock mode](#clock-mode)
  - [Entering date and time](#entering-date-and-time)
  - [Clock modes](#clock-modes)
  - [Timer](#timer)
  - [Stopwatch](#stopwatch)
- [Menu mode](#menu-mode)
  - [Navigation](#navigation)
  - [Menu table](#menu-table)
- [Web server](#web-server)
- [Peripherals module](#peripherals-module)
- [Reset SSID and passwords](#reset-ssid-and-passwords)
- ["Factory" reset](#factory-reset)
- [OTA firmware update](#ota-firmware-update)
- [Troubleshooting](#troubleshooting)
- [Table of shortcuts](#table-of-shortcuts)
- [List of error codes](#list-of-error-codes)
- [List of operations](#list-of-operations)

---

## General information

Apart from the power supply, the nixie calculator works completely independently and requires neither a network connection nor any peripheral devices.

## Power supply

The calculator needs a DC 12V/2A regulated power supply with a center positive barrel jack plug (5.5/2.1mm compatible).

## Power on / Power off

Use the power switch to the left of the keyboard to power on and off the calculator. During start-up, the calculator briefly shows the controller firmware version on the left and the keyboard firmware version on the right. By default, the calculator starts in calculator mode.

## The [F] key

The `[F]` key has several functions:

- Press and release the `[F]` key to switch between the calculator and the clock mode and to leave the menu mode.
- Hold the `[F]` key for 3 seconds to enter the menu mode.
- Press the `[F]` key + an operator key to access the lower functions in calculator mode, for example x².
- Press the `[F]` key + some defined keys to get a shortcut for some settings, for example LED lighting (see [Table of shortcuts](#table-of-shortcuts)).

## The [↑] key

Press the `[↑]` key + an operator key to access the upper functions in calculator mode, for example n!

## Calculator mode

In this mode the device works like a normal calculator (see [List of operations](#list-of-operations)). However, be aware that the arbitrary-precision arithmetic used by the calculator is experimental.

The precision of the calculations and of the internal registers has been set to 32 as a compromise between accuracy and performance. Some calculations with very big numbers can be slow. Exponents can be entered and displayed between -9999 and 9999.

If an error occurs (e.g. overflow, domain, divide by zero), an error code is shown in the center of the display (see [List of error codes](#list-of-error-codes)).

## Clock mode

### Entering date and time

In clock modes (0-8) press the `[AC]` key. A blinking zero indicates that you can enter the date and the time in the `YYYYMMDDhhmmss` format. The `[C]` key deletes the last entered digit. Press `[=]` to confirm or `[AC]` to abort.

### Clock modes

There are several clock modes. They can be reached directly with the `[0]` to `[9]` keys and the `[00]` key for the stopwatch mode:

**Clock modes**

| Mode | Description |
|------|-------------|
| 0 | time only |
| 1 | time with no seconds |
| 2 | moving time |
| 3 | time or date |
| 4 | time and date |
| 5 | time and temperature |
| 6 | time and date and temperature |
| 7 | raw date and time |
| 8 | dual time |

**Special clock modes**

| Mode | Description |
|------|-------------|
| 9 | timer |
| 10 | stopwatch |

### Timer

In timer mode press the `[AC]` key. A blinking zero indicates that you can enter the number of days, hours, minutes, and seconds in the format `DDhhmmss`. The `[C]` key deletes the last entered digit. Press `[=]` to confirm or `[AC]` to abort. Press `[=]` to start and stop the timer. Press `[C]` to reset the timer. Blinking LEDs indicate the end of the countdown; there is no sound. The flashing can be stopped by pressing the `[AC]` key.

The accuracy depends on the internal MCU oscillator.

### Stopwatch

In stopwatch mode press the `[=]` key to start the stopwatch. Press `[=]` to pause the display while the stopwatch keeps running. Press `[C]` to reset the stopwatch.

The accuracy depends on the internal MCU oscillator.

## Menu mode

Hold the `[F]` key for 3 seconds to enter the menu mode. The menu ID (see [Menu table](#menu-table)) is displayed on the left, the menu value(s) on the right. Key autorepeat is enabled in menu mode and starts after 1 second. The autorepeat speed increases after some time. Press the `[F]` key to leave the menu mode and store the values.

### Navigation

| Keys | Description |
|------|-------------|
| `[M+]` | Next menu |
| `[M-]` | Previous menu |
| `[+]` | Next value |
| `[-]` | Previous value |
| `[=]` | Accept value and move to the next column if any |
| `[C]` | Restore to previously stored value |
| `[AC]` | Reset to default value |
| `[F]` + `[MC]` | Reset all settings to default |
| `[F]` + `[AC]` | Exit menu mode and discard changes |
| `[F]` | Exit menu mode and commit changes |

### Menu table

| ID | Name | Description | Values |
|----|------|-------------|--------|
| 1 | startupmode | Start in calculator or in clock mode | 0 = calculator (default)<br>1 = clock |
| 2 | showversion | Show version at startup | 0 = off<br>1 = on (default) |
| 3 | autooffmode | Auto off action after a period of no keyboard activity | 0 = auto off disabled<br>1 = shutdown high voltage<br>2 = switch to clock mode (default) |
| 4 | autooffdelay | Delay in minutes for auto off mode | 1 - 720 (default 5) |
| 5 | clockmode | Initial clock mode | 0 = time (default)<br>1 = time, no seconds<br>2 = moving time<br>3 = time or date<br>4 = time and date<br>5 = time and temperature<br>6 = time and date and temperature<br>7 = raw date and time<br>8 = dual time<br>9 = timer<br>10 = stopwatch |
| 6 | hourmode | 12 or 24 hours mode | 0 = 12 hours<br>1 = 24 hours (default) |
| 7 | leadingzero | Show hours leading zero | 0 = off<br>1 = on (default) |
| 8 | timeseparator | Separator mode in compact time format | 0 = off<br>1 = blink (default)<br>2 = on |
| 9 | dateformat | Date format | 0 = ddmmyy (default)<br>1 = yymmdd<br>2 = mmddyy<br>3 = yyddmm |
| 10 | pirmode | Use PIR to reduce the operating time of the nixie tubes | 0 = off (default)<br>1 = on |
| 11 | pirdelay | PIR delay time in minutes before shutting down the high voltage | 1 - 720 (default 5) |
| 12 | gpsmode | Sync with GPS time | 0 = off (default)<br>1 = on |
| 13 | gpsspeed | GPS communication baud rate | 0 = 2400<br>1 = 4800<br>2 = 9600<br>3 = 19200<br>4 = 38400 (default)<br>5 = 57600<br>6 = 115200 |
| 14 | gpssyncinterval | GPS time sync interval in minutes | 1 - 720 (default 10) |
| 15 | temperaturemode | Use temperature sensor | 0 = off (default)<br>1 = on |
| 16 | temperaturecf | Temperature in C or F | 0 = Celsius (default)<br>1 = Fahrenheit |
| 17 | ledmode | LEDs on by time or always * | 0 = time<br>1 = always (default) |
| 18 | calcrgbmode | RGB mode in calculator mode * | 0 = off (default)<br>1 = by content<br>2 = by content (all digits)<br>3 = random (turned on digits only)<br>4 = full random (turned on digits only)<br>5 = fixed color (all digits)<br>6 = random (all digits)<br>7 = full random (all digits) |
| 19 | clockrgbmode | RGB mode in clock mode * | 0 = off (default)<br>1 = by content<br>2 = random (turned on digits only)<br>3 = full random (turned on digits only)<br>4 = fixed color (all digits)<br>5 = random (all digits)<br>6 = full random (all digits) |
| 20 | trigcolorchange | Trigger color change for random RGB modes in clock mode * (this setting does not apply to the "moving time" clock mode as the color changes with every movement) | 0 = off (default)<br>1 = every second<br>2 = every minute<br>3 = every hour |
| 21 | ledstarttime | Start time of LED lighting * | 00:00 - 23:59 (default 00:00) |
| 22 | ledduration | Duration in minutes of LED lighting * | 0 - 720 (default 0) |
| 23 | ledstarttime2 | Start time of LED lighting * | 00:00 - 23:59 (default 00:00) |
| 24 | ledduration2 | Duration in minutes of LED lighting * | 0 - 720 (default 0) |
| 25 | acpstarttime | Start time of cathode poisoning prevention | 00:00 - 23:59 (default 00:00) |
| 26 | acpduration | Duration in minutes of cathode poisoning prevention | 0 - 720 (default 0) |
| 27 | acpforceon | Force turning nixies on during cathode poisoning prevention | 0 = off<br>1 = on (default) |
| 28 | negativecolor | RGB LED color for negative numbers in calculator mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 29 | positivecolor | RGB LED color for positive numbers in calculator mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 30 | errorcolor | RGB LED color for error in calculator mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 31 | negexpcolor | RGB LED color for negative exponents in calculator mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 32 | posexpcolor | RGB LED color for positive exponents in calculator mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 33 | fixedcalccolor | RGB LED fixed color in calculator mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 34 | timecolor | RGB LED color for time in clock mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 35 | timecolor2 | RGB LED color for dual time in clock mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 36 | datecolor | RGB LED color for date in clock mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 37 | tempcolor | RGB LED color for temperature in clock mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 38 | fixedcolor | RGB LED fixed color in clock mode * | 0-255,0-255,0-255 (default 0,0,0) |
| 39 | dstweek | Daylight saving time change, week of month | 1 = first<br>2 = second<br>3 = third<br>4 = fourth<br>5 = last (default) |
| 40 | dstdow | Daylight saving time change, day of week | 0 = sunday (default) - 6 = saturday |
| 41 | dstmonth | Daylight saving time change, month | 0 = jan - 11 = dec (default 2 = mar) |
| 42 | dsthour | Daylight saving time change, hour | 0 - 23 (default 2) |
| 43 | dstoffset | Daylight saving time change, offset to UTC in minutes | -720 - 840 (default 120) |
| 44 | stdweek | Standard time change, week of month | 1 = first<br>2 = second<br>3 = third<br>4 = fourth<br>5 = last (default) |
| 45 | stddow | Standard time change, day of week | 0 = sunday (default) - 6 = saturday |
| 46 | stdmonth | Standard time change, month | 0 = jan - 11 = dec (default 9 = oct) |
| 47 | stdhour | Standard time change, hour | 0 - 23 (default 3) |
| 48 | stdoffset | Standard time change, offset to UTC in minutes | -720 - 840 (default 60) |
| 49 | dstweek2 | Dual time daylight saving time change, week of month | 1 = first<br>2 = second<br>3 = third<br>4 = fourth<br>5 = last (default) |
| 50 | dstdow2 | Dual time daylight saving time change, day of week | 0 = sunday (default) - 6 = saturday |
| 51 | dstmonth2 | Dual time daylight saving time change, month | 0 = jan - 11 = dec (default 2 = mar) |
| 52 | dsthour2 | Dual time daylight saving time change, hour | 0 - 23 (default 2) |
| 53 | dstoffset2 | Dual time daylight saving time change, offset to UTC in minutes | -720 - 840 (default 0) |
| 54 | stdweek2 | Dual time standard time change, week of month | 1 = first<br>2 = second<br>3 = third<br>4 = fourth<br>5 = last (default) |
| 55 | stddow2 | Dual time standard time change, day of week | 0 = sunday (default) - 6 = saturday |
| 56 | stdmonth2 | Dual time standard time change, month | 0 = jan - 11 = dec (default 9 = oct) |
| 57 | stdhour2 | Dual time standard time change, hour | 0 - 23 (default 3) |
| 58 | stdoffset2 | Dual time standard time change, offset to UTC in minutes | -720 - 840 (default 0) |
| 59 | gpsnotifysync | Notify GPS time sync with a short LED flash * | 0 = off (default)<br>1 = on |
| 60 | gpssynccolor | RGB LED color for notifying GPS time sync * | 0-255,0-255,0-255 (default 255,0,0) |
| 61 | notifytimer | Notify end of timer with flashing LEDs * | 0 = off<br>1 = on (default) |
| 62 | timercolor | RGB LED color for notifying end of timer * | 0-255,0-255,0-255 (default 255,255,255) |
| 63 | fixeddecimals | Number of fixed decimals | 0 = floating (default)<br>1 - 8 = number of fixed decimals |
| 64 | anglemode | Startup angle mode | 0 = degrees (default)<br>1 = radians |
| 65 | showbusycalc | Show animation during long calculations | 0 = off<br>1 = moving decimal point (default)<br>2 = digit flickering |
| 66 | maxexplength | Max length of the exponent | 2 - 4 (default 4) |
| 67 | scrolldelay | Interval while scrolling result in 1/10 seconds | 1 - 20 (default 5) |
| 68 | precision | Calculator precision (restart needed) | 20 - 32 (default 32) |
| 69 | calcinputdirec | Calculator input direction and output format | 0 = left to right (default)<br>1 = right to left<br>2 = right to left with zero padding |
| 70 | inputblinking | Blinking behavior during time, timer and menu value input | 0 = off<br>1 = on (default) |
| 71 | brightness | Display brightness: 15 disables PWM dimming on nixie displays ** | 1 - 15 (default 15) |
| 72 | dimbrightness | Display brightness during the dimming period | 1 - 15 (default 1) |
| 73 | dimstarttime | Start time of display dimming | 00:00 - 23:59 (default 00:00) |
| 74 | dimduration | Duration in minutes of display dimming | 0 - 720 (default 0) |
| 75 | apautostart | Enable the access point and the web server at startup *** | 0 = off (default)<br>1 = on |
| 76 | rtcdriftcorr | RTC drift compensation in seconds per month | -60 - 60 (default 0) |
| 77 | exttempcorr | External temperature sensor correction in 0.1 °C | -100 - 100 (default 0) |

`(*)` not available for 7-segment LED version
`(**)` not available for IN-12A and IN-12B versions
`(***)` only available if `WEB_SUPPORT` is set to `true`

## Web server

If enabled in the firmware, press `[F]` + `[=]` in calculator mode to start an ESP32 access point (AP) and a web server. After starting, the IP address of the server is displayed for 2 seconds and the network activity LED lights up. By default, the IP address is `192.168.4.1`. You can now connect with a smartphone or a PC to this AP and open the site (`http://192.168.4.1`) with a browser.

- **[Calculator]** shows a fully functional keypad and mirrors the display. This page is WebSocket based and there is no polling or browser refresh needed.

  There are 4 buttons:
  - **[Clock/Calculator/Exit Menu]** toggles the device mode or exits the menu mode
  - **[Menu]** enters the menu mode
  - **[Show/Hide Registers]** shows/hides all the stack and memory registers with all the digits. The X register holds the current result.
  - **[Back]** back to homepage

- **[Configuration]** provides full access to the configuration of the calculator.
- **[Status]** displays some status information.
- **[Time Sync]** provides synchronization of the calculator time with the browser time.
- **[SSID & Passwords]** change the SSID, the AP password and the firmware update password
- **[Firmware Update]** OTA firmware update

> **Note:** Be aware that the browser connection is not encrypted and the clear text password and the SSID of the AP are stored in flash memory and are also visible in the source code. While connected to this AP, you may have no Internet connection. Press `[F]` + `[=]` again to stop the server and the AP.

## Peripherals module

If you have assembled the peripherals module you can connect it to the calculator with a straight ethernet patch cable. Turn off the calculator while connecting or disconnecting the ethernet cable.

The default communication speed for the BE-220 GPS module is 38400 and 9600 for the older BN-220 module.

## Reset SSID and passwords

You can reset the SSID, the AP password and the firmware update password to the initial values (defined in the source code) by pressing `[F]` + `[←]` in menu mode. This only works if using the physical keyboard.

## "Factory" reset

Press and hold the button on the back for about 4 seconds during the startup. If the reset is successful, all decimal places will flash 5 times. No restart is required after the reset.

## OTA firmware update

If enabled in the firmware configuration, the firmware can be updated "over-the-air" using the web interface:

1. Open the web interface in your browser as described in the [Web server](#web-server) section.
2. Press `[F]` + `[±]` to enable OTA firmware update for a few minutes. During this time, the net activity LED will blink.
3. Go to the firmware update page.
4. Enter the firmware update password.
5. Select the firmware file.
6. Select **[Upload]**.

After the update, the calculator restarts automatically.

## Troubleshooting

If the calculator loses the time, please change the CR2032 battery.

## Table of shortcuts

| Keys | Description | Mode |
|------|-------------|------|
| `[F]` + `[C]` | Switch LED lighting mode, overrides the time constraints (*) | Calculator, Clock |
| `[F]` + `[AC]` | Restore the lighting time constraints | Calculator, Clock |
| `[F]` + `[MS]` | Commit temporarily changed settings | Calculator, Clock |
| `[F]` + `[=]` | Start/stop Wi-Fi access point and web server | Calculator, Clock |
| `[F]` + `[±]` | Enable/disable OTA firmware update | Calculator, Clock |
| `[F]` + `[0]` – `[8]` | Change the number of fixed decimals, 0 = floating (*) | Calculator |
| `[F]` + `[9]` | Change the input direction and output format (*) | Calculator |
| `[F]` + `[EXP]` | Force scientific notation on or off (*) | Calculator |
| `[F]` + `[.]` | Start/stop scrolling additional result digits | Calculator |
| `[F]` + `[+]` | Increase brightness (*) | Calculator |
| `[F]` + `[-]` | Decrease brightness (*) | Calculator |
| `[F]` + `[00]` | Trim X register to displayed value | Calculator |
| `[0]` - `[9]`, `[00]` | Switch the clock mode (*) | Clock |
| `[F]` + `[+]` | Adjust the time by plus one second | Clock |
| `[F]` + `[-]` | Adjust the time by minus one second | Clock |
| `[F]` + `[0]` | Display free heap memory and minimum free heap memory | Clock |
| `[F]` + `[00]` | Display firmware versions | Clock |
| `[F]` + `[.]` | Display board temperature | Clock |
| `[F]` + `[M-]` | Display uptime in format `ddddd hh mm` | Clock |
| `[F]` + `[MC]` | Reset all settings to default | Menu |
| `[F]` + `[AC]` | Exit menu mode and discard changes | Menu |
| `[F]` + `[C]` | Reset Wi-Fi access point password | Menu |

`(*)` temporarily, changed settings are not committed

## List of error codes

| Error Code | Description |
|------------|-------------|
| 1 | Overflow |
| 2 | Divide by zero |
| 3 | Domain error / Invalid input |
| 4 | Out of memory |
| 5 | Indefinite result |
| 6 | Invalid range |
| 7 | No result |
| 8 | Unknown operation |
| 9 | Unknown error |

## List of operations

| Operation | Description |
|-----------|-------------|
| EXP | Enter exponent |
| C | Clear last entered digit / clear display if not in input mode |
| AC | All clear |
| ↑ | Shift key |
| F | Function key |
| x^y | Power |
| x² | Square |
| ʸ√x | Root |
| x³ | Cube |
| 1/x | Reciprocal |
| n! | Factorial |
| eˣ | Exponential |
| ln | Natural logarithm |
| e | e constant |
| mod | Modulo |
| logy | Logarithm base y |
| Py,x | Permutations |
| int | Integer portion |
| sin | Sine |
| sin⁻¹ | Arcsine |
| sinh | Hyperbolic sine |
| cos | Cosine |
| cos⁻¹ | Arccosine |
| cosh | Hyperbolic cosine |
| tan | Tangent |
| tan⁻¹ | Arctangent |
| tanh | Hyperbolic tangent |
| log | Logarithm base 10 |
| π | Pi |
| rnd | Pseudorandom number |
| d↔r | Switch between degrees and radians |
| Cy,x | Combinations |
| Δ% | Percent difference |
| ± | Change sign |
| √ | Square root |
| % | Percent |
| ÷ | Division |
| × | Multiplication |
| - | Subtraction |
| + | Addition |
| MC | Clear memory |
| MR | Read memory |
| MS | Store memory |
| M+ | Add to memory |
| M- | Subtract from memory |

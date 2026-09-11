## How to build the controller firmware
### With VS Code and ESP-IDF extension
- Install the ESP-IDF version used for the latest firmware version (see [Release Notes](RELEASE_NOTES.md)) with the ESP-IDF Installation Manager<br/>
- Download the calculator repository and unzip it into your project directory
- Open the Firmware/Controller folder in VS Code
- If you trust the contents of the folder, set the folder to trusted
- In include/Config.h:
  - select your display type
  - define the calculator mode (RPN or algebraic)
  - enable or disable web server support
  - set the AP SSID and password
  - enable or disable OTA firmware update
  - set the firmware update password

``` C++
// -------------------------------------------------------------------------------
// Display type
// -------------------------------------------------------------------------------
// display_type::in12a -> IN-12A or IN-12B nixie display using neon decimal points
// display_type::in12b -> IN-12B nixie display using nixie decimal points
// display_type::in16  -> IN-16 nixie display
// display_type::in17  -> IN-17 nixie display
// display_type::b5870 -> B5870 nixie display
// display_type::led   -> 7-segment LED display
// -------------------------------------------------------------------------------
constexpr auto DISPLAY_TYPE = display_type::undefined;

// -------------------------------------------------------------------------------
// RPN mode
// -------------------------------------------------------------------------------
// false -> algebraic
// true  -> reverse polish notation
// -------------------------------------------------------------------------------
#define RPN_MODE true

// -------------------------------------------------------------------------------
// Web server support for the calculator web interface
// -------------------------------------------------------------------------------
// false -> disable
// true -> enable
// -------------------------------------------------------------------------------
#define WEB_SUPPORT true

// -------------------------------------------------------------------------------
// Calculator access point SSID and password
// -------------------------------------------------------------------------------
// Please note that this applies to the AP provided by the calculator.
// The calculator itself does not connect to any AP.
// -------------------------------------------------------------------------------
constexpr auto AP_SSID = "NixieCALC";
constexpr auto AP_PWD = "NIXIESareGreat!";

// -------------------------------------------------------------------------------
// OTA (over-the-air) firmware update support
// -------------------------------------------------------------------------------
// false -> disable
// true  -> enable (requires WEB_SUPPORT)
// -------------------------------------------------------------------------------
#define OTA_SUPPORT true

// -------------------------------------------------------------------------------
// Firmware update (OTA) password
// -------------------------------------------------------------------------------
// Separate pasword to flash new firmware using the web interface
// -------------------------------------------------------------------------------
constexpr auto OTA_PWD = "FlashMeOverTheAir!";

```

- Build the project

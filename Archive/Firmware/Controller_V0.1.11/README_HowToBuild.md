## How to build the controller firmware
### With VS Code and ESP-IDF extension
- Install the ESP-IDF version used for the latest firmware version (see [Release Notes](RELEASE_NOTES.md)) with the ESP-IDF Installation Manager<br/>
- Download the calculator repository and unzip it into your project directory
- Follow [Dependencies](README_Dependencies.md) to add libraries
- Open the Software/Firmware/Controller folder in VS Code
- In include/Config.h select your display type, enable or disable web server support, define the calculator mode (RPN or algebraic) and set the AP SSID and password

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
// Web server support for register view
// -------------------------------------------------------------------------------
// false -> disable
// true -> enable
// -------------------------------------------------------------------------------
#define WEB_SUPPORT true

// -------------------------------------------------------------------------------
// RPN mode
// -------------------------------------------------------------------------------
// false -> algebraic
// true  -> reverse polish notation
// -------------------------------------------------------------------------------
#define RPN_MODE true

// -------------------------------------------------------------------------------
// Calculator access point SSID and password
// -------------------------------------------------------------------------------
// Please note that this applies to the AP provided by the calculator.
// The calculator itself does not connect to any AP.
// -------------------------------------------------------------------------------
constexpr auto AP_SSID = "NixieCALC";
constexpr auto AP_PWD = "NIXIESareGreat!";

```

- Build project

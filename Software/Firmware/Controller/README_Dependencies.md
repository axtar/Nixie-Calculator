The Controller firmware depends on libraries that are not available in the ESP-IDF component registry. These libraries must be installed manually. 
**Do not delete the CMakeLists.txt files in the target directories.**

**Adafruit_NeoPixel Library** <br/>
Download from: https://github.com/adafruit/Adafruit_NeoPixel<br/>
Version: 1.15.2<br/>
Copy the following files to components/Adrafruit_Neopixel:
- Adafruit_NeoPixel.cpp
- Adafruit_NeoPixel.h
- Adafruit_Neopixel_RP2.cpp
- esp.c
- esp8266.c
- psoc6.c
- rp2040_pio.h

**DS18B20 Library** <br/>
Download from: https://github.com/RobTillaart/DS18B20_RT<br/>
Version: 0.2.4<br/>
Copy the following files to components/DS18B20:
- DS18B20.h
- DS18B20.cpp

**DS3232RTC Library** <br/>
Download from https://github.com/JChristensen/DS3232RTC<br/>
Version: 3.1.2<br/>
Copy the following files to components/DS3232RTC:
- DS3232RTC.h
- DS3232RTC.cpp
- GenericRTC.h

**OneWire Library** <br/>
Download from https://github.com/PaulStoffregen/OneWire<br/>
Version: 2.3.8<br/>
Copy the following files to components/OneWire:
- OneWire.h
- OneWire.cpp
  
Create components/OneWire/util directory<br/>
Copy the following files to components/OneWire/util
- OneWire_direct_gpio.h
- OneWire_direct_regtype.h

**Time library** <br/>
Download from https://github.com/PaulStoffregen/Time<br/>
Version: 1.6.1<br/>
Copy the following files to components/Time:
- TimeLib.h
- Time.cpp

**Timezone library** <br/>
Download from https://github.com/JChristensen/Timezone<br/>
Version: 1.2.6<br/>
Copy the following files to components/Timezone:
- Timezone.h"
- Timezone.cpp"

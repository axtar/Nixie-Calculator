idf_component_register(SRCS 
"main.cpp"


INCLUDE_DIRS "." "../include" 
 "../components/DS18B20"
 "../components/DS3232RTC"
 "../components/OneWire"
 "../components/Time"
 "../components/Timezone"
 "../components/Adafruit_NeoPixel"
 "../components/ratpak"


REQUIRES arduino-esp32
REQUIRES nvs_flash
REQUIRES ratpak
REQUIRES Adafruit_NeoPixel
REQUIRES DS18B20
REQUIRES DS3232RTC
REQUIRES OneWire
REQUIRES Time
REQUIRES Timezone
)

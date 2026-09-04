idf_component_register(SRCS 
"main.cpp"


INCLUDE_DIRS "." "../include"
 "../components/ratpak"


REQUIRES arduino-esp32
REQUIRES nvs_flash
REQUIRES ratpak
REQUIRES led_strip
REQUIRES onewire_bus
REQUIRES ds18b20
)

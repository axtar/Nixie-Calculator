idf_component_register(SRCS
    "Adafruit_NeoPixel_RP2.cpp"
    "Adafruit_NeoPixel.cpp"
    "esp.c"
    "esp8266.c"
    "kendyte_k210.c"
    "psoc6.c"
    
    INCLUDE_DIRS "."

    REQUIRES arduino-esp32
    
    )
    target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=parentheses)
    

// main.cpp

// Nixie Calculator Firmware

// Copyright (C) 2020-2026 highvoltglow
// Licensed under the MIT License

#include <main.h>

// setup
void setup()
{
  // init serial print for debugging
  __serial_begin(115200);

  // init controller
  int err = controller.begin();
  if (err != ERR_SUCCESS)
  {
    __serial_println("Failed to initialize controller. Error: " +
                     String(err) + " " + Errors::getErrorText(err));
    while (true)
      ;
  }
  __serial_println("Controller initialized successfully");

  // set time sync function
  setSyncProvider([]()
                  { return controller.getClock()->getRTC()->get(); });
}

// main loop
void loop()
{
  controller.process();
  vTaskDelay(5 / portTICK_PERIOD_MS); // keep watchdog happy
}

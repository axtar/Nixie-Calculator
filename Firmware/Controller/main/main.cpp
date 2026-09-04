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
    {
      // yield so the idle task can run and the watchdog doesn't reboot us
      delay(1000);
    }
  }
  __serial_println("Controller initialized successfully");
}

// main loop
void loop()
{
  controller.process();
  delay(5); // keep watchdog happy
}

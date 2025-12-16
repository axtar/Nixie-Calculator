// main.cpp

// Nixie Calculator Firmware

// Copyright (C) 2020-2025 highvoltglow
// Licensed under the MIT License

//- INCLUDES
#include <main.h>

// setup
void setup()
{
  // init Serial for debugging
  D_Begin(115200);

  // init controller
  int err = controller.begin();
  if (err != ERR_SUCCESS)
  {
    D_println("Failed to initialize controller. Error: " +
              String(err) + " " + Errors::getErrorText(err));
    while (true)
      ;
  }
  D_println("Controller initialized successfully");

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

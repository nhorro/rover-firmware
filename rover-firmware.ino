#include <Arduino.h>
#include "application.h"

// this function has to be present, otherwise watchdog won't work
void watchdogSetup(void) {}

application app;

void setup() 
{ 
 	watchdogEnable(5000); // Watchdog 2 segundos. FIXME pasar a Config
  	app.setup();
}

void loop() 
{
  	watchdogReset();
	app.loop();	
}

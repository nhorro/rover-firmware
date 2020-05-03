#include <Arduino.h>
#include "application.h"

application app;

void setup() 
{
	app.setup();
}

void loop() 
{
	app.loop();	
}

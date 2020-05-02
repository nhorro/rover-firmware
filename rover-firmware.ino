#include <Arduino.h>
#include "application.h"

application app;


void setup() 
{
	Serial.begin(115200);
	pinMode(LED_BUILTIN, OUTPUT);
	app.setup();
}

void loop() 
{
	if(int n= Serial.available())
	{
		do {
			app.feed(Serial.read());
		} while(--n);
	}
	app.loop();	
}

#pragma once

#include "constants/config.h"

class InsteonApi
{
public:
	static void SetOn()
	{
		PerformAction(TURN_ON_URL);
	}

	static void SetOff()
	{
		PerformAction(TURN_OFF_URL);
	}

private:
	static void PerformAction(const String& URL)
	{
		HTTPClient http;

		http.begin(URL);
		http.addHeader("Content-Type", "text/plain");

		int httpCode = http.POST("Message from ESP8266");
		String payload = http.getString();

		Serial.println(httpCode);
		Serial.println(payload);

		http.end();
	}
};
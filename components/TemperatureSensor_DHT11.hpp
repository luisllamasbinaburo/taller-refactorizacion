#pragma once

#include "../bases/ITemperatureSensor.hpp"
#include <MeanFilterLib.h>


class TemperatureSensorComponent_DHT11: public ISensor
{
public:
	TemperatureSensorComponent_DHT11() : meanFilter(10)
	{
	}
	
	MeanFilter<float> meanFilter;

	DHTesp dht;

	void Init()
	{
		dht.setup(DHT_PIN, DHTesp::DHT11);
	}

	float Read()
	{
		auto rawTemperature = dht.getTemperature();
		auto humidity = dht.getHumidity();  //TODO: Esto no lo usa para nada
		auto temperature = meanFilter.AddValue(rawTemperature);

		Serial.print("\tAVG:\t");
		Serial.println(temperature, 0);

		return temperature;
	}
};
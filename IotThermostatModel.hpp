#pragma once

#include "constants/config.h"
#include "constants/pinout.h"
#include "constants/enums.hpp"

#include "bases/CommandBase.hpp"
#include "bases/ITemperatureSensor.hpp"
#include "bases/IDisplay.hpp"

#include "components/Led.hpp"
#include "components/Encoder.hpp"
#include "components/Button.hpp"
#include "components/Display_I2C_Lcd.hpp"
#include "components/TemperatureSensor_DHT11.hpp"
#include "components/ThresholdController.hpp"

#include "services/UserInputService.hpp"
#include "services/ComunicationService.hpp"

#include "InsteonApi.h"

class IotThermostatModel
{
public:
	IotThermostatModel() : communicationService(303), led(LED_PIN)
	{
	}

	void Init(IDisplay& idisplay, ITemperatureSensor& itempSensor)
	{
		display = &idisplay;
		temperatureSensor = &itempSensor;

		display->Init();
		temperatureSensor->Init();
		encoder.Init();
		button.Init();

		communicationService.Start();
	}

	float GetTemperature()
	{
		ReadTemperature();
		return currentTemperature;
	}

	void SetSetpoint(float value)
	{
		controller.SetPoint = value;
		encoder.SetCounter(value);
	}

	float GetSetPoint()
	{
		return controller.SetPoint;
	}

	void Run()
	{
		// TODO: habr�a que pensarse el orden de las acciones y la prioridad de lo input / comunicacion
		auto userInputResult = inputService.HandleUserInput(encoder, button);
		ProcessCommand(userInputResult);

		ReadTemperature();

		UpdateControllerStatus();

		auto communicationResult = communicationService.HandleCommunications(currentTemperature, GetSetPoint());
		ProcessCommand(communicationResult);

		display->Show(currentTemperature, GetSetPoint());
	}


private:
	void ReadTemperature()
	{
		currentTemperature = temperatureSensor->Read();
	}

	void UpdateControllerStatus()
	{
		auto isControllerTurnedOn = controller.Update(currentTemperature);
		auto newStatus = isControllerTurnedOn ? THERMOSTAT_STATUS::ON : THERMOSTAT_STATUS::OFF;

		if(status != newStatus)
		{
			PerformStatusChange(newStatus);
			status = newStatus;
		}
	}

	void PerformStatusChange(THERMOSTAT_STATUS status)
	{
		switch(status)
		{
		case OFF:
			InsteonApi::SetOff();
			led.TurnOff();
			break;

		case ON:
			InsteonApi::SetOn();
			led.TurnOn();
			break;

		default:
			break;
		}
	}

	void ProcessCommand(CommandBase command)
	{
		if(command.Result == COMMAND_RESULT::VALID);
		{
			ProcessOperation(command.Operation);
			Serial.println(controller.SetPoint);
		}
	}

	void ProcessOperation(COMMAND_OPERATION operation)
	{
		switch(operation)
		{
		case SET_DEFAULT:
			SetSetpoint(DEFAULT_TEMPERATURE);
			break;

		case SET_SETPOINT:
			SetSetpoint(encoder.GetCounter());
			break;

		case INCREASE:
			encoder.DecreaseCounter();
			Serial.println("You clicked -");
			break;

		case DECREASE:
			encoder.IncreaseCounter();
			Serial.println("You clicked -");

		case TURN_ON:
			SetSetpoint(currentTemperature + TURN_ON_THRESHOLD);
			Serial.println("You clicked Boiler On");
			break;

		case TURN_OFF:
			SetSetpoint(DEFAULT_TEMPERATURE);
			Serial.println("You clicked Boiler Off");
			break;

		default:
			break;
		}
	}

	THERMOSTAT_STATUS status = THERMOSTAT_STATUS::OFF;

	EncoderComponent encoder;
	ButtonComponent button;
	LedComponent led;

	IDisplay* display;
	ISensor* temperatureSensor;

	ThresholdController controller;

	UserInputService inputService;
	ComunicationService communicationService;

	float currentTemperature;
};
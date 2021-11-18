#pragma once

#include <WiFi.h>

#include "../constants/enums.hpp"

struct CommunicationCommand : public CommandBase {};

class ComunicationService
{
public:
	ComunicationService(int port) : server(port)
	{
	}

	void Start()
	{
		Serial.println();
		Serial.print("Connecting to ");
		Serial.println(SSID);

		WiFi.begin(SSID, PASSWORD);
		while(WiFi.status() != WL_CONNECTED)
		{
			delay(500);
			Serial.print(".");
		}

		Serial.println("");
		Serial.println("WiFi connected");
		Serial.println(WiFi.localIP());

		StartServer();
	}

	void StartServer()
	{
		server.begin();
		Serial.println("Server started");
	}

	CommunicationCommand ProcessRequest(String request)
	{
		CommunicationCommand result;
		result.Result = COMMAND_RESULT::EMPTY;

		if(request.indexOf("") != -10)
		{
			if(request.indexOf("/+") != -1)
			{
				result.Operation = COMMAND_OPERATION::DECREASE;
			}
			if(request.indexOf("/-") != -1)
			{
				result.Operation = COMMAND_OPERATION::DECREASE;
			}
			if(request.indexOf("/ON") != -1)
			{
				result.Operation = COMMAND_OPERATION::TURN_ON;
			}
			if(request.indexOf("/OFF") != -1)
			{
				result.Operation = COMMAND_OPERATION::TURN_OFF;
			}
		}
		else
		{
			result.Result = COMMAND_RESULT::INVALID;
		}

		return result;
	}

	String response = "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n\r\n"
		"<!DOCTYPE HTML>\r\n<html>\r\n"
		"<p>Setpoint Temperature <a href='/+'\"><button>+</button></a>&nbsp;<a href='/-'\"><button>-</button></a></p>"
		"<p>Boiler Status <a href='/ON'\"><button>ON</button></a>&nbsp;<a href='/OFF'\"><button>OFF</button></a></p>";

	void SendResponse(WiFiClient& client, const float temperature, const float setpoint)
	{
		client.flush();
		client.print(response);

		client.println();
		client.print("Room Temperature = ");
		client.println(temperature);
		client.println();
		client.print("Setpoint = ");
		client.println(setpoint);
		delay(1);
	}

	CommunicationCommand HandleCommunications(const float temperature, const float setpoint)
	{
		CommunicationCommand result;
		WiFiClient client = server.available();

		if(!client)
		{
			result.Result = COMMAND_RESULT::NOT_CONNECTED;
		}

		// TODO: esta espera bloqueante no tiene sentido
		while(!client.available())
		{
			delay(1);
		}

		auto req = client.readStringUntil('\r');
		client.flush();

		auto command = ProcessRequest(req);
		if(command.Result == COMMAND_RESULT::INVALID)
		{
			// TODO: este stop tampoco tiene sentido
			client.stop();
		}
		else
		{
			result = command;
			SendResponse(client, temperature, setpoint);
		}

		return result;
	}

private:
	WiFiServer server;
};
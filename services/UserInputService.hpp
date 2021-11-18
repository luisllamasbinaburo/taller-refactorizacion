#pragma once

#include <WiFi.h>

#include "../constants/enums.hpp"

struct UserInputCommand : public CommandBase {};

class UserInputService
{
public:
	UserInputCommand HandleUserInput(EncoderComponent encoder, ButtonComponent button)
	{
		UserInputCommand result;
		result.Result = COMMAND_RESULT::EMPTY;

		CompoundUserInputCommand(result,  HandleUserInput(encoder));
		CompoundUserInputCommand(result,  HandleUserInput(button));
		
		return result;
	}

	UserInputCommand HandleUserInput(EncoderComponent encoder)
	{
		UserInputCommand result;
		result.Result = COMMAND_RESULT::EMPTY;

		if(encoder.HasChanged())
		{
			result.Result = COMMAND_RESULT::VALID;
			result.Operation = COMMAND_OPERATION::SET_SETPOINT;
		}

		return result;
	}

	UserInputCommand HandleUserInput(ButtonComponent button)
	{
		UserInputCommand result;
		result.Result = COMMAND_RESULT::EMPTY;

		if(button.HasBeenPressed())
		{
			result.Result = COMMAND_RESULT::VALID;
			result.Operation = COMMAND_OPERATION::SET_DEFAULT;
		}

		return result;
	}

private:
	UserInputCommand CompoundUserInputCommand(UserInputCommand base, UserInputCommand candidate)
	{
		UserInputCommand compounded = base;

		if(candidate.Result == COMMAND_RESULT::VALID)
		{
			compounded = candidate;
		}
		
		return compounded;
	}
};

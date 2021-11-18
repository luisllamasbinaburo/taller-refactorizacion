#pragma once

class ButtonComponent
{
public:
	void Init()
	{
		pinMode(SWITCH_PIN, INPUT_PULLUP);
		attachInterruptArg(digitalPinToInterrupt(SWITCH_PIN), ButtonComponent::GetISR, this, FALLING);
	}


	bool HasBeenPressed()
	{
		return pressedStatus;
	}

	void Restart()
	{
		pressedStatus = false;
		lastMillis = millis();
	}

	void ISR()
	{
		unsigned long lastMillis;
		if((unsigned long)(millis() - lastMillis) >= DEBOUNCE_MILLIS)
		{
			lastMillis = millis();

			pressedStatus = true;
		}
	}

private:
	volatile bool pressedStatus;

	const unsigned long DEBOUNCE_MILLIS = 100;
	unsigned long lastMillis;

	void static GetISR(void* switchComponent)
	{
		((ButtonComponent*)switchComponent)->ISR();
	}
};
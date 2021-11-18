#pragma once

class EncoderComponent
{
public:
	void Init()
	{
		pinMode(CLOCK_PIN, INPUT);
		pinMode(DATA_PIN, INPUT);

		attachInterruptArg(digitalPinToInterrupt(CLOCK_PIN), EncoderComponent::GetISR, this, FALLING);
	}

	void SetCounter(int value)
	{
		counter = value;
	}

	bool HasChanged()
	{
		return change != 0;
	}

	int GetCounter()
	{
		return counter;
	}

	void IncreaseCounter()
	{
		counter++;
	}

	void DecreaseCounter()
	{
		counter--;
	}
	
	void Restart()
	{
		change = 0;
	}

	void ISR()
	{
		if((unsigned long)(millis() - lastMillis) >= DEBOUNCE_MILLIS)
		{
			lastMillis = millis();

			if(digitalRead(DATA_PIN) == LOW)
			{
				counter++;
			}
			else
			{
				counter--;
			}

			change++;
		}
	}

private:
	volatile int counter;
	volatile int change;

	const unsigned long DEBOUNCE_MILLIS = 100;
	unsigned long lastMillis;
	
	void static GetISR(void* encoderComponent)
	{
		((EncoderComponent*)encoderComponent)->ISR();
	}
};


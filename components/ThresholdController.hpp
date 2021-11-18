
class ThresholdController
{

public:
	float SetPoint = DEFAULT_TEMPERATURE;
	float Threshold = DEFAULT_THRESHOLD;

	void TurnOn()
	{
		isTurnedOn = true;
	}
	void TurnOff()
	{
		isTurnedOn = false;
	}

	bool IsTurnedOn()
	{
		return isTurnedOn;
	}

	bool IsTurnedOff()
	{
		return !isTurnedOn;
	}

	bool Update(float value)
	{
		if(IsTurnedOn() && value >= SetPoint + Threshold)
		{
			TurnOff();
		}

		if(IsTurnedOff() && value <= SetPoint - Threshold)
		{
			TurnOn();
		}
	}

private:
	bool isTurnedOn;
};
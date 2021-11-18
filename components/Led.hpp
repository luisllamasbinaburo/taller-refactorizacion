class LedComponent
{
public:
	uint8_t Led_Pin;

	LedComponent(uint8_t led_Pin)
	{
		Led_Pin = led_Pin;
	}

	void Init()
	{
		pinMode(Led_Pin, OUTPUT);
		TurnOff();
	}

	void TurnOn()
	{
		digitalWrite(Led_Pin, HIGH);
	}

	void TurnOff()
	{
		digitalWrite(Led_Pin, LOW);
	}
};
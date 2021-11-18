#pragma once

#include "../bases/IDisplay.hpp"

class DisplayComponent_I2C_Lcd : public IDisplay
{
public:
	DisplayComponent_I2C_Lcd()
	{
		Lcd = nullptr;
	}

	DisplayComponent_I2C_Lcd(LiquidCrystal_I2C& lcd)
	{
		Lcd = &lcd;
	}

	void Init()
	{
		Lcd->backlight();
		Lcd->clear();

		Lcd->print("Room Temp:");
		Lcd->setCursor(0, 1);
		Lcd->print("Setpoint:");
	}

	void Show(float temp, float target)
	{
		Lcd->setCursor(11, 0);
		Lcd->print(temp, 0);
		Lcd->print(" C ");
		Lcd->setCursor(11, 1);
		Lcd->print(target);
		Lcd->print(" C ");
	}
private:
	LiquidCrystal_I2C* Lcd;
};

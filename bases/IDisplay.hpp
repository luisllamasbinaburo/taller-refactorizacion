#pragma once

class IDisplay
{
public:
	void virtual Init() = 0;

	void virtual Show(float temp, float target) = 0;
};
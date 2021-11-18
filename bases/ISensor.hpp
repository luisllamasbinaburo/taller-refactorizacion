#pragma once

class ISensor
{
public:
	void virtual Init() = 0;

	float virtual Read() = 0;
};
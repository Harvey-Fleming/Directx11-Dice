#include "DiceRollerHelper.h"

void DiceRollerHelper::CalculateAngle()
{
	speedMultiplier = std::lerp(speedMultiplier, 0, (float)(((-cos(3.14 * 0.1)) / 2) + .5));


	//std::ostringstream ss;
	//ss << speedMultiplier;
	//std::string s(ss.str());
	//OutputDebugStringW(StringHelper::StringToWide(s.c_str()).c_str());

	if (speedMultiplier > 1)
	{
		nextAngle[0] = timer.Peek() * speedMultiplier;
		nextAngle[1] = timer.Peek() * speedMultiplier;
		//nextAngle[2] = timer.Peek() * speedMultiplier;

	}
	else
	{

		nextAngle[0] = std::lerp(nextAngle[0] , targetAngle[0], (float)(((-cos(3.14 * 0.1)) / 2) + .5));

		nextAngle[1] = std::lerp(nextAngle[1] , targetAngle[1], (float)(((-cos(3.14 * 0.1)) / 2) + .5));

		//nextAngle[2] = std::lerp(nextAngle[2] , targetAngle[2], (float)(((-cos(3.14 * 0.1)) / 2) + .5));
	}
}

float DiceRollerHelper::GetAngleX()
{
	return nextAngle[0] + 1.5f;
}
float DiceRollerHelper::GetAngleY()
{
	return nextAngle[1];
}
float DiceRollerHelper::GetAngleZ()
{
	return nextAngle[2] + 0.350f;
}

void DiceRollerHelper::ReRoll(float face)
{
	speedMultiplier = maxSpeedMultiplier;

	targetAngle[0] = (face - 1) *  (1.0466f);
	targetAngle[1] = (face - 1) *  (2.093f);
	targetAngle[2] = 0;
}


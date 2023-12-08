#pragma once
#include <string>
#include "ChiliTimer.h"


#include <windows.h>
#include "Graphics/StringHelper.h"

using namespace std;
using angle = float[3];


class DiceRollerHelper
{
public:

	void CalculateAngle();
	float GetAngleX();
	float GetAngleY();
	float GetAngleZ();
	void ReRoll(float face);

	ChiliTimer timer;

private:
	angle nextAngle;
	angle currentAngle;
	angle targetAngle;

	float targetFace;

	float speedMultiplier;
	float maxSpeedMultiplier = 10;
};

#include "DiceRollerHelper.h"

void DiceRollerHelper::CalculateAngle()
{
	speedMultiplier = std::lerp(speedMultiplier, 0, (float)(((-cos(3.14 * 0.2)) / 2) + .5));


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
		nextAngle[2] = std::lerp(nextAngle[2] , targetAngle[2], (float)(((-cos(3.14 * 0.1)) / 2) + .5));	
	}
}

XMMATRIX DiceRollerHelper::getrotMatrix(float xOffset, float yOffset, float zOffset)
{

	XMMATRIX newRotationMatrix = (XMMatrixRotationX(nextAngle[0] + xOffset) * XMMatrixRotationY(nextAngle[1] + yOffset) * XMMatrixRotationZ(nextAngle[2] + zOffset));
	return newRotationMatrix;
}

void DiceRollerHelper::seedRand()
{
	srand(time(NULL));
}

void DiceRollerHelper::ReRoll(int numberoffaces, DiceType diceType, float debugFace)
{

	speedMultiplier = maxSpeedMultiplier;
	targetFace = rand() % numberoffaces + 1;
	oldRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f,0.0f, 0);

	switch (diceType)
	{
	case DiceType::D20:
		if (debugFace == 0)
		{
			targetAngle[0] = d20Faces[targetFace - 1][0] + 1.5f;
			targetAngle[1] = d20Faces[targetFace - 1][1] + 0.35f;
			targetAngle[2] = d20Faces[targetFace - 1][2];
		}
		else
		{
			targetFace = debugFace;
			targetAngle[0] = d20Faces[targetFace - 1][0] + 1.5f;
			targetAngle[1] = d20Faces[targetFace - 1][1] + 0.35f;
			targetAngle[2] = d20Faces[targetFace - 1][2];
		}
		break;
	case DiceType::D12:
		if (debugFace == 0)
		{
			targetAngle[0] = d12Faces[targetFace - 1][0];
			targetAngle[1] = d12Faces[targetFace - 1][1];
			targetAngle[2] = d12Faces[targetFace - 1][2];
		}
		else
		{
			targetFace = debugFace;
			targetAngle[0] = d12Faces[targetFace - 1][0];
			targetAngle[1] = d12Faces[targetFace - 1][1];
			targetAngle[2] = d12Faces[targetFace - 1][2];
		}
		break;
	case DiceType::D10:
		if (debugFace == 0)
		{
			targetAngle[0] = d10Faces[targetFace - 1][0];
			targetAngle[1] = d10Faces[targetFace - 1][1];
			targetAngle[2] = d10Faces[targetFace - 1][2];
		}
		else
		{
			targetFace = debugFace;
			targetAngle[0] = d10Faces[targetFace - 1][0];
			targetAngle[1] = d10Faces[targetFace - 1][1];
			targetAngle[2] = d10Faces[targetFace - 1][2];
		}
		break;
	case DiceType::D8:
		if (debugFace == 0)
		{
			targetAngle[0] = d8Faces[targetFace - 1][0];
			targetAngle[1] = d8Faces[targetFace - 1][1];
			targetAngle[2] = d8Faces[targetFace - 1][2];
		}
		else
		{
			targetFace = debugFace;
			targetAngle[0] = d8Faces[targetFace - 1][0];
			targetAngle[1] = d8Faces[targetFace - 1][1];
			targetAngle[2] = d8Faces[targetFace - 1][2];
		}
		break;
	case DiceType::D6:
		if (debugFace == 0)
		{
			targetAngle[0] = d6Faces[targetFace - 1][0];
			targetAngle[1] = d6Faces[targetFace - 1][1];
			targetAngle[2] = d6Faces[targetFace - 1][2];
		}
		else
		{
			targetFace = debugFace;
			targetAngle[0] = d6Faces[targetFace - 1][0];
			targetAngle[1] = d6Faces[targetFace - 1][1];
			targetAngle[2] = d6Faces[targetFace - 1][2];
		}
		break;
	case DiceType::D4:
		if (debugFace == 0)
		{
			targetAngle[0] = d4Faces[targetFace - 1][0];
			targetAngle[1] = d4Faces[targetFace - 1][1];
			targetAngle[2] = d4Faces[targetFace - 1][2];
		}
		else
		{
			targetFace = debugFace;
			targetAngle[0] = d4Faces[targetFace - 1][0];
			targetAngle[1] = d4Faces[targetFace - 1][1];
			targetAngle[2] = d4Faces[targetFace - 1][2];
		}
		break;
	default:
		break;
	}

	if (debugFace == 21)
	{
		targetAngle[0] = 0;
		targetAngle[1] = 0;
		targetAngle[2] = 0;
	}
}


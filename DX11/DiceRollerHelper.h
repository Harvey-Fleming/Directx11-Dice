#pragma once
#include <string>
#include "ChiliTimer.h"
#include <time.h>
#include <directxmath.h>

#include <windows.h>
#include "Graphics/StringHelper.h"

using namespace std;
using namespace DirectX;
using angle = float[3];



class DiceRollerHelper
{
public:
	enum DiceType
	{
		D20,
		D12,
		D10,
		D8,
		D6,
		D4,
	};
	void CalculateAngle();
	XMMATRIX getrotMatrix(float xOffset, float yOffset, float zOffset);
	void ReRoll(int face, DiceType diceType, float debugFace);

	void seedRand();



	ChiliTimer timer;

private:
	angle nextAngle;
	angle currentAngle;
	angle targetAngle;

	XMMATRIX oldRotationMatrix;
	angle d20Faces[20] = {{0.35f,-0.2f,-2.0f}, {1.3f,-0.2f,-3.0f}, {-1.550f,-0.950f,1.6f}, {-3.550f,-1.400f,-0.700f}, {0.450f,-1.400f,-2.200f}, {-1.550f,-1.710f,-5.700f}, {-1.050f,0.150f,-2.910f}, {-0.850f,-0.550f,-2.910f}, {-1.350f,1.250f,-1.410}, {-1.350f,2.250f,0.090f}, {-1.200f,3.250f,-2.410f}, {-2.100f,3.150f,-2.410f}, {0.400f,3.050f,-1.150f}, {-3.100f,3.050f,-0.150f}, {-5.500f,2.700f,3.850f},{0,2.500f,6.350f} ,{0.500f,2.000f,1.380f} ,{1.000f,2.290f,2.880f} ,{5.250f, 2.600f, 2.880f} ,{3.0f,1.900f,-0.250f}};
	angle d12Faces[12] = { {0,0,-2.830}, {-1.2f, 0.256f, 0}, {-3.1f,0.05f,-3.5f}, {-4.3,-0.350,-6.5}, {-0.9f,-0.85f,0.5f}, {1.0f,-0.85f,-3.0f}, {1.0f,0.3f,0}, {1.0f,2.3f,-3.0f}, {3.500f,2.000f,-0.500f}, {-0.75,2.3f,-0.5f}, {-3.25f,-1.05,-3}, {-2.25,-0.25,0}};
	angle d10Faces[10] = {{0.5f,0,0}, {0.5f,-1.0f,1.0f}, {2.0f,-2.5f,-0.250}, {2.2f,-3.6f,0.35f}, {3.7f,-4.2f,2.2f}, {3.7f,-5.2f,-1.0f}, {2.2f,-5.7f,-3.0f}, {2.2f,-6.7f,-3.5f}, {3.7f,-1.0f,-5.0f}, {4.2f,0,-6.5f}};
	angle d8Faces[8] = { {1.0f,5.6f,0.5f}, {0.950f,6.900f,-0.5f}, {-2.5,-0.5,2.5f}, {2.500f,0.500f,3.750f}, {0.700f,-2.600f,-2.500f}, {2.600f,-2.600f,-0.750f}, {2.600f,-3.600f,0.650f}, {0.600f,-3.600f,2.520f}};
	angle d6Faces[6] = { {0,0,3.141f}, {1.5f,0,1.56f}, {3.0f,0,-3.151f}, {4.5f,0,-1.600f}, {0,-1.5f,0}, {0,1.5f,0}};
	angle d4Faces[4] = { {1.5f,-0.5f,0}, {1.5f,0,2.1f}, {1.5f,-2.5f,2.1f}, {0.5f,8.0f,1.1f}};

	int targetFace;

	float speedMultiplier;
	float maxSpeedMultiplier = 10;
};



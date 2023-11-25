#pragma once
#include <DirectXMath.h>

using rgba = float[4]; //Allows for us to create an RGBA vlaue
struct Vertex 
{ 
	float X, Y, Z; rgba Color; 
	DirectX::XMFLOAT2 texCoord;
};    // a struct to define a vertex

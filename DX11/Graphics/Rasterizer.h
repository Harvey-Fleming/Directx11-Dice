#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

class Rasterizer
{
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

public:

	ID3D11RasterizerState* Get()const
	{
		return rasterizerState.Get();
	}

	ID3D11RasterizerState* const* GetAddressOf()const
	{
		return rasterizerState.GetAddressOf();
	}

	HRESULT Initialise(ID3D11Device* device)
	{
		D3D11_RASTERIZER_DESC raterizerDesc = {};

		raterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		raterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		HRESULT hr = device->CreateRasterizerState(&raterizerDesc, this->rasterizerState.GetAddressOf());
		return hr;
	}
};

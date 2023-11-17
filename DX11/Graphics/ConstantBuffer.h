#pragma once
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <cassert>

#include "ConstBufferType.h"

template<class T>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<T>& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	ID3D11DeviceContext * deviceContext = nullptr;

public:
	ConstantBuffer() {}

	T data;

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext * deviceContext)
	{
		this->deviceContext = deviceContext;

		///////////////////////////////////////////////////////////////////////////////////////////
		// Create Constant Buffer
		///////////////////////////////////////////////////////////////////////////////////////////
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
		cbd.StructureByteStride = 0u;


		HRESULT hr = device->CreateBuffer(&cbd, 0, this->buffer.GetAddressOf());       // Create the buffer
		return hr;
	}

	bool ApplyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = this->deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(hr));
		CopyMemory(mappedResource.pData, &data, sizeof(T));
		this->deviceContext->Unmap(buffer.Get(), 0);
		return true;
	}
};

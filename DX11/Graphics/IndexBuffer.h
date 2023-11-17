#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>


class IndexBuffer
{
private:
	IndexBuffer(const IndexBuffer& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT bufferSize = 0;

public:
	IndexBuffer() {}

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	UINT BufferSize() const
	{
		return this->bufferSize;
	}

	HRESULT Initialize(ID3D11Device *device, DWORD  * data, UINT numIndices)
	{
		///////////////////////////////////////////////////////////////////////////////////////////
		// Create Index Buffer
		///////////////////////////////////////////////////////////////////////////////////////////

		this->bufferSize = numIndices;

		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = sizeof(DWORD) * numIndices;

		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&ibd, &isd, buffer.GetAddressOf());       // Create the buffer
		return hr;
	}
};


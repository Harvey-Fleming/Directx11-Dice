#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template<class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	std::shared_ptr<UINT> stride;
	UINT bufferSize = 0; 

public:
	VertexBuffer() {}

	VertexBuffer(const VertexBuffer<T>& rhs) 
	{
		this->buffer = rhs.buffer;
		this->bufferSize = rhs.bufferSize;
		this->stride = rhs.stride;
	}

	VertexBuffer<T>& operator=(const VertexBuffer<T>& a)
	{
		this->buffer = a.buffer;
		this->bufferSize = a.bufferSize;
		this->stride = a.stride;
		return *this;
	}

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

	const UINT Stride() const
	{
		return *this->stride.get();
	}

	const UINT * StridePtr() const
	{
		return this->stride.get();
	}

	HRESULT Initialize(ID3D11Device *device, T * data, UINT numVertices)
	{
		this->bufferSize = numVertices;
		this->stride = std::make_shared<UINT>(sizeof(T));

		///////////////////////////////////////////////////////////////////////////////////////////
		// Create Vertex Buffer
		///////////////////////////////////////////////////////////////////////////////////////////

		// create the vertex buffer
		D3D11_BUFFER_DESC bd = {};

		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(T) * numVertices;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&bd, &sd, this->buffer.GetAddressOf());       // Create the buffer
		return hr;
	}
};

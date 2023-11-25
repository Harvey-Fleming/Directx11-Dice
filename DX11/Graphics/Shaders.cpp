#include "Shaders.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
    constexpr UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to Compile Vertex Shader");
    }

    // Create Shader Objects
    hr = device->CreateVertexShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to Create Vertex Shader");
    }

    hr = device->CreateInputLayout(layoutDesc, numElements, this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), this->inputLayout.GetAddressOf());
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to Create Input Layout");
    }

    return true;
}

ID3D11VertexShader* VertexShader::GetShader()
{
    return this->shader.Get();
}

ID3D10Blob* VertexShader::GetBuffer()
{
    return this->shader_buffer.Get();
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
    return this->inputLayout.Get();
}

bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
    constexpr UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
    HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to Compile Pixel Shader");
    }

    // Create Shader Objects
    hr = device->CreatePixelShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
    if (FAILED(hr))
    {
        OutputDebugStringA("Failed to Create Pixel Shader");
    }

    return true;
}

ID3D11PixelShader * PixelShader::GetShader()
{
    return this->shader.Get();
}

ID3D10Blob* PixelShader::GetBuffer()
{
    return this->shader_buffer.Get();
}

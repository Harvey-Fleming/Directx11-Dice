#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Mesh.h"

#include <string>
#include <vector>

using namespace DirectX;
using namespace std;

class Model
{
public:
	bool Initialize(const string & filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, ConstantBuffer<ConstBuffer>& constantBuffer);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void Draw(const XMMATRIX & viewProjectionMatrix);

private:
	vector<Mesh> meshes;
	
	bool LoadModel(const string& filePath);
	void ProcessNode(aiNode * node, const aiScene * scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	void UpdateWorldMatrix();

	ID3D11Device* D3D_device = nullptr;
	ID3D11DeviceContext* D3D_device_context = nullptr;
	ConstantBuffer<ConstBuffer>* constantbuffer = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;

	XMMATRIX worldMatrix = XMMatrixIdentity();
};

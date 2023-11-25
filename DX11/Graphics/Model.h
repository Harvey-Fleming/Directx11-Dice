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
	bool Initialize(const string & filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<ConstBuffer>& constantBuffer);
	void Draw(const XMMATRIX & viewProjectionMatrix);

private:
	vector<Mesh> meshes;
	
	bool LoadModel(const string& filePath);
	void ProcessNode(aiNode * node, const aiScene * scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	int GetTextureIndex(aiString* pStr);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);

	void UpdateWorldMatrix();

	ID3D11Device* D3D_device = nullptr;
	ID3D11DeviceContext* D3D_device_context = nullptr;
	ConstantBuffer<ConstBuffer>* constantbuffer = nullptr;
	std::string directory = "";

	XMMATRIX worldMatrix = XMMatrixIdentity();
};

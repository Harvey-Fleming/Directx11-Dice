#include "Model.h"

bool Model::Initialize(const string & filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, ConstantBuffer<ConstBuffer>& constantBuffer)
{
	this->D3D_device = device;
	this->D3D_device_context = deviceContext;
	this->texture = texture;
	this->constantbuffer = &constantBuffer;
    srand(time(NULL));
    LoadModel(filePath);

	this->UpdateWorldMatrix();
	return true;
}

void Model::SetTexture(ID3D11ShaderResourceView* texture)
{
	this->texture = texture;
}



bool Model::LoadModel(const string& filePath)
{
    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

    if (pScene == NULL)
        return false;

    this->ProcessNode(pScene->mRootNode, pScene);
    return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(this->ProcessMesh(mesh, scene));
    }

    for (UINT i = 0; i < node->mNumChildren; i++)
    {
        this->ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<DWORD> indices;
    //Will go through each vertex in the file and create a new vertex struct to store the position, colours and texture.
    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.X = mesh->mVertices[i].x;
        vertex.Y = mesh->mVertices[i].y;
        vertex.Z = mesh->mVertices[i].z;

        vertex.Color[0] = (rand() % 100) / static_cast<float>(100);
        vertex.Color[1] = (rand() % 100) / static_cast<float>(100);
        vertex.Color[2] = (rand() % 100) / static_cast<float>(100);
        vertex.Color[3] = 1.0f;


        //if (mesh->mTextureCoords[0])
        //{
        //    
        //}

        vertices.push_back(vertex);
    }

    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (UINT j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    return Mesh(this->D3D_device, this->D3D_device_context, vertices, indices);
}

void Model::Draw(const XMMATRIX& viewProjectionMatrix)
{
    //Update Constant buffer
    this->constantbuffer->data.transform = viewProjectionMatrix;//this->worldMatrix *  //Calculate World-View-Projection Matrix
    this->constantbuffer->ApplyChanges();
    this->D3D_device_context->VSSetConstantBuffers(0, 1, this->constantbuffer->GetAddressOf());

    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw();
    }
}



void Model::UpdateWorldMatrix()
{
	this->worldMatrix = XMMatrixIdentity();
}
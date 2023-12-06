#include "Model.h"

bool Model::Initialize(const string & filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<ConstBuffer>& constantBuffer)
{
	this->D3D_device = device;
	this->D3D_device_context = deviceContext;
	this->constantbuffer = &constantBuffer;
    srand(time(NULL));
    LoadModel(filePath);

	this->UpdateWorldMatrix();
	return true;
}

void Model::Draw(const XMMATRIX& viewProjectionMatrix)
{
    //Update Constant buffer
    this->constantbuffer->data.transform = viewProjectionMatrix;//  //Calculate World-View-Projection Matrix
    this->constantbuffer->ApplyChanges();
    this->D3D_device_context->VSSetConstantBuffers(0, 1, this->constantbuffer->GetAddressOf());


    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw();
    }
}

bool Model::LoadModel(const string& filePath)
{
    this->directory = StringHelper::GetDirectoryFromPath(filePath);
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

TextureStorageType Model::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType)
{
    if (pMat->GetTextureCount(textureType) == 0)
        return TextureStorageType::None;

    aiString path;
    pMat->GetTexture(textureType, index, &path);
    std::string texturePath = path.C_Str();
    //Check if texture is an embedded indexed texture by seeing if the file path is an index #
    if (texturePath[0] == '*')
    {
        if (pScene->mTextures[0]->mHeight == 0)
        {
            return TextureStorageType::EmbeddedIndexCompressed;
        }
        else
        {
            assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
            return TextureStorageType::EmbeddedIndexNonCompressed;
        }
    }
    //Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
    if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
    {
        if (pTex->mHeight == 0)
        {
            return TextureStorageType::EmbeddedCompressed;
        }
        else
        {
            assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
            return TextureStorageType::EmbeddedNonCompressed;
        }
    }
    //Lastly check if texture is a filepath by checking for period before extension name
    if (texturePath.find('.') != std::string::npos)
    {
        return TextureStorageType::Disk;
    }

    return TextureStorageType::None; // No texture exists
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


        if (mesh->mTextureCoords[0])
        {
            vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
            vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
        }

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

    std::vector<Texture> textures;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
    textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());


    return Mesh(this->D3D_device, this->D3D_device_context, vertices, indices, textures);
}


std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene)
{
    std::vector<Texture> materialTextures;
    TextureStorageType storetype = TextureStorageType::Invalid;
    unsigned int textureCount = pMaterial->GetTextureCount(textureType);

    if (textureCount == 0) //If there are no textures
    {
        storetype = TextureStorageType::None;
        aiColor3D aiColor(0.0f, 0.0f, 0.0f);
        switch (textureType)
        {
        case aiTextureType_DIFFUSE:
            pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
            if (aiColor.IsBlack()) //If color = black, just use grey
            {
                materialTextures.push_back(Texture(this->D3D_device, Colors::UnloadedTextureColor, textureType));
                return materialTextures;
            }
            materialTextures.push_back(Texture(this->D3D_device, Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
            return materialTextures;
        }
    }
    else
    {
        for (UINT i = 0; i < textureCount; i++)
        {
            aiString path;
            pMaterial->GetTexture(textureType, i, &path);
            TextureStorageType storetype = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
            switch (storetype)
            {
            case TextureStorageType::Disk:
            {
                std::string filename = this->directory + '\\' + path.C_Str();
                Texture diskTexture(this->D3D_device, filename, textureType);
                materialTextures.push_back(diskTexture);
                break;
            }
            case TextureStorageType::EmbeddedIndexCompressed:
            {
                int index = GetTextureIndex(&path);
                Texture embeddedIndexedTexture(this->D3D_device,
                    reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
                    pScene->mTextures[index]->mWidth,
                    textureType);
                materialTextures.push_back(embeddedIndexedTexture);
                break;
            }
            case TextureStorageType::EmbeddedCompressed:
            {
                const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
                Texture embeddedTexture(this->D3D_device,
                    reinterpret_cast<uint8_t*>(pTexture->pcData),
                    pTexture->mWidth,
                    textureType);
                materialTextures.push_back(embeddedTexture);
                break;
            }
            }
        }
    }

    if (materialTextures.size() == 0)
    {
        materialTextures.push_back(Texture(this->D3D_device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
        return materialTextures;
    }

    return materialTextures;
}

int Model::GetTextureIndex(aiString* pStr)
{
    assert(pStr->length >= 2);
    return atoi(&pStr->C_Str()[1]);
}

void Model::UpdateWorldMatrix()
{
	this->worldMatrix = XMMatrixIdentity();
}
#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include "Pipeline.h"
#include "Texture.h"
#include "Util.h"

class CMesh
{
public:
	CMesh();
	~CMesh();

	bool loadMesh(const std::string& vFileName);
	void render();

private:
	void __clear();
	bool __initFromScene(const aiScene* vScene, const std::string& vFilename);
	void __initMesh(unsigned int vIndex, const aiMesh* vPaiMesh);
	bool __initMaterials(const aiScene* vScene, const std::string& vFilename);

private:
#define INVALID_MATERIAL 0xFFFFFFFF
	struct SMeshEntry
	{
		SMeshEntry();
		~SMeshEntry();

		void init(const std::vector<SVertex>& vVertices, const std::vector<unsigned int>& vIndices);
		
		GLuint m_VB;
		GLuint m_IB;
		unsigned int m_NumIndices;
		unsigned int m_MaterialIndex;
	};

	std::vector<SMeshEntry> m_Entries;
	std::vector<CTexture*> m_Textures;
};

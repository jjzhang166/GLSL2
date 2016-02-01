#include "mesh.h"

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
	__clear();
}

CMesh::SMeshEntry::SMeshEntry()
{
	m_VB = INVALID_OGL_VALUE;
	m_IB = INVALID_OGL_VALUE;
	m_NumIndices  = 0;
	m_MaterialIndex = INVALID_MATERIAL;
}

CMesh::SMeshEntry::~SMeshEntry()
{
	if (m_VB != INVALID_OGL_VALUE)
	{
		glDeleteBuffers(1, &m_VB);
	}

	if (m_IB != INVALID_OGL_VALUE)
	{
		glDeleteBuffers(1, &m_IB);
	}
}

//******************************************************************
//FUNCTION:
void CMesh::__clear()
{
	for (unsigned int i = 0 ; i < m_Textures.size() ; i++) {
		SAFE_DELETE(m_Textures[i]);
	}
}

//******************************************************************
//FUNCTION:
void CMesh::SMeshEntry::init(const std::vector<SVertex>& vVertices, const std::vector<unsigned int>& vIndices)
{
	m_NumIndices = vIndices.size();

	glGenBuffers(1, &m_VB);
	glBindBuffer(GL_ARRAY_BUFFER, m_VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SVertex) * vVertices.size(), &vVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_NumIndices, &vIndices[0], GL_STATIC_DRAW);
}


//******************************************************************
//FUNCTION:
bool CMesh::loadMesh(const std::string& vFileName)
{
	__clear();

	bool Ret = false;
	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(vFileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (pScene) {
		Ret = __initFromScene(pScene, vFileName);
	}
	else {
		printf("Error parsing '%s': '%s'\n", vFileName.c_str(), Importer.GetErrorString());
	}

	return Ret;
}

//******************************************************************
//FUNCTION:
bool CMesh::__initFromScene(const aiScene* vScene, const std::string& vFilename)
{
	m_Entries.resize(vScene->mNumMeshes);
	m_Textures.resize(vScene->mNumMaterials);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
		const aiMesh* paiMesh = vScene->mMeshes[i];
		__initMesh(i, paiMesh);
	}

	return __initMaterials(vScene, vFilename);
}

//******************************************************************
//FUNCTION:
void CMesh::__initMesh(unsigned int vIndex, const aiMesh* vPaiMesh)
{
	m_Entries[vIndex].m_MaterialIndex = vPaiMesh->mMaterialIndex;

	std::vector<SVertex> Vertices;
	std::vector<unsigned int> Indices;

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0 ; i < vPaiMesh->mNumVertices ; i++) {
		const aiVector3D* pPos      = &(vPaiMesh->mVertices[i]);
		const aiVector3D* pNormal   = &(vPaiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = vPaiMesh->HasTextureCoords(0) ? &(vPaiMesh->mTextureCoords[0][i]) : &Zero3D;
		const aiVector3D* pTangent  = &(vPaiMesh->mTangents[i]);

		SVertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
			      glm::vec2(pTexCoord->x, pTexCoord->y),
				  glm::vec3(pNormal->x, pNormal->y, pNormal->z),
				  glm::vec3(pTangent->x, pTangent->y, pTangent->z));

		Vertices.push_back(v);
	}

	for (unsigned int i = 0 ; i < vPaiMesh->mNumFaces ; i++) {
		const aiFace& Face = vPaiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}

	m_Entries[vIndex].init(Vertices, Indices);
}

//******************************************************************
//FUNCTION:
bool CMesh::__initMaterials(const aiScene* vScene, const std::string& vFilename)
{
	std::string::size_type SlashIndex = vFilename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = vFilename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	for (unsigned int i = 0 ; i < vScene->mNumMaterials ; i++) {
		const aiMaterial* pMaterial = vScene->mMaterials[i];

		m_Textures[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string FullPath = Dir + "/" + Path.data;
				m_Textures[i] = new CTexture(GL_TEXTURE_2D, FullPath.c_str());

				if (!m_Textures[i]->loadImage()) {
					printf("Error loading texture '%s'\n", FullPath.c_str());
					delete m_Textures[i];
					m_Textures[i] = NULL;
					Ret = false;
				}
				else {
					printf("Loaded texture '%s'\n", FullPath.c_str());
				}
			}
		}//end if
	}//end for

	return Ret;
}

//******************************************************************
//FUNCTION:
void CMesh::render()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].m_VB);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (const GLvoid*)12);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (const GLvoid*)20);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].m_IB);

		const unsigned int MaterialIndex = m_Entries[i].m_MaterialIndex;

		if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
			m_Textures[MaterialIndex]->bindTexture(GL_TEXTURE0);
		}

		glDrawElements(GL_TRIANGLES, m_Entries[i].m_NumIndices, GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
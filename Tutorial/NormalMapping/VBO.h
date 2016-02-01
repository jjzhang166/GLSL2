#pragma once
#include "Pipeline.h"
template<typename T>
class CVBO
{
public:
	CVBO();
	~CVBO();

	void createVBO(T* vData, int vElemNum);
	void useVBO(GLuint vIndex, GLint vSize, const void* vPointer);

private:
//	void __computeVertexNormal(const unsigned int* vIndices, unsigned int vIndexCount, SVertex* vVertex, unsigned int vVertexCount);

private:
	GLuint m_VBO;
};

template<typename T>
CVBO<T>::CVBO()
{
}

template<typename T>
CVBO<T>::~CVBO()
{
}

//******************************************************************
//FUNCTION:
template<typename T>
void CVBO<T>::createVBO(T* vData, int vElemNum)
{
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vElemNum*sizeof(T), vData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//******************************************************************
//FUNCTION:
template<typename T>
void CVBO<T>::useVBO(GLuint vIndex, GLint vSize, const void* vPointer)
{
	glEnableVertexAttribArray(vIndex);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(vIndex, vSize, GL_FLOAT, GL_FALSE, sizeof(T), vPointer);
}

//******************************************************************
//FUNCTION:
//template<typename T>
//void CVBO<T>::__computeVertexNormal(const unsigned int* vIndices, unsigned int vIndexCount, SVertex* vVertex, unsigned int vVertexCount)
//{
//	for (unsigned int i=0; i<vIndexCount; i+=3)
//	{
//		unsigned int Index0 = vIndices[0];
//		unsigned int Index1 = vIndices[1];
//		unsigned int Index2 = vIndices[2];
//
//		glm::vec3 V10 = vVertex[1].m_Pos - vVertex[0].m_Pos;
//		glm::vec3 V20 = vVertex[2].m_Pos - vVertex[0].m_Pos;
//		glm::vec3 Normal = glm::cross(V10, V20);
//		Normal = glm::normalize(Normal);
//
//		vVertex[Index0].m_Normal += Normal;
//		vVertex[Index1].m_Normal += Normal;
//		vVertex[Index2].m_Normal += Normal;
//	}
//
//	for (unsigned int i=0; i<vVertexCount; i++)
//	{
//		vVertex[i].m_Normal = glm::normalize(vVertex[i].m_Normal);
//	}
//}
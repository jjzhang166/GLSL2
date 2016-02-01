#pragma once

template<typename T>
class CVBO
{
public:
	CVBO();
	~CVBO();

	void createVBO(T* vData, int vElemNum);
	void useVBO(GLuint vIndex, GLint vSize, const void* vPointer);
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
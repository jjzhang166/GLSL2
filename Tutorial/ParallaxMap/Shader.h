#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <gl/glew.h>
#include <gl/glut.h>

class CShader
{
public:
	CShader();
	~CShader();
	
	void initShader(const std::string& vVertex, const std::string& vFragment);
	void useShader();
	void banShader();
	const unsigned int& getProgramID() const;

	void setTextureUniformValue(const char *name, unsigned int n);
	void setMatUniformValue(const char *name, float Matrix[16]);

	void setFloatUniformValue(const char* name, GLfloat v0);
	void setFloatUniformValue(const char* name, GLfloat v0,  GLfloat v1);
	void setFloatUniformValue(const char* name, GLfloat v0,  GLfloat v1, GLfloat v2);
	void setFloatUniformValue(const char* name, GLfloat v0,  GLfloat v1, GLfloat v2, GLfloat v3);
	void setFloatUnifromValue3v(const char* vName, GLsizei vCount, const GLfloat* vValues);

	void setIntUniformValue(const char *name, GLint v0);
	void setIntUniformValue(const char *name, GLint v0,  GLint v1);
	void setIntUniformValue(const char *name, GLint v0,  GLint v1, GLint v2);
	void setIntUniformValue(const char *name, GLint v0,  GLint v1, GLint v2, GLint v3);

private:
	void __printShaderVersion();
	const GLchar* const __readShaderText(const std::string& vFileName);
	void __compileShader(GLuint& vShader);
	void __linkProgram(GLuint& vProgram);

private:
	unsigned int m_VertexID;
	unsigned int m_FragmentID;
	unsigned int m_ProgramID;
};
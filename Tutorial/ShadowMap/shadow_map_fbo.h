#pragma once
#include <gl/glew.h>
#include <iostream>

class CShaderMapFBO
{
public:
	CShaderMapFBO();
	~CShaderMapFBO();

	bool initFBO(unsigned int vWidth, unsigned int vHeight);
	void bindForWriting();
	void bindForReading(GLenum vTextureUnit);

private:
	GLuint m_Fbo;
	GLuint m_ShadowMap;
	GLuint m_DepthRenderBuffer;
};
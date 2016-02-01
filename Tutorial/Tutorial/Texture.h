#pragma once
#include <iostream>
#include <string>
#include <gl/glew.h>
#include <gl/glut.h>
#include <Magick++.h>

class CTexture
{
public:
	CTexture(GLenum vTextureTarget, const std::string& vFileName);
	~CTexture();

	bool loadImage();
	void bindTexture(GLenum vTextureUnit);

private:
	std::string   m_FileName;
	GLenum        m_TextureTarget;
	GLuint        m_TextureObj;
	Magick::Image m_Image;
	Magick::Blob  m_Blob;
};

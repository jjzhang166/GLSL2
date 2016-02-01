#include "Texture.h"

CTexture::CTexture(GLenum vTextureTarget, const std::string& vFileName)
{
	m_TextureTarget = vTextureTarget;
	m_FileName = vFileName;
}

CTexture::~CTexture()
{
}

//******************************************************************
//FUNCTION:
bool CTexture::loadImage()
{
	 try{
		 m_Image.read(m_FileName);
		 m_Image.write(&m_Blob, "RGBA");
	 }
	 catch(Magick::Error& Error)
	 {
		 std::cout << "Error loading texture '" << m_FileName << "': " << Error.what() << std::endl;
		 return false;
	 }

	 glGenTextures(1, &m_TextureObj);
	 glBindTexture(m_TextureTarget, m_TextureObj);
	 glTexImage2D(m_TextureTarget, 0, GL_RGBA, m_Image.columns(), m_Image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Blob.data());
	 glTexParameterf(m_TextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	 glTexParameterf(m_TextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 glBindTexture(m_TextureTarget, 0);

	 return true;
}

//******************************************************************
//FUNCTION:
void CTexture::bindTexture(GLenum vTextureUnit)
{
	glActiveTexture(vTextureUnit);
	glBindTexture(m_TextureTarget, m_TextureObj);
}


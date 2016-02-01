#include "Shader.h"

CShader::CShader()
{
}

CShader::~CShader()
{
}

//******************************************************************
//FUNCTION:
void CShader::initShader(const std::string& vVertex, const std::string& vFragment)
{
	_ASSERT(vVertex.size()!=0 && vFragment.size()!=0);
	__printShaderVersion();

	const GLchar* pVertexShaderText = __readShaderText(vVertex);
	const GLchar* pFragmentShaderText = __readShaderText(vFragment);

	if (pVertexShaderText==NULL|| pFragmentShaderText==NULL)
	{
		std::cout << "Shader text is empty" << std::endl;
		return;
	}

	m_VertexID = glCreateShader(GL_VERTEX_SHADER);
	m_FragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(m_VertexID, 1, &pVertexShaderText, NULL);
	glShaderSource(m_FragmentID, 1, &pFragmentShaderText, NULL);

	__compileShader(m_VertexID);
	__compileShader(m_FragmentID);

	m_ProgramID = glCreateProgram();
	glAttachShader(m_ProgramID, m_VertexID);
	glAttachShader(m_ProgramID, m_FragmentID);

	__linkProgram(m_ProgramID);

	delete pVertexShaderText;
	delete pFragmentShaderText;
}

//******************************************************************
//FUNCTION:
const GLchar* const CShader::__readShaderText(const std::string& vFileName)
{
	_ASSERT(vFileName.size() != 0);
	GLchar* pShaderText = NULL;
	std::filebuf* pFileBuffer = NULL;
	unsigned int FileSize = 0;

	std::ifstream FileIn;
	FileIn.open(vFileName, std::ios::binary);

	if (FileIn.fail())
	{
		std::cout << "Fail to open " << vFileName << std::endl;
		return NULL;
	}

	pFileBuffer = FileIn.rdbuf();
	FileSize = pFileBuffer->pubseekoff(0, std::ios::end, std::ios::in);
	pFileBuffer->pubseekpos(0, std::ios::in);
	
	pShaderText = new GLchar[FileSize+1];
	pFileBuffer->sgetn(pShaderText, FileSize);
	pShaderText[FileSize] = '\0';
	FileIn.close();

	return pShaderText;
}

//******************************************************************
//FUNCTION:
void CShader::__compileShader(GLuint& vShader)
{
	glCompileShader(vShader);
	
	GLint Compile;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &Compile);

	if (!Compile)
	{
		GLint LogLength;
		GLchar* pLog = NULL;
		glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &LogLength);
		
		if (LogLength > 0)
		{
			pLog = new GLchar[LogLength];
			glGetShaderInfoLog(vShader, LogLength, &LogLength, pLog);
			fprintf(stderr, "Compile log = '%s' \n", pLog);
			delete[] pLog;
		}
	}
}

//******************************************************************
//FUNCTION:
void CShader::__linkProgram(GLuint& vProgram)
{
	glLinkProgram(vProgram);
	
	GLint Link;
	glGetProgramiv(vProgram, GL_LINK_STATUS, &Link);

	if (!Link)
	{
		GLint LogLength;
		glGetProgramiv(vProgram, GL_INFO_LOG_LENGTH, &LogLength);

		if (LogLength > 0)
		{
			GLchar* pLog = new GLchar[LogLength];
			glGetProgramInfoLog(vProgram, LogLength, &LogLength, pLog);
			fprintf(stderr, "Link log = '%s' \n", pLog);
			delete[] pLog;
		}
	}
}

//******************************************************************
//FUNCTION:
void CShader::__printShaderVersion()
{
	const GLubyte *renderer = glGetString( GL_RENDERER );
	const GLubyte *vendor = glGetString( GL_VENDOR );
	const GLubyte *version = glGetString( GL_VERSION );
	const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );
	
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("GL Vendor : %s\n", vendor);
	printf("GL Renderer : %s\n", renderer);
	printf("GL Version (string) : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glslVersion);
}

//******************************************************************
//FUNCTION:
void CShader::useShader()
{
	glUseProgram(m_ProgramID);
}

//******************************************************************
//FUNCTION:
void CShader::banShader()
{
	glUseProgram(0);
}

//******************************************************************
//FUNCTION:
const unsigned int& CShader::getProgramID() const
{
	return m_ProgramID;
}

//******************************************************************
//FUNCTION:
void CShader::setTextureUniformValue(const char *name, unsigned int n)
{
	GLint key;
	key = glGetUniformLocation(m_ProgramID, name);
	if(key >= 0)
		glUniform1i(key, n);
	else
		printf("TextureUniformValue %s not found\n", name);
}

//******************************************************************
//FUNCTION:
void CShader::setMatUniformValue(const char *name, float Matrix[16])
{
	GLint key;
	key = glGetUniformLocation(m_ProgramID, name);
	if(key >= 0)
		glUniformMatrix4fv(key , 1, GL_FALSE, Matrix);
	else
		printf("MatUniformValue %s not found\n", name);
}

//******************************************************************
//FUNCTION:
void CShader::setFloatUniformValue(const char *name, GLfloat v0)
{
	GLint key;
	key = glGetUniformLocation(m_ProgramID, name);
	if(key >= 0)
		glUniform1f(key, v0);
	else
		printf("FloatUniformValue %s not found\n", name);
}

//******************************************************************
//FUNCTION:
void CShader::setFloatUniformValue(const char *name, GLfloat v0, GLfloat v1)
{
	GLint key;
	key = glGetUniformLocation(m_ProgramID, name);
	if(key >= 0)
		glUniform2f(key, v0, v1);
	else
		printf("FloatUniformValue %s not found\n", name);
}

//******************************************************************
//FUNCTION:
void CShader::setFloatUniformValue(const char *name, GLfloat v0, GLfloat v1, GLfloat v2)
{
	GLint key;
	key = glGetUniformLocation(m_ProgramID, name);
	if(key >= 0)
		glUniform3f(key, v0, v1, v2);
	else
		printf("FloatUniformValue %s not found\n", name);
}

//******************************************************************
//FUNCTION:
void CShader::setFloatUniformValue(const char *name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	GLint key;
	key = glGetUniformLocation(m_ProgramID, name);
	if(key >= 0)
		glUniform4f(key, v0, v1, v2, v3);
	else
		printf("FloatUniformValue %s not found\n", name);
}

//******************************************************************
//FUNCTION:
void CShader::setIntUniformValue(const char *name, GLint v0)
{
	GLint key;
	key = glGetUniformLocation(m_ProgramID, name);
	if(key >= 0)
		glUniform1i(key, v0);
	else
		printf("IntUniformValue %s not found\n", name);
}

//******************************************************************
//FUNCTION:
void CShader::setIntUniformValue(const char *name, GLint v0, GLint v1)
{
	GLint key;
	key = glGetUniformLocation(m_ProgramID, name);
	if(key >= 0)
		glUniform2i(key, v0, v1);
	else
		printf("IntUniformValue %s not found\n", name);
}

//******************************************************************
//FUNCTION:
void CShader::setIntUniformValue(const char *name, GLint v0, GLint v1, GLint v2)
{
	GLint key;
	key = glGetUniformLocation(m_ProgramID, name);
	if(key >= 0)
		glUniform3i(key, v0, v1, v2);
	else
		printf("IntUniformValue %s not found\n", name);
}

//******************************************************************
//FUNCTION:
void CShader::setIntUniformValue(const char *name, GLint v0, GLint v1, GLint v2, GLint v3)
{
	GLint key;
	key = glGetUniformLocation(m_ProgramID, name);
	if(key >= 0)
		glUniform4i(key, v0, v1, v2, v3);
	else
		printf("IntUniformValue %s not found\n", name);
}

//******************************************************************
//FUNCTION:
void CShader::setFloatUnifromValue3v(const char* vName, GLsizei vCount, const GLfloat* vValues)
{
	GLint Key;
	Key = glGetUniformLocation(m_ProgramID, vName);

	if (Key >= 0)
		glUniform3fv(Key, vCount, vValues);
	else
		std::cout << "Float uniform value " << vName << " not found" << std::endl;
}
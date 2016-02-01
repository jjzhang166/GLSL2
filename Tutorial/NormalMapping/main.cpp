#include "Shader.h"
#include "VBO.h"
#include <glm/glm.hpp>
#include "Pipeline.h"
#include "Texture.h"
#include "Light.h"
#include "Mesh.h"

GLfloat g_Lookat[3] = {0.0, 0.0, 5.0};
GLint g_MouseX, g_MouseY;
GLint g_MotionX, g_MotionY;
GLboolean g_Rotated = false;

GLuint g_IBO;
CShader* g_Shader = new CShader;
CVBO<SVertex>* g_VBO = new CVBO<SVertex>;

glm::mat4 World;
float Scale = 0.0f;
CTexture* pTexture = NULL;
CTexture* pNormalMap = NULL;
CTexture* pTrivialNormalMap = NULL;

SDirectionalLight DirectionalLight;
SPointLight PointLight;
SSpotLight SpotLight;
CMesh* pMesh;
bool isBumpEnabled = true;

//******************************************************************
//FUNCTION:
void PrintMatrix( float matrix[16] )
{
	assert( matrix != 0 );
	printf( "%8.2f%8.2f%8.2f%8.2f\n"
		"%8.2f%8.2f%8.2f%8.2f\n"
		"%8.2f%8.2f%8.2f%8.2f\n"
		"%8.2f%8.2f%8.2f%8.2f\n",
		matrix[0], matrix[1], matrix[2], matrix[3],
		matrix[4], matrix[5], matrix[6], matrix[7],
		matrix[8], matrix[9], matrix[10], matrix[11],
		matrix[12], matrix[13], matrix[14], matrix[15] );
	printf("\n");
}

//******************************************************************
//FUNCTION:
void initGlew()
{
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Fail to initialize GLEW" << std::endl;
		exit(0);
	}
}

//******************************************************************
//FUNCTION:
void initLight()
{
	DirectionalLight.m_AmbientColor = glm::vec3(0.3, 0.3, 0.3);
	DirectionalLight.m_DiffuseColor = glm::vec3(0.9, 0.9, 0.9);
	DirectionalLight.m_SpecularColor = glm::vec3(1.0, 0, 0);
	DirectionalLight.m_Direction = glm::vec3(1.0, 1.0, 1.0);
	
	PointLight.m_AmbientColor = glm::vec3(0, 0, 1);
	PointLight.m_DiffuseColor = glm::vec3(0, 1, 0);
	PointLight.m_SpecularColor = glm::vec3(1, 0, 0);
	PointLight.m_LightPosition = glm::vec3(0, 0, 30);
	PointLight.Attenuation.m_Constant = 1.0;
	PointLight.Attenuation.m_Linear = 0.1;
	PointLight.Attenuation.m_Quadratic = 0.0;

	SpotLight.m_AmbientColor = glm::vec3(0.0, 0.0, 1);
	SpotLight.m_DiffuseColor = glm::vec3(0.0, 1, 0.0);
	SpotLight.m_SpecularColor = glm::vec3(1.0, 0, 0);
	SpotLight.m_LightPosition = glm::vec3(5, 10, 5);
	SpotLight.m_Direction = glm::vec3(0.0, -1.0, 0.0);
	SpotLight.m_Cutoff = cosf(toRadian(20));
	SpotLight.Attenuation.m_Constant = 1.0;
	SpotLight.Attenuation.m_Linear = 0.1;
	SpotLight.Attenuation.m_Quadratic = 0.0;
}

//******************************************************************
//FUNCTION:
void createVertexBuffer(const unsigned int* vIndices, unsigned int vIndexCount, SVertex* vVertex, unsigned int vVertexCount)
{
	for (unsigned int i=0; i<vIndexCount; i+=3)
	{
		unsigned int Index0 = vIndices[i];
		unsigned int Index1 = vIndices[i+1];
		unsigned int Index2 = vIndices[i+2];

		glm::vec3 V10 = vVertex[Index1].m_Pos - vVertex[Index0].m_Pos;
		glm::vec3 V20 = vVertex[Index2].m_Pos - vVertex[Index0].m_Pos;
		glm::vec3 Normal = glm::cross(V10, V20);
		Normal = glm::normalize(Normal);

		vVertex[Index0].m_Normal += Normal;
		vVertex[Index1].m_Normal += Normal;
		vVertex[Index2].m_Normal += Normal;
	}

	for (unsigned int i=0; i<vVertexCount; i++)
	{
		vVertex[i].m_Normal = glm::normalize(vVertex[i].m_Normal);
	}
}

//******************************************************************
//FUNCTION:
void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	initGlew();
	initLight();

	pTexture = new CTexture(GL_TEXTURE_2D, "../Content/bricks.jpg");
	if (!pTexture->loadImage())
		return;

	pNormalMap = new CTexture(GL_TEXTURE_2D, "../Content/normal_map.jpg");
	if (!pNormalMap->loadImage())
		return;

	pTrivialNormalMap = new CTexture(GL_TEXTURE_2D, "../Content/normal_up.jpg");
	if (!pTrivialNormalMap->loadImage())
		return;
	
	pMesh = new CMesh;
	if (!pMesh->loadMesh("../Content/box.obj"))
		return;

	g_Shader->initShader("Shaders/0vertex.glsl", "Shaders/0fragment.glsl");
}	

//******************************************************************
//FUNCTION:
glm::mat3 computeNormalMatrix(const glm::mat4& vModelView)
{
	glm::mat3 NormalMatrix;
	NormalMatrix[0][0] = vModelView[0][0];
	NormalMatrix[1][0] = vModelView[1][0];
	NormalMatrix[2][0] = vModelView[2][0];

	NormalMatrix[0][1] = vModelView[0][1];
	NormalMatrix[1][1] = vModelView[1][1];
	NormalMatrix[2][1] = vModelView[2][1];

	NormalMatrix[0][2] = vModelView[0][2];
	NormalMatrix[1][2] = vModelView[1][2];
	NormalMatrix[2][2] = vModelView[2][2];

	glm::mat3 Normal = glm::transpose(glm::inverse(NormalMatrix));

	return Normal;
}

//******************************************************************
//FUNCTION:
void setDirectionalLight()
{
	g_Shader->setFloatUniformValue("uDirectionalLight.m_BaseLight.m_AmbientColor", DirectionalLight.m_AmbientColor.x, DirectionalLight.m_AmbientColor.y, DirectionalLight.m_AmbientColor.z);
	g_Shader->setFloatUniformValue("uDirectionalLight.m_BaseLight.m_DiffuseColor", DirectionalLight.m_DiffuseColor.x, DirectionalLight.m_DiffuseColor.y, DirectionalLight.m_DiffuseColor.z);
	g_Shader->setFloatUniformValue("uDirectionalLight.m_BaseLight.m_SpecularColor", DirectionalLight.m_SpecularColor.x, DirectionalLight.m_SpecularColor.y, DirectionalLight.m_SpecularColor.z);
	g_Shader->setFloatUniformValue("uDirectionalLight.m_Direction", DirectionalLight.m_Direction.x, DirectionalLight.m_Direction.y, DirectionalLight.m_Direction.z);
}

//******************************************************************
//FUNCTION:
void setPointLight()
{
	g_Shader->setFloatUniformValue("uPointLight.m_BaseLight.m_AmbientColor", PointLight.m_AmbientColor.x, PointLight.m_AmbientColor.y, PointLight.m_AmbientColor.z);
	g_Shader->setFloatUniformValue("uPointLight.m_BaseLight.m_DiffuseColor", PointLight.m_DiffuseColor.x, PointLight.m_DiffuseColor.y, PointLight.m_DiffuseColor.z);
	g_Shader->setFloatUniformValue("uPointLight.m_BaseLight.m_SpecularColor", PointLight.m_SpecularColor.x, PointLight.m_SpecularColor.y, PointLight.m_SpecularColor.z);
	g_Shader->setFloatUniformValue("uPointLight.m_LightPosition", PointLight.m_LightPosition.x, PointLight.m_LightPosition.y, PointLight.m_LightPosition.z);

	g_Shader->setFloatUniformValue("uPointLight.m_Attenuation.m_Constant", PointLight.Attenuation.m_Constant);
	g_Shader->setFloatUniformValue("uPointLight.m_Attenuation.m_Linear", PointLight.Attenuation.m_Linear);
	g_Shader->setFloatUniformValue("uPointLight.m_Attenuation.m_Quadratic", PointLight.Attenuation.m_Quadratic);
}

//******************************************************************
//FUNCTION:
void setSpotLight()
{
	g_Shader->setFloatUniformValue("uSpotLight.m_PointLight.m_BaseLight.m_AmbientColor", SpotLight.m_AmbientColor.x, SpotLight.m_AmbientColor.y, SpotLight.m_AmbientColor.z);
	g_Shader->setFloatUniformValue("uSpotLight.m_PointLight.m_BaseLight.m_DiffuseColor", SpotLight.m_DiffuseColor.x, SpotLight.m_DiffuseColor.y, SpotLight.m_DiffuseColor.z);
	g_Shader->setFloatUniformValue("uSpotLight.m_PointLight.m_BaseLight.m_SpecularColor", SpotLight.m_SpecularColor.x, SpotLight.m_SpecularColor.y, SpotLight.m_SpecularColor.z);

	g_Shader->setFloatUniformValue("uSpotLight.m_PointLight.m_LightPosition", SpotLight.m_LightPosition.x, SpotLight.m_LightPosition.y, SpotLight.m_LightPosition.z);
	g_Shader->setFloatUniformValue("uSpotLight.m_Direction", SpotLight.m_Direction.x, SpotLight.m_Direction.y, SpotLight.m_Direction.z);
	g_Shader->setFloatUniformValue("uSpotLight.m_Cutoff", SpotLight.m_Cutoff);

	g_Shader->setFloatUniformValue("uSpotLight.m_PointLight.m_Attenuation.m_Constant", SpotLight.Attenuation.m_Constant);
	g_Shader->setFloatUniformValue("uSpotLight.m_PointLight.m_Attenuation.m_Linear", SpotLight.Attenuation.m_Linear);
	g_Shader->setFloatUniformValue("uSpotLight.m_PointLight.m_Attenuation.m_Quadratic", SpotLight.Attenuation.m_Quadratic);
}

//******************************************************************
//FUNCTION:
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CPileline* pPileline = new CPileline;
	
	pPileline->lookAt(glm::vec3(g_Lookat[0], g_Lookat[1], g_Lookat[2]), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
 	//pPileline->translate(0.0, -2.0, -10.0);
 	pPileline->rotate(g_MotionX, 1.0, 0.0, 0.0);
	pPileline->rotate(g_MotionY, 0.0, 1.0, 0);
 //	pPileline->scale(0.1, 0.1, 0.1);
 	pPileline->perspective(60, 1, 1, 50);

	glm::mat4 Model = pPileline->getModelMat();
	glm::mat4 View = pPileline->getViewMat();
	glm::mat4 Project = pPileline->getProjectMat();
	glm::mat4 ModelViewProject = Project * View * Model;
	glm::mat4 ModelView = View * Model;
	glm::mat4 NormalMatrix = glm::transpose(glm::inverse(ModelView));

	g_Shader->useShader();

	pTexture->bindTexture(GL_TEXTURE0);
	g_Shader->setTextureUniformValue("uColorSampler", 0);

	if (isBumpEnabled)
	{
		pNormalMap->bindTexture(GL_TEXTURE1);
		g_Shader->setTextureUniformValue("uNormalMapSampler", 1);
	}
	else
	{
		pTrivialNormalMap->bindTexture(GL_TEXTURE1);
		g_Shader->setTextureUniformValue("uNormalMapSampler", 1);
	}
	

	g_Shader->setMatUniformValue("uMVP", &ModelViewProject[0][0]);
	g_Shader->setMatUniformValue("uModelView", &ModelView[0][0]);
	g_Shader->setMatUniformValue("uEyeNormalMatrix", &NormalMatrix[0][0]);
	g_Shader->setMatUniformValue("uViewMat", &View[0][0]);
	
	setDirectionalLight();
//	setPointLight();
//	setSpotLight();

	pMesh->render();
   	g_Shader->banShader();

	glutSwapBuffers();
}

//******************************************************************
//FUNCTION:
void reshape(int vWidth, int vHeight)
{
	glViewport(0, 0, (GLsizei)vWidth, (GLsizei)vHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 1, 1, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//******************************************************************
//FUNCTION:
void idle()
{
	Scale += 0.1f;
	glutPostRedisplay();		
}

//******************************************************************
//FUNCTION:
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		g_Lookat[2] -= 0.1;
		break;
	case 's':
		g_Lookat[2] += 0.1;
		break;
	case 'a':
		g_Lookat[0] -= 0.1;
		break;
	case 'd':
		g_Lookat[0] += 0.1;
		break;
	case 'q':
		g_Lookat[1] -= 0.1;
		break;
	case 'e':
		g_Lookat[1] += 0.1;
		break;
	case 'b':
		isBumpEnabled = !isBumpEnabled;
		break;
	default:
		break;
	}

	glutPostRedisplay();

}

//******************************************************************
//FUNCTION:

void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			g_MouseX = x;
			g_MouseY = y;
			g_Rotated = true;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

//******************************************************************
//FUNCTION:
void motion(int x, int y)
{
	if (g_Rotated)
	{
		g_MotionX = (g_MotionX + (y - g_MouseY)) % 360;
		g_MotionY = (g_MotionY + (x - g_MouseX)) % 360;

		g_MouseX = x;
		g_MouseY = y;
	}

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
    Magick::InitializeMagick(*argv);    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("NormalMap");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	//glutIdleFunc(idle);
	//glutReshapeFunc(reshape);
	glutMainLoop();
	
	return 1;
}


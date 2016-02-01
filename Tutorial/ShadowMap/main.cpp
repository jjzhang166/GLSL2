#include "Shader.h"
#include "VBO.h"
#include <glm/glm.hpp>
#include "Pipeline.h"
#include "Texture.h"
#include "Light.h"
#include "Mesh.h"
#include "shadow_map_fbo.h"

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 800;

GLfloat g_Lookat[3] = {0.0, 0.0, 10.0};
GLint g_MouseX, g_MouseY;
GLint g_MotionX, g_MotionY;
GLboolean g_Rotated = false;
float Scale = 0.0f;

SDirectionalLight DirectionalLight;
SPointLight PointLight;
SSpotLight SpotLight;
CMesh* pMesh;
CMesh* pQuad;

CShaderMapFBO g_ShadowMapFBO;
CShader* g_ShadowMapShader = new CShader;
CShader* g_RenderShader = new CShader;
CTexture* pGroundTexture;

glm::mat4 MVPLightSpace;
glm::mat4 PlaneMVPLightSpace;
glm::mat4 LightViewMatrix;
glm::mat4 LightProjectionMatrix;

// SVertex Plane[4] = {SVertex(glm::vec3(-5.0, -5.0, 5.0), glm::vec2(0.0f, 0.0f)),
// 					SVertex(glm::vec3(5.0, -5.0, 5.0), glm::vec2(1.0, 0.0f)),
// 					SVertex(glm::vec3(5.0, -5.0, -5.0), glm::vec2(1.0f, 1.0f)),
// 					SVertex(glm::vec3(-5.0, -5.0, -5.0), glm::vec2(0.0, 1.0f)) };
 SVertex Plane[4] = {SVertex(glm::vec3(-5.0, -5.0, 5.0), glm::vec2(0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0)),
 					SVertex(glm::vec3(5.0, -5.0, 5.0), glm::vec2(1.0, 0.0f),  glm::vec3(0.0, 1.0, 0.0)),
 					SVertex(glm::vec3(5.0, -5.0, -5.0), glm::vec2(1.0f, 1.0f),  glm::vec3(0.0, 1.0, 0.0)),
 					SVertex(glm::vec3(-5.0, -5.0, -5.0), glm::vec2(0.0, 1.0f),  glm::vec3(0.0, 1.0, 0.0))};

GLuint g_PlaneVBO;
bool g_DepthFlag = false;
float g_angle = 0.707;
bool g_stopLightMove = true;

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
// 	PointLight.m_AmbientColor = glm::vec3(0.0, 0.0, 0.3);
// 	PointLight.m_DiffuseColor = glm::vec3(0.0, 0.9, 0.0);
// 	PointLight.m_SpecularColor = glm::vec3(1.0, 0.0, 0.0);
	PointLight.m_AmbientColor = glm::vec3(0.3, 0.3, 0.3);
	PointLight.m_DiffuseColor = glm::vec3(0.9, 0.9, 0.9);
	PointLight.m_SpecularColor = glm::vec3(1.0, 1.0, 1.0);

	PointLight.m_LightPosition = glm::vec3(10.0, 10.0, 10.0);
	PointLight.Attenuation.m_Constant = 1.0;
	PointLight.Attenuation.m_Linear = 0.1;
	PointLight.Attenuation.m_Quadratic = 0.0;
}

//******************************************************************
//FUNCTION:
void createPlaneVBO()
{
	glGenBuffers(1, &g_PlaneVBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_PlaneVBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(Plane), Plane, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
		
	createPlaneVBO();
	pMesh = new CMesh;
	if (!pMesh->loadMesh("../Content/phoenix_ugv.md2"))
		return;

	pQuad = new CMesh();
	if (!pQuad->loadMesh("../Content/quad.obj"))
		return;

	pGroundTexture = new CTexture(GL_TEXTURE_2D, "../Content/test.png");
	if (!pGroundTexture->loadImage())
		return;	

	g_ShadowMapShader->initShader("Shaders/0vertex.glsl", "Shaders/0fragment.glsl");
	g_RenderShader->initShader("Shaders/1vertrender.glsl", "Shaders/1fragrender.glsl");
	if (!g_ShadowMapFBO.initFBO(WINDOW_WIDTH, WINDOW_HEIGHT))
		return;
}

//******************************************************************
//FUNCTION:
void shadowMapPass()
{
	g_ShadowMapFBO.bindForWriting();
	//glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(2.0, 4.0);
	CPileline Pileline;
	Pileline.lookAt(glm::vec3(PointLight.m_LightPosition[0], PointLight.m_LightPosition[1], PointLight.m_LightPosition[2]), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	LightViewMatrix = Pileline.getViewMat();
	//glm::mat4 LView = glm::lookAt(glm::vec3(PointLight.m_LightPosition[0], PointLight.m_LightPosition[1], PointLight.m_LightPosition[2]), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	Pileline.translate(0.0, -2.0, 0.0);
	Pileline.rotate(g_MotionX, 1.0, 0.0, 0.0);
	Pileline.rotate(g_MotionY, 0.0, 1.0, 0);
	Pileline.scale(0.05, 0.05, 0.05);
	Pileline.perspective(60, 1, 1, 100);
	LightProjectionMatrix = Pileline.getProjectMat();
	glm::mat4 Model = Pileline.getModelMat();
	glm::mat4 View = Pileline.getViewMat();
	glm::mat4 Projection = Pileline.getProjectMat();
	glm::mat4 ModelViewProject = Projection * View * Model;
	MVPLightSpace = ModelViewProject;

	g_ShadowMapShader->useShader();
	g_ShadowMapShader->setMatUniformValue("uModel", &Model[0][0]);
	g_ShadowMapShader->setMatUniformValue("uView", &View[0][0]);
	g_ShadowMapShader->setMatUniformValue("uProjection", &Projection[0][0]);	
	pMesh->render();

	Pileline.setIdentity("Model");
	Pileline.translate(0.0, 2.0, 0.0);
	Pileline.rotate(g_MotionX, 1.0, 0.0, 0.0);
	Pileline.rotate(g_MotionY, 0.0, 1.0, 0);

 	glm::mat4 ModelPlane = Pileline.getModelMat();
 	PlaneMVPLightSpace = Projection * View * ModelPlane;
	g_ShadowMapShader->setMatUniformValue("uModel", &ModelPlane[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, g_PlaneVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), 0);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableVertexAttribArray(0);
	g_ShadowMapShader->banShader();

	//glDisable(GL_POLYGON_OFFSET_FILL);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

//******************************************************************
//FUNCTION:
void generateShadow()
{
	CPileline Pileline;

	Pileline.lookAt(glm::vec3(g_Lookat[0], g_Lookat[1], g_Lookat[2]), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	Pileline.translate(0.0, -2.0, 0.0);
	Pileline.rotate(g_MotionX, 1.0, 0.0, 0.0);
	Pileline.rotate(g_MotionY, 0.0, 1.0, 0);
	Pileline.scale(0.05, 0.05, 0.05);
	Pileline.perspective(60, 1, 1, 100);

	glm::mat4 Model = Pileline.getModelMat();
	glm::mat4 View = Pileline.getViewMat();
	glm::mat4 Projection = Pileline.getProjectMat();
	glm::mat4 ModelView = View * Model;
	glm::mat4 NormalMat = glm::transpose(glm::inverse(ModelView));

	g_RenderShader->useShader();

	g_RenderShader->setTextureUniformValue("uTextureSampler", 0);
	g_ShadowMapFBO.bindForReading(GL_TEXTURE1);
	g_RenderShader->setTextureUniformValue("uShadowMapSampler", 1);

	g_RenderShader->setMatUniformValue("uModel", &Model[0][0]);
	g_RenderShader->setMatUniformValue("uView", &View[0][0]);
	g_RenderShader->setMatUniformValue("uProjection", &Projection[0][0]);
	g_RenderShader->setMatUniformValue("uMVPLightSpace", &MVPLightSpace[0][0]);
	g_RenderShader->setMatUniformValue("uNormalMatEye", &NormalMat[0][0]);

	g_RenderShader->setFloatUniformValue("uPointLight.m_BaseLight.m_AmbientColor", PointLight.m_AmbientColor.x, PointLight.m_AmbientColor.y, PointLight.m_AmbientColor.z);
	g_RenderShader->setFloatUniformValue("uPointLight.m_BaseLight.m_DiffuseColor", PointLight.m_DiffuseColor.x, PointLight.m_DiffuseColor.y, PointLight.m_DiffuseColor.z);
	g_RenderShader->setFloatUniformValue("uPointLight.m_BaseLight.m_SpecularColor", PointLight.m_SpecularColor.x, PointLight.m_SpecularColor.y, PointLight.m_SpecularColor.z);
	g_RenderShader->setFloatUniformValue("uPointLight.m_LightPosition", PointLight.m_LightPosition.x, PointLight.m_LightPosition.y, PointLight.m_LightPosition.z);
	g_RenderShader->setFloatUniformValue("uPointLight.m_Attenuation.m_Constant", PointLight.Attenuation.m_Constant);
	g_RenderShader->setFloatUniformValue("uPointLight.m_Attenuation.m_Linear", PointLight.Attenuation.m_Linear);
	g_RenderShader->setFloatUniformValue("uPointLight.m_Attenuation.m_Quadratic", PointLight.Attenuation.m_Quadratic);
	g_RenderShader->setMatUniformValue("uLightViewMat", &LightViewMatrix[0][0]);
	g_RenderShader->setMatUniformValue("uLightProjectionMatrix", &LightProjectionMatrix[0][0]);

	pMesh->render();

	pGroundTexture->bindTexture(GL_TEXTURE0);
	g_RenderShader->setTextureUniformValue("uTextureSampler", 0);

	Pileline.setIdentity("Model");
	Pileline.translate(0.0, 2.0, 0.0);
	Pileline.rotate(g_MotionX, 1.0, 0.0, 0.0);
	Pileline.rotate(g_MotionY, 0.0, 1.0, 0);

	glm::mat4 ModelPlane = Pileline.getModelMat();
	glm::mat4 PlaneModelView = View * ModelPlane;
	glm::mat4 PlaneNormalMat = glm::transpose(glm::inverse(PlaneModelView));
	g_RenderShader->setMatUniformValue("uModel", &ModelPlane[0][0]);
	g_RenderShader->setMatUniformValue("uMVPLightSpace", &PlaneMVPLightSpace[0][0]);
	g_RenderShader->setMatUniformValue("uNormalMatEye", &PlaneNormalMat[0][0]);
	glm::mat4 CameraInserse = glm::inverse(View);

//	g_RenderShader->setMatUniformValue("uCameraInverse", &CameraInserse[0][0]);
//	g_RenderShader->setMatUniformValue("uLightViewMat", &LightViewMatrix[0][0]);
//	g_RenderShader->setMatUniformValue("uLightProjectionMatrix", &LightProjectionMatrix[0][0]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, g_PlaneVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (const GLvoid*)12);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (const GLvoid*)20);
	glDrawArrays(GL_QUADS, 0, 4);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	g_RenderShader->banShader();
}

//******************************************************************
//FUNCTION:
void renderPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (g_DepthFlag)
	{
		glLoadIdentity();
		gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		g_ShadowMapFBO.bindForReading(GL_TEXTURE0);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);glVertex2f(-1.0, -1.0);
		glTexCoord2f(1.0, 0.0);glVertex2f(1.0, -1.0);
		glTexCoord2f(1.0, 1.0);glVertex2f(1.0, 1.0);
		glTexCoord2f(0.0, 1.0);glVertex2f(-1.0, 1.0);
		glEnd();
	}
	else
	{
		generateShadow();
	}
}


//******************************************************************
//FUNCTION:
void display()
{
 	shadowMapPass();
 	renderPass();

	glutSwapBuffers();
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
		g_DepthFlag = !g_DepthFlag;
		break;
	case 'p':
		g_stopLightMove = !g_stopLightMove;
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

//******************************************************************
//FUNCTION:
void reshape(int vWidth, int vHeight)
{
	glViewport(0, 0, (GLsizei)vWidth, (GLsizei)vHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, vWidth/vHeight, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//******************************************************************
//FUNCTION:
void idleLightPositionChange()
{
	if(!g_stopLightMove)
	{
		g_angle += 3.14/180;
		if( g_angle >= 6.28f )
			g_angle = 0.0f;
	}

	PointLight.m_LightPosition.x = 10 * sin(g_angle); 
	PointLight.m_LightPosition.z = 10 * cos(g_angle);

	display();
}

int main(int argc, char** argv)
{
    Magick::InitializeMagick(*argv);    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("ShadowMap");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutIdleFunc(idleLightPositionChange);


	glutMainLoop();
	return 1;
}


#include "Shader.h"
#include "VBO.h"
#include <glm/glm.hpp>
#include "Pipeline.h"
#include "Texture.h"
#pragma comment(lib, "glew32.lib")

GLuint g_IBO;
CShader* g_Shader = new CShader;
CVBO<SVertex>* g_VBO = new CVBO<SVertex>;
SVertex VerticesData[4] = {SVertex(glm::vec3(-1.0f, -1.0f, 0.5773f), glm::vec2(0.0f, 0.0f)),
						   SVertex(glm::vec3(0.0f, -1.0f, -1.15475f), glm::vec2(0.5f, 0.0f)),
						   SVertex(glm::vec3(1.0f, -1.0f, 0.5773f), glm::vec2(1.0f, 0.0f)),
						   SVertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.5f, 1.0f)) };

glm::mat4 World;
float Scale = 0.0f;
CTexture* pTexture = NULL;

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
void createIBO()
{
	unsigned int Indices[] = { 0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2 };

	glGenBuffers(1, &g_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

//******************************************************************
//FUNCTION:
void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	initGlew();

	pTexture = new CTexture(GL_TEXTURE_2D, "../Content/test.png");
	if (!pTexture->loadImage())
		return;
	
	createIBO();
	g_VBO->createVBO(VerticesData, 4);
	//g_ColorVBO->createVBO((GLfloat**)ColorData, 12);
	g_Shader->initShader("0vertex.glsl", "0fragment.glsl");
}	

//******************************************************************
//FUNCTION:
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 	CPileline* pPileline = new CPileline;
	pPileline->lookAt(glm::vec3(0.0, 0.0, 5), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
 	//pPileline->translate(0.1, 0.2, 0.3);
 	pPileline->rotate(Scale, 0.0, 1.0, 0);
 	//pPileline->scale(0.5, 0.5, 0.5);
 	pPileline->perspective(60, 1, 1, 10);

	glm::mat4 Model = pPileline->getModelMat();
	glm::mat4 View = pPileline->getViewMat();
	glm::mat4 Project = pPileline->getProjectMat();
	glm::mat4 ProjectModelView = Project * View * Model;

	g_Shader->useShader();
	g_Shader->setMatUniformValue("uWorld", &ProjectModelView[0][0]);

  	g_VBO->useVBO(0, 3, 0);
	g_VBO->useVBO(1, 2, (const void*)12);
	//g_ColorVBO->useVBO(1);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IBO);
	pTexture->bindTexture(GL_TEXTURE0);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
 	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
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

int main(int argc, char** argv)
{
    Magick::InitializeMagick(*argv);    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Perspective");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	//glutReshapeFunc(reshape);
	glutMainLoop();
	
	return 1;
}


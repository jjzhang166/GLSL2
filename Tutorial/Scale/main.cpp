#include "Shader.h"
#include "VBO.h"
#include <glm/glm.hpp>

#pragma comment(lib, "glew32.lib")

CShader* g_Shader = new CShader;
CVBO<GLfloat>* g_VBO = new CVBO<GLfloat>;
CVBO<GLfloat>* g_ColorVBO = new CVBO<GLfloat>;
GLfloat VerticesData[][3] = {-1.0f, -1.0f, 0.0f,
					      1.0f, -1.0f, 0.0f,
						  0.0f, 1.0f, 0.0f};

GLfloat ColorData[][3] = {1.0f, 0.0f, 0.0f,
						  0.0f, 1.0f, 0.0f,
						  0.0f, 0.0f, 1.0f };

glm::mat4 World;
float Scale = 0.0f;

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
void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	initGlew();

	g_VBO->createVBO((GLfloat**)VerticesData, 9);
	g_ColorVBO->createVBO((GLfloat**)ColorData, 9);
	g_Shader->initShader("0vertex.glsl", "0fragment.glsl");
}

//******************************************************************
//FUNCTION:
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	World[0][0] = sinf(Scale); World[0][1] = 0.0f; World[0][2] = 0.0f; World[0][3] = 0.0f;
	World[1][0] = 0.0f; World[1][1] = sinf(Scale); World[1][2] = 0.0f; World[1][3] = 0.0f;
	World[2][0] = 0.0f; World[2][1] = 0.0f; World[2][2] = sinf(Scale); World[2][3] = 0.0f;
	World[3][0] = 0.0f; World[3][1] = 0.0f; World[3][2] = 0.0f; World[3][3] = 1.0f;
  	
	g_Shader->useShader();
	g_Shader->setMatUniformValue("uWorld", &World[0][0]);
  	g_VBO->useVBO(0);
	g_ColorVBO->useVBO(1);
 	glDrawArrays(GL_TRIANGLES, 0, 3);
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
	gluPerspective(60, 0, 0, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//******************************************************************
//FUNCTION:
void idle()
{
	Scale += 0.01f;
	glutPostRedisplay();		
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Shader");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();
	
	return 1;
}


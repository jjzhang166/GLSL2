#include "Shader.h"
#include "VBO.h"
#include <glm/glm.hpp>
#include "Pipeline.h"
#pragma comment(lib, "glew32.lib")

GLuint g_IBO;
CShader* g_Shader = new CShader;
CVBO<GLfloat>* g_VBO = new CVBO<GLfloat>;
CVBO<GLfloat>* g_ColorVBO = new CVBO<GLfloat>;
GLfloat VerticesData[][3] = {-1.0f, -1.0f, 0.5773f,
					         0.0f, -1.0f, -1.15475f,
						     1.0f, -1.0f, 0.5773f,
						     0.0f, 1.0f, 0.0f};

GLfloat ColorData[][3] = {1.0f, 0.0f, 0.0f,
						  0.0f, 1.0f, 0.0f,
						  0.0f, 0.0f, 1.0f,
						  1.0f, 1.0f, 1.0f};

glm::mat4 World;
float Scale = 0.0f;

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
	//glEnable(GL_DEPTH_TEST);
	initGlew();

	createIBO();
	g_VBO->createVBO((GLfloat**)VerticesData, 12);
	g_ColorVBO->createVBO((GLfloat**)ColorData, 12);
	g_Shader->initShader("0vertex.glsl", "0fragment.glsl");
}

//******************************************************************
//FUNCTION:
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 	glPushMatrix();
// 	glMatrixMode(GL_MODELVIEW);
// 	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
// 	glTranslatef(0.1, 0.2, 0.3);
// 	glRotatef(30.0, 1, 2, 3);
// 	glScalef(0.5, 0.5, 0.5);
// 
// 	glMatrixMode(GL_PROJECTION);
// 	gluPerspective(60, 1, 1, 10);
// 	glOrtho(-3, 5, -3, 5, -1, 10);
// 
// 	float ModelView[16];
// 	glGetFloatv(GL_MODELVIEW_MATRIX, ModelView);
// 	PrintMatrix(ModelView);
// 
// 	glm::mat4 Projection;
// 	glGetFloatv(GL_PROJECTION_MATRIX, &Projection[0][0]);
// 	PrintMatrix(&Projection[0][0]);
// 	glPopMatrix();

 	CPileline* pPileline = new CPileline;
	pPileline->lookAt(glm::vec3(0.0, 0.0, 5), glm::vec3(0.0, 0.0, 0), glm::vec3(0.0, 1.0, 0.0));
 	pPileline->translate(0.0, 0.0, 3.0);
 	pPileline->rotate(Scale, 0.0, 1.0, 0);
 	pPileline->scale(0.5, 0.5, 0.5);
  	pPileline->perspective(60, 1, 1.0, 100);
	//pPileline->ortho(-3, 5, -3, 5, -1, 10);

	glm::mat4 Model = pPileline->getModelMat();
	glm::mat4 View = pPileline->getViewMat();
	glm::mat4 Project = pPileline->getProjectMat();
	glm::mat4 ProjectModelView = Project * View * Model;

	PrintMatrix(&ProjectModelView[0][0]);
	g_Shader->useShader();
	g_Shader->setMatUniformValue("uWorld", &ProjectModelView[0][0]);
  	g_VBO->useVBO(0);
	g_ColorVBO->useVBO(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IBO);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
 	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
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
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Perspective");
	init();
	glutDisplayFunc(display);
	//glutIdleFunc(idle);
	//glutReshapeFunc(reshape);
	glutMainLoop();
	
	return 1;
}


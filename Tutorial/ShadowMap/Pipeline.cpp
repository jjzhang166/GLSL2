#include "Pipeline.h"

CPileline::CPileline()
{
	m_ModelMat = glm::mat4(1.0);
	m_ViewMat = glm::mat4(1.0);
	m_ProjectMat = glm::mat4(1.0);
}

CPileline::~CPileline()
{
}

//******************************************************************
//FUNCTION:
void CPileline::translate(float vTranslateX, float vTranslateY, float vTranslateZ)
{
	glm::mat4 TranslationMat = glm::mat4(1.0, 0.0, 0.0, 0.0,
	 									 0.0, 1.0, 0.0, 0.0,	
	 									 0.0, 0.0, 1.0, 0.0,
	 								     vTranslateX, vTranslateY, vTranslateZ, 1.0);

	m_ModelMat = m_ModelMat * TranslationMat;
}

//******************************************************************
//FUNCTION:
void CPileline::rotate(float vAngle, float vRotateX, float vRotateY, float vRotateZ)
{
	glm::mat4 RotationMat;
	const float Angle = toRadian(vAngle);
	float Sin = sinf(Angle);
	float Cos = cosf(Angle);
	float T = 1 - Cos;

	float Length = sqrt(vRotateX*vRotateX + vRotateY*vRotateY + vRotateZ*vRotateZ);
	float X = vRotateX / Length;
	float Y = vRotateY / Length;
	float Z = vRotateZ / Length;


	RotationMat[0][0] = T * X * X + Cos;
	RotationMat[0][1] = T * X * Y + Sin * Z;
	RotationMat[0][2] = T * X * Z - Sin * Y;
	RotationMat[0][3] = 0.0f;

	RotationMat[1][0] = T * X * Y - Sin * Z;
	RotationMat[1][1] = T * Y * Y + Cos;
	RotationMat[1][2] = T * Y * Z + Sin * X;
	RotationMat[1][3] = 0.0f;

	RotationMat[2][0] = T * X * Z + Sin * Y;
	RotationMat[2][1] = T * Y * Z - Sin * X;
	RotationMat[2][2] = T * Z * Z + Cos;
	RotationMat[2][3] = 0.0f;

	RotationMat[3][0] = 0.0f;
	RotationMat[3][1] = 0.0f;
	RotationMat[3][2] = 0.0f;
	RotationMat[3][3] = 1.0f;

	m_ModelMat =  m_ModelMat * RotationMat;
}

//******************************************************************
//FUNCTION:
void CPileline::scale(float vScaleX, float vScaleY, float vScaleZ)
{
	glm::mat4 ScaleMat = glm::mat4(vScaleX, 0.0,     0.0,     0.0,
								   0.0,     vScaleY, 0.0,     0.0,	
								   0.0,     0.0,     vScaleZ, 0.0,
								   0.0,     0.0,     0.0,     1.0);

	m_ModelMat = m_ModelMat * ScaleMat;
}

//******************************************************************
//FUNCTION:
const glm::mat4& CPileline::getModelMat() const
{
	return m_ModelMat;
}

//******************************************************************
//FUNCTION:
void CPileline::lookAt(const glm::vec3& vEye, const glm::vec3& vLookat, const glm::vec3& vUp)
{
	glm::vec3 N = vLookat - vEye;
	N = glm::normalize(N);

	glm::vec3 U = glm::cross(N, vUp);
	U = glm::normalize(U);

	glm::vec3 V = glm::cross(U, N);
	V = glm::normalize(V);

	glm::mat4 ViewMat;
	ViewMat[0][0] = U[0];
	ViewMat[1][0] = U[1];
	ViewMat[2][0] = U[2];
	ViewMat[3][0] = -glm::dot(vEye, U);

	ViewMat[0][1] = V[0];
	ViewMat[1][1] = V[1];
	ViewMat[2][1] = V[2];
	ViewMat[3][1] = -glm::dot(vEye, V);

	ViewMat[0][2] = -N[0];
	ViewMat[1][2] = -N[1];
	ViewMat[2][2] = -N[2];
	ViewMat[3][2] = glm::dot(vEye, N);

	ViewMat[0][3] = 0.0;
	ViewMat[1][3] = 0.0;
	ViewMat[2][3] = 0.0;
	ViewMat[3][3] = 1.0;

	m_ViewMat = m_ViewMat * ViewMat;
}

//******************************************************************
//FUNCTION:
const glm::mat4& CPileline::getViewMat() const
{
	return m_ViewMat;
}

//******************************************************************
//FUNCTION:
void CPileline::perspective(const float& vFovy, const float& vAspect, const float& vNear, const float& vFar)
{
	glm::mat4 PerspectiveMat;
 	const float tanHalfFovy = tanf(toRadian(vFovy/2));

	_ASSERT(tanHalfFovy);
	PerspectiveMat[0][0] = 1 / (vAspect * tanHalfFovy);
	PerspectiveMat[1][0] = 0.0;
	PerspectiveMat[2][0] = 0.0;
	PerspectiveMat[3][0] = 0.0;
	
	PerspectiveMat[0][1] = 0.0;
	PerspectiveMat[1][1] = 1 / tanHalfFovy;
	PerspectiveMat[2][1] = 0.0;
	PerspectiveMat[3][1] = 0.0;

	PerspectiveMat[0][2] = 0.0;
	PerspectiveMat[1][2] = 0.0;
	PerspectiveMat[2][2] = -(vFar + vNear) / (vFar -vNear);
	PerspectiveMat[3][2] = -2 * vFar * vNear / (vFar -vNear);

	PerspectiveMat[0][3] = 0.0;
	PerspectiveMat[1][3] = 0.0;
	PerspectiveMat[2][3] = -1.0;
	PerspectiveMat[3][3] = 0.0;

	m_ProjectMat = m_ProjectMat * PerspectiveMat;
}

//******************************************************************
//FUNCTION:
const glm::mat4& CPileline::getProjectMat() const
{
	return m_ProjectMat;
}

//******************************************************************
//FUNCTION:
void CPileline::ortho(const float& vLeft, const float& vRight, const float& vBottom, const float& vTop, const float& vNear, const float& vFar)
{
	glm::mat4 OrthoMat;
	OrthoMat[0][0] = 2 / (vRight - vLeft);
	OrthoMat[1][0] = 0.0;
	OrthoMat[2][0] = 0.0;
	OrthoMat[3][0] = -(vRight + vLeft) / (vRight - vLeft);

	OrthoMat[0][1] = 0.0;
	OrthoMat[1][1] = 2 / (vTop - vBottom);
	OrthoMat[2][1] = 0.0;
	OrthoMat[3][1] = -(vTop + vBottom) / (vTop - vBottom);

	OrthoMat[0][2] = 0.0;
	OrthoMat[1][2] = 0.0;
	OrthoMat[2][2] = -2/ (vFar - vNear);
	OrthoMat[3][2] = -(vFar + vNear) / (vFar - vNear);

	OrthoMat[0][3] = 0.0;
	OrthoMat[1][3] = 0.0;
	OrthoMat[2][3] = 0.0;
	OrthoMat[3][3] = 1.0;

	m_ProjectMat = m_ProjectMat * OrthoMat;
}

//******************************************************************
//FUNCTION:
void CPileline::setIdentity(const std::string& vSig)
{
	if (vSig.compare("Model") == 0)
	{
		m_ModelMat = glm::mat4(1.0);
	}
	else if(vSig.compare("View") == 0)
	{
		m_ViewMat = glm::mat4(1.0);
	}
	else if(vSig.compare("Projection") == 0)
	{
		m_ProjectMat = glm::mat4(1.0);
	}
}

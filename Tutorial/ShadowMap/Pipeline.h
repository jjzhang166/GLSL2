#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define M_PI 3.14159265358979323846

//******************************************************************
//FUNCTION:
inline float toRadian(float vDegree)
{
	return vDegree * M_PI / 180.0;
}

//******************************************************************
//FUNCTION:
inline float toDegree(float vRadian)
{
	return vRadian * 180.0 / M_PI;
}

struct SVertex
{
	glm::vec3 m_Pos;
	glm::vec2 m_Tex;
	glm::vec3 m_Normal;
	glm::vec3 m_Tangent;

	SVertex(){}

	SVertex(const glm::vec3& vPos, const glm::vec2& vTex)
	{
		m_Pos = vPos;
		m_Tex = vTex;
		m_Normal = glm::vec3(0.0, 0.0, 0.0);
	}

	SVertex(const glm::vec3& vPos, const glm::vec2& vTex, const glm::vec3& vNormal)
	{
		m_Pos = vPos;
		m_Tex = vTex;
		m_Normal = vNormal;
	}

	SVertex(const glm::vec3& vPos, const glm::vec2& vTex, const glm::vec3& vNormal, const glm::vec3& vTangent)
	{
		m_Pos = vPos;
		m_Tex = vTex;
		m_Normal = vNormal;
		m_Tangent = vTangent;
	}
};

class CPileline
{
public:
	CPileline();
	~CPileline();

	void translate(float vTranslateX, float vTranslateY, float vTranslateZ);
	void rotate(float vAngle, float vRotateX, float vRotateY, float vRotateZ);
	void scale(float vScaleX, float vScaleY, float vScaleZ);
	void lookAt(const glm::vec3& vEye, const glm::vec3& vLookat, const glm::vec3& vUp);
	void perspective(const float& vFovy, const float& vAspect, const float& vNear, const float& vFar);
	void ortho(const float& vLeft, const float& vRight, const float& vBottom, const float& vTop, const float& vNear, const float& vFar);
	void setIdentity(const std::string& vSig);

	const glm::mat4& getModelMat() const;
	const glm::mat4& getViewMat() const;
	const glm::mat4& getProjectMat() const;

private:
	glm::mat4 m_ModelMat;
	glm::mat4 m_ViewMat;
	glm::mat4 m_ProjectMat;
};
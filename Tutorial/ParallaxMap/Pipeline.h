#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define M_PI 3.14159265358979323846


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

	const glm::mat4& getModelMat() const;
	const glm::mat4& getViewMat() const;
	const glm::mat4& getProjectMat() const;

private:
	glm::mat4 m_ModelMat;
	glm::mat4 m_ViewMat;
	glm::mat4 m_ProjectMat;
};


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

inline void computeTangent(std::vector<SVertex>& vVertices, unsigned int vIndives[])
{
	for (unsigned int i = 0 ; i < 6; i += 3) {
		SVertex& v0 = vVertices[vIndives[i]];
		SVertex& v1 = vVertices[vIndives[i+1]];
		SVertex& v2 = vVertices[vIndives[i+2]];

		glm::vec3 Edge1 = v1.m_Pos - v0.m_Pos;
		glm::vec3 Edge2 = v2.m_Pos - v0.m_Pos;

		float DeltaU1 = v1.m_Tex.x - v0.m_Tex.x;
		float DeltaV1 = v1.m_Tex.y - v0.m_Tex.y;
		float DeltaU2 = v2.m_Tex.x - v0.m_Tex.x;
		float DeltaV2 = v2.m_Tex.y - v0.m_Tex.y;

		float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

		glm::vec3 Tangent, Bitangent;

		Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
		Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
		Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

		Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
		Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
		Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);

		v0.m_Tangent += Tangent;
		v1.m_Tangent += Tangent;
		v2.m_Tangent += Tangent;
	}

	for (unsigned int i = 0 ; i < vVertices.size() ; i++) {
		vVertices[i].m_Tangent = glm::normalize(vVertices[i].m_Tangent);
	}
}
#pragma once
#include <glm/glm.hpp>

struct SBaseLight 
{
	glm::vec3 m_AmbientColor;
	glm::vec3 m_DiffuseColor;
	glm::vec3 m_SpecularColor;

	SBaseLight()
	{
		m_AmbientColor = glm::vec3(0.0);
		m_DiffuseColor = glm::vec3(0.0);
		m_SpecularColor = glm::vec3(0.0);
	}
};

struct SDirectionalLight : public SBaseLight
{
	glm::vec3 m_Direction;

	SDirectionalLight()
	{
		m_Direction = glm::vec3(0.0);
	}
};

struct SPointLight : public SBaseLight
{
	glm::vec3 m_LightPosition;

	struct
	{
		float m_Constant;
		float m_Linear;
		float m_Quadratic;
	}Attenuation;

	SPointLight()
	{
		m_LightPosition = glm::vec3(0.0);
		Attenuation.m_Constant = 1.0;
		Attenuation.m_Linear = 0.0;
		Attenuation.m_Quadratic = 0.0;
	}
};

struct SSpotLight : public SPointLight
{
	glm::vec3 m_Direction;
	float m_Cutoff;

	SSpotLight()
	{
		m_Direction = glm::vec3(0.0, 0.0, 0.0);
		m_Cutoff = 0.0; 
	}
};
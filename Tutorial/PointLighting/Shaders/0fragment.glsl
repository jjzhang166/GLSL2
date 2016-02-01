#version 400

in vec2 TexCoord;
in vec3 NormalE;
in vec3 PositionE;

out vec4 FragColor;

struct SBaseLight 
{
	vec3 m_AmbientColor;
	vec3 m_DiffuseColor;
	vec3 m_SpecularColor;
};

struct SAttenuation
{
	float m_Constant;
	float m_Linear;
	float m_Quadratic;
};

struct SPointLight
{
	SBaseLight m_BaseLight;
	vec3 m_LightPosition;
	SAttenuation m_Attenuation;
};

uniform SPointLight uPointLight;
uniform sampler2D uSampler;
uniform float uShininess = 128;
uniform mat4 uViewMat;

//******************************************************************
//FUNCTION:
vec4 computeLightInternal(in SBaseLight vBaseLight, in vec3 vLightDirection, in vec3 vNormal)
{
	vec4 AmbientColor = vec4(vBaseLight.m_AmbientColor, 1.0);
	vec4 DiffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 SpecularColor = vec4(0.0, 0.0, 0.0, 0.0);
	
	vec3 LightDirection = normalize(-vLightDirection);
	float DiffuseFactor = dot(normalize(vNormal), LightDirection);
	if (DiffuseFactor > 0.0)
	{
		DiffuseColor = vec4(vBaseLight.m_DiffuseColor, 1.0) * DiffuseFactor;
		
		vec3 ReflectLight = normalize(reflect(LightDirection, normalize(vNormal)));	

		float SpecularFactor = dot(normalize(-PositionE), ReflectLight);
		if (SpecularFactor > 0.0)
		{
			SpecularColor = vec4(pow(SpecularFactor, uShininess) * vBaseLight.m_SpecularColor, 1.0);
		}
	}

	return AmbientColor + DiffuseColor + SpecularColor;
}

void main()
{
	vec3 Normal = normalize(NormalE);

	vec3 LightPositionE = (uViewMat * vec4(uPointLight.m_LightPosition, 1.0)).xyz;
	//vec3 LightPositionE = uPointLight.m_LightPosition;
	vec3 LightDirection = PositionE - LightPositionE;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	vec4 LightColor;
	LightColor = computeLightInternal(uPointLight.m_BaseLight, LightDirection, Normal);
	float Attenutation = uPointLight.m_Attenuation.m_Constant + uPointLight.m_Attenuation.m_Linear * Distance +  uPointLight.m_Attenuation.m_Quadratic * Distance * Distance;
	
	if (Attenutation != 0)
		FragColor = LightColor / Attenutation * texture2D(uSampler, TexCoord.xy);
	else
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
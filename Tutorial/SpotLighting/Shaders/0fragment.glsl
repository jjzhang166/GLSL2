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

struct SSpotLight
{
	SPointLight m_PointLight;
	vec3 m_Direction;
	float m_Cutoff;
};

uniform SSpotLight uSpotLight;
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

//******************************************************************
//FUNCTION:
vec4 computePointLight(in SPointLight vPointLight, in vec3 vNormal)
{
	vec3 LightPositionE = (uViewMat * vec4(vPointLight.m_LightPosition, 1.0)).xyz;
	
	vec3 LightDirection = PositionE - LightPositionE;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	vec4 LightColor;
	LightColor = computeLightInternal(vPointLight.m_BaseLight, LightDirection, vNormal);
	float Attenutation = vPointLight.m_Attenuation.m_Constant + vPointLight.m_Attenuation.m_Linear * Distance +  vPointLight.m_Attenuation.m_Quadratic * Distance * Distance;

	if (Attenutation != 0)
		LightColor = LightColor / Attenutation * texture2D(uSampler, TexCoord.xy);
	else
		LightColor = vec4(0.0, 0.0, 0.0, 1.0);

	return LightColor;
}

//******************************************************************
//FUNCTION:
vec4 computeSpotLight(in SSpotLight vSpotLight, in vec3 vNormal)
{
	vec3 LightPositionE = (uViewMat * vec4(vSpotLight.m_PointLight.m_LightPosition, 1.0)).xyz;
	
	vec3 LightToPixel = PositionE - LightPositionE;
	float SpotFactor = dot(normalize(LightToPixel), normalize(vSpotLight.m_Direction));

	if (SpotFactor > vSpotLight.m_Cutoff)
	{
		vec4 Color = computePointLight(vSpotLight.m_PointLight, vNormal);
		
	//	return Color;
		return Color * (1.0 - (1.0 - SpotFactor) * 1.0 / (1.0 - vSpotLight.m_Cutoff));
	}
	else
		return vec4(0.0, 0.0, 0.0, 0.0);
}

void main()
{
	vec3 Normal = normalize(NormalE);
	vec4 AmbientColor = texture2D(uSampler, TexCoord) * vec4(0.2, 0.2, 0.2, 1.0);
	FragColor = AmbientColor + computeSpotLight(uSpotLight, Normal);
}
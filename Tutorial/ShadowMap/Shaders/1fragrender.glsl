#version 400

in vec4 PositionLightSpace;
in vec3 NormalEye;
in vec2 TexCoord;
in vec3 PositionE;
in mat4 ViewE;
in vec4 PositionL;
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
uniform sampler2D uShadowMapSampler;
uniform sampler2D uTextureSampler;
uniform float uShininess = 128;
uniform mat4 uLightProjectionMatrix;

//******************************************************************
//FUNCTION:
float computeShadowFactor()
{
	vec4 ShadowMapCoord = uLightProjectionMatrix * PositionL;
	ShadowMapCoord.xy = ShadowMapCoord.xy / ShadowMapCoord.w * 0.5 + 0.5;
	
	//vec3 ProjCoords = PositionLightSpace.xyz / PositionLightSpace.w;
	//vec2 UVCoords;
	//UVCoords.x = 0.5 * ProjCoords.x + 0.5;
	//UVCoords.y = 0.5 * ProjCoords.y + 0.5;
	//float Z = 0.5 * ProjCoords.z + 0.5;
	float DepthL = texture(uShadowMapSampler, ShadowMapCoord.xy).x;
	
	//FragColor = texture(uShadowMapSampler, UVCoords.xy);
	//FragColor = vec4(DepthL, DepthL, DepthL, 0);
	
	if(-PositionL.z < DepthL + 0.1)
	{
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		return 1.0;
	}
	else
	{
		FragColor = vec4(1.0, 1.0, 0, 0);
		return 0.0;
	}	
	//FragColor = vec4(DepthL-999.5, 0.0, 0.0, 1.0);
	//return 1.0;
}

//******************************************************************
//FUNCTION:
vec4 computeLightInternal(in SBaseLight vBaseLight, in vec3 vLightDirection, in vec3 vNormal, in float vShadowFactor)
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

	return AmbientColor + vShadowFactor * (DiffuseColor + SpecularColor);
}

void main()
{
	vec3 Normal = normalize(NormalEye);

	vec3 LightPositionE = (ViewE * vec4(uPointLight.m_LightPosition, 1.0)).xyz;
	vec3 LightDirection = PositionE - LightPositionE;
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);

	float ShadowFactor = computeShadowFactor();
	vec4 LightColor;
	
	LightColor = computeLightInternal(uPointLight.m_BaseLight, LightDirection, Normal, ShadowFactor);
	float Attenutation = uPointLight.m_Attenuation.m_Constant + uPointLight.m_Attenuation.m_Linear * Distance +  uPointLight.m_Attenuation.m_Quadratic * Distance * Distance;

	if (Attenutation != 0)
		FragColor = LightColor / Attenutation * texture(uTextureSampler, TexCoord.xy);
	else
		FragColor = texture(uTextureSampler, TexCoord)* vec4(1.0);
//
////	FragColor = texture(uTextureSampler, TexCoord.xy);
//	if (ShadowFactor > 1.0)
//		FragColor =  texture(uTextureSampler, TexCoord.xy);
//	else
//		FragColor =  texture(uTextureSampler, TexCoord.xy);
	//FragColor =  texture(uTextureSampler, TexCoord.xy);
	//ShadowFactor = 1.0;
	//FragColor = vec4(ShadowFactor, 0.0, 0.0, 1.0);
	//FragColor =  LightColor;
}
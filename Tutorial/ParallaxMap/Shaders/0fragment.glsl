#version 400 compatibility

in vec3 PositionW;
in vec3 PositionE;
in vec2 TexCoord;
in vec3 NormalW;
in vec3 TangentW;

out vec4 FragColor;

struct SBaseLight 
{
	vec3 m_AmbientColor;
	vec3 m_DiffuseColor;
	vec3 m_SpecularColor;
};

struct SDirectionalLight
{
	SBaseLight m_BaseLight;
	vec3 m_Direction;
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

uniform SDirectionalLight uDirectionalLight;
uniform SPointLight uPointLight;
uniform SSpotLight uSpotLight;
uniform float uShininess = 128;

uniform sampler2D uColorSampler;
uniform sampler2D uNormalMapSampler;
uniform sampler2D uHeightMap;

uniform mat4 uViewMat;
uniform vec3 uViewPos;
uniform float Height_Scale = 0.1;

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
vec4 computeDirectionalLight(in vec3 vNormal)
{
	return computeLightInternal(uDirectionalLight.m_BaseLight, uDirectionalLight.m_Direction, vNormal);
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
		LightColor = LightColor / Attenutation;
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
		
		return Color * (1.0 - (1.0 - SpotFactor) * 1.0 / (1.0 - vSpotLight.m_Cutoff));
	}
	else
		return vec4(0.0, 0.0, 0.0, 1.0);
}

//******************************************************************
//FUNCTION:
vec3 normalMap(in vec2 vTexCoord, in mat3 vTBN)
{
	vec3 NormalMap = texture(uNormalMapSampler, vTexCoord).xyz;
	NormalMap = 2 * NormalMap - vec3(1.0, 1.0, 1.0);
	vec3 NormalW = normalize(vTBN * NormalMap);

	return NormalW;
}

//******************************************************************
//FUNCTION:
vec2 parallaxMap(in vec2 vTexCoord, in vec3 vViewDir)
{	
	float Height = texture(uHeightMap, vTexCoord).r;
	
	return vTexCoord - (Height * Height_Scale) * vViewDir.xy / vViewDir.z;
}

void main()
{
	vec3 Normal = normalize(NormalW);
	vec3 Tangent = normalize(TangentW);
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);	
	vec3 Bitangent = cross(Tangent, Normal);
	mat3 TBN = mat3(Tangent, Bitangent, Normal);

	vec3 ViewPosT = TBN * uViewPos;
	vec3 PositionT = TBN * PositionW;
	vec3 ViewDir = normalize(ViewPosT - PositionT);

	vec2 NewTexCoord = parallaxMap(TexCoord, ViewDir);

	vec3 NormalW = normalMap(NewTexCoord, TBN);
	Normal = normalize(uViewMat * vec4(NormalW, 1.0)).xyz;;

	vec4 TotalLight = computeDirectionalLight(Normal);                                         
    TotalLight += computePointLight(uPointLight, Normal);                                                                                    
	TotalLight += computeSpotLight(uSpotLight, Normal);
/*    for (int i = 0 ; i < uNumPointLights ; i++) {                                           
        TotalLight += computePointLight(uPointLights[i], Normal);                              
    }                                                                                       
                                                                                            
    for (int i = 0 ; i < uNumSpotLights ; i++) {                                            
        TotalLight += computeSpotLight(uSpotLights[i], Normal);                                
    } */                                                                                      
                                                                                            
    FragColor = texture(uColorSampler, TexCoord.xy) * TotalLight;
 //	FragColor =  texture(uColorSampler, TexCoord.xy) * vec4(0.0, 0.0, 1.0, 1.0);
//	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
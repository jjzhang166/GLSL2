#version 400 compatibility

in vec3 PositionE;
in vec2 TexCoord;
in vec3 NormalE;
in vec3 TangentE;

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

uniform sampler2D uColorSampler;
uniform sampler2D uNormalMapSampler;
uniform sampler2D uTrivialNormalMapSampler;

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
vec3 convertTangentNormalToWorld()
{
	vec3 Normal = normalize(NormalE);
	vec3 Tangent = normalize(TangentE);

	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);	
	vec3 Bitangent = cross(Tangent, Normal);

	vec3 NormalMap = texture(uNormalMapSampler, TexCoord).xyz;
	NormalMap = 2 * NormalMap - vec3(1.0, 1.0, 1.0);

	mat3 TBN = mat3(Tangent, Bitangent, Normal);
	//mat3 TBN = mat3(Tangent.x, Tangent.y, Tangent.z,
	//	            Bitangent.x, Bitangent.y, Bitangent.z,
	//				Normal.x, Normal.y, Normal.z);
	//mat3 TBN = mat3(Tangent.x, Bitangent.x, Normal.x,
	//	            Tangent.y, Bitangent.y, Normal.y,
	//				Tangent.z, Bitangent.z, Normal.z);
	vec3 NormalE = normalize(TBN * NormalMap);

	return NormalE;
}

void main()
{
	vec3 Normal = convertTangentNormalToWorld();
//	vec3 Normal = normalize(NormalE);
//	Normal = normalize(uViewMat * vec4(Normal, 1.0)).xyz;
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
//	FragColor =  texture(uSampler, TexCoord.xy) * vec4(0.0, 0.0, 1.0, 1.0);
}
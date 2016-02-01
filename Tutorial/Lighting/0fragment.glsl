#version 400

in vec2 TexCoord;
in vec3 EyeNormal;
in vec3 EyePosition;

out vec4 FragColor;

struct SDirectionalLight
{
	vec3 m_AmbientColor;
	vec3 m_DiffuseColor;
	vec3 m_SpecularColor;
	vec3 m_Direction;
};

uniform SDirectionalLight uDirectionalLight;
uniform sampler2D uSampler;
uniform float uShininess = 128;

void main()
{
	vec4 AmbientColor = vec4(uDirectionalLight.m_AmbientColor, 1.0);
	vec4 DiffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 SpecularColor = vec4(0.0, 0.0, 0.0, 0.0);
	
	vec3 LightDirection = normalize(uDirectionalLight.m_Direction);

	float DiffuseFactor = dot(normalize(EyeNormal), LightDirection);
	if (DiffuseFactor > 0.0)
	{
		DiffuseColor = vec4(uDirectionalLight.m_DiffuseColor, 1.0) * DiffuseFactor;
		
		vec3 ReflectLight = normalize(reflect(-LightDirection, normalize(EyeNormal)));	

		float SpecularFactor = dot(normalize(-EyePosition), ReflectLight);
		if (SpecularFactor > 0.0)
		{
			SpecularColor = vec4(pow(SpecularFactor, uShininess) * uDirectionalLight.m_SpecularColor, 1.0);
		}
	}

	if (gl_FragCoord.x < 400)
		FragColor = texture2D(uSampler, TexCoord.xy) * AmbientColor;
	else
	{
		FragColor = texture2D(uSampler, TexCoord.xy) * (DiffuseColor + SpecularColor);
	}
}
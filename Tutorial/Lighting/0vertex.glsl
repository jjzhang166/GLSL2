#version 400

layout (location = 0) in vec3 _Position;
layout (location = 1) in vec2 _TexCoord;
layout (location = 2) in vec3 _Normal;

uniform mat4 uEyeNormalMatrix;
uniform mat4 uMVP;
uniform mat4 uModelView;

out vec2 TexCoord;
out vec3 EyeNormal;
out vec3 EyePosition;

void main()
{
	TexCoord = _TexCoord;
	EyePosition = (uModelView * vec4(_Position, 1.0)).xyz; 

	EyeNormal = (uEyeNormalMatrix * vec4(_Normal, 0.0)).xyz;
	gl_Position = uMVP * vec4(_Position, 1.0);
}
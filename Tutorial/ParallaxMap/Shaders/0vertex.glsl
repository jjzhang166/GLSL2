#version 400

layout (location = 0) in vec3 _Position;
layout (location = 1) in vec2 _TexCoord;
layout (location = 2) in vec3 _Normal;
layout (location = 3) in vec3 _Tangent;

uniform mat4 uEyeNormalMatrix;
uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjectionMat;

out vec3 PositionW;
out vec3 PositionE;
out vec2 TexCoord;
out vec3 NormalW;
out vec3 TangentW;

void main()
{
	TexCoord = _TexCoord;

	PositionW = (uModelMat * vec4(_Position, 1.0)).xyz; 
	PositionE = ((uViewMat * uModelMat) * vec4(_Position, 1.0)).xyz;
	TangentW = (uModelMat * vec4(_Tangent, 0.0)).xyz;
	NormalW = (uModelMat * vec4(_Normal, 0.0)).xyz;

	gl_Position = uProjectionMat * uViewMat * uModelMat * vec4(_Position, 1.0);
}
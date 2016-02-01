#version 400 compatibility

layout (location = 0) in vec3 _Position;
layout (location = 1) in vec2 _TexCoord;
layout (location = 2) in vec3 _Normal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uMVPLightSpace;
uniform mat4 uNormalMatEye;
//uniform mat4 uCameraInverse;
uniform mat4 uLightViewMat;

out vec4 PositionLightSpace;
out vec3 NormalEye;
out vec2 TexCoord;
out vec3 PositionE;
out mat4 ViewE;
out vec4 PositionL;
//out vec3 PositionTest;

void main()
{
	gl_Position = uProjection * uView * uModel * vec4(_Position, 1.0);
//	gl_Position = gl_ModelViewProjectionMatrix * vec4(_Position, 1.0);
	PositionE = (uView * uModel * vec4(_Position, 1.0)).xyz;
	PositionLightSpace = uMVPLightSpace * vec4(_Position, 1.0);
	TexCoord = _TexCoord;
	NormalEye = (uNormalMatEye * vec4(_Normal, 0.0)).xyz;
	ViewE = uView;
	PositionL = uLightViewMat * uModel * vec4(_Position, 1.0); 
	//vec4 PositionW = uModel * vec4(_Position, 1.0);
	//PositionL = uLightViewMat * PositionW;
//	PositionTest = PositionL.xyz;
//	gl_Position = uMVPLightSpace * vec4(_Position, 1.0);
}
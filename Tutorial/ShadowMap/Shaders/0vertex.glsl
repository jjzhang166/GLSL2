#version 400

layout (location = 0) in vec3 _Position;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out float DepthL;

void main()
{
	vec4 Depth = /*uProjection **/ uView * uModel * vec4(_Position, 1.0);
	DepthL = Depth.z /*/ Depth.w*/;
	//vec4 Depth = uProjection * uView * uModel * vec4(_Position, 1.0);
	//DepthL = Depth.z / Depth.w;

	gl_Position = uProjection * uView * uModel * vec4(_Position, 1.0);
}
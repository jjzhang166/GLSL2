#version 400

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 VertexColor;

out vec3 Color;
out vec4 PositionTest;

uniform mat4 uWorld;

void main()
{
	Color = VertexColor;
	//PositionTest = uWorld * vec4(Position, 1.0);
	gl_Position = uWorld * vec4(Position, 1.0);
	PositionTest = gl_Position;
	
	//Color = (clamp(Position, 0.0, 1.0));
}

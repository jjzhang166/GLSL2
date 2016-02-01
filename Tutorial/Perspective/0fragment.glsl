#version 400

out vec4 FragColor;
in vec3 Color;

in vec4 PositionTest;

void main()
{
	float Test = PositionTest.z / PositionTest.w;
	if(PositionTest.w == 1.0)
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	else
		FragColor = vec4(PositionTest.w, PositionTest.w, PositionTest.w, 1.0);
	//FragColor = vec4(1.0);
	//FragColor = vec4(Color, 1.0);
}
#version 400

in float DepthL;
out vec4 FragColor;

void main()
{
    FragColor = vec4(-DepthL, -DepthL, -DepthL, 1.0);
	//FragColor = vec4(DepthL, DepthL, DepthL, 1.0);
}
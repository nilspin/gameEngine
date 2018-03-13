#version 330

in vec3 position;

out vec2 pos;

void main()
{
	pos = (position.xy + vec2(1,1))/2.0;
	gl_Position = vec4(position,1);
}

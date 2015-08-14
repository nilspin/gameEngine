in vec3 position;

out vec3 pos;

void main()
{
	pos = 2*vec3(position) - vec3(1,1,1);
	gl_Position = vec4(position,1);
}
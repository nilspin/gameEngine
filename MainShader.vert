in vec3 position;
in vec3 color;//uniform vec3 color;//
out vec3 Color;
out vec3 pos;

uniform mat4 MVP;

void main() 
{
	pos = position;
	Color = color;
	gl_Position = MVP*vec4(position, 1.0);
}

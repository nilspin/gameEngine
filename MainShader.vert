in vec3 position;
in vec3 color;//uniform vec3 color;//
in vec3 normal;
out vec4 Color;
out vec4 pos;
out vec4 Normal;

uniform mat4 MVP;

void main()
{
	Normal = MVP*vec4(normal, 1.0);
	pos = vec4(position,1.0);
	Color = vec4(color,1.0);
	gl_Position = MVP*vec4(position, 1.0);
}

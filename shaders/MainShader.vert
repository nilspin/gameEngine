#version 430

in vec3 position;
in vec3 color;
//in vec3 normal;

out vec3 v_position;
//out vec3 v_normal;
out vec3 v_color; //color interpolation - is this required?

void main()
{
    v_position = position;
//	v_normal = normal;
	v_color = color;
    gl_Position = vec4( position, 1 );
}

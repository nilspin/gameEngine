#version 430

in vec3 v_color;
//in vec3 v_position;	//required in world space because of distance calculation
//in vec3 v_normal;

out vec4 outColor;

void main()
{

	outColor =    vec4(v_color, 1);

}

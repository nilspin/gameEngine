#version 430

in vec3 fragColor;
//in vec3 v_position;	//required in world space because of distance calculation
//in vec3 v_normal;

out vec4 outColor;

void main()
{

	outColor =    vec4(fragColor, 1);

}

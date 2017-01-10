#version 330

uniform sampler2D sampler;
in vec2 pos;
out vec4 outColor;

void main()
{
	outColor = texture(sampler,pos);
//	float temp = normalize(texture(sampler,pos).x);
//	outColor = vec4(temp,temp,temp,1);
}

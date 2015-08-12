in vec3 pos;
in vec3 Color;
out vec4 outColor;

void main()
{
	vec3 temp = vec3(0,4,0);
	float d = distance(temp,pos);
	outColor = (10.0/(d*d))*vec4(Color, 1.0f);
}
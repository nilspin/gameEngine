uniform sampler2D sampler;
in vec3 pos;
out vec3 outColor;

void main()
{
	outColor = texture(sampler,pos.xy).xyz;
}
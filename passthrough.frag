uniform sampler2D sampler;
in vec2 pos;
out vec4 outColor;

void main()
{
	outColor = vec4(texture(sampler,pos).x,texture(sampler,pos).x,texture(sampler,pos).x,1);//vec4(0.4,0.9,1,1);
}
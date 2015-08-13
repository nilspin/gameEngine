in vec4 pos;
in vec4 Color;
in vec4 Normal;
out vec4 outColor;

void main()
{
	vec3 LightPos = vec3(4,4,0);
	float d = distance(LightPos,pos.xyz);
	float cosTheta = clamp(dot((LightPos-pos.xyz),Normal.xyz),0,1);
	outColor = cosTheta*(50.0/(d*d))*Color;
}
in vec3 Color;
in vec3 pos;	//required in world space because of distance calculation
in vec3 pos_camspace;
in vec3 Normal_camspace;
in vec3 LightPos_camspace;


out vec3 outColor;

uniform vec3 LightPosition;		//light in world space

void main()
{

	float LightPower = 100.0;
	vec3 LightColor = vec3(0.5,0.5,0.5);
	vec3 specularColor = vec3(0.6,0.4,0.2);
	vec3 ambientColor = vec3(0,0.05,0.04);

	float d = distance(LightPosition, pos);

	vec3 n = normalize(Normal_camspace);
	vec3 lightDir = normalize(LightPos_camspace - pos_camspace);

	float cosTheta = clamp(dot(lightDir,n),0,1);
	//This gives "how much the LightPos and normal affect the final color"
	
	vec3 tempDiffuseColor = vec3(0.1,0,0) + (Color*LightColor*LightPower*cosTheta)/(d*d);
	//Diffuse color

	// NOW SPECULAR PART

	vec3 incidentLight = normalize(pos_camspace - LightPos_camspace);

	vec3 R = reflect(incidentLight, n);
	//direction in which normal reflects incidentLight

	vec3 toCamera = normalize(-pos);
	//points to camera from each fragment

	float cosAlpha = clamp(dot(R, toCamera), 0, 1);	
	//how much of reflected vector reaches camera

	vec3 tempSpecularColor =  (specularColor*LightColor*LightPower*pow(cosAlpha,5))/(d*d);

	outColor =    tempSpecularColor + tempDiffuseColor ;//+

}
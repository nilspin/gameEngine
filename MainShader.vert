in vec3 position;
in vec3 color;//uniform vec3 color;//
in vec3 normal;

out vec3 Color;
out vec3 pos;	//required in world space because of distance calculation
out vec3 pos_camspace;
out vec3 Normal_camspace;
out vec3 LightPos_camspace;
out vec3 pos_lightspace;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 depthBiasMVP;
uniform vec3 LightPosition;

void main()
{	
	//position in light space
	pos_lightspace = (depthBiasMVP*vec4(position,1)).xyz;

	//position in world space
	pos = (M*vec4(position,1)).xyz;

	//position in cam space
	pos_camspace = (V*M*vec4(position,1)).xyz;

	//normal in cam space
	Normal_camspace = (V*M*vec4(normal, 1.0)).xyz;

	//light in cam space
	LightPos_camspace = (V*vec4(LightPosition,1)).xyz;

	Color = color;

	gl_Position = MVP*vec4(position, 1.0);
}

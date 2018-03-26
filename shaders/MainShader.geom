//Shader to voxelize a mesh

#version 430

layout (triangles) in;
layout (line_strip, max_vertices = 10) out;

in vec3 v_color[];
in vec3 v_position[];

//flat out vec4 BBox;
out vec3 fragColor;

uniform mat4 MVP;

//declare
void drawLineBbox(vec4, vec4);

void main()
{
	fragColor = v_color[0];

	vec4 pos[3];

	//transform vertices to clip space
	pos[0] = MVP * gl_in[0].gl_Position;
	pos[1] = MVP * gl_in[1].gl_Position;
	pos[2] = MVP * gl_in[2].gl_Position;

	vec4 BoxMin = min(min(pos[0], pos[1]), pos[2]);
	vec4 BoxMax = max(max(pos[0], pos[1]), pos[2]);

	drawLineBbox(BoxMin, BoxMax);

	
/*
	gl_Position = MVP*gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = MVP*gl_in[1].gl_Position;
	EmitVertex();

	gl_Position = MVP*gl_in[2].gl_Position;
	EmitVertex();

//	gl_Position = MVP*gl_in[0].gl_Position;
//	EmitVertex();
*/
	EndPrimitive();
}

void drawLineBbox(vec4 min, vec4 max)
{
	vec4 p0, p1, p2, p3, p4, p5, p6, p7;
	p0 = min;
	p7 = max;
	p1 = vec4(p0.x, p7.y, p0.z, p0.w);
	p2 = vec4(p7.x, p7.y, p0.z, p0.w);
	p3 = vec4(p7.x, p0.y, p0.z, p0.w);
	p4 = vec4(p7.x, p0.y, p7.z, p7.w);
	p5 = vec4(p0.x, p0.y, p7.z, p7.w);
	p6 = vec4(p0.x, p7.y, p7.z, p7.w);

	gl_Position = p0;
	EmitVertex();
	gl_Position = p1;
	EmitVertex();
	gl_Position = p2;
	EmitVertex();
	gl_Position = p3;
	EmitVertex();
	gl_Position = p0;
	EmitVertex();
	gl_Position = p5;
	EmitVertex();
	gl_Position = p6;
	EmitVertex();
	gl_Position = p7;
	EmitVertex();
	gl_Position = p4;
	EmitVertex();
	gl_Position = p5;
	EmitVertex();

}
//Shader to voxelize a mesh

#version 430

layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

in vec3 v_color[];
in vec3 v_position[];

out vec3 fragColor;

uniform mat4 MVP;

void main()
{
	fragColor = v_color[0];

	gl_Position = MVP*gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = MVP*gl_in[1].gl_Position;
	EmitVertex();

	gl_Position = MVP*gl_in[2].gl_Position;
	EmitVertex();

	gl_Position = MVP*gl_in[0].gl_Position;
	EmitVertex();
}
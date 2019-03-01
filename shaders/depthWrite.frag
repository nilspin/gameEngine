#version 440

layout(location = 0) out float fragDepth;

void main()
{
	fragDepth = gl_FragCoord.z;
}

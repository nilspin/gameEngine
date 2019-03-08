//Shader to voxelize a mesh

#version 440

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 v_color[];
in vec3 v_position[];

out vec3 f_position;
out vec3 f_color;

uniform mat4 MVPx;
uniform mat4 MVPy;
uniform mat4 MVPz;
uniform vec2 pix;

flat out int domAxis;//dominant axis

void main()
{
  vec4 p[3];
  for(int i=0; i<gl_in.length(); i++) {
    p[i] = gl_in[i].gl_Position;
  }


  vec3 faceNormal = normalize(cross(v_position[1] - v_position[0], v_position[2] - v_position[0]));
  float NdotXaxis = abs(faceNormal.x);
  float NdotYaxis = abs(faceNormal.y);
  float NdotZaxis = abs(faceNormal.z);
  mat4 proj;

  //find axis to maximize projected area of triangle
  if(NdotXaxis > NdotYaxis && NdotXaxis > NdotZaxis)
  {
    proj = MVPx;
    domAxis = 0;
  }
  if(NdotYaxis > NdotXaxis && NdotYaxis > NdotZaxis)
  {
    proj = MVPy;
    domAxis = 1;
  }
  if(NdotZaxis > NdotXaxis && NdotZaxis > NdotYaxis)
  {
    proj = MVPz;
    domAxis = 2;
  }

  //pixel size
  float pl = 1.4142135637309/pix.x;

  // Forget about conservative rasterisation for now
  // For every vertex sent in vertices
  for(int i = 0;i < gl_in.length(); i++) {
    f_position = vec4(proj * p[i]).xyz;
    f_color = v_color[i];
    gl_Position = proj * p[i];
    EmitVertex();
  }
  EndPrimitive();
}


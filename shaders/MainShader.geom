//Shader to voxelize a mesh

#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 v_color[];
in vec3 v_position[];

out vec3 f_position;
out vec3 f_color;

uniform mat4 MVPx;
uniform mat4 MVPy;
uniform mat4 MVPz;
uniform ivec3 gridDim;

flat out vec4 AABB;
flat out int domAxis;//dominant axis

void main()
{

  vec3 faceNormal = normalize(cross(v_position[1] - v_position[0], v_position[2] - v_position[0]));
  float NdotXaxis = abs(faceNormal.x);
  float NdotYaxis = abs(faceNormal.y);
  float NdotZaxis = abs(faceNormal.z);
  mat4 proj;

  //find axis to maximize projected area of triangle
  if(NdotXaxis > NdotYaxis && NdotXaxis > NdotZaxis)
  {
    proj = MVPx;
    domAxis = 1;
  }
  if(NdotYaxis > NdotXaxis && NdotYaxis > NdotZaxis)
  {
    proj = MVPy;
    domAxis = 2;
  }
  if(NdotZaxis > NdotXaxis && NdotZaxis > NdotYaxis)
  {
    proj = MVPz;
    domAxis = 3;
  }

  //pixel size
  vec2 pix = (1/gridDim.x, 1/gridDim.y);
  float pl = 1.4142135637309/gridDim.x;

  //transform to clip space
  vec3 p0 = proj*vec4(v_position[0],1.0);
  vec3 p1 = proj*vec4(v_position[1],1.0);
  vec3 p2 = proj*vec4(v_position[2],1.0);

  //calculate bounding box
  AABB.xy = p0.xy;
  AABB.zw = p0.xy;

  AABB.xy = min(p1.xy , AABB.xy);
  AABB.xy = min(p2.xy, AABB.xy);
  AABB.zw = max(p1.zw, AABB.zw);
  AABB.zw = max(p2.zw, AABB.zw);

  AABB.xy -= pix;
  AABB.zw += pix;

  //Triangle dilation
  vec3 e0 = vec3(p1.xy - p0.xy, 0);
  vec3 e1 = vec3(p2.xy - p1.xy, 0);
  vec3 e2 = vec3(p0.xy - p2.xy, 0);
  //now find perpendiculars to edges
  vec3 n0 = cross(e0,vec3(0,0,1));
  vec3 n1 = cross(e1,vec3(0,0,1));
  vec3 n2 = cross(e2,vec3(0,0,1));
  //now expand the triangle
	p0.xy = p0.xy + pl*( (e2.xy/dot(e2.xy,n0.xy)) + (e0.xy/dot(e0.xy,n2.xy)) );
	p1.xy = p1.xy + pl*( (e0.xy/dot(e0.xy,n1.xy)) + (e1.xy/dot(e1.xy,n0.xy)) );
	p2.xy = p2.xy + pl*( (e1.xy/dot(e1.xy,n2.xy)) + (e2.xy/dot(e2.xy,n1.xy)) );

  //Done. Spit out enlarged triangle.
  f_position[0] = p0;
  f_color[0] = v_color[0];
  gl_Position[0] = p0;
  EmitVertex();

  f_position[1] = p1;
  f_color[1] = v_color[1];
  gl_Position[1] = p1;
  EmitVertex();

  f_position[2] = p2;
  f_color[2] = v_color[2];
  gl_Position[2] = p2;
  EmitVertex();

}


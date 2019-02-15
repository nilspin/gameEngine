#version 430
//Shader to write to 3d texture

in vec3 f_position;
in vec3 f_color;
flat in int domAxis;  //axis which projection uses

//atomic counter
layout( binding=0, offset=0 ) uniform atomic_uint voxelFragCount;
layout( binding =1, rgb32f) uniform image3D volTexture;

layout(pixel_center_integer) in gl_FragCoord;
uniform ivec3 gridDim;

void main()
{
  ivec3 temp = ivec3( gl_FragCoord.xy, gridDim.z * gl_FragCoord.z) ;
  if(f_position.x < AABB.x || f_position.y < AABB.y || f_position.z > AABB.z || f_position.y > AABB.w)
  {
    discard;
  }
  vec4 data = vec4(1,0,0,0);
  //note how we can simply multiply with gridDim to get voxel-space coords
  uvec4 temp = vec4(gl_FragCoord.x, gl_FragCoord.y, gl_FragCoord.z*gridDim.z, 0);
  uvec4 texcoord;
	if( domAxis == 1 )
	{
	    texcoord.x = gridDim.x - temp.z;
		texcoord.z = temp.x;
		texcoord.y = temp.y;
	}
	else if( domAxis == 2 )
    {
	    texcoord.z = temp.y;
		texcoord.y = gridDim.x - temp.z;
		texcoord.x = temp.x;
	}
	else
	    texcoord = temp;

	uint idx = atomicCounterIncrement( voxelFragCount );
  imageStore(volTexture, texcoord, vec4(f_color, 0));

}

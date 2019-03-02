#version 440
//Shader to write to 3d texture
#define MAX_COLOR_VALUES 256.0

in vec3 f_position;
in vec3 f_color;
flat in vec4 f_AABB;
flat in int domAxis;  //axis which projection uses

//atomic counter
//layout( binding=0, offset=0 ) uniform atomic_uint voxelFragCount;
layout( r32ui, binding=1 ) uniform coherent volatile uimage3D volTexture;

layout(pixel_center_integer) in vec4 gl_FragCoord;

out vec4 outColor;

void imageAtomicAverage(ivec3 pos, vec4 addingColor, ivec3 gridDim)
{
  //New value to store in the image
  uint newValue = packUnorm4x8(addingColor);
  //Expected value, that data can be stored in the image
  uint expectedValue = 0;
  //Actual data in the image
  uint  actualValue;

  //Average sum of the voxel
  vec4 color;

  //Try to store
  actualValue = imageAtomicCompSwap(volTexture, pos, expectedValue, newValue);

  while(actualValue != expectedValue)
  {
    expectedValue = actualValue;

    //Unpack current average
    color = unpackUnorm4x8(actualValue);
    //...and convert back to sum
    color.a *= MAX_COLOR_VALUES;
    color.rgb *= color.a;

    //Add the current color;
    color.rgb += addingColor.rgb;
    color.a += 1.0;

    //..and normalize back
    color.rgb /= color.a;
    color.a /= MAX_COLOR_VALUES;

    newValue = packUnorm4x8(color);

    //try to store again
    actualValue = imageAtomicCompSwap(volTexture, pos, expectedValue, newValue);
  }
}

void main()
{
  ivec3 gridDim = imageSize(volTexture);

  ivec4 temp = ivec4(gl_FragCoord.x, gl_FragCoord.y, gl_FragCoord.z*gridDim.z, 0);
  if(f_position.x < f_AABB.x || f_position.y < f_AABB.y || f_position.z > f_AABB.z || f_position.y > f_AABB.w)
  {
    discard;
  }
  vec4 data = vec4(1,0,0,0);
  //note how we can simply multiply with gridDim to get voxel-space coords
  ivec4 texcoord;
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

	//uint idx = atomicCounterIncrement( voxelFragCount );
  imageStore(volTexture, texcoord.xyz, uvec4(f_color, 0));
  //imageAtomicAverage(texcoord.xyz, vec4(f_color, 0), gridDim);

  outColor = vec4(f_color, 1);
}

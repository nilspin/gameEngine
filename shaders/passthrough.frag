#version 440

layout(binding = 1, r32ui) uniform uimage3D volTexture;
in vec2 pos;
out vec4 outColor;



void main()
{
  ivec3 dim = imageSize(volTexture);
  // Sample point location and integer conversion results in range from 0 to length - 1.
  ivec2 xy = ivec2(vec2(dim.xy) * pos);

  //RGBA is in red unsigned integer channel
  uint packedValue;

  //Final color;
  vec4 color;

  //Iterate from front to back
  for(int z=0; z < dim.z; z++)  {

    //Retrieve color information
    packedValue = imageLoad(volTexture, ivec3(xy,z)).r;

    //convert it back
    color = unpackUnorm4x8(packedValue);

    //If counter is > 0, a color is found
    if(color.a > 0) {
        outColor = color;
        return;
    }
  }

  //nothing found.
  discard;



	//outColor = texture(sampler,pos);
//	float temp = normalize(texture(sampler,pos).x);
//	outColor = vec4(temp,temp,temp,1);
}

/*
//RAYMARCHER
//http://www.michaelwalczyk.com/blog/2017/5/25/ray-marching
//http://www.gamasutra.com/blogs/DavidArppe/20170405/295240/How_to_get_Stunning_Graphics_with_Raymarching_in_Games.php

float dist_from_sphere( vec3 p, vec3 center, float r)
{
  return length(p-center) - r;
}


vec3 rayMarch(vec3 origin, vec3 dir)
{
  float total_dist = 0.0;
  const int MAX_STEPS = 32;
  const float MAX_DISTANCE = 1000.0;
  const float MIN_HIT_DISTANCE = 0.001;

  for(int i=0;i<MAX_STEPS;++i)
  {
    vec3 p = origin + total_dist*dir;
    float dist_to_closest = dist_from_sphere(p, vec3(0),1.0);
    /*For voxels it'll go something like this:
    //if(isVoxelSolid(p))
    //{
    //  return VoxelColor(p);
    //}
    //
    if(dist_to_closest < MIN_HIT_DISTANCE)
    {
      return vec3(1,0,0);//return red
    }
    if(total_dist > MAX_DISTANCE)
    {
      break;
    }
    total_dist += dist_to_closest;
  }
  return texture(sampler, pos).xyz;
}

void main()
{
  vec3 camPos = vec3(0,0,-5);
  vec3 origin = camPos;
  vec2 position = 2*pos - vec2(1);
  vec3 dir = vec3(position, 1.0);
  vec3 shaded_color = rayMarch(origin, dir);
  outColor = vec4(shaded_color, 1.0);
}
*/

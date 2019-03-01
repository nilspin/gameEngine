#version 440

uniform sampler2D sampler;
in vec2 pos;
out vec4 outColor;



void main()
{
	outColor = texture(sampler,pos);
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

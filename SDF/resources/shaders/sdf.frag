#version 330 core

in vec2 vtx;
out vec4 out_color;

uniform mat4 invP;
uniform mat4 invV;
uniform vec3 eye;

float dBox(vec3 p, vec3 s)
{
	return length(max(abs(p)-s, 0.));
}
float sdSphere(vec3 p, vec3 s, float r)
{
	return length(p-s)-r;
}

float distance_scene(vec3 p)
{
    float sd = sdSphere(p, vec3(-2, 0, 0), 1);
    float bd = dBox(p-vec3(2, 0, 0), vec3(1,1,1));
    
    return min(sd, bd);
}

vec3 ray_march(vec3 ray_origin, vec3 ray_dir)
{
	const int max_it=100;
	const float max_dist=100.0;
	const float min_dist=0.001;
    
	float ray_dist = 0.0;
	vec3 color = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < max_it; ++i)
	{
    	vec3 p = ray_origin + ray_dir * ray_dist;
        float dist = distance_scene(p);

		if(dist < min_dist)
		{
			color = vec3(1.0, 0.0, 0.0);
			break;
		}

        ray_dist += dist;

		if(ray_dist > max_dist)
			break;
    }
    
    return color;
}

void main()
{
	vec4 view_point = invP * vec4(vtx, -1.0, 1.0);
	view_point /= view_point.w;
	vec3 ray_origin = vec3(invV * view_point);
	vec3 ray_dir = normalize(ray_origin-eye);

    vec3 diffuse = ray_march(ray_origin, ray_dir);

	out_color = vec4(diffuse, 1.0f);
}
#version 330 core

in vec2 vtx;
out vec4 out_color;

uniform mat4 invP;
uniform mat4 invV;
uniform vec3 eye;


struct object
{
	int operation;
	vec3 data1;
	vec3 data2;
};
uniform object scene_data[100];
uniform int draw_list = 0;
uniform vec3 light_pos;
uniform float light_rad;


uniform int s_technique = 0;
uniform int march_it = 100;
uniform float min_dist = 0.001;
uniform float max_dist = 1.0e6;
uniform int shadow_samples = 30;

// Forward Declarations
vec3 rand_ball(int s, float radius);
float map_scene(vec3 point);

// Ray Structure
struct ray
{
	vec3  o; // Ray origin
	vec3  d; // Ray direction
};

//    
//   ***   Render   ***
//
float ray_cast_light(in vec3 ray_origin, in vec3 ray_dir)
{
	vec3 ray_s = ray_origin - light_pos;
	float a = dot(ray_dir, ray_dir);
	float b = 2.0 * dot(ray_s, ray_dir);
	float c = dot(ray_s, ray_s) - light_rad*light_rad;
	float disc = b*b-4*a*c;
	if(disc < 0.0f)
		return -1.0;
	
	float t1 = (-b-sqrt(disc)) / (2.0*a);
	float t2 = (-b+sqrt(disc)) / (2.0*a);
	if(t1 < 0.0f)
		return t2 < 0.0f ? -1.0 : t2;
	return t1;
}

float calcTime(in ray r)
{
	float ray_dist = 0.0;
    for(int i = 0; i < march_it; ++i)
	{
    	vec3 p = r.o + r.d * ray_dist;
        float dist = map_scene(p);

        ray_dist += dist;
		if(dist < min_dist)
			return ray_dist;


		if(ray_dist > max_dist)
			break;
    }
    
	return -1.0;
}
vec3 calcNormal(const in vec3 point)
{
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*map_scene( point + e.xyy ) + 
					  e.yyx*map_scene( point + e.yyx ) + 
					  e.yxy*map_scene( point + e.yxy ) + 
					  e.xxx*map_scene( point + e.xxx ) );
}
float calcShadowHard(in ray r)
{
	float l_dist_2 = dot(r.d,r.d);
	r.d = normalize(r.d);
	float shad_time = calcTime(r);

	if(shad_time < 0.0)
		return 1.0;
	else if(shad_time*shad_time > l_dist_2)
		return 1.0;
	return 0.0;
}
float calcShadowMontecarlo(in ray r)
{
	if(shadow_samples == 0)
		return 1.0;

	int hit_count=0;
	for(int i =0; i < shadow_samples; ++i)
	{
		vec3 offset = i==0 ? vec3(0.0) : rand_ball(i, light_rad);
		vec3 local_d = r.d + offset;
		float l_dist_2 = dot(local_d,local_d);
		ray local_r = ray(r.o, normalize(local_d));
		float shad_time = calcTime(local_r);
		
		if(shad_time < 0.0)
			++hit_count;
		else if(shad_time*shad_time > l_dist_2)
			++hit_count;
	}
	return hit_count / shadow_samples;
}
uniform float rad0 = 0.1;
uniform float eps = 0.1;
uniform float tstep = 0.1;
float calcShadowConeTrace(in ray r)
{
	float l_dist = length(r.d);
	r.d = normalize(r.d);
	
	float shad = 1.0;
    float ray_dist = rad0;
    float prev_dist;
    
    for( int i=0; i<32; i++ )
    {
		float dist = map_scene( r.o + r.d*ray_dist );

        float y = (i==0) ? 0.0 : dist*dist/(2.0*prev_dist);
        float d = sqrt(dist*dist-y*y);
        shad = min( shad, 10.0*d/max(0.0,ray_dist-y) );
        prev_dist = dist;
        
        ray_dist += dist;
        
        if( shad<0.0001 || ray_dist>l_dist ) break;
        
    }
    return clamp( shad, 0.0, 1.0 );
}
vec3 render(in ray r)
{
	float t = calcTime(r);
	float l = ray_cast_light(r.o, r.d);
	
	bool hit_void = t < 0.0;
	bool hit_light = l > 0.0 && (l < t || hit_void);

	if(hit_light)
		return vec3(0.8,0.8,0.8);

	if(hit_void)
		return vec3(0.0);
		
    vec3 p = r.o + r.d * t;

    vec3 n = calcNormal(p);

	vec3 l_vec = light_pos-p;

	vec3 p_ext = p + n * 0.01;
	ray shad_ray = ray(p_ext, l_vec);

	float shad=1.0;
	switch (s_technique)
	{
	case 0:
		shad = calcShadowHard(shad_ray);
		break;
	case 1:
		shad = calcShadowMontecarlo(shad_ray);
		break;
	case 2:
		shad = calcShadowConeTrace(shad_ray);
		break;
	}

	vec3 l_dir = normalize(l_vec);
    vec3  hal = normalize( l_dir-r.d );
	const float spe_exp = 64.0;


    float dif = shad * max( dot( n, l_dir ), 0.0 );
	float spe = shad * pow( max( dot( n, hal ), 0.0),spe_exp);

	const vec3 mate = vec3(0.3,0.4,0.7);
	vec3 col = mate * dif + spe;
	return vec3(col);
}

float seed;
void main()
{
	vec4 view_point = invP * vec4(vtx, -1.0, 1.0);
	view_point /= view_point.w;
    seed = sin(vtx.x*114.0)*sin(vtx.y*211.1);

	vec3 ray_origin = vec3(invV * view_point);
	vec3 ray_dir = normalize(ray_origin-eye);
	ray r = ray(ray_origin,ray_dir);

    vec3 diffuse = render(r);
	out_color = vec4(diffuse, 1.0f);
}



//    
//   ***   CSG Scene   ***
// 

// Distance functions
float sdSphere(in vec3 point, in float rad)
{
	return length(point)-rad;
} 
float sdBox(in vec3 point, in vec3 scale)
{
    vec3 d = abs(point) - scale;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}
float sdCylinder(in vec3 point, in float height, in float rad)
{
    vec2 q = vec2( length(point.xz)-rad, abs(point.y-height*0.5)-height*0.5 );
    return min( max(q.x,q.y),0.0) + length(max(q,0.0));
}
float get_distance(in int id, in vec3 point)
{
	switch(scene_data[id].operation)
		{
		case 1:
			return sdSphere(point - scene_data[id].data1, scene_data[id].data2.x);
		case 2:
			return sdBox(point - scene_data[id].data1, scene_data[id].data2);
		case 3:
			return sdCylinder(point - scene_data[id].data1, scene_data[id].data2.x,scene_data[id].data2.y);
		default:
			break;
		}
	return 1.0/0.0;
}
// CSG Operations
float make_operation(in int id, in vec3 point)
{
	if(scene_data[id].operation < 0)
	{
		float d1 = get_distance(int(scene_data[id].data1.x)+draw_list, point);
		float d2 = get_distance(int(scene_data[id].data1.y)+draw_list, point);
		switch(scene_data[id].operation)
			{
			case -1:
				return min(d1,d2);
			case -2:
				return max(-d1,d2);
			case -3:
				return max(d1,d2);
			}
	}
	else
		return get_distance(id,point);
}
// Distance Scene Mapping
float map_scene(vec3 point)
{
	float min_dist = 1.0/0.0;;
	for(int i = 0; i < draw_list; ++i)
		min_dist=min(min_dist, make_operation(i,point));
	return min_dist;
}



//    
//   ***   Utils   ***
//

// Screen-space noise
float rand(void) { return fract(sin(seed++)*768.475278); }
// Random vector unit sphere
vec3 rand_ball(int s, float radius)
{
	vec3 rand_vec = vec3(
		rand(),
		rand(),
		rand()
	);
	rand_vec*=2.0;
	rand_vec-=1.0;
	rand_vec = normalize(rand_vec);
	float rand_rad = rand();
	rand_rad = pow(rand_rad, 1.0/3.0);
	return rand_vec*rand_rad*radius;
}
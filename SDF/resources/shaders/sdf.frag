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


uniform int march_it = 100;
uniform float min_dist = 0.001;
uniform float max_dist = 1.0e6;

uniform int s_technique = 0;
uniform int shadow_samples = 30;
uniform float rad0 = 0.1;
uniform float min_step = 0.001;
uniform float factor_step = 0.001;

uniform bool replicate = false;
uniform bool display_shad = false;

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

// Fast test for intersecting light
float ray_cast_light(in vec3 ray_origin, in vec3 ray_dir)
{
	// Compute sphere discriminant
	vec3 ray_s = ray_origin - light_pos;
	float a = dot(ray_dir, ray_dir);
	float b = 2.0 * dot(ray_s, ray_dir);
	float c = dot(ray_s, ray_s) - light_rad*light_rad;
	float disc = b*b-4*a*c;
	// If discriminant is negative there is no intersection
	if(disc < 0.0f)
		return -1.0;
	

	// Compute intersection times
	float t1 = (-b-sqrt(disc)) / (2.0*a);
	float t2 = (-b+sqrt(disc)) / (2.0*a);
	// Return minimum intersection time
	if(t1 < 0.0f)
		return t2 < 0.0f ? -1.0 : t2;
	return t1;
}

// Compute ray intersection time with the scene
float calcTime(in ray r)
{
	// Initialize ray distance
	float ray_dist = 0.0;
	// Ray march
    for(int i = 0; i < march_it; ++i)
	{
		// Find distance to the scene
    	vec3 p = r.o + r.d * ray_dist;
        float dist = map_scene(p);
		

		// Check scene intersection
		if(dist < min_dist)
			return ray_dist;
		// Advance ray
        ray_dist += dist;
		// Check for scene limit
		if(ray_dist > max_dist)
			break;
    }
	// Terminating condition
	return -1.0;
}

// Approximates the gradient
vec3 calcNormal(const in vec3 point)
{
	// Sall step differential
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
	// Average results as gradient
    return normalize( e.xyy*map_scene( point + e.xyy ) + 
					  e.yyx*map_scene( point + e.yyx ) + 
					  e.yxy*map_scene( point + e.yxy ) + 
					  e.xxx*map_scene( point + e.xxx ) );
}



//    
//   ***   Shadows   ***
//

// Simple hard shadow
float calcShadowHard(in ray r)
{
	// Get light distance
	float l_dist = length(r.d);
	// Normalize ray direction
	r.d = normalize(r.d);

	
	// Compute ray intersection time with the scene
	float shad_time = calcTime(r);
	// No Intersection
	if(shad_time < 0.0)
		return 1.0;
	// Intersection is behind the light
	else if(shad_time> l_dist)
		return 1.0;
	// Intersection is in front of light
	return 0.0;
}

// Simple Montecarlo soft shadow
float calcShadowMontecarlo(in ray r)
{
	// Initialize hit count
	float hit_count=0;


	// Raymarch multiple shadowrays
	for(int i =0; i < shadow_samples; ++i)
	{
		// Compute offset
		vec3 offset = i==0 ? vec3(0.0) : rand_ball(i, light_rad);
		// Compute new ray direction
		vec3 local_d = r.d + offset;
		// Distance to new light position
		float l_dist = length(local_d);
		// Create new ray
		ray local_r = ray(r.o, local_d/l_dist);


		// Compute ray intersection time with the scene
		float shad_time = calcTime(local_r);
		// No Intersection
		if(shad_time < 0.0)
			++hit_count;
		// Intersection is behind the light
		else if(shad_time > l_dist)
			++hit_count;
		// Intersection is in front of light
		// Count as not hitted
	}
	//Compute shadow visibility
	return hit_count / shadow_samples;
}

// Soft shadow Sphere Tracing
float calcShadowSphereTrace(in ray r)
{
	// Get light distance
	float l_dist = length(r.d);
	// Normalize ray direction
	r.d = normalize(r.d);
	// Initialize visibility factor
	float shad = 1.0;
	// Initialize previous radius
    float prev_rad = rad0;
    

	// Ray march towards the light
    for( float ray_dist = rad0; ray_dist<l_dist;)
    {
		// Get distance to the scene as radius
		// for the current sphere (dist == rad)
		float rad = map_scene( r.o + r.d*ray_dist );
		// Check for scene intersection
		if(rad < 0.0) return 0.0;
		// Compute minimum radius
		float min_rad = sqrt(rad*rad - (prev_rad-rad)*(prev_rad-rad));
		// Update minimum visibility factor
        shad = min( shad, min_rad/(light_rad / l_dist * ray_dist) );


		// Store current sphere radius
        prev_rad = rad;        
		// Advance ray
        ray_dist += max(min_step, factor_step*rad);
		// Break if full black
        if( shad<min_dist) break;
    }
    return clamp( shad, 0.0, 1.0 );
}

// Soft shadow Cone Tracing
float calcShadowConeTrace(in ray r)
{
	// Get light distance
	float l_dist = length(r.d);
	// Compute cone half_angle
	float tan_a = light_rad/l_dist;
	// Normalize ray direction
	r.d = normalize(r.d);
	// Initialize visibility factor
	float shad = 1.0;
    

	// Ray march towards the light
    for( float ray_dist = rad0; ray_dist<l_dist;)
    {
		// Compute current cone radius
		float rad = tan_a * ray_dist;
		// Get distance to the scene
		float dist = map_scene( r.o + r.d*ray_dist );
		// Get visibility ratio
		float m = dist / rad;
		// Check for full scene intersection
		if(m < -1.0)
			return 0.0;

		shad = min(shad,m);

		// Advance ray
        ray_dist += max(min_step, factor_step*abs(dist));
    }
    return smoothstep( 0.0, 1.0, 0.5+shad*0.5 );
}

vec3 render(in ray r)
{
	float t = calcTime(r);
	float l = ray_cast_light(r.o, r.d);
	
	bool hit_void = t < 0.0;
	bool hit_light = l > 0.0 && (l < t || hit_void);

	if(hit_light)
		return vec3(0.9,0.9,0.9);

	if(hit_void)
		return vec3(0.0);
		
    vec3 p = r.o + r.d * t;

    vec3 n = calcNormal(p);

	vec3 l_vec = light_pos-p;

	vec3 p_ext = p + n * 0.01;
	ray shad_ray = ray(p_ext, l_vec);

	float shad;
	switch (s_technique)
	{
	case 0:
		shad = calcShadowHard(shad_ray);
		break;
	case 1:
		shad = calcShadowMontecarlo(shad_ray);
		break;
	case 2:
		shad = calcShadowSphereTrace(shad_ray);
		break;
	case 3:
		shad = calcShadowConeTrace(shad_ray);
		break;
	}
	if(display_shad)
		return vec3(1.0-shad);

	// Lightning constants
	const vec3 mate = vec3(0.3);
	const float spe_exp = 16.0;
	const vec3 light_dif = 4.0*vec3(1.0, 0.7, 0.5);
	const vec3 light_spe = 12.0*vec3(1.0, 0.7, 0.5);

	// Do Phong Lightning
	vec3 l_dir = normalize(l_vec);
    vec3  hal = normalize( l_dir-r.d );


    float dif = shad * max( dot( n, l_dir ), 0.0 );
	float spe = pow( max( dot( n, hal ), 0.0),spe_exp)
	* dif * (0.04 + 0.96*pow( clamp(1.0+dot(hal,r.d),0.0,1.0), 5.0 ));

	vec3 col = mate * dif * light_dif + spe * light_spe
	+mate * clamp( 0.5+0.5*n.y, 0.0, 1.0 ) * vec3(0.0, 0.08, 0.1);
    col *= exp( -0.00005*t*t*t );
	return col;
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
	vec3 real_point = replicate
	? vec3(mod(point.x+5.0,10.0)-5.0,point.y,mod(point.z+5.0,10.0)-5.0)
	: point;
	float min_dist = 1.0/0.0;
	for(int i = 0; i < draw_list; ++i)
		min_dist=min(min_dist, make_operation(i,real_point));
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
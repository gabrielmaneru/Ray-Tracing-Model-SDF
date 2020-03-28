#version 330 core

in vec2 vtx;
out vec4 out_color;

uniform mat4 invP;
uniform mat4 invV;
uniform vec3 eye;



//    
//   ***   Shapes   ***
//  
float sdPlane(in float height)
{
	return abs(height);
} 
float sdSphere(in vec3 point, in float rad)
{
	return length(point)-rad;
} 
float sdBox(in vec3 point, in vec3 scale)
{
	vec3 diff = abs(point)-scale;
	return abs(min(max(diff.x,max(diff.y,diff.z)),0.0) + length(max(diff,0.0)));
}



//    
//   ***   Scene   ***
//    

struct object
{
	int type;
	float data[6];
};

float distance_scene(vec3 point)
{
	const object scene_data[] = object[](
		object(0,float[](-3,   0,0,0,0,0)),
		object(1,float[](-2,0,0,1,   0,0)),
		object(2,float[](2,0,0,1,1,1))
	);

	float min_dist = 1.0/0.0;;
	for(int i = 0; i < 3; ++i)
	{
		vec3 data_1 = vec3(
				scene_data[i].data[0],
				scene_data[i].data[1],
				scene_data[i].data[2]);
		vec3 data_2 = vec3(
				scene_data[i].data[3],
				scene_data[i].data[4],
				scene_data[i].data[5]);

		switch(scene_data[i].type)
		{
		case 0:
			min_dist=min(min_dist, sdPlane(point.y - data_1.x));
			break;
		case 1:
			min_dist=min(min_dist, sdSphere(point - data_1, data_2.x));
			break;
		case 2:
			min_dist=min(min_dist, sdBox(point - data_1, data_2));
		default:
			break;
		}
	}
	return min_dist;
}



//    
//   ***   Render   ***
//    
float calcTime(in vec3 ray_origin, in vec3 ray_dir)
{
	const int max_it=1000;
	const float max_dist=1.0e6;
	const float min_dist=0.001;
    
	float ray_dist = 0.0;
    for(int i = 0; i < max_it; ++i)
	{
    	vec3 p = ray_origin + ray_dir * ray_dist;
        float dist = distance_scene(p);

		if(dist < min_dist)
			return ray_dist;

        ray_dist += dist;

		if(ray_dist > max_dist)
			break;
    }
    
	return -1.0;
}
vec3 calcNormal(const in vec3 point)
{
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*distance_scene( point + e.xyy ) + 
					  e.yyx*distance_scene( point + e.yyx ) + 
					  e.yxy*distance_scene( point + e.yxy ) + 
					  e.xxx*distance_scene( point + e.xxx ) );
}
vec3 render(in vec3 ray_origin, in vec3 ray_dir)
{
	float t = calcTime(ray_origin, ray_dir);

	if(t < 0.0)
		return vec3(0.0);
		
    vec3 p = ray_origin + ray_dir * t;
    vec3 n = calcNormal(p);

	const vec3 mate = vec3(0.3);
	const vec3 l = normalize(vec3(-0.1, 0.3, 0.6));
	const float spe_exp = 64.0;
    vec3  hal = normalize( l-ray_dir );

	float shad = 1.0;

    float dif = shad * max( dot( n, l ), 0.0 );
	float spe = shad * pow( max( dot( n, hal ), 0.0),spe_exp);

	vec3 col = mate * dif + spe;
	return col;
}



//    
//   ***   Main   ***
// 
void main()
{
	vec4 view_point = invP * vec4(vtx, -1.0, 1.0);
	view_point /= view_point.w;
	vec3 ray_origin = vec3(invV * view_point);
	vec3 ray_dir = normalize(ray_origin-eye);

    vec3 diffuse = render(ray_origin, ray_dir);

	out_color = vec4(diffuse, 1.0f);
}
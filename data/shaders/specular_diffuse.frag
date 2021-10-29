
#version 330

in V2F{
	vec2 texcoord_0;
	vec3 normal;
	vec4 world_pos;
} v2f;

out vec4 color;

struct Per_instance
{
	// rgb = color, a = alpha
	vec4 diffuse_color;
	vec3 specular_color;
	vec3 ambient_color;
	// x = diffuse, y = specular intensity, z = specular power, w = ambient
	vec4 intensities;
	sampler2D albedo_tex;
};

struct Directional_light
{
	// rgb = color, a = intensity
	vec4 color;
	vec3 direction;
};

struct Point_light
{
	vec3 position;
	vec3 color;
	vec2 attenuation_constants;
};

struct Spot_light
{
	vec3 position;
	vec3 orientation;
	vec3 color;
	vec2 attenuation_constants;
	float cos_theta_min;
	float cos_theta_max;
};

// Note on attenuation_constants:
//
// For exponential attenuation, use vec3
// (x= -k2/rmax2), y = 1/(1-exp(-k2), z - exp(-k2)/(1-(exp(-k2))
//
// For inverse square, use vec2
// ( x = r0^2/(rmax2 - r0^2), y = rmax2)


#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

uniform Directional_light directional_light;
uniform Per_instance per_instance;
uniform vec3 camera_world_pos;

uniform Point_light[MAX_POINT_LIGHTS] point_lights;
uniform int point_light_count;

uniform Spot_light[MAX_SPOT_LIGHTS] spot_lights;
uniform int spot_light_count;

float calculate_diffuse(vec3 normal, vec3 light_direction)
{
	return clamp(dot(normal,light_direction),0,1);
}

float calculate_specular(vec3 normal, vec3 halfway, float specular_power, float nl)
{
	return pow(clamp(dot(normal,halfway),0,1), specular_power) * float(nl > 0.0);
}

float calculate_exponential_attenuation(vec3 l, vec3 attenuation_constants)
{
	float r2 = dot(l, l);
	return clamp(exp(r2 * attenuation_constants.x) * attenuation_constants.y - attenuation_constants.z, 0, 1);
}

float calculate_inverse_square_attenuation(vec3 ldir, vec2 attenuation_constants)
{
	float r2 = dot(ldir,ldir);
	return clamp(attenuation_constants.x * (attenuation_constants.y / r2 - 1.0),0,1);
}

float calculate_angular_attenuation(float cos_theta, float cos_theta_min, float cos_theta_max)
{
	float t_theta = clamp((cos_theta - cos_theta_min)/(cos_theta_max-cos_theta_min),0,1);
	return 1-t_theta;
}

vec3 calculate_light_from_direction(vec3 n, vec3 l, vec3 view)
{
	float diffuse = calculate_diffuse(n,l) * per_instance.intensities.x;
	vec3 halfway = normalize(view + l);
	float specular = calculate_specular(n, halfway, per_instance.intensities.z, dot(n, l)) * per_instance.intensities.y;
	return diffuse * per_instance.diffuse_color.rgb + specular * per_instance.specular_color;
}

void main()
{
	vec4 albedo = texture(per_instance.albedo_tex, v2f.texcoord_0);
	vec3 view = normalize(camera_world_pos - v2f.world_pos.xyz);
	vec3 n = normalize(v2f.normal);

	vec3 ldir = normalize(directional_light.direction);
	color.rgb = albedo.rgb * per_instance.diffuse_color.rgb;
	vec3 light_color = calculate_light_from_direction(n,ldir,view) * directional_light.color.rgb * directional_light.color.a;
	for(int i = 0; i < point_light_count; i++)
	{
		Point_light p = point_lights[i];
		ldir = p.position.xyz - v2f.world_pos.xyz;
		vec3 current_light_color = calculate_light_from_direction(n,normalize(ldir),view) * p.color;
		current_light_color *= calculate_inverse_square_attenuation(ldir, p.attenuation_constants);
		light_color += current_light_color;
	}
	for(int i = 0; i < spot_light_count; i++)
	{
		Spot_light light = spot_lights[i];
		ldir = light.position.xyz - v2f.world_pos.xyz;
		vec3 ldir_normalized = normalize(ldir);
		vec3 current_light_color = calculate_light_from_direction(n, ldir_normalized, view) * light.color;
		current_light_color *= calculate_inverse_square_attenuation(ldir, light.attenuation_constants);
		float cos_theta = dot(light.orientation, -ldir_normalized);
		current_light_color *= calculate_angular_attenuation(cos_theta, light.cos_theta_min, light.cos_theta_max);
		light_color += current_light_color;
	}
	light_color += per_instance.ambient_color * per_instance.intensities.w;
	color.rgb *= light_color;
	color.a = albedo.a * per_instance.diffuse_color.a;
}

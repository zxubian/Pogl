
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
	// (-k2/rmax2), 1/(1-exp(-k2), exp(-k2)/(1-(exp(-k2))
	vec3 attenuation_constants;
};

#define MAX_POINT_LIGHTS 10

uniform Directional_light directional_light;
uniform Per_instance per_instance;
uniform vec3 camera_world_pos;
uniform Point_light[MAX_POINT_LIGHTS] point_lights;

uniform int point_light_count;

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

vec3 calculate_light_from_direction(vec3 n, vec3 l, vec3 view)
{
	float diffuse = calculate_diffuse(n,l) * per_instance.intensities.x;
	vec3 halfway = normalize(view + l);
	float specular = calculate_specular(n, halfway, per_instance.intensities.z, dot(n, l)) * per_instance.intensities.y;
	return diffuse * per_instance.diffuse_color.rgb + specular * per_instance.specular_color + per_instance.ambient_color * per_instance.intensities.w;
}

void main()
{
	vec4 albedo = texture(per_instance.albedo_tex, v2f.texcoord_0);
	vec3 view = normalize(camera_world_pos - v2f.world_pos.xyz);
	vec3 n = normalize(v2f.normal);

	vec3 l = normalize(directional_light.direction);
	color.rgb = albedo.rgb * per_instance.diffuse_color.rgb;
	vec3 light_color = calculate_light_from_direction(n,l,view) * directional_light.color.rgb * directional_light.color.a;
	for(int i = 0; i < point_light_count; i++)
	{
		Point_light p = point_lights[i];
		l = normalize(p.position.xyz - v2f.world_pos.xyz);
		light_color += calculate_light_from_direction(n,l,view) * p.color;
	}
	color.rgb *= light_color;
	color.a = albedo.a * per_instance.diffuse_color.a;
}

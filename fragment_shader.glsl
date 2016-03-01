#version 330

//uv coordinates from the texture
in vec2 UV;
in vec3 original_normal_direction;
in vec3 original_vertex_position;

in vec3 normal_b_vector;
in vec3 normal_r_vector;
in vec3 normal_g_vector;

in vec3 eyedirection;
in vec3 vertex_position_worldspace;

// material uniforms
uniform float bump_value;
uniform int specular_dampening;
uniform float specular_value;
uniform vec3 specular_color;
uniform vec3 default_diffuse_color;
uniform int enable_diffuse_map;
uniform int enable_bump_map;
uniform int enable_normal_map;
uniform int enable_transparency_map;
uniform int enable_specular_map;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D bumpMap;
uniform sampler2D transparencyMap;
uniform sampler2D specularMap;

//lighting dependencies
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 light_transform;

//light parameters
vec3 light_position = vec3(200.0, 200.0, 200.0);
vec3 light_color = vec3(1.0, 1.0, 1.0);
float light_power = 1.0;
float ambient_light_value = .2;

out vec4 output_color;
float transparency;

void main()
{
	light_position = (light_transform * vec4(light_position, 1.0)).xyz;

	vec3 diffuse_color;

	if (enable_diffuse_map > 0) {
		diffuse_color = texture2D(diffuseMap, UV).rgb;
	}

	else diffuse_color = default_diffuse_color;
		
	vec3 ambient_light = diffuse_color * ambient_light_value;

	vec3 light_direction_worldspace = normalize(vertex_position_worldspace - light_position);
	vec3 translated_normal = original_vertex_position + original_normal_direction;
	translated_normal = vec3(model_matrix * vec4(translated_normal, 1.0));
	vec3 translated_vertex_position = vec3(model_matrix * vec4(original_vertex_position, 1.0));
	vec3 new_normal_direction = translated_vertex_position - translated_normal;
	float geometry_cosTheta = clamp( dot(light_direction_worldspace, normalize(new_normal_direction)), 0, 1 );
		
	vec3 modified_color = light_color * light_power * geometry_cosTheta * diffuse_color;
	vec3 calculated_normal;

	if (enable_normal_map > 0) {
		vec3 normal_color = normalize((texture2D(normalMap, UV).rgb * 2) - vec3(1.0));
		vec3 map_normal = ((normal_color.z * normal_b_vector) + (normal_color.x * normal_r_vector) + (normal_color.y * normal_g_vector)) / 3.0;

		vec3 map_translated_normal = original_vertex_position + map_normal;
		map_translated_normal = vec3(model_matrix * vec4(map_translated_normal, 1.0));
		vec3 new_map_normal_direction = translated_vertex_position - map_translated_normal;
		float map_cosTheta = clamp( dot(light_direction_worldspace, normalize(new_map_normal_direction)), 0, 1 );

		vec3 map_normal_modified_color = light_color * light_power * map_cosTheta * diffuse_color;

		modified_color = (modified_color * (1.0 - bump_value)) + (map_normal_modified_color * bump_value);
		calculated_normal = new_map_normal_direction;
	}

	else calculated_normal = new_normal_direction;

	if (enable_bump_map > 0) {
		vec3 bump_color = texture2D(bumpMap, UV).rgb;
		float bump_factor = (bump_color.x + bump_color.y + bump_color.z) / 3.0;
		modified_color = (modified_color * bump_factor) + ambient_light;
	}

	else modified_color += ambient_light;

	if (geometry_cosTheta > 0) {
		float new_specular_value = specular_value;
		if (enable_specular_map > 0) {
			vec3 specular_map_color = texture2D(specularMap, UV).rgb;
			new_specular_value = (specular_map_color.x + specular_map_color.y + specular_map_color.z) / 3.0;
		}

		vec3 reflected_light_direction = reflect(normalize(light_direction_worldspace), normalize(calculated_normal));
		float reflection_value = dot(normalize(reflected_light_direction), normalize(eyedirection));
		reflection_value = clamp(reflection_value, 0.0, 1.0);
		float damp_factor = pow(reflection_value, specular_dampening);
		vec3 final_specular = damp_factor * specular_color * new_specular_value;

		modified_color += final_specular;
	}

	modified_color.x = clamp(modified_color.x, ambient_light.x, 1.0);
	modified_color.y = clamp(modified_color.y, ambient_light.y, 1.0);
	modified_color.z = clamp(modified_color.z, ambient_light.z, 1.0);

	if (enable_transparency_map > 0)
	{
		vec3 transparency_color = texture2D(transparencyMap, UV).rgb;
		transparency = 1 - ((transparency_color.x + transparency_color.y + transparency_color.z) / 3.0);
	}

	else transparency = 0.0;

	output_color = vec4(modified_color, 1.0 - transparency);
}
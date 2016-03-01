#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 model_matrix;
uniform mat4 view_matrix;

out vec2 UV;
out vec3 original_normal_direction;
out vec3 original_vertex_position;

out vec3 normal_b_vector;
out vec3 normal_r_vector;
out vec3 normal_g_vector;

out vec3 eyedirection;
out vec3 vertex_position_worldspace;

out vec3 vertexNormal_cameraspace;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);

	vertex_position_worldspace = (model_matrix * vec4(position, 1.0)).xyz;

	UV = vertexUV;
	original_vertex_position = position;
	original_normal_direction = normal;

	normal_b_vector = normalize(normal);
	normal_r_vector = normalize(tangent);
	normal_g_vector = normalize(bitangent);

	vec3 vertex_position_cameraspace = (view_matrix * model_matrix * vec4(position, 1.0)).xyz;
	eyedirection = vec3(0.0, 0.0, 0.0) - vertex_position_cameraspace;
}
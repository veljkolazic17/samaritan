#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;

layout(set = 0, binding = 0) uniform global_uniform_object{
	mat4 projection;
	mat4 view;
	vec4 ambient_color;
	vec4 dir_light_direction;
	vec4 dir_light_color;
} global_ubo;

layout(push_constant) uniform push_constants{

	// Only guaranteed a total of 128 bytes.
	mat4 model; // 64 bytes
} u_push_constants;

layout(location = 0) out int out_mode;

// Data Transfer Object
layout(location = 1) out struct dto {
	vec2 tex_coord;
	vec3 normal;
	vec3 frag_pos;
	mat3 TBN;
} out_dto;

void main() {
	vec4 worldPos = u_push_constants.model * vec4(in_position, 1.0);
	out_dto.tex_coord = in_texcoord;
	out_dto.normal = normalize(mat3(u_push_constants.model) * in_normal);
	out_dto.frag_pos = worldPos.xyz;
	gl_Position = global_ubo.projection * global_ubo.view * worldPos;

	//Tangent, Bitangent, Normal
	vec3 T = normalize(mat3(u_push_constants.model) * in_tangent);
	vec3 N = normalize(mat3(u_push_constants.model) * in_normal);
	//Gram-Schmidt orthogonalization to ensure T, B, N are orthogonal
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	out_dto.TBN = mat3(T, B, N);
}
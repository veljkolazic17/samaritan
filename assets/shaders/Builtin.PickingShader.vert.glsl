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
	mat4 model; // 64 bytes
	uint object_id;
} u_push_constants;

void main() {
	vec4 worldPos = u_push_constants.model * vec4(in_position, 1.0);
	gl_Position = global_ubo.projection * global_ubo.view * worldPos;
}

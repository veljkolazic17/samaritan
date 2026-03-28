#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out uint out_colour;

layout(push_constant) uniform push_constants{
	mat4 model; // 64 bytes
	uint object_id;
} u_push_constants;

void main() {
	out_colour = u_push_constants.object_id;
}

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform global_uniform_object{
	mat4 projection;
	mat4 view;
} global_ubo;

const float GRID_SIZE = 100.0;

const vec3 Pos[4] = vec3[4](
	vec3(-GRID_SIZE, 0.0, -GRID_SIZE),
	vec3( GRID_SIZE, 0.0, -GRID_SIZE),
	vec3( GRID_SIZE, 0.0,  GRID_SIZE),
	vec3(-GRID_SIZE, 0.0,  GRID_SIZE)
);

const int Indices[6] = int[6](0, 2, 1, 2, 0, 3);

layout(location = 0) out vec3 world_pos;
layout(location = 1) out vec3 cam_pos;

void main()
{
	int index = Indices[gl_VertexIndex];
	vec4 vPos = vec4(Pos[index], 1.0);

	vec3 camPos = -(transpose(mat3(global_ubo.view)) * global_ubo.view[3].xyz);

	vPos.x += camPos.x;
	vPos.z += camPos.z;
	gl_Position = global_ubo.projection * global_ubo.view * vPos;

	world_pos = vPos.xyz;
	cam_pos = camPos;
}

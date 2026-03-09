#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 out_colour;

layout(set = 0, binding = 0) uniform global_uniform_object {
    mat4 projection;
    mat4 view;
    vec4 ambient_color;
    //TODO : [MATERIAL] Maybe change renderer to allocate with padding idk, but for now we can just make these vec4 and ignore the w component
    vec4 dir_light_direction; //make this vec4 because of aligment requirements, but only use xyz
    vec4 dir_light_color;
} global_ubo;

layout(set = 1, binding = 0) uniform local_uniform_object {
    vec4 diffuse_colour;
} object_ubo;

// Samplers
layout(set = 1, binding = 1) uniform sampler2D diffuse_sampler;

// Data Transfer Object
layout(location = 1) in struct dto {
    vec2 tex_coord;
    vec3 normal;
} in_dto;

void main() {
    float diffuse_intensity = max(dot(in_dto.normal, -global_ubo.dir_light_direction.xyz), 0.0);
    vec4 texColor = texture(diffuse_sampler, in_dto.tex_coord);
    vec4 ambient = vec4(vec3(global_ubo.ambient_color * object_ubo.diffuse_colour), texColor.a);
    vec4 diffuse = vec4(vec3(global_ubo.dir_light_color * diffuse_intensity), texColor.a);

    diffuse *= texColor;
    ambient *= texColor;

    out_colour = ambient + diffuse;
}
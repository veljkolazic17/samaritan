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
    float shininess;
} object_ubo;

// Samplers — packed into a single binding as an array to match Vulkan descriptor layout
// [0] = diffuse, [1] = specular, [2] = normal
layout(set = 1, binding = 1) uniform sampler2D material_samplers[3];

// Data Transfer Object
layout(location = 1) in struct dto {
    vec2 tex_coord;
    vec3 normal;
    vec3 frag_pos;
    mat3 TBN;
} in_dto;

void main() {
    //optimizations can be implemented
    //Source: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    vec3 normalMap = texture(material_samplers[2], in_dto.tex_coord).rgb;
    normalMap = normalMap * 2.0 - 1.0;
    vec3 normal = normalize(in_dto.TBN * normalMap);

    vec3 lightDir = normalize(-global_ubo.dir_light_direction.xyz);

    float diffuse_intensity = max(dot(normal, lightDir), 0.0);

    vec3 camPos = -(transpose(mat3(global_ubo.view)) * global_ubo.view[3].xyz);
    vec3 viewDir = normalize(camPos - in_dto.frag_pos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), object_ubo.shininess);

    vec4 texColor    = texture(material_samplers[0], in_dto.tex_coord);
    vec4 texSpecular = texture(material_samplers[1], in_dto.tex_coord);

    vec4 baseColor = texColor * object_ubo.diffuse_colour;

    vec4 ambient  = vec4(vec3(global_ubo.ambient_color),                baseColor.a) * baseColor;
    vec4 diffuse  = vec4(vec3(global_ubo.dir_light_color * diffuse_intensity), baseColor.a) * baseColor;
    vec4 specular = vec4(vec3(global_ubo.dir_light_color * spec),       baseColor.a) * texSpecular;

    out_colour = ambient + diffuse + specular;
}
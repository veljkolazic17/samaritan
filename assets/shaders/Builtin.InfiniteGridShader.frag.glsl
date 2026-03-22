#version 450
#extension GL_ARB_separate_shader_objects : enable

//shit taken from ogldev

layout(location = 0) in vec3 world_pos;
layout(location = 1) in vec3 cam_pos;

layout(location = 0) out vec4 out_colour;

const float GRID_SIZE = 100.0;

const float grid_cell_size = 0.025;
const float grid_min_num_pixel_cell = 2.0;
const vec4 grid_colour_thin = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 grid_colour_thick = vec4(0.0, 0.0, 0.0, 1.0);

void main()
{
	vec2 dvx = vec2(dFdx(world_pos.x), dFdy(world_pos.x));
	vec2 dvy = vec2(dFdx(world_pos.z), dFdy(world_pos.z));

	float lx = length(dvx);
	float ly = length(dvy);

	vec2 dudv = vec2(lx, ly);

	float l = length(dudv);
	float LOD = max(0.0, log2(l * grid_min_num_pixel_cell / grid_cell_size) / log2(10.0) + 1.0);

	float GridCellSizeLod0 = grid_cell_size * pow(10.0, floor(LOD));
	float GridCellSizeLod1 = GridCellSizeLod0 * 10;
	float GridCellSizeLod2 = GridCellSizeLod1 * 10;

	dudv *= 4.0;

	vec2 mod_div_dudv = mod(world_pos.xz, GridCellSizeLod0) / dudv;
	vec2 Lod0a_v = max(vec2(1.0) - abs(clamp(mod_div_dudv, 0.0, 1.0) * 2.0 - vec2(1.0)), 0.0);
	float Lod0a = max(Lod0a_v.x, Lod0a_v.y);

	mod_div_dudv = mod(world_pos.xz, GridCellSizeLod1) / dudv;
	vec2 Lod1a_v = max(vec2(1.0) - abs(clamp(mod_div_dudv, 0.0, 1.0) * 2.0 - vec2(1.0)), 0.0);
	float Lod1a = max(Lod1a_v.x, Lod1a_v.y);

	mod_div_dudv = mod(world_pos.xz, GridCellSizeLod2) / dudv;
	vec2 Lod2a_v = max(vec2(1.0) - abs(clamp(mod_div_dudv, 0.0, 1.0) * 2.0 - vec2(1.0)), 0.0);
	float Lod2a = max(Lod2a_v.x, Lod2a_v.y);

	vec4 colour;

	float LOD_fade = fract(LOD);

	if(Lod2a > 0.0)
	{
		colour = grid_colour_thick;
		colour.a *= Lod2a;
	}
	else
	{
		if(Lod1a > 0.0)
		{
			colour = mix(grid_colour_thick, grid_colour_thin, LOD_fade);
			colour.a *= Lod1a;
		}
		else
		{
			colour = grid_colour_thin;
			colour.a *= (Lod0a * (1.0 - LOD_fade));
		}
	}

	float OpacityFalloff = 1.0 - clamp(length(world_pos.xz - cam_pos.xz) / GRID_SIZE, 0.0, 1.0);
	colour.a *= OpacityFalloff;

	out_colour = colour;
}
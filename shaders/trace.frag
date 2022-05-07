/*  This file is part of vtrace.
    vtrace is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    vtrace is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with vtrace. If not, see <https://www.gnu.org/licenses/>.  */

#version 460 core

#define M_PI 3.1415926535
#define SQRT_2 1.4142135624
#define CAM_DIST 400.0

#define SKY_COLOR vec3(0.7, 0.6, 0.8)

#define MAX_DIST 100
#define MAX_ITER 100

#define CHUNK_WIDTH 8
#define CHUNK_SIZE (CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH)

#define REFLECT_MAG 0.1
#define REFLECT_POW 2.0
#define REFLECT_DAMPEN 0.5

#define VOLUMETRIC_COEFF 0.05

in vec2 position;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

uniform vec3 camera_loc;
uniform vec2 camera_rot;

uniform uint window_width;
uniform uint window_height;

uniform uint time;

uniform chunk {
    uint _chunk_data[CHUNK_SIZE];
};

uint hash(uint x) {
    x += x << 10u;
    x ^= x >>  6u;
    x += x <<  3u;
    x ^= x >> 11u;
    x += x << 15u;
    return x;
}

float rand(uint x) {
    uint h = hash(x);
    return float(h & 0x00FFFFFF) / float(0x00FFFFFF);
}

uint get_voxel(ivec3 pos) {
    if (bool(int(pos.x < 0) |
	     int(pos.y < 0) |
	     int(pos.z < 0) |
	     int(pos.x >= CHUNK_WIDTH) |
	     int(pos.y >= CHUNK_WIDTH) |
	     int(pos.z >= CHUNK_WIDTH))) return 0;
    uint index = pos.x + pos.y * CHUNK_WIDTH + pos.z * CHUNK_WIDTH * CHUNK_WIDTH;
    return _chunk_data[index];
}

void main() {
    const mat3 camera_rot_mat = mat3(cos(camera_rot[0]), 0.0, sin(camera_rot[0]), 0.0, 1.0, 0.0, -sin(camera_rot[0]), 0.0, cos(camera_rot[0])) * mat3(1.0, 0.0, 0.0, 0.0, cos(camera_rot[1]), -sin(camera_rot[1]), 0.0, sin(camera_rot[1]), cos(camera_rot[1]));
    vec3 ray_dir = camera_rot_mat * normalize(vec3(gl_FragCoord.x - window_width / 2, gl_FragCoord.y - window_height / 2, CAM_DIST));
    vec3 ray_pos = camera_loc;

    ivec3 map_pos = ivec3(floor(ray_pos));
    ivec3 ray_step = ivec3(sign(ray_dir));

    vec3 delta_dist = vec3(length(ray_dir)) / ray_dir;
    vec3 side_dist = (sign(ray_dir) * (vec3(map_pos) - ray_pos) + sign(ray_dir) * 0.5 + 0.5) * abs(delta_dist);

    vec3 hit = vec3(1.0, 1.0, 1.0);
    uint iter = 0;
    float reflectance = 1.0;
    bool hit_light = false;
    while (iter < MAX_ITER && dot(vec3(map_pos) - camera_loc, vec3(map_pos) - camera_loc) < MAX_DIST * MAX_DIST) {
	bvec3 mask = lessThanEqual(side_dist.xyz, min(side_dist.yzx, side_dist.zxy));

	side_dist += vec3(mask) * abs(delta_dist);
	map_pos += ivec3(vec3(mask)) * ray_step;
	
	uint voxel = get_voxel(map_pos);
	uint hash = iter + time + uint(gl_FragCoord.x) + window_width * uint(gl_FragCoord.y);
	if ((voxel & 0x00000001) != 0) {

	    vec3 voxel_color = vec3(
				    float(voxel >> 24) / 255.0,
				    float((voxel >> 16) & 0x000000FF) / 255.0,
				    float((voxel >> 8) & 0x000000FF) / 255.0
				    );
	    vec3 border = vec3(map_pos) + (-ray_step + 1) / 2;
	    vec3 dist_xyz = (border - ray_pos) * delta_dist * vec3(mask);
	    float dist = dist_xyz.x + dist_xyz.y + dist_xyz.z;

	    ray_pos += dist * ray_dir;
	    ray_dir *= -2 * vec3(mask) + 1;
	    vec3 offset = REFLECT_MAG * vec3(rand(hash),
					     rand(hash * 7),
					     rand(hash * 13));
	    ray_dir += pow(2.0 * offset - REFLECT_MAG, vec3(REFLECT_POW));
	    ray_dir = normalize(ray_dir);

	    map_pos = ivec3(floor(ray_pos));
	    ray_step = ivec3(sign(ray_dir));
	    
	    delta_dist = 1.0 / ray_dir;
	    side_dist = (sign(ray_dir) * (vec3(map_pos) - ray_pos) + sign(ray_dir) * 0.5 + 0.5) * abs(delta_dist);

	    if ((voxel & 0x00000002) > 0) {
		hit *= voxel_color;
		hit_light = true;
		break;
	    }
	    hit = (hit * voxel_color) * reflectance + hit * (1.0 - reflectance);
	    float scattering = 1.0 - exp(-dist * VOLUMETRIC_COEFF);
	    hit = hit * (1.0 - scattering) + SKY_COLOR * scattering;
	    reflectance *= REFLECT_DAMPEN;
	}
	++iter;
    }
    if (!hit_light)
	hit *= SKY_COLOR;

    frag_color = vec4(hit, 1.0);
    bright_color = vec4(hit, 1.0) * float(hit_light);
}

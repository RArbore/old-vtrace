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

#define SKY_COLOR vec3(0.2, 0.3, 0.8)

#define MAX_DIST 1000
#define MAX_ITER 1000

#define CHUNK_WIDTH 16
#define CHUNK_SIZE ((CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH) >> 5)

in vec2 position;

out vec4 frag_color;

uniform vec3 camera_loc;
uniform vec2 camera_rot;

uniform uint window_width;
uniform uint window_height;

uniform uint time;

layout (std140) uniform chunk {
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

bool point_in_cube(ivec3 pos) {
    if (bool(int(pos.x < 0) |
	     int(pos.y < 0) |
	     int(pos.z < 0) |
	     int(pos.x >= CHUNK_WIDTH) |
	     int(pos.y >= CHUNK_WIDTH) |
	     int(pos.z >= CHUNK_WIDTH))) return false;
	uint index = pos.x + pos.y * CHUNK_WIDTH + pos.z * CHUNK_WIDTH * CHUNK_WIDTH;
    return bool(_chunk_data[index >> 7] & (1 << (index << 27 >> 27)));
}

void main() {
    mat3 camera_rot_mat = mat3(cos(camera_rot[0]), 0.0, sin(camera_rot[0]), 0.0, 1.0, 0.0, -sin(camera_rot[0]), 0.0, cos(camera_rot[0])) * mat3(1.0, 0.0, 0.0, 0.0, cos(camera_rot[1]), -sin(camera_rot[1]), 0.0, sin(camera_rot[1]), cos(camera_rot[1]));
    vec3 ray_dir = camera_rot_mat * normalize(vec3(gl_FragCoord.x - window_width / 2, gl_FragCoord.y - window_height / 2, CAM_DIST));
    vec3 ray_pos = camera_loc;

    ivec3 map_pos = ivec3(floor(ray_pos));
    ivec3 ray_step = ivec3(sign(ray_dir));

    vec3 delta_dist = vec3(length(ray_dir)) / ray_dir;
    vec3 side_dist = (sign(ray_dir) * (vec3(map_pos) - ray_pos) + sign(ray_dir) * 0.5 + 0.5) * abs(delta_dist);

    vec3 hit = SKY_COLOR;
    uint iter = 0;
    while (iter < MAX_ITER && dot(vec3(map_pos) - camera_loc, vec3(map_pos) - camera_loc) < MAX_DIST * MAX_DIST) {
	bvec3 mask = lessThanEqual(side_dist.xyz, min(side_dist.yzx, side_dist.zxy));

	side_dist += vec3(mask) * abs(delta_dist);
	map_pos += ivec3(vec3(mask)) * ray_step;
	
	if (point_in_cube(map_pos)) {
	    vec3 border = vec3(map_pos) + (-ray_step + 1) / 2;
	    vec3 dist_xyz = (border - ray_pos) * delta_dist * vec3(mask);
	    float dist = dist_xyz.x + dist_xyz.y + dist_xyz.z;

	    ray_pos += dist * ray_dir;
	    ray_dir *= -2 * vec3(mask) + 1;
	    uint hash = iter + time + uint(gl_FragCoord.x) + window_width * uint(gl_FragCoord.y);
	    ray_dir += 0.02 * vec3(2.0 * rand(hash) - 1.0,
				   2.0 * rand(hash * 7) - 1.0,
				   2.0 * rand(hash * 13) - 1.0);
	    ray_dir = normalize(ray_dir);

	    map_pos = ivec3(floor(ray_pos));
	    ray_step = ivec3(sign(ray_dir));
	    
	    delta_dist = 1.0 / ray_dir;
	    side_dist = (sign(ray_dir) * (vec3(map_pos) - ray_pos) + sign(ray_dir) * 0.5 + 0.5) * abs(delta_dist);

	    hit *= 0.3;
	}
	++iter;
    }

    frag_color = vec4(hit, 0.0);
}

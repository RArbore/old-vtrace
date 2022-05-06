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

#define MAX_DIST 10
#define STEP_SIZE 0.001
#define STEP_SIZE_GROWTH 0.01

#define CHUNK_WIDTH 16
#define CHUNK_SIZE ((CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH) >> 5)

in vec2 position;

out vec4 frag_color;

uniform vec3 camera_loc;
uniform vec2 camera_rot;

uniform uint window_width;
uniform uint window_height;

layout (std140) uniform chunk {
    uint _chunk_data[CHUNK_SIZE];
};

bool point_in_cube(vec3 pos, float half_len) {
    vec3 abs_pos = abs(pos);
    return bool(int(abs_pos.x <= half_len) & int(abs_pos.y <= half_len) & int(abs_pos.z <= half_len));
}

void main() {
    mat3 camera_rot_mat = mat3(cos(camera_rot[0]), 0.0, sin(camera_rot[0]), 0.0, 1.0, 0.0, -sin(camera_rot[0]), 0.0, cos(camera_rot[0])) * mat3(1.0, 0.0, 0.0, 0.0, cos(camera_rot[1]), -sin(camera_rot[1]), 0.0, sin(camera_rot[1]), cos(camera_rot[1]));
    vec3 ray_dir = camera_rot_mat * normalize(vec3(gl_FragCoord.x - window_width / 2, gl_FragCoord.y - window_height / 2, CAM_DIST));
    vec3 ray_pos = camera_loc;

    vec3 cube_pos = vec3(0.0, 0.0, 0.0);
    mat3 cube_rot = camera_rot_mat;

    float hit = 0.0;
    float step_size = STEP_SIZE;
    while (dot(ray_pos - camera_loc, ray_pos - camera_loc) < MAX_DIST * MAX_DIST) {
	ray_pos += step_size * ray_dir;
	vec3 marched_ray_pos = mod(ray_pos + 0.5, 4.0) - 0.5;
	if (point_in_cube(inverse(cube_rot) * (marched_ray_pos - cube_pos), 0.1)) {
	    hit += 0.02;
	}
	step_size += STEP_SIZE * STEP_SIZE_GROWTH;
    }
    hit = min(hit, 1.0);

    frag_color = vec4((1.0 - hit) * SKY_COLOR, 0.0);
}

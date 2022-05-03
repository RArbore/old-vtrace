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

#define MAX_DIST 100
#define STEP_SIZE 0.1

in vec2 position;

out vec4 frag_color;

uniform vec3 camera_loc;
uniform mat3 camera_rot;

uniform uint window_width;
uniform uint window_height;

bool point_in_cube(vec3 pos, float half_len) {
    vec3 abs_pos = abs(pos);
    return abs_pos.x <= half_len && abs_pos.y <= half_len && abs_pos.z <= half_len;
}

void main() {
    vec3 ray_dir = camera_rot * normalize(vec3(gl_FragCoord.x - window_width / 2, gl_FragCoord.y - window_height / 2, CAM_DIST));
    vec3 ray_pos = camera_loc;

    vec3 cube_pos = vec3(4.0, 1.0, 20.0);
    mat3 cube_rot = mat3(1.0, 0.0, 0.0, 0.0, SQRT_2, -SQRT_2, 0.0, SQRT_2, SQRT_2);

    bool hit = false;
    while (dot(ray_pos - camera_loc, ray_pos - camera_loc) < MAX_DIST * MAX_DIST) {
	ray_pos += STEP_SIZE * ray_dir;
	if (point_in_cube(inverse(cube_rot) * (ray_pos - cube_pos), 1.0)) {
	    hit = true;
	    break;
	}
    }

    frag_color = hit ? vec4(0.0, 0.0, 0.0, 1.0) : vec4(SKY_COLOR, 1.0);
}

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

#define M_PI 3.1415926535897932384
#define CAM_DIST 100.0

#define SKY_COLOR vec3(0.2, 0.3, 0.8)

#define MAX_STEPS 100
#define STEP_SIZE 0.1

in vec2 position;

out vec4 frag_color;

uniform vec3 camera_loc;
uniform mat3 camera_rot;

uniform uint window_width;
uniform uint window_height;

void main() {
    vec3 ray_dir = camera_rot * normalize(vec3(gl_FragCoord.x - window_width / 2, gl_FragCoord.y - window_height / 2, CAM_DIST));
    frag_color = vec4(
		      (position[0] + 1.0) / 2.0,
		      (position[1] + 1.0) / 2.0,
		      ray_dir.z > 0.8 ? 1.0 : 0.0,
		      camera_loc[0] + camera_rot[0][0]
		      );
}

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

in vec2 tex_coords;

out vec4 frag_color;

uniform sampler2D image;
uniform float horizontal;

void main() {
    const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    const vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, tex_coords).rgb * weight[0];
    for (uint i = 1; i < 5; ++i) {
	result += texture(image, tex_coords + vec2(tex_offset.x * i * horizontal, tex_offset.y * i * (1.0 - horizontal))).rgb * weight[i];
	result += texture(image, tex_coords - vec2(tex_offset.x * i * horizontal, tex_offset.y * i * (1.0 - horizontal))).rgb * weight[i];
    }
    frag_color = vec4(result, 1.0);
}

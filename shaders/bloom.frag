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
uniform sampler2D bloom;

void main() {
    const vec3 combined = clamp(texture(image, tex_coords).rgb + texture(bloom, tex_coords).rgb, 0.0, 1.0);
    frag_color = vec4(combined, 1.0);
}

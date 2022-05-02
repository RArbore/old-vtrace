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

#define GL_GLEXT_PROTOTYPES

#include "window.h"
#include "render.h"
#include "error.h"

extern char _binary_shaders_shader_vert_start;
extern char _binary_shaders_shader_vert_end;

extern char _binary_shaders_shader_frag_start;
extern char _binary_shaders_shader_frag_end;

int create_context(GLFWwindow* window) {
    glfwMakeContextCurrent(window);

    char* shader_vert_start = &_binary_shaders_shader_vert_start;
    char* shader_vert_end = &_binary_shaders_shader_vert_end;
    size_t vert_len = (size_t) shader_vert_end - (size_t) shader_vert_start;

    printf("%.*s", (int) vert_len, shader_vert_start);

    char* shader_frag_start = &_binary_shaders_shader_frag_start;
    char* shader_frag_end = &_binary_shaders_shader_frag_end;
    size_t frag_len = (size_t) shader_frag_end - (size_t) shader_frag_start;

    printf("%.*s", (int) frag_len, shader_frag_start);

    return SUCCESS;
}

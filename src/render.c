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

static GLuint create_shader(GLenum shader_type, const char* shader_text, const int shader_len) {
    int shader_iv = 0;
    GLuint shader = glCreateShader(shader_type);

    glShaderSource(shader, 1, &shader_text, &shader_len);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_iv);
    if (!shader_iv) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &shader_iv);
	char* error_log = malloc((size_t) shader_iv * sizeof(char));

	glGetShaderInfoLog(shader, shader_iv, NULL, error_log);
	PROPAGATE(0, 0, error_log);
	
	free(error_log);
    }
    return shader;
}

int create_context(GLFWwindow* window) {
    glfwMakeContextCurrent(window);

    const char* shader_vert_start = &_binary_shaders_shader_vert_start;
    const char* shader_vert_end = &_binary_shaders_shader_vert_end;
    const int vert_len = (int) ((size_t) shader_vert_end - (size_t) shader_vert_start);

    const char* shader_frag_start = &_binary_shaders_shader_frag_start;
    const char* shader_frag_end = &_binary_shaders_shader_frag_end;
    const int frag_len = (int) ((size_t) shader_frag_end - (size_t) shader_frag_start);

    GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, shader_vert_start, vert_len);
    PROPAGATE(vertex_shader, ERROR, "Couldn't create vertex shader.");

    GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, shader_frag_start, frag_len);
    PROPAGATE(fragment_shader, ERROR, "Couldn't create fragment shader.");

    return SUCCESS;
}

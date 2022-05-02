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
	PROPAGATE(error_log, 0, "Couldn't allocate space for error log.");

	glGetShaderInfoLog(shader, shader_iv, NULL, error_log);
	PROPAGATE_CLEANUP_BEGIN(0, error_log);
	free(error_log);
	PROPAGATE_CLEANUP_END(0);
    }

    return shader;
}

static GLuint create_shader_program(GLuint vertex_shader, GLuint fragment_shader) {
    int shader_iv = 0;
    GLuint shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    
    glGetProgramiv(shader_program, GL_LINK_STATUS, &shader_iv);
    if (!shader_iv) {
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &shader_iv);
	char* error_log = malloc((size_t) shader_iv * sizeof(char));
	PROPAGATE(error_log, 0, "Couldn't allocate space for error log.");

	glGetProgramInfoLog(shader_program, shader_iv, NULL, error_log);
	PROPAGATE_CLEANUP_BEGIN(0, error_log);
	free(error_log);
	PROPAGATE_CLEANUP_END(0);
    }
    return shader_program;
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

    GLuint shader_program = create_shader_program(vertex_shader, fragment_shader);
    PROPAGATE(shader_program, ERROR, "Couldn't create shader program.");

    glUseProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return SUCCESS;
}

int render_frame(GLFWwindow* window) {
    glfwMakeContextCurrent(window);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);

    GLenum error = glGetError();
    PROPAGATE_CLEANUP_BEGIN(error == GL_NO_ERROR, "Encountered a GL error:");
    printf("Error code: %d\n", error);
    PROPAGATE_CLEANUP_END(ERROR);
    
    return SUCCESS;
}

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

#include "render.h"

extern char _binary_shaders_shader_vert_start;
extern char _binary_shaders_shader_vert_end;

extern char _binary_shaders_shader_frag_start;
extern char _binary_shaders_shader_frag_end;

static GLuint create_shader(GLenum shader_type, const char* shader_text, const int32_t shader_len) {
    int32_t shader_iv = 0;
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
    int32_t shader_iv = 0;
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

int32_t create_context(window_t* window) {
    glfwMakeContextCurrent(window->_glfw_window);

    const char* shader_vert_start = &_binary_shaders_shader_vert_start;
    const char* shader_vert_end = &_binary_shaders_shader_vert_end;
    const int vert_len = (int32_t) ((size_t) shader_vert_end - (size_t) shader_vert_start);

    const char* shader_frag_start = &_binary_shaders_shader_frag_start;
    const char* shader_frag_end = &_binary_shaders_shader_frag_end;
    const int frag_len = (int32_t) ((size_t) shader_frag_end - (size_t) shader_frag_start);

    GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, shader_vert_start, vert_len);
    PROPAGATE(vertex_shader, ERROR, "Couldn't create vertex shader.");

    GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, shader_frag_start, frag_len);
    PROPAGATE(fragment_shader, ERROR, "Couldn't create fragment shader.");

    GLuint shader_program = create_shader_program(vertex_shader, fragment_shader);
    PROPAGATE(shader_program, ERROR, "Couldn't create shader program.");

    glUseProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGenBuffers(1, &window->_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, window->_ubo);
    glBufferData(GL_UNIFORM_BUFFER, CHUNK_SIZE * sizeof(uint32_t), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint ubo_index = glGetUniformBlockIndex(shader_program, "chunk");
    PROPAGATE(ubo_index != GL_INVALID_INDEX, ERROR, "Couldn't find chunk uniform buffer.");
    glUniformBlockBinding(shader_program, ubo_index, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, window->_ubo); 

    window->_camera_loc_uniform = glGetUniformLocation(shader_program, "camera_loc");
    PROPAGATE(window->_camera_loc_uniform != -1, ERROR, "Couldn't find camera_loc uniform.");

    window->_camera_rot_uniform = glGetUniformLocation(shader_program, "camera_rot");
    PROPAGATE(window->_camera_rot_uniform != -1, ERROR, "Couldn't find camera_rot uniform.");

    window->_window_width_uniform = glGetUniformLocation(shader_program, "window_width");
    PROPAGATE(window->_window_width_uniform != -1, ERROR, "Couldn't find window_width uniform.");

    window->_window_height_uniform = glGetUniformLocation(shader_program, "window_height");
    PROPAGATE(window->_window_height_uniform != -1, ERROR, "Couldn't find window_height uniform.");

    window->_time_uniform = glGetUniformLocation(shader_program, "time");
    PROPAGATE(window->_time_uniform != -1, ERROR, "Couldn't find time uniform.");

    return SUCCESS;
}

int32_t render_frame(window_t* window) {
    glfwMakeContextCurrent(window->_glfw_window);

    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    glUniform3fv(window->_camera_loc_uniform, 1, window->_world._camera._camera_loc);
    glUniform2fv(window->_camera_rot_uniform, 1, window->_world._camera._camera_rot);
    glUniform1ui(window->_window_width_uniform, DEFAULT_WIDTH);
    glUniform1ui(window->_window_height_uniform, DEFAULT_HEIGHT);
    glUniform1ui(window->_time_uniform, (GLuint) spec.tv_nsec);

    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(GL_UNIFORM_BUFFER, window->_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, CHUNK_SIZE * sizeof(uint32_t), window->_world._chunk._chunk_data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);  

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window->_glfw_window);

    GLenum error = glGetError();
    PROPAGATE_CLEANUP_BEGIN(error == GL_NO_ERROR, "Encountered a GL error:");
    printf("Error code: 0x%x\n", error);
    PROPAGATE_CLEANUP_END(ERROR);
    
    return SUCCESS;
}

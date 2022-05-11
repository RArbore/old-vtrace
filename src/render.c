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

extern char _binary_shaders_rect_vert_start;
extern char _binary_shaders_rect_vert_end;

extern char _binary_shaders_tex_vert_start;
extern char _binary_shaders_tex_vert_end;

extern char _binary_shaders_trace_comp_start;
extern char _binary_shaders_trace_comp_end;

extern char _binary_shaders_blur_frag_start;
extern char _binary_shaders_blur_frag_end;

extern char _binary_shaders_bloom_frag_start;
extern char _binary_shaders_bloom_frag_end;

static const unsigned BLUR_ITERS = 5;

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

static GLuint create_shader_program(GLuint* shaders, unsigned num_shaders) {
    int32_t shader_iv = 0;
    GLuint shader_program = glCreateProgram();

    for (unsigned i = 0; i < num_shaders; ++i)
	glAttachShader(shader_program, shaders[i]);
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

static int32_t setup_textures(window_t* window) {
    for (unsigned i = 0; i < 4; ++i) {
	glBindTexture(GL_TEXTURE_2D, window->_trace_color_buffers[i]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, DEFAULT_WIDTH, DEFAULT_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindImageTexture(i, window->_trace_color_buffers[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
    }

    for (unsigned i = 0; i < 2; ++i) {
	glBindFramebuffer(GL_FRAMEBUFFER, window->_blur_fbos[i]);
	glBindTexture(GL_TEXTURE_2D, window->_blur_color_buffers[i]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DEFAULT_WIDTH, DEFAULT_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, window->_blur_color_buffers[i], 0);
	PROPAGATE_CLEANUP_BEGIN(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Blurring framebuffer not complete.");
	printf("Framebuffer number that's not complete: %u\n", i);
	PROPAGATE_CLEANUP_END(ERROR);
    }

    return SUCCESS;
}

int32_t create_context(window_t* window) {
    glfwMakeContextCurrent(window->_glfw_window);

    const char* rect_vert_start = &_binary_shaders_rect_vert_start;
    const char* rect_vert_end = &_binary_shaders_rect_vert_end;
    const int rect_len = (int32_t) ((size_t) rect_vert_end - (size_t) rect_vert_start);

    const char* tex_vert_start = &_binary_shaders_tex_vert_start;
    const char* tex_vert_end = &_binary_shaders_tex_vert_end;
    const int tex_len = (int32_t) ((size_t) tex_vert_end - (size_t) tex_vert_start);

    const char* trace_comp_start = &_binary_shaders_trace_comp_start;
    const char* trace_comp_end = &_binary_shaders_trace_comp_end;
    const int trace_len = (int32_t) ((size_t) trace_comp_end - (size_t) trace_comp_start);

    const char* blur_frag_start = &_binary_shaders_blur_frag_start;
    const char* blur_frag_end = &_binary_shaders_blur_frag_end;
    const int blur_len = (int32_t) ((size_t) blur_frag_end - (size_t) blur_frag_start);

    const char* bloom_frag_start = &_binary_shaders_bloom_frag_start;
    const char* bloom_frag_end = &_binary_shaders_bloom_frag_end;
    const int bloom_len = (int32_t) ((size_t) bloom_frag_end - (size_t) bloom_frag_start);

    GLuint rect_shader = create_shader(GL_VERTEX_SHADER, rect_vert_start, rect_len);
    PROPAGATE(rect_shader, ERROR, "Couldn't create rect shader.");

    GLuint tex_shader = create_shader(GL_VERTEX_SHADER, tex_vert_start, tex_len);
    PROPAGATE(tex_shader, ERROR, "Couldn't create tex shader.");

    GLuint trace_shader = create_shader(GL_COMPUTE_SHADER, trace_comp_start, trace_len);
    PROPAGATE(trace_shader, ERROR, "Couldn't create trace shader.");

    GLuint blur_shader = create_shader(GL_FRAGMENT_SHADER, blur_frag_start, blur_len);
    PROPAGATE(blur_shader, ERROR, "Couldn't create blur shader.");

    GLuint bloom_shader = create_shader(GL_FRAGMENT_SHADER, bloom_frag_start, bloom_len);
    PROPAGATE(bloom_shader, ERROR, "Couldn't create bloom shader.");

    GLuint trace_shaders[] = {trace_shader};
    window->_trace_shader = create_shader_program(trace_shaders, 1);
    PROPAGATE(window->_trace_shader, ERROR, "Couldn't create trace shader program.");

    GLuint blur_shaders[] = {tex_shader, blur_shader};
    window->_blur_shader = create_shader_program(blur_shaders, 2);
    PROPAGATE(window->_blur_shader, ERROR, "Couldn't create blur shader program.");

    GLuint bloom_shaders[] = {tex_shader, bloom_shader};
    window->_bloom_shader = create_shader_program(bloom_shaders, 2);
    PROPAGATE(window->_bloom_shader, ERROR, "Couldn't create bloom shader program.");

    glDeleteShader(rect_shader);
    glDeleteShader(tex_shader);
    glDeleteShader(trace_shader);
    glDeleteShader(blur_shader);
    glDeleteShader(bloom_shader);

    glGenBuffers(1, &window->_voxel_ssbo);
    glGenTextures(4, window->_trace_color_buffers);
    glGenFramebuffers(2, window->_blur_fbos);
    glGenTextures(2, window->_blur_color_buffers);

    PROPAGATE(setup_textures(window) == SUCCESS, ERROR, "Couldn't setup textures.");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    window->_camera_loc_uniform = glGetUniformLocation(window->_trace_shader, "camera_loc");
    PROPAGATE(window->_camera_loc_uniform != -1, ERROR, "Couldn't find camera_loc uniform.");

    window->_camera_rot_uniform = glGetUniformLocation(window->_trace_shader, "camera_rot");
    PROPAGATE(window->_camera_rot_uniform != -1, ERROR, "Couldn't find camera_rot uniform.");

    window->_time_uniform = glGetUniformLocation(window->_trace_shader, "time");
    PROPAGATE(window->_time_uniform != -1, ERROR, "Couldn't find time uniform.");

    window->_blend_uniform = glGetUniformLocation(window->_trace_shader, "blend");
    PROPAGATE(window->_blend_uniform != -1, ERROR, "Couldn't find blend uniform.");

    window->_horizontal_uniform = glGetUniformLocation(window->_blur_shader, "horizontal");
    PROPAGATE(window->_horizontal_uniform != -1, ERROR, "Couldn't find horizontal uniform.");

    glUseProgram(window->_bloom_shader);
    GLint image_uniform = glGetUniformLocation(window->_bloom_shader, "image");
    PROPAGATE(image_uniform != -1, ERROR, "Couldn't find image uniform");
    GLint bloom_uniform = glGetUniformLocation(window->_bloom_shader, "bloom");
    PROPAGATE(bloom_uniform != -1, ERROR, "Couldn't find bloom uniform");
    glUniform1i(image_uniform, 0);
    glUniform1i(bloom_uniform, 1);

    return SUCCESS;
}

int32_t render_frame(window_t* window) {
    glfwMakeContextCurrent(window->_glfw_window);

    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    glUseProgram(window->_trace_shader);
    glUniform3fv(window->_camera_loc_uniform, 1, window->_world._camera._camera_loc);
    glUniform2fv(window->_camera_rot_uniform, 1, window->_world._camera._camera_rot);
    glUniform1ui(window->_time_uniform, (GLuint) spec.tv_nsec);
    glUniform1ui(window->_blend_uniform, (GLuint) window->_moved);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, window->_trace_color_buffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, window->_trace_color_buffers[1]);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, window->_voxel_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, CHUNK_SIZE * sizeof(uint32_t), window->_world._chunk._chunk_data, GL_STREAM_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, window->_voxel_ssbo);
    
    glDispatchCompute(DEFAULT_WIDTH / 8 + 1, DEFAULT_HEIGHT / 8 + 1, 1);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(window->_blur_shader);
    glActiveTexture(GL_TEXTURE0);
    for (unsigned i = 0; i < BLUR_ITERS * 2; ++i) {
	glBindFramebuffer(GL_FRAMEBUFFER, window->_blur_fbos[i % 2]);
	glUniform1f(window->_horizontal_uniform, (float) (i % 2));
	glBindTexture(GL_TEXTURE_2D, i ? window->_blur_color_buffers[(i + 1) % 2] : window->_trace_color_buffers[1]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(window->_bloom_shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, window->_trace_color_buffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, window->_blur_color_buffers[1]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glfwSwapBuffers(window->_glfw_window);
    CHECK_GL_ERROR();
    
    return SUCCESS;
}

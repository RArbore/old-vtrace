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

#ifndef WINDOW_H_
#define WINDOW_H_

#include <stdint.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "world.h"
#include "error.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 800

typedef struct window_t {
    GLFWwindow* _glfw_window;

    double _last_mouse_xpos;
    double _last_mouse_ypos;
    
    GLint _camera_loc_uniform;
    GLint _camera_rot_uniform;
    GLint _time_uniform;
    GLint _horizontal_uniform;

    GLuint _trace_shader;
    GLuint _blur_shader;
    GLuint _bloom_shader;

    GLuint _voxel_ssbo;
    GLuint _trace_color_buffers[2];
    GLuint _blur_fbos[2];
    GLuint _blur_color_buffers[2];

    world_t _world;
} window_t;

int32_t create_window(window_t*);

int32_t should_close(window_t*);

void destroy_window(window_t*);

#endif // WINDOW_H_

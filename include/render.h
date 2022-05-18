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

#ifndef RENDER_H_
#define RENDER_H_

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "window.h"
#include "error.h"

#define CHECK_GL_ERROR() {						\
	GLenum error = glGetError();					\
	PROPAGATE_CLEANUP_BEGIN(error == GL_NO_ERROR, "Encountered a generic GL error:"); \
	printf("Error code: 0x%x, occurred at line %d.\n", error, __LINE__); \
	PROPAGATE_CLEANUP_END(ERROR);					\
    }

int32_t create_context(window_t* window);

int32_t render_frame(window_t* window);

#endif // RENDER_H_

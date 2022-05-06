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

#include <stdlib.h>
#include <stdint.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "window.h"
#include "error.h"

int32_t create_context(window_t*);

int32_t render_frame(window_t*);

#endif // RENDER_H_

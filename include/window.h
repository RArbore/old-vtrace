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

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 800

GLFWwindow* create_window(void);

int should_close(GLFWwindow*);

void destroy_window(GLFWwindow*);

#endif // WINDOW_H_

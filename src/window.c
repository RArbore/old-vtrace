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

#include "window.h"
#include "error.h"

GLFWwindow* create_window(void) {
    PROPAGATE(glfwInit() == GLFW_TRUE, NULL, "Couldn't initialize GLFW.");

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "vtrace", NULL, NULL);
    PROPAGATE(window, NULL, "Couldn't create GLFW window.");

    glfwShowWindow(window);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);

    return window;
}

int should_close(GLFWwindow* window) {
    return glfwWindowShouldClose(window);
}

void destroy_window(GLFWwindow* window) {
    glfwDestroyWindow(window);
}

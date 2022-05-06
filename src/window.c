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

int32_t create_window(window_t* window) {
    init_world(&window->_world);
    
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window->_glfw_window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "vtrace", NULL, NULL);
    PROPAGATE(window->_glfw_window, ERROR, "Couldn't create GLFW window.");
    glfwSetInputMode(window->_glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwShowWindow(window->_glfw_window);
    glfwMakeContextCurrent(window->_glfw_window);
    glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    glEnable(GL_FRAMEBUFFER_SRGB); 
    glClear(GL_COLOR_BUFFER_BIT);

    glfwGetCursorPos(window->_glfw_window, &window->_last_mouse_xpos, &window->_last_mouse_ypos);

    return SUCCESS;
}

int32_t should_close(window_t* window) {
    return glfwWindowShouldClose(window->_glfw_window);
}

void destroy_window(window_t* window) {
    glfwDestroyWindow(window->_glfw_window);
    destroy_world(&window->_world);
}

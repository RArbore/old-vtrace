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

#include "control.h"

void handle_control(window_t* window, float dt) {
    glfwMakeContextCurrent(window->_glfw_window);

    if (glfwGetKey(window->_glfw_window, GLFW_KEY_W)) {
	window->_world._camera._camera_loc[0] += dt;
    }
    if (glfwGetKey(window->_glfw_window, GLFW_KEY_S)) {
	window->_world._camera._camera_loc[0] -= dt;
    }
    if (glfwGetKey(window->_glfw_window, GLFW_KEY_A)) {
	window->_world._camera._camera_loc[2] += dt;
    }
    if (glfwGetKey(window->_glfw_window, GLFW_KEY_D)) {
	window->_world._camera._camera_loc[2] -= dt;
    }
    if (glfwGetKey(window->_glfw_window, GLFW_KEY_SPACE)) {
	window->_world._camera._camera_loc[1] += dt;
    }
    if (glfwGetKey(window->_glfw_window, GLFW_KEY_LEFT_SHIFT)) {
	window->_world._camera._camera_loc[1] -= dt;
    }
}

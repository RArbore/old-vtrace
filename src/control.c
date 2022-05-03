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

#define MOVE_SPEED 10.0f

void handle_control(window_t* window, float dt) {
    glfwMakeContextCurrent(window->_glfw_window);

    if (glfwGetKey(window->_glfw_window, GLFW_KEY_W)) {
	window->_world._camera._camera_loc[2] += MOVE_SPEED * cosf(window->_world._camera._camera_rot[0]) * dt;
	window->_world._camera._camera_loc[0] -= MOVE_SPEED * sinf(window->_world._camera._camera_rot[0]) * dt;
    }
    if (glfwGetKey(window->_glfw_window, GLFW_KEY_S)) {
	window->_world._camera._camera_loc[2] -= MOVE_SPEED * cosf(window->_world._camera._camera_rot[0]) * dt;
	window->_world._camera._camera_loc[0] += MOVE_SPEED * sinf(window->_world._camera._camera_rot[0]) * dt;
    }
    if (glfwGetKey(window->_glfw_window, GLFW_KEY_A)) {
	window->_world._camera._camera_loc[0] -= MOVE_SPEED * cosf(window->_world._camera._camera_rot[0]) * dt;
	window->_world._camera._camera_loc[2] -= MOVE_SPEED * sinf(window->_world._camera._camera_rot[0]) * dt;
    }
    if (glfwGetKey(window->_glfw_window, GLFW_KEY_D)) {
	window->_world._camera._camera_loc[0] += MOVE_SPEED * cosf(window->_world._camera._camera_rot[0]) * dt;
	window->_world._camera._camera_loc[2] += MOVE_SPEED * sinf(window->_world._camera._camera_rot[0]) * dt;
    }
    if (glfwGetKey(window->_glfw_window, GLFW_KEY_SPACE)) {
	window->_world._camera._camera_loc[1] += MOVE_SPEED * dt;
    }
    if (glfwGetKey(window->_glfw_window, GLFW_KEY_LEFT_SHIFT)) {
	window->_world._camera._camera_loc[1] -= MOVE_SPEED * dt;
    }

    double new_xpos, new_ypos;
    glfwGetCursorPos(window->_glfw_window, &new_xpos, &new_ypos);
    window->_world._camera._camera_rot[0] -= (float) (new_xpos - window->_last_mouse_xpos) / DEFAULT_WIDTH;
    window->_world._camera._camera_rot[1] -= (float) (new_ypos - window->_last_mouse_ypos) / DEFAULT_HEIGHT;
    window->_last_mouse_xpos = new_xpos;
    window->_last_mouse_ypos = new_ypos;
}

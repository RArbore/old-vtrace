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

#include <stdio.h>

#include "window.h"
#include "render.h"
#include "error.h"

int main(void) {
    PROPAGATE(glfwInit() == GLFW_TRUE, ERROR, "Couldn't initialize GLFW.");
    GLFWwindow* window = create_window();
    PROPAGATE(window, ERROR, "Couldn't create a window.");

    PROPAGATE_CLEANUP_BEGIN(create_context(window) == SUCCESS, "Couldn't create graphics context.");
    destroy_window(window);
    glfwTerminate();
    PROPAGATE_CLEANUP_END(ERROR);

    while (!should_close(window)) {
	glfwPollEvents();
	PROPAGATE_CLEANUP_BEGIN(render_frame(window) == SUCCESS, "Failed to render frame.");
	destroy_window(window);
	glfwTerminate();
	PROPAGATE_CLEANUP_END(ERROR);
    }

    destroy_window(window);
    glfwTerminate();
    
    return SUCCESS;
}

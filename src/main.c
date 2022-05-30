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

#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "control.h"
#include "window.h"
#include "render.h"
#include "error.h"

static float get_frame_time() {
    static int64_t last = ~0;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    int64_t diff = spec.tv_nsec >= last ? spec.tv_nsec - last : 0;
    last = spec.tv_nsec;
    return (float) diff;
}

int32_t main(void) {
    srand((uint32_t) time(NULL));

    PROPAGATE(glfwInit() == GLFW_TRUE, ERROR, "Couldn't initialize GLFW.");
    window_t window = {0};
    PROPAGATE(create_window(&window) == SUCCESS, ERROR, "Couldn't create a window.");

    PROPAGATE_CLEANUP_BEGIN(create_context(&window) == SUCCESS, "Couldn't create graphics context.");
    destroy_window(&window);
    glfwTerminate();
    PROPAGATE_CLEANUP_END(ERROR);

    while (!should_close(&window)) {
	float raw_dt = get_frame_time();
	float dt = raw_dt / (float) 1e9;
	//printf("%f\n", (float) 1e9 / raw_dt);
	glfwPollEvents();

	PROPAGATE_CLEANUP_BEGIN(render_frame(&window) == SUCCESS, "Failed to render frame.");
	destroy_window(&window);
	glfwTerminate();
	PROPAGATE_CLEANUP_END(ERROR);

	handle_control(&window, dt);
    }

    destroy_window(&window);
    glfwTerminate();
    
    return SUCCESS;
}

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

#include "world.h"

void init_camera(camera_t* camera) {
    memset(&camera->_camera_loc, 0, 3 * sizeof(float));
    memset(&camera->_camera_rot, 0, 2 * sizeof(float));
}

void init_chunk(chunk_t* chunk) {
    memset(&chunk->_chunk_data, 0, CHUNK_SIZE);
}

void init_world(world_t* world) {
    init_camera(&world->_camera);
}

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
    chunk->_chunk_data = malloc(CHUNK_SIZE * sizeof(uint32_t));
    memset(chunk->_chunk_data, 0, CHUNK_SIZE * sizeof(uint32_t));
    for (size_t x = 0; x < 17; ++x)
	for (size_t z = 0; z < 17; ++z)
	    chunk->_chunk_data[x + z * CHUNK_WIDTH * CHUNK_WIDTH] = 0xFFFFFF01;
    for (size_t x = 0; x < 17; ++x)
	for (size_t y = 1; y < 17; ++y)
	    for (size_t z = 0; z < 17; ++z)
		if ((x - 8) * (x - 8) + (z - 8) * (z - 8) <= 4 * 4) {
		    chunk->_chunk_data[x + y * CHUNK_WIDTH + z * CHUNK_WIDTH * CHUNK_WIDTH] = y % 2 ? 0x88888801 : 0x88888803;
		}
}

void destroy_chunk(chunk_t* chunk) {
    free(chunk->_chunk_data);
}

void init_world(world_t* world) {
    init_camera(&world->_camera);
    init_chunk(&world->_chunk);
}

void destroy_world(world_t* world) {
    destroy_chunk(&world->_chunk);
}

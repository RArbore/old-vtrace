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

static uint64_t split_by_3(uint32_t a){
    uint64_t x = (uint64_t) a;
    x &= 0x1fffff;
    x = (x | x << 32) & 0x1f00000000ffff;
    x = (x | x << 16) & 0x1f0000ff0000ff;
    x = (x | x << 8) & 0x100f00f00f00f00f;
    x = (x | x << 4) & 0x10c30c30c30c30c3;
    x = (x | x << 2) & 0x1249249249249249;
    return x;
}

static uint32_t compact_by_3(uint64_t x) {
    x &= 0x1249249249249249;
    x = (x | x >> 2) & 0x10c30c30c30c30c3;
    x = (x | x >> 4) & 0x100f00f00f00f00f;
    x = (x | x >> 8) & 0x1f0000ff0000ff;
    x = (x | x >> 16) & 0x1f00000000ffff;
    x = (x | x >> 32) & 0x1fffff;
    return (uint32_t) x;
}

static uint64_t morton_encode(uint32_t x, uint32_t y, uint32_t z){
    uint64_t answer = 0;
    answer |= split_by_3(x) | split_by_3(y) << 1 | split_by_3(z) << 2;
    return answer;
}

static void morton_decode(uint64_t morton, uint32_t* x, uint32_t* y, uint32_t* z) {
    *x = compact_by_3(morton);
    *y = compact_by_3(morton >> 1);
    *z = compact_by_3(morton >> 2);
}

int32_t construct_svo(svo_node_t* dst, uint32_t max_nodes, uint32_t* voxels, uint32_t w) {
    return SUCCESS;
}

void init_chunk(chunk_t* chunk) {
    chunk->_chunk_data = malloc(CHUNK_SIZE * sizeof(uint32_t));
    memset(chunk->_chunk_data, 0, CHUNK_SIZE * sizeof(uint32_t));
    for (size_t i = 0; i < CHUNK_SIZE; ++i) {
	chunk->_chunk_data[i] = (uint32_t) rand();
	if (rand() % 10 > 1)
	    chunk->_chunk_data[i] &= 0xFFFFFFFD;
	if (rand() % 10 > 3)
	    chunk->_chunk_data[i] = 0;
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

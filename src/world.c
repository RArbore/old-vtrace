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

typedef enum svo_type_t {
    INVALID = 0,
    LEAF = 1,
    COMPOSITE = 2,
} svo_type_t;

static int32_t construct_svo_internal(svo_node_t* dst, uint32_t max_nodes, uint32_t target, uint32_t* num_nodes, uint32_t* voxels, svo_type_t* created_type, uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
    PROPAGATE(target < max_nodes, ERROR, "Not enough space for new SVO node.");
    if (w == 1) {
	uint32_t voxel = voxels[x + w * (y + w * z)];
	dst[target]._raw = voxel;
	*created_type = voxel ? LEAF : INVALID;
    }
    else {
	svo_type_t children_types[8];
	uint32_t prev_num_nodes = *num_nodes;
	dst[target]._parent._child_pointer = (uint16_t) ++(*num_nodes);
	*num_nodes += 8;
	PROPAGATE(construct_svo_internal(dst, max_nodes, prev_num_nodes + 1, num_nodes, voxels, &children_types[0], x, y, z, w / 2) == SUCCESS, ERROR, "Couldn't construct SVO node.");
	PROPAGATE(construct_svo_internal(dst, max_nodes, prev_num_nodes + 2, num_nodes, voxels, &children_types[1], x + w / 2, y, z, w / 2) == SUCCESS, ERROR, "Couldn't construct SVO node.");
	PROPAGATE(construct_svo_internal(dst, max_nodes, prev_num_nodes + 3, num_nodes, voxels, &children_types[2], x, y + w / 2, z, w / 2) == SUCCESS, ERROR, "Couldn't construct SVO node.");
	PROPAGATE(construct_svo_internal(dst, max_nodes, prev_num_nodes + 4, num_nodes, voxels, &children_types[3], x + w / 2, y + w / 2, z, w / 2) == SUCCESS, ERROR, "Couldn't construct SVO node.");
	PROPAGATE(construct_svo_internal(dst, max_nodes, prev_num_nodes + 5, num_nodes, voxels, &children_types[4], x, y, z + w / 2, w / 2) == SUCCESS, ERROR, "Couldn't construct SVO node.");
	PROPAGATE(construct_svo_internal(dst, max_nodes, prev_num_nodes + 6, num_nodes, voxels, &children_types[5], x + w / 2, y, z + w / 2, w / 2) == SUCCESS, ERROR, "Couldn't construct SVO node.");
	PROPAGATE(construct_svo_internal(dst, max_nodes, prev_num_nodes + 7, num_nodes, voxels, &children_types[6], x, y + w / 2, z + w / 2, w / 2) == SUCCESS, ERROR, "Couldn't construct SVO node.");
	PROPAGATE(construct_svo_internal(dst, max_nodes, prev_num_nodes + 8, num_nodes, voxels, &children_types[7], x + w / 2, y + w / 2, z + w / 2, w / 2) == SUCCESS, ERROR, "Couldn't construct SVO node.");

	int8_t all_invalid = 1;
	for (int8_t i = 0; i < 8; ++i) {
	    dst[target]._parent._invalid_mask >>= 1;
	    dst[target]._parent._invalid_mask |= 0x80 * children_types[i] == INVALID;
	    dst[target]._parent._leaf_mask >>= 1;
	    dst[target]._parent._leaf_mask |= 0x80 * children_types[i] == LEAF;
	}
	*created_type = all_invalid ? INVALID : COMPOSITE;

	if (dst[target]._parent._invalid_mask == 0xFF) dst[target]._raw = 0;
    }
    return SUCCESS;
}

int32_t construct_svo(svo_node_t* dst, uint32_t max_nodes, uint32_t* voxels, uint32_t w) {
    uint32_t num_nodes = 0;
    return construct_svo_internal(dst, max_nodes, 0, &num_nodes, voxels, NULL, 0, 0, 0, w);
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

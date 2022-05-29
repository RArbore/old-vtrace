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

static const uint32_t SCRATCH_SPACE_MULTIPLIER = 8;

typedef enum svo_node_type_t {
    PARENT = 0,
    LEAF = 1,
    INVALID = 2,
    SVO_ERROR = 3,
} svo_node_type_t;

static svo_node_type_t construct_svo_tree(svo_node_t* scratch_arena, uint32_t* num_alloc, uint32_t target, uint32_t max_nodes, uint32_t* voxels, uint32_t w, uint32_t x, uint32_t y, uint32_t z) {
    if (w == 1) {
	uint32_t voxel = voxels[x + w * (y + w * z)];
	scratch_arena[target]._raw = voxel;
	return voxel ? LEAF : INVALID;
    }
    else {
	svo_node_type_t codes[8];
	uint32_t start = *num_alloc;
	scratch_arena[target]._parent._child_pointer = (uint16_t) start;
	*num_alloc += 8;
	if (max_nodes <= *num_alloc) return SVO_ERROR;
	uint32_t hw = w / 2;

	PROPAGATE(codes[0] = construct_svo_tree(scratch_arena, num_alloc, start, max_nodes, voxels, hw, x, y, z) != SVO_ERROR, SVO_ERROR, "Couldn't create initial sub-SVO structure.");
	PROPAGATE(codes[1] = construct_svo_tree(scratch_arena, num_alloc, start + 1, max_nodes, voxels, hw, x + hw, y, z) != SVO_ERROR, SVO_ERROR, "Couldn't create initial sub-SVO structure.");
	PROPAGATE(codes[2] = construct_svo_tree(scratch_arena, num_alloc, start + 2, max_nodes, voxels, hw, x, y + hw, z) != SVO_ERROR, SVO_ERROR, "Couldn't create initial sub-SVO structure.");
	PROPAGATE(codes[3] = construct_svo_tree(scratch_arena, num_alloc, start + 3, max_nodes, voxels, hw, x + hw, y + hw, z) != SVO_ERROR, SVO_ERROR, "Couldn't create initial sub-SVO structure.");
	PROPAGATE(codes[4] = construct_svo_tree(scratch_arena, num_alloc, start + 4, max_nodes, voxels, hw, x, y, z + hw) != SVO_ERROR, SVO_ERROR, "Couldn't create initial sub-SVO structure.");
	PROPAGATE(codes[5] = construct_svo_tree(scratch_arena, num_alloc, start + 5, max_nodes, voxels, hw, x + hw, y, z + hw) != SVO_ERROR, SVO_ERROR, "Couldn't create initial sub-SVO structure.");
	PROPAGATE(codes[6] = construct_svo_tree(scratch_arena, num_alloc, start + 6, max_nodes, voxels, hw, x, y + hw, z + hw) != SVO_ERROR, SVO_ERROR, "Couldn't create initial sub-SVO structure.");
	PROPAGATE(codes[7] = construct_svo_tree(scratch_arena, num_alloc, start + 7, max_nodes, voxels, hw, x + hw, y + hw, z + hw) != SVO_ERROR, SVO_ERROR, "Couldn't create initial sub-SVO structure.");

	for (int8_t i = 0; i < 8; ++i) {
	    scratch_arena[target]._parent._invalid_mask >>= 1;
	    scratch_arena[target]._parent._invalid_mask |= 0x80 * codes[i] == INVALID;
	    scratch_arena[target]._parent._leaf_mask >>= 1;
	    scratch_arena[target]._parent._leaf_mask |= 0x80 * codes[i] == LEAF;
	}
	
	if (scratch_arena[target]._parent._invalid_mask == 0xFF) {
	    scratch_arena[target]._raw = 0;
	    return INVALID;
	}
	if (scratch_arena[target]._parent._leaf_mask == 0xFF) {
	    uint8_t i;
	    for (i = 0; i < 7; ++i) {
		if (scratch_arena[start + i]._raw != scratch_arena[start + i + 1]._raw) break;
	    }
	    if (i == 7) scratch_arena[target]._raw = scratch_arena[start]._raw;
	    return LEAF;
	}
	return PARENT;
    }
}

int32_t construct_svo(svo_node_t* dst, uint32_t max_nodes, uint32_t* voxels, uint32_t w) {
    svo_node_t* scratch_arena = malloc(SCRATCH_SPACE_MULTIPLIER * max_nodes * sizeof(svo_node_t));
    uint32_t num_alloc = 1;

    PROPAGATE_CLEANUP_BEGIN(construct_svo_tree(scratch_arena, &num_alloc, 0, max_nodes, voxels, w, 0, 0, 0) != SVO_ERROR, "Couldn't create initial SVO structure.");
    free(scratch_arena);
    PROPAGATE_CLEANUP_END(ERROR);

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

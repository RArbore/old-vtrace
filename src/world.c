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

static uint32_t morton_read(uint32_t* voxels, uint64_t morton, uint32_t w) {
    uint32_t x, y, z;
    morton_decode(morton, &x, &y, &z);
    return voxels[x + w * (y + w * z)];
}

int32_t construct_svo(svo_node_t* dst, uint32_t max_nodes, uint32_t* voxels, uint32_t w, uint32_t* num_nodes) {
    svo_node_t buffer[MAX_OCTREE_DEPTH][8];
    uint32_t buffer_size[MAX_OCTREE_DEPTH] = {0};
    uint32_t num_written = 0, num_read = 0;

    uint8_t last_node_level = 0;
    while (num_read < w * w * w) {
	uint8_t valid = 0;
	for (uint8_t i = 0; i < 8; ++i) {
	    valid <<= 1;
	    uint32_t voxel = morton_read(voxels, num_read + i, w);
	    buffer[0][i]._raw = voxel;
	    if (voxel) valid |= 0x01;
	}
	num_read += 8;
	buffer_size[0] = 8;
	for (uint8_t d = 0; d < MAX_OCTREE_DEPTH - 1 && buffer_size[d] >= 8; ++d) {
	    uint32_t prev_num_written = num_written;
	    valid = 0;
	    for (uint8_t i = 0; i < 8; ++i) {
		PROPAGATE(num_written < max_nodes, ERROR, "Exceeded maximum number of SVO nodes.");
		valid <<= 1;
		if (buffer[d][i]._raw) {
		    dst[num_written++] = buffer[d][i];
		    valid |= 0x01u;
		}
	    }
	    buffer_size[d] = 0;
	    if (valid) {
		buffer[d + 1][buffer_size[d + 1]]._parent._children = (uint16_t) prev_num_written;
		buffer[d + 1][buffer_size[d + 1]]._parent._valid_mask = valid;
		buffer[d + 1][buffer_size[d + 1]]._parent._leaf_mask = d == 0 ? valid : 0;
	    }
	    else {
		buffer[d + 1][buffer_size[d + 1]]._raw = 0;
	    }
	    ++buffer_size[d + 1];
	    last_node_level = d + 1;
	}
    }
    PROPAGATE(buffer_size[last_node_level] == 1, ERROR, "No single root node constructed for SVO.");
    PROPAGATE(num_written < max_nodes, ERROR, "Exceeded maximum number of SVO nodes.");
    dst[num_written++] = buffer[last_node_level][0];
    *num_nodes = num_written;
   
    return SUCCESS;
}

static uint32_t query_svo_helper(svo_node_t* svo, uint32_t x, uint32_t y, uint32_t z, uint32_t w, uint32_t pos, uint8_t leaf) {
    if (leaf) return svo[pos]._raw; 

    uint8_t bx = x < w / 2;
    uint8_t by = y < w / 2;
    uint8_t bz = z < w / 2;
    uint32_t rx = bx ? x : x - w / 2;
    uint32_t ry = by ? y : y - w / 2;
    uint32_t rz = bz ? z : z - w / 2;
    uint8_t bit = (uint8_t) (7 - (bx | (by << 1) | (bz << 2)));
    uint8_t mask = 0x80 >> bit;

    if (svo[pos]._parent._valid_mask & mask) {
	mask = svo[pos]._parent._valid_mask;
	uint8_t count = 0;
	for (uint8_t i = 0; i < bit; ++i) {
	    count += (mask >> (7 - i)) & 1;
	}
	uint32_t child = svo[pos]._parent._children + count;
	return query_svo_helper(svo, rx, ry, rz, w / 2, child, svo[pos]._parent._leaf_mask & mask);
    }
    return 0;
}

static uint32_t query_svo(svo_node_t* svo, uint32_t num_nodes, uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
    return query_svo_helper(svo, x, y, z, w, num_nodes - 1, 0);
}

static uint32_t query_svo_gpu_helper(svo_node_t* svo, uint32_t x, uint32_t y, uint32_t z, uint32_t w, uint32_t pos, int32_t leaf) {
    while (1) {
	if (leaf) return svo[pos]._raw;
	
	int32_t bx = x < w / 2;
	int32_t by = y < w / 2;
	int32_t bz = z < w / 2;
	uint32_t rx = bx ? x : x - w / 2;
	uint32_t ry = by ? y : y - w / 2;
	uint32_t rz = bz ? z : z - w / 2;
	uint32_t bit = (uint32_t) (7 - (bx | (by << 1) | (bz << 2)));
	uint32_t mask = 0x80 >> bit;
	
	uint32_t valid_mask = (svo[pos]._raw & 0x00FF0000u) >> 16;
	uint32_t leaf_mask = (svo[pos]._raw & 0xFF000000u) >> 24;
	if ((valid_mask & mask) != 0) {
	    mask = valid_mask;
	    uint32_t count = 0;
	    for (uint32_t i = 0; i < bit; ++i) {
		count += (mask >> (7 - i)) & 1u;
	    }
	    uint32_t child = (svo[pos]._raw & 0x0000FFFFu) + count;
	    x = rx;
	    y = ry;
	    z = rz;
	    w /= 2;
	    pos = child;
	    leaf = (leaf_mask & mask) != 0;
	}
	else {
	    break;
	}
    }
    return 0;
}

static uint32_t query_svo_gpu(svo_node_t* svo, uint32_t num_nodes, uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
    return query_svo_gpu_helper(svo, x, y, z, w, num_nodes - 1, 0);
}

static int32_t max(int32_t a, int32_t b) {
    return a > b ? a : b;
}

void init_chunk(chunk_t* chunk) {
    uint32_t* chunk_raw = malloc(CHUNK_SIZE * sizeof(uint32_t));
    memset(chunk_raw, 0, CHUNK_SIZE * sizeof(uint32_t));
    for (uint32_t i = 0; i < CHUNK_SIZE; ++i) {
	int32_t h =  rand() % (255 * 3);
	int32_t r = max(255 - abs(h), 0) + max(255 - abs(h - 255 * 3), 0);
	int32_t g = max(255 - abs(h - 255), 0);
	int32_t b = max(255 - abs(h - 255 * 2), 0);
	chunk_raw[i] = (uint32_t) ((r << 24) | (g << 16) | (b << 8) | (uint8_t) rand());
	if (rand() % 10 > 1)
	    chunk_raw[i] &= 0xFFFFFFFE;
	if (rand() % 10 > 2)
	    chunk_raw[i] = 0;
    }
    svo_node_t* svo = calloc(CHUNK_SIZE * 2, sizeof(svo_node_t));
    uint32_t num_nodes;
    construct_svo(svo, CHUNK_SIZE * 2, chunk_raw, CHUNK_WIDTH, &num_nodes);
    printf("Num nodes: %u\n", num_nodes);
    uint32_t bad_count = 0;
    uint32_t bad_count_gpu = 0;
    uint32_t non_zero_count = 0;
    uint32_t non_zero_count_gpu = 0;
    for (uint32_t i = 0; i < CHUNK_SIZE; ++i) {
	uint32_t x = i % CHUNK_WIDTH;
	uint32_t y = i / CHUNK_WIDTH % CHUNK_WIDTH;
	uint32_t z = i / CHUNK_WIDTH / CHUNK_WIDTH;
	uint32_t queried_voxel = query_svo(svo, num_nodes, x, y, z, CHUNK_WIDTH);
	uint32_t queried_voxel_gpu = query_svo_gpu(svo, num_nodes, x, y, z, CHUNK_WIDTH);
	uint32_t actual_voxel = chunk_raw[i];
	if (queried_voxel != actual_voxel) {
	    printf("CPU: %u %u\n", queried_voxel, actual_voxel);
	    ++bad_count;
	}
	if (queried_voxel_gpu != actual_voxel) {
	    printf("CPU: %u %u\n", queried_voxel, actual_voxel);
	    ++bad_count_gpu;
	}
	non_zero_count += queried_voxel != 0;
	non_zero_count_gpu += queried_voxel_gpu != 0;
    }
    printf("Bad voxels: %u %u\n", bad_count, bad_count_gpu);
    printf("Non-zero voxels: %u %u\n", non_zero_count, non_zero_count_gpu);

    free(chunk_raw);
    chunk->_svo = svo;
    chunk->_num_nodes = num_nodes;
}

void destroy_chunk(chunk_t* chunk) {
    free(chunk->_svo);
}

void init_world(world_t* world) {
    init_camera(&world->_camera);
    init_chunk(&world->_chunk);
}

void destroy_world(world_t* world) {
    destroy_chunk(&world->_chunk);
}

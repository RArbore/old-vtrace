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

#ifndef WORLD_H_
#define WORLD_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define CHUNK_WIDTH 64
#define CHUNK_SIZE (CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH)

typedef struct camera_t {
    float _camera_loc[3];
    float _camera_rot[2];
} camera_t;

void init_camera(camera_t* camera);

typedef struct child_desc_t {
    uint32_t _child_pointer : 15;
    uint32_t _far : 1;
    uint32_t _valid_mask : 8;
    uint32_t _leaf_mask : 8;
} child_desc_t;

typedef struct leaf_voxel_t {
    uint8_t _red;
    uint8_t _green;
    uint8_t _blue;
    uint8_t _flags;
} leaf_voxel_t;

typedef union svo_node_t {
    child_desc_t _parent;
    leaf_voxel_t _leaf;
    uint32_t _far_pointer;
    uint32_t _raw;
} svo_node_t;

typedef struct chunk_t {
    uint32_t* _chunk_data;
} chunk_t;

void init_chunk(chunk_t* chunk);
void destroy_chunk(chunk_t* chunk);

typedef struct world_t {
    camera_t _camera;
    chunk_t _chunk;
} world_t;

void init_world(world_t* world);
void destroy_world(world_t* world);

#endif // WORLD_H_

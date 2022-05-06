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

#include <stdint.h>
#include <string.h>

#define CHUNK_WIDTH 16
#define CHUNK_SIZE ((CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_WIDTH) >> 5)

typedef struct camera_t {
    float _camera_loc[3];
    float _camera_rot[2];
} camera_t;

void init_camera(camera_t* camera);

typedef struct chunk_t {
    uint32_t _chunk_data[CHUNK_SIZE];
} chunk_t;

void init_chunk(chunk_t* chunk);

typedef struct world_t {
    camera_t _camera;
    chunk_t _chunk;
} world_t;

void init_world(world_t* world);

#endif // WORLD_H_

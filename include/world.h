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

#include <string.h>

typedef struct camera_t {
    float _camera_loc[3];
    float _camera_rot[2];
} camera_t;

void init_camera(camera_t* camera);

typedef struct world_t {
    camera_t _camera;
} world_t;

void init_world(world_t* world);

#endif // WORLD_H_

#   This file is part of vtrace. \
\
    vtrace is free software: you can redistribute it and/or modify \
    it under the terms of the GNU Lesser General Public License as published by \
    the Free Software Foundation, either version 3 of the License, or \
    any later version. \
\
    vtrace is distributed in the hope that it will be useful, \
    but WITHOUT ANY WARRANTY; without even the implied warranty of \
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
    GNU Lesser General Public License for more details. \
\
    You should have received a copy of the GNU Lesser General Public License \
    along with vtrace. If not, see <https://www.gnu.org/licenses/>.

CC=gcc
LD=gcc
OBJ=objcopy

W_FLAGS=-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion -Wswitch-default -Wundef -Werror -Wno-unused -Wconversion

CC_FLAGS=-g -std=c99 -Ofast -fno-signed-zeros -fno-trapping-math -frename-registers -funroll-loops -march=native -Iinclude $(W_FLAGS)
LD_FLAGS=-lGL -lglfw

build/vtrace: build/main.o build/control.o build/window.o build/render.o build/world.o build/vertex.o build/fragment.o
	$(LD) -o $@ $^ $(LD_FLAGS)
build/main.o: src/main.c include/control.h include/window.h include/render.h include/error.h
	$(CC) $(CC_FLAGS) -c -o $@ $<
build/control.o: src/control.c include/control.h include/window.h include/world.h include/error.h
	$(CC) $(CC_FLAGS) -c -o $@ $<
build/window.o: src/window.c include/window.h include/world.h include/error.h
	$(CC) $(CC_FLAGS) -c -o $@ $<
build/render.o: src/render.c include/window.h include/render.h include/error.h
	$(CC) $(CC_FLAGS) -c -o $@ $<
build/world.o: src/world.c include/world.h
	$(CC) $(CC_FLAGS) -c -o $@ $<

build/vertex.o: shaders/shader.vert
	$(OBJ) --input binary --output elf64-x86-64 $< $@
build/fragment.o: shaders/shader.frag
	$(OBJ) --input binary --output elf64-x86-64 $< $@

exe: build/vtrace
	__GL_SYNC_TO_VBLANK=0 build/vtrace
clean:
	rm -rf build/*

.DEFAULT: build/vtrace
.PHONY: exe clean

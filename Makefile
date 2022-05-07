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

CC=clang
LD=clang
OBJ=objcopy

W_FLAGS=-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion -Wswitch-default -Wundef -Werror -Wno-unused -Wconversion

CC_FLAGS=-g -std=c99 -Ofast -fno-signed-zeros -fno-trapping-math -funroll-loops -march=native -Iinclude $(W_FLAGS)

PLATFORM := $(shell uname)
ifeq  ($(PLATFORM),Darwin)
	LD_FLAGS=-lm -framework OpenGL -lglfw
else
	LD_FLAGS=-lm -lGL -lglfw
endif

build/vtrace: build/main.o build/control.o build/window.o build/render.o build/world.o build/rect.o build/tex.o build/trace.o build/blur.o build/bloom.o
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

build/rect.o: shaders/rect.vert
	$(OBJ) --input binary --output elf64-x86-64 $< $@
build/tex.o: shaders/tex.vert
	$(OBJ) --input binary --output elf64-x86-64 $< $@
build/trace.o: shaders/trace.frag
	$(OBJ) --input binary --output elf64-x86-64 $< $@
build/blur.o: shaders/blur.frag
	$(OBJ) --input binary --output elf64-x86-64 $< $@
build/bloom.o: shaders/bloom.frag
	$(OBJ) --input binary --output elf64-x86-64 $< $@

exe: build/vtrace
	__GL_SYNC_TO_VBLANK=0 build/vtrace
clean:
	rm -rf build/*

.DEFAULT: build/vtrace
.PHONY: exe clean

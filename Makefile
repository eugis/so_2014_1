C_FLAGS = -std=c99 -pedantic -Wall -Wextra -g -Wno-unused-parameter -D_GNU_SOURCE -D_POSIX_C_SOURCE -DIPC=$(IPC)

C_FILES = $(wildcard src/*.c)
H_FILES = $(wildcard src/*.h)

O_FILES = $(patsubst src/%.c, obj/%.o, $(C_FILES))


build: bin obj $(O_FILES)
	gcc -o bin/test $(O_FILES) $(C_FLAGS) src/exe/test.c
	gcc -o bin/reset $(O_FILES) $(C_FLAGS) src/exe/reset.c
	gcc -o bin/client $(O_FILES) $(C_FLAGS) src/exe/client.c

	rm -r obj

obj:
	mkdir obj

bin:
	mkdir bin

obj/%.o: src/%.c inc/%.h
	gcc  -c $< -o $@ $(C_FLAGS)

clean:
	rm -rf obj tmp test


.PHONY: build clean
.SILENT:
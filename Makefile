C_FLAGS = -std=c99 -pedantic -Wall -Wextra -g -D_GNU_SOURCE -D_POSIX_C_SOURCE -DIPC=$(IPC)

C_FILES = $(wildcard src/*.c)
H_FILES = $(wildcard src/*.h)

O_FILES = $(patsubst src/%.c, obj/%.o, $(C_FILES))


build: obj $(O_FILES)
	gcc -o test $(O_FILES) $(C_FLAGS)
	rm -r obj

obj:
	mkdir obj

obj/%.o: src/%.c inc/%.h
	gcc  -c $< -o $@ $(C_FLAGS)

clean:
	rm -r obj
	rm test


.PHONY: build clean
.SILENT:
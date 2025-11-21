all: shell

shell: ZambranoJ-bash.c
	gcc ZambranoJ-bash.c -o ZambranoJ-bash

clean:
	rm -f ZambranoJ-bash

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

#define O_CREAT 0x0200 // create if nonexistant

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf(2, "Help ...\n");
	} else {
		int i;
		for (i = 1; i < argc; i++) {
			if (open(argv[i], O_CREAT) == -1) {
				printf(2,"creating file failed..\n");
			}
		}
	}

	exit();
}

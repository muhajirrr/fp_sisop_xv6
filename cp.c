#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {
	int input, output, n;
	char buffer[1024];

	if (argc < 2) {
		printf(1, "help..\n");
		exit();
	}

	if ((input = open(argv[1], O_RDONLY)) < 0) {
		printf(1, "cp: cannot open %s\n", argv[1]);
		exit();
	}

	if ((output = open(argv[2], O_CREATE|O_RDWR)) < 0) {
                printf(1, "cp: cannot open %s\n", argv[2]);
                exit();
        }

	while ((n = read(input, buffer, sizeof(buffer))) > 0) {
		write(output, buffer, n);
	}

	close(input);
	close(output);

	exit();
}

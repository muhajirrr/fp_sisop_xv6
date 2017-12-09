#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

int isDirectory(char *s) {
	struct stat st;

	int fd = open(s, O_RDONLY);
	fstat(fd, &st);
	int res;

	if (st.type == T_DIR) 
		res = 1;
	else
		res = 0;

	close(fd);
	return res;
}

void split(char *s, int bsize) {
	int fs, fd, n;
	char buf[bsize];

	if (isDirectory(s)) {
		printf(1, "split: %s is a directory\n", s);
		return;
	} else {
		if ((fs = open(s, O_RDONLY)) < 0) {
			printf(1, "cp: cannot open %s\n", s);
			return;
		}

		char cname = 'a';
		char fname[10];

		while ((n = read(fs, buf, sizeof(buf))) > 0) {
			memmove(fname, &cname, 1);
			fname[strlen(fname)] = '\0'; 
			fd = open(fname, O_CREATE|O_RDWR);
			write(fd, buf, n);
			close(fd);
			cname++;
		}

		close(fs);
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		printf(1, "Split Usage...");
	else if (!strcmp(argv[2], "-b"))
		split(argv[1], atoi(argv[3]));

	exit();
}
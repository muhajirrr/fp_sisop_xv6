#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

void find(char  *dir) {
	int fd;
	char buf[512], *p;
	struct stat st;
	struct dirent de;

	if ((fd = open(dir, O_RDONLY)) < 0) {
		printf(1, "find: cannot open %s\n", dir);
		return;
	}

	if (fstat(fd, &st) < 0) {
		printf(1, "find: cannot stat %s\n", dir);
		return;
	}

	if (st.type == T_FILE) {
		printf(1, "%s\n", dir);
	} else if (st.type == T_DIR) {
		printf(1, "%s\n", dir);
		
		strcpy(buf, dir);
		p = buf+strlen(buf);
		*p = '/';
		p++;

		while (read(fd, &de, sizeof(de)) == sizeof(de)) {
			if(de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
				continue;

			memmove(p, de.name, strlen(de.name));
			p[strlen(de.name)] = 0;

			fstat(open(buf, O_RDONLY), &st);
			if (st.type == T_FILE)
				printf(1, "%s\n", buf);
			else if (st.type == T_DIR) {
				find(buf);
			}
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		find(".");
	else
		find(argv[1]);

	exit();
}
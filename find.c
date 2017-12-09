#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

#define M_DEFAULT 0
#define M_NAME 1
#define M_PATH 2

char *getFileName(char *s) {
	char *filename = s;
	char *temp = s;
	int i;

	for (i = strlen(temp); i >= 0; i--) {
		if (temp[i] == '/') {
			filename = &temp[i+1];
			break;
		}
	}

	return filename;
}

void find(char  *dir, int mode, char *key) {
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
		switch (mode) {
			case M_NAME:
				if (!strcmp(getFileName(buf), getFileName(key)))
					printf(1, "%s\n", dir);
				break;
			case M_PATH:
				if (!strcmp(buf, key))
					printf(1, "%s\n", dir);
				break;
			default:
				printf(1, "%s\n", dir);
				break;
		}
	} else if (st.type == T_DIR) {
		switch (mode) {
			case M_NAME:
				if (!strcmp(getFileName(buf), getFileName(key)))
					printf(1, "%s\n", dir);
				break;
			case M_PATH:
				if (!strcmp(buf, key))
					printf(1, "%s\n", dir);
				break;
			default:
				printf(1, "%s\n", dir);
				break;
		}
		
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
				switch (mode) {
					case M_NAME:
						if (!strcmp(getFileName(buf), getFileName(key)))
							printf(1, "%s\n", buf);
						break;
					case M_PATH:
						if (!strcmp(buf, key))
							printf(1, "%s\n", buf);
						break;
					default:
						printf(1, "%s\n", buf);
						break;
				}
			else if (st.type == T_DIR) {
				find(buf, mode, key);
			}
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		find(".", M_DEFAULT, "");
	else if (!strcmp(argv[2], "-name"))
		find(argv[1], M_NAME, argv[3]);
	else if (!strcmp(argv[2], "-path"))
		find(argv[1], M_PATH, argv[3]);
	else
		find(argv[1], M_DEFAULT, "");

	exit();
}
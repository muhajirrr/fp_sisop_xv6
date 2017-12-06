#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

void rm_one(char *s) {
	if (unlink(s) < 0) {
		printf(2, "rm: %s failed to delete\n", s);
		return;
	}
}

void rm_recurse(char *dir) {
	int fd;
	char buf[512], *p;
	struct dirent de;
	struct stat st;

	fd = open(dir, O_RDONLY);

	fstat(fd, &st);

	switch (st.type) {
		case T_FILE:
			rm_one(dir);
			break;
		case T_DIR:
			strcpy(buf, dir);
			p = buf+strlen(buf);
			*p = '/';
			p++;

			while(read(fd, &de, sizeof(de)) == sizeof(de)) {
				if(de.inum == 0 || de.name[0] == '.')
					continue;

				memmove(p, de.name, DIRSIZ);
				p[DIRSIZ] = 0;

				fstat(fd, &st);
				if (st.type == T_FILE) 
					rm_one(buf);
				else if (st.type == T_DIR)
					rm_recurse(buf);
			}

			break;
	}

	unlink(dir);

	close(fd);
}

int main(int argc, char *argv[])
{
	int i;

	if(argc < 2){
		printf(2, "Usage: rm [OPTIONS] files...\n");
		exit();
	}

	if (!strcmp(argv[1], "-rf")) {
		rm_recurse(argv[2]);
	} else {
		for(i = 1; i < argc; i++){
			rm_one(argv[i]);
		}
	}

	exit();
}
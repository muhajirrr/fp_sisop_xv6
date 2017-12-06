#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

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

void copy_one(char *src, char *dest) {
	int input, output, i, n;
	char buffer[1024];
	char *filename = getFileName(src);

	if ((input = open(src, O_RDONLY)) < 0) {
		printf(1, "cp: cannot open %s\n", src);
		return;
	}

	if ((output = open(dest, O_CREATE|O_RDWR)) < 0) {
		int dest_len = strlen(dest);
		int filename_len = strlen(filename);

		char *dir = (char *) malloc(dest_len+filename_len+2);
		for (i = 0; i < dest_len; i++) dir[i] = dest[i];
		if (dir[dest_len-1] != '/') {
			dir[dest_len] = '/';
			dest_len++;
		}
		for (i = dest_len; i < dest_len+filename_len; i++)
			dir[i] = filename[i-dest_len];

		if ((output = open(dir, O_CREATE|O_RDWR)) < 0) {
			printf(1, "cp: cannot open %s\n", dest);
			return;
		}
	}

	while ((n = read(input, buffer, sizeof(buffer))) > 0) {
		write(output, buffer, n);
	}

	close(input);
	close(output);
}

void copy_all(char *path) {
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(".", 0)) < 0){
		printf(2, "cp: cannot open %s\n", ".");
		return;
	}

	if(fstat(fd, &st) < 0){
		printf(2, "cp: cannot stat %s\n", ".");
		close(fd);
		return;
	}

	switch(st.type) {
		case T_DIR:
			if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
				printf(1, "cp: path too long\n");
				break;
			}

			strcpy(buf, ".");
			p = buf+strlen(buf);
			*p = '/';
			p++;

			while(read(fd, &de, sizeof(de)) == sizeof(de)) {
				if(de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
					continue;

				memmove(p, de.name, DIRSIZ);
				p[DIRSIZ] = 0;

				fstat(open(buf, O_RDONLY), &st);
				if (st.type == T_DIR) {
					printf(1, "cp: -r not specified; omitting directory '%s'\n", buf);
					continue;
				}

				copy_one(buf, path);
			}

			break;
	}

	close(fd);
}

void copy_recurse(char *src, char *dest) {
	int fs;
	char *p, buf[512], bufdir[512], *d;
	struct dirent de;
	struct stat st;

	if ((fs = open(src, O_RDONLY)) < 0) {
		printf(2, "cp: cannot open %s\n", src);
		return;
	}

	if (fstat(fs, &st) < 0) {
		printf(2, "cp: cannot stat %s\n", src);
		close(fs);
		return;
	}

	if (st.type == T_FILE) {
		copy_one(src, dest);
	} else if (st.type == T_DIR) {
		if(strlen(src) + 1 + DIRSIZ + 1 > sizeof buf) {
			printf(1, "cp: path too long\n");
			return;
		}

		char *tmp = getFileName(src);
		strcpy(bufdir, dest);
		d = bufdir+strlen(bufdir)-1;
		if (strcmp(d, "/")) {
			d++;
			*d = '/';
		}
		d++;
		memmove(d, tmp, strlen(tmp));
		d[strlen(tmp)] = 0;

		printf(1, "dir: %s\n", tmp);
		printf(1, "mkdir: %s\n", bufdir);
		mkdir(bufdir);

		strcpy(buf, src);
		p = buf+strlen(buf);
		*p = '/';
		p++;

		while(read(fs, &de, sizeof(de)) == sizeof(de)) {
			if(de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
				continue;

			memmove(p, de.name, strlen(de.name));
			p[strlen(de.name)] = 0;

			fstat(open(buf, O_RDONLY), &st);
			if (st.type == T_FILE)
				copy_one(buf, bufdir);
			else if (st.type == T_DIR)
				copy_recurse(buf, bufdir);
		}
	}
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		printf(1, "help..\n");
		exit();
	} else if (argc < 3) {
		printf(1, "cp: missing destination file operand after %s\n", argv[1]);
		exit();
	}

	if (!strcmp(argv[1], "*"))
		copy_all(argv[2]);
	else if (!strcmp(argv[1], "-r"))
		copy_recurse(argv[2], argv[3]);
	else
		copy_one(argv[1], argv[2]);

	exit();
}

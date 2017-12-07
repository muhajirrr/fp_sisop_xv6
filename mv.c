#include "types.h"
#include "user.h"
#include "stat.h"
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

void move_file(char *src, char *dest) {
	int fs, fd, n;
	char buffer[512];

	char *newdest = (char *) malloc(strlen(getFileName(src))+strlen(dest)+2);
	strcpy(newdest, dest);

	if (isDirectory(dest)) {
		if (dest[strlen(dest)-1] != '/') strcat(newdest, "/");

		strcat(newdest, getFileName(src));
	} else if (dest[strlen(dest)-1] == '/') {
		printf(1, "mv: %s is not a directory\n", dest);
		return;
	}

	if ((fs = open(src, O_RDONLY)) < 0) {
		printf(1, "mv: cannot open %s\n", src);
		return;
	}

	if ((fd = open(newdest, O_CREATE|O_RDWR)) < 0) {
		printf(1, "mv: cannot open %s\n", dest);
		return;
	}

	while ((n = read(fs, buffer, sizeof(buffer))) > 0) {
		write(fd, buffer, n);
	}

	close(fs);
	close(fd);

	unlink(src);
}

void move_dir(char *src, char *dest) {
	char buf[512], bufdir[512], *p;
	int fd;
	struct dirent de;

	if((fd = open(src, 0)) < 0){
		printf(2, "mv: cannot open %s\n", src);
		return;
	}

	strcpy(bufdir, dest);
	if (dest[strlen(dest)-1] != '/') strcat(bufdir, "/");
	strcat(bufdir, getFileName(src));

	if (mkdir(bufdir) < 0) {
		mkdir(dest);
		strcpy(bufdir, dest);
	}

	strcpy(buf, src);
	p = buf+strlen(buf);
	*p = '/';
	p++;

	while(read(fd, &de, sizeof(de)) == sizeof(de)) {
		if(de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
			continue;

		memmove(p, de.name, strlen(de.name));
		p[strlen(de.name)] = 0;

		if (isDirectory(buf))
			move_dir(buf, bufdir);
		else
			move_file(buf, bufdir);
	}

	close(fd);

	unlink(src);
}

void move_one(char *src, char *dest) {
	if (isDirectory(src))
		move_dir(src, dest);
	else
		move_file(src, dest);
}

void move_all(char *path) {
	char buf[512], *p;
	int fd;
	struct dirent de;

	if((fd = open(".", 0)) < 0){
		printf(2, "mv: cannot open %s\n", ".");
		return;
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

		move_one(buf, path);
	}

	close(fd);
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		printf(1, "help..\n");
		exit();
	} else if (argc < 3) {
		printf(1, "mv: missing destination file operand after '%s'\n", argv[1]);
		exit();
	}

	if (!strcmp(argv[1], "*"))
		move_all(argv[2]);
	else
		move_one(argv[1], argv[2]);

	exit();
}
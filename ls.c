
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>


int two_arg (char * path) {
	DIR *dir;
	dir = opendir(path);
	struct dirent *file;
	while ((file = readdir (dir)) != 0) {
		if ((strcmp(file -> d_name, ".") && strcmp (file -> d_name, "..")))
		printf ("%s\n", file -> d_name);
	}
	closedir (dir);
	return 0;
}

void pr_mode (int mode){
	if (mode & S_IFDIR) printf("d");
	else if (mode & S_IFBLK) printf("b");
	else if (mode & S_IFCHR) printf("c");
	else if (mode & S_IFIFO) printf("p");
	else if (mode & S_IFREG) printf("-");
	else printf("?");
	if (mode & S_IRUSR) printf("r"); else printf("-");
	if (mode & S_IWUSR) printf("w"); else printf("-");
	if (mode & S_IXUSR) printf("x"); else printf("-");
	if (mode & S_IRGRP) printf("r"); else printf("-");
	if (mode & S_IWGRP) printf("w"); else printf("-");
	if (mode & S_IXGRP) printf("x"); else printf("-");
	if (mode & S_IROTH) printf("r"); else printf("-");
	if (mode & S_IWOTH) printf("w"); else printf("-");	
	if (mode & S_IXOTH) printf("x"); else printf("-");
	printf (" ");
}


int three_arg (char *path) {
	DIR *dir = opendir (path);
	struct dirent *file;
	while ((file = readdir(dir)) != 0) {
		if (strcmp (file -> d_name, ".") && strcmp (file -> d_name, "..")) {
			struct stat buf;
			stat (file -> d_name, &buf);
			int mode = buf.st_mode;
			pr_mode(mode);
			struct passwd* user;
			printf("%d ", (int)(buf.st_nlink));
			user = getpwuid(buf.st_uid);
			printf("%s ", user -> pw_name);
			printf("%ld %ld ", buf.st_rdev, buf.st_size);
			printf("%s\n", file -> d_name);
		}
	}
	closedir(dir);
	return 0;
}


int main(int argc, char** argv) {
	if (argc == 2 && !strcmp(argv[1], "ls")) {
		two_arg("./");
	}
	else {
		if (argc == 3 && !strcmp(argv[1], "ls") && !strcmp(argv[2], "-l")){
			three_arg("./");
		}
	}
	return (0);
}

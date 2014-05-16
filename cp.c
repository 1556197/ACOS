#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>

int buf_size = 100;

void cp_file (char* from, char* to) {
	int fd_from = open (from, O_RDONLY);
	if (fd_from < 0) {
		perror ("error_opening_from");
		close (fd_from);
		exit(1);	
	}
	struct stat st;
	stat (from, &st);
	int fd_to = open (to, O_WRONLY | O_TRUNC | O_CREAT, st.st_mode);
	if (fd_to < 0) {
		perror ("error_opening_to");
		close (fd_from);
		close(fd_to);
		exit(1);
	}
	chmod (to, st.st_mode);
	char *buf = malloc (buf_size);
	
	int count = 0, n = 0;
	count = read(fd_from, buf, buf_size);
	if (count == -1) {
		perror("error_reading_from");
		close(fd_from);
		close(fd_to);
		exit(1);
	}
	while(count > 0) {
		n = write(fd_to, buf, count);
		if (n == -1) {
			perror("error_writing_to");
			close(fd_from);
			close(fd_to);
			free(buf);
			exit(1);
		}
		count = read(fd_from, buf, buf_size);
		if (count == -1) {
			perror("error_reading_from");
			close(fd_from);
			close(fd_to);
			free(buf);
			exit(1);
		}
	}
	close(fd_from);
	close(fd_to);
	free(buf);
}

void cp_file_to_dir (char* from, char* to) {
	char* new_to;
	char* file_name = strrchr(from, '/');
	if (!file_name){
		file_name = from;
	}
	else {
		file_name = file_name + 1;
	}
	new_to = malloc(strlen(to) + strlen(file_name) + 6 * sizeof(char));
	strcpy(new_to, to);
	strcat(new_to, "/");
	strcat(new_to, file_name);
	

	int fd_from = open (from, O_RDONLY);
	if (fd_from < 0) {
		perror ("error_opening_from");
		close (fd_from);
		exit(1);	
	}
	struct stat st;
	stat (from, &st);
	int fd_to = open (new_to, O_WRONLY | O_TRUNC | O_CREAT, st.st_mode);
	if (fd_to < 0) {
		perror ("error_opening_to");
		close (fd_from);
		close(fd_to);
		exit(1);
	}
	chmod (new_to, st.st_mode);
	char *buf = malloc (buf_size);
	
	int count = 0, n = 0;
	count = read(fd_from, buf, buf_size);
	if (count == -1) {
		perror("error_reading_from");
		close(fd_from);
		close(fd_to);
		exit(1);
	}
	while(count > 0) {
		n = write(fd_to, buf, count);
		if (n == -1) {
			perror("error_writing_to");
			close(fd_from);
			close(fd_to);
			free(buf);
			exit(1);
		}
		count = read(fd_from, buf, buf_size);
		if (count == -1) {
			perror("error_reading_from");
			close(fd_from);
			close(fd_to);
			free(buf);
			exit(1);
		}
	}
	close(fd_from);
	close(fd_to);
	free(buf);
}

void cp_dir (char* from, char* to) {	
	DIR* dir_from = opendir (from);
	if (!dir_from) {
		perror ("error_opening_dir");
		exit(1);
	}
	
	struct stat buf;
	stat (from, &buf);
	
	mkdir (to, buf.st_mode);
	
	char* dir_name = strrchr(from, '/');
	if (!dir_name)
	dir_name = from;
	else dir_name = dir_name + 1;
	char* copy_from = malloc(strlen(to) + strlen(dir_name) + 2 * sizeof(char));
	strcpy(copy_from, to);
	strcat(copy_from, "/");
	strcat(copy_from, dir_name);
	
	mkdir (copy_from, buf.st_mode);
	
	struct dirent* file;
	while ((file = readdir(dir_from)) != 0) {
		if (strcmp (file->d_name, ".") != 0 && strcmp (file->d_name, "..") != 0) {
			if (file->d_type == DT_REG) {
				char* from_new = malloc (strlen(from) + strlen (file->d_name) + 2 * sizeof(char));
				strcpy (from_new, from);
				strcat (from_new, "/");
				strcat (from_new, file->d_name);
				cp_file_to_dir (from_new, copy_from);
				free (from_new);
			}
			else {
				if (file->d_type == DT_DIR) {
					char* from_new = malloc (strlen(from) + strlen (file->d_name) + 2 * sizeof(char));
					strcpy (from_new, from);
					strcat (from_new, "/");
					strcat (from_new, file->d_name);
					cp_dir (from_new, copy_from);
					free (from_new);
				}
			}
		}
	}
	free (copy_from);
}

int main (int argc, char** argv) {
	if (argc >= 3) {
		if (!strcmp (argv[1], "-R")){
			struct stat buf_stat_from;
			struct stat buf_stat_to;
			int i = 2;
			while (i < argc - 1) {
				stat (argv[i], &buf_stat_from);
				stat (argv[argc - 1], &buf_stat_to);
				if (buf_stat_from.st_mode == S_IFDIR && buf_stat_to.st_mode == S_IFDIR){
					cp_dir (argv[i], argv[argc - 1]);
				}
				else if (buf_stat_to.st_mode == S_IFDIR){
					cp_file_to_dir (argv[i], argv[argc - 1]);
				}
				else {
					perror ("error_argument_1");
					exit(1);
				}
				i ++;
			}
		}
		else {
			struct stat buf_stat_from;
			struct stat buf_stat_to;
			int i = 1;
			while (i < argc - 1) {
				stat (argv[i], &buf_stat_from);
				stat (argv[argc - 1], &buf_stat_to);
				if (buf_stat_from.st_mode != S_IFDIR && buf_stat_to.st_mode != S_IFDIR){
					cp_file (argv[i], argv[argc - 1]);
				}
				else {
					perror ("error_argument_2");
					exit(1);
				}
				i ++;
			}
		}
	}
	else {
		perror("not_enough_arguments");
		exit(1);
	}
	return(0);
}




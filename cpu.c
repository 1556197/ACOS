#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

void handler(int i) {
	exit(0);
}



int main(int argc, char** argv) {
	if (argc < 3) {
		perror("Not enough arguments");
		return 0;
	}
	int pid = fork();
	if (pid == -1) {
		perror("Fork failure");
		return 0;
	} 
	else {
		if (!pid){
			if(execvp(argv[2], argv + 2) == -1) {
				perror("Unable to execute");
				return 0;
			}
		} 
		else {
			struct sigaction s;
			s.sa_handler = handler;
			s.sa_flags = SA_NOCLDSTOP;
			sigaction(SIGCHLD, &s, NULL);
			int time = atoi(argv[1]) * 10000;
			while (1) {
				usleep(time);
				kill(pid, SIGSTOP);
				usleep(1000000 - time);
				kill(pid, SIGCONT);
			}
		}
	}
	return 0;
}





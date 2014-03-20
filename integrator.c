#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

double f (double a){
	return a;
}

double integrate (double a, double b, double dx, double (*f)(double)){
	double x, result;
	for (x = a; x < b; x += dx){
		result += (f(x) + f(x + dx))/2 * dx;
	}
	return result;
}

double mp_integrate (double a, double b, double dx, double (*f)(double), int procNum){
	double delta = (b - a) / procNum;
	for (int i = 0; i < procNum; ++i){
		int pid = fork();
		if (pid == 0){
			int fd = open ("tmp", O_WRONLY|O_CREAT|O_APPEND, 0666);
			double res = integrate (a + delta * i, a + delta * (i + 1), dx, f);
			write (fd, &res, sizeof(res));
			close (fd);
			exit(0);
		}
		else {
			if (pid == -1){
				printf ("error\n");
			}
		}
	}
	double res_total = 0, res;
	int fd = open ("tmp", O_RDONLY|O_APPEND);
	for (int i = 0; i < procNum; ++i){
		while (read (fd, &res, sizeof (res)) != sizeof(res)){
			sleep(1);
		}
		res_total += res;
	}
	close (fd);
	unlink ("tmp");
	return res_total;
}

int main (int argc, char **argv){
	printf ("%f", mp_integrate (0, 6, 1, f, 6));
	printf ("\n");
	return 0;
}

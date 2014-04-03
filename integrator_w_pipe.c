#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

double f (double a){
	return a;
}

double integrate (double a, double b, double dx, double (*f)(double)){
	double x, result = 0;
	for (x = a; x < b; x += dx){
		result += (f(x) + f(x + dx))/2 * dx;
	}
	return result;
}

double process (double a, double b, double dx, double (*f)(double), int num){
	double d = (b - a) / num;
	int fd[2];
	pipe (fd);
	for (int i = 0; i < num; ++i){
		int indicator = fork();
		if (indicator == 0){
			close (fd[0]);
			double res = integrate (a + d * i, a + d * (i + 1), dx, f);
			write (fd[1], &res, sizeof(res));
			close (fd[1]);
			exit(0);
		}
		else {
			if (indicator == -1){
				printf ("fork error /n");
			}
		}
	}	
	double result = 0, res;
	for (int i = 0; i < num; ++i){
		read (fd[0], &res, sizeof (res));
		result += res;
	}
	close (fd[1]);
	close (fd[0]);
	return result;
}

int main (int argc, char **argv){
	printf ("%f", process (0, 6, 1, f, 6));
	printf ("\n");
	return 0;
}

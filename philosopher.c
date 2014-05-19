#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int lifecycle_num = 1;
pthread_t philosophers[10];
pthread_mutex_t forks[10];


void* lifetime (void *k){
	int id = (int) k;
	int i = 0;
	int randomSeed = id;
	for (i; i < lifecycle_num; i ++){
		randomSeed = rand_r(&randomSeed);
		int sleeptime = (randomSeed % 2) + 1;
		printf("Philosopher %d fell asleep\n", id);
		sleep(sleeptime);
		
		int primary_fork;
		int secondary_fork;
		if (id % 2 == 0){
			primary_fork = id;
			secondary_fork = (id + 10 - 1) % 10;      //number of philosophers is used
		}
		else {
			primary_fork = id - 1;
			secondary_fork = id;
		}
		
		printf("Philosopher %d woke up and trying to grab fork #%d\n", id, primary_fork);
		
		randomSeed = rand_r(&randomSeed);
		int waittime1 = (randomSeed % 2) + 1;
		
		while (pthread_mutex_trylock(&forks[primary_fork])){
			sleep (waittime1);
			printf("Philosopher %d was waiting for fork #%d\n", id, primary_fork);
		}
		
		printf("Philosopher %d grabbed fork #%d and trying to grab the second one #%d\n", id, primary_fork, secondary_fork);
		
		randomSeed = rand_r(&randomSeed);
		int waittime2 = (randomSeed % 2) + 1;
		
		while (pthread_mutex_trylock(&forks[secondary_fork])){
			sleep (waittime2);
			printf("Philosopher %d was waiting for fork #%d\n", id, secondary_fork);
		}
		
		printf("Philosopher %d grabbed fork #%d and #%d and now begins to eat\n", id, primary_fork, secondary_fork);
		
		randomSeed = rand_r(&randomSeed);
		int eattime = (randomSeed % 2) + 1;
		sleep (eattime);
		printf("Philosopher %d is well fed and leaving forks #%d and #%d\n", id, primary_fork, secondary_fork);
		
		pthread_mutex_unlock (&forks[primary_fork]);
		pthread_mutex_unlock (&forks[secondary_fork]);
	}
	pthread_exit (NULL);
}

int main (int argc, char* argv[]){     // use (gcc -pthread -o term philosopher.c) to compile
	srandom (time(NULL));
	int i = 0;
	int n = 10;            //number of philosophers is used
	for (i = 0; i < n; i++){
		if (pthread_mutex_init (&forks[i], NULL)){
			printf("Error init");
		}
	}
	for (i = 0; i < n; i++){
		if (pthread_create (&philosophers[i], NULL, lifetime, (void*)i)){
			printf("Error create");
		}
	}
	void** retval;
	for (i = 0; i < n; i++){
		pthread_join(philosophers[i], NULL);
	}
	for (i = 0; i < n; i++){
		pthread_mutex_destroy (&forks[i]);
	}
	printf("The end.\n");
	return 0;
}

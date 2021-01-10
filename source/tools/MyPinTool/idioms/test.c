#include <stdio.h>
#include <pthread.h>

int a = 0;

void *func1(void *arg) {
	a = 1;
	a = 2;
	a = 3;
	a = 4;
	a = 5;
	return NULL;
}

void *func2(void *arg) {
	a = 1;
	a = 2;
	a = 3;
	a = 4;
	a = 5;
	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_t t1;
	pthread_t t2;

	pthread_create(&t1, NULL, func1, NULL);
	pthread_create(&t2, NULL, func2, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}
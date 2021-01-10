#include <stdio.h>
#include <pthread.h>

int a = 0;
int b = 0;
int c = 0;

void *func1(void *arg) {
	b = 1;
	a = 1;
	c = 454;
	if(a == 2) {
		printf("Bug manifest\n");
	}
	b = 323;
	return NULL;
}

void *func2(void *arg) {
	c = 64;
	if (a == 1) {
		a = 2;
	}
	b = 45;
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

	// printf("a == 2 means bug manifests. Right now a == %d.\n", a);

	return 0;
}
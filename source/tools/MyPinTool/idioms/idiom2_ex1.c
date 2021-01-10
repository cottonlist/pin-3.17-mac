#include <stdio.h>
#include <pthread.h>

volatile static int a = 0;

pthread_mutex_t lock;

struct filter {
	int value;
	int isLoggable;
};

int global = 3;

struct filter f;
struct filter *p;

void *func1(void *arg) {
	if (p!=NULL){
		printf("Value of filter is %d\n", p->value);
	}
	pthread_mutex_lock(&lock);
	pthread_mutex_unlock(&lock);
	return NULL;
}

void *func2(void *arg) {
	global = 1;
	p = NULL;
	global = 2;
	return NULL;
}

int main(int argc, char const *argv[])
{
	f.value = 10;
	f.isLoggable = 1;
	p = &f;
	
	pthread_t t1;
	pthread_t t2;

	pthread_create(&t1, NULL, func1, NULL);
	pthread_create(&t2, NULL, func2, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}
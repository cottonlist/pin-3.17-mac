#include <stdio.h>
#include <pthread.h>
FILE *fp;

int a = 0;

struct filter {
	int value;
	int isLoggable;
};

int global = 3;

int error = 0;

struct filter f;
struct filter *p;

void *func1(void *arg) {
	if (p!=NULL){
		if(p == NULL) {
			error = 1;
		}
		printf("Value of filter is %d\n", p->value);
	}
	return NULL;
}

void *func2(void *arg) {
	global = 3;
	p = NULL;
	global = 56;
	return NULL;
}

int main(int argc, char const *argv[])
{
	f.value = 10;
	f.isLoggable = 1;
	p = &f;

	fp = fopen("../errorflag_idiom2_ex1", "w");
	fprintf(fp, "%d", error);
	fclose(fp);
	
	pthread_t t1;
	pthread_t t2;

	pthread_create(&t1, NULL, func1, NULL);
	pthread_create(&t2, NULL, func2, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}
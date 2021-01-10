#include <stdio.h>

struct filter {
	int value;
	int isLoggable;
};

struct filter f;
struct filter *p;

FILE *fp;

int error = 0;

int main(int argc, char const *argv[])
{
	f.value = 10;
	f.isLoggable = 1;
	p = &f;

	fp = fopen("../error_flag", "w");

	p = NULL;
	if (p == NULL)
	{
		printf("p is a pointer to NULL.\n");
		// error = 1;
		// fputs("error", fp);
		fprintf(fp, "%d", error);
		fclose(fp);
	}
	// printf("%d\n", p->value);

	
	return 0;
}
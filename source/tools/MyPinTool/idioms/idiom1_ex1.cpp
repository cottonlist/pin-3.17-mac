#include <stdio.h>
#include <pthread.h>
#include <string>
#include <iostream>

int * p1;
int * p2;
int q = 4;

using namespace std;

class Student
{
public:

	string name;
	Student(string a){
		this->name = a;
	}
	~Student();
	
};

Student * student1;

void *func1(void *arg) {
	
	student1 = new Student("taro");
	cout << student1->name << endl;

	return NULL;
}

void *func2(void *arg) {
	// cout << student1->name << endl;
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

	// printf("Final value of a is %d.\n", a);

	return 0;
}
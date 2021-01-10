#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

ifstream InFile1("nonStackInsCount");
ifstream InFile2("numOfInsTh1_v2");
ifstream InFile3("numOfInsTh2_v2");
ofstream OutFile1("execution_order_v2");

int executionOrder1;
int executionOrder2;
int executionOrder3;
int executionOrder4;

class ExecutedInstruction {
	public:
		int threadId;
		int memoryOffset;
};

class InstructionThread1 {
    public:
    	int memoryOffset;
};

class InstructionThread2 {
    public:
    	int memoryOffset;
};

ExecutedInstruction * totalIns;
InstructionThread1 * insTh1;
InstructionThread2 * insTh2;

int main(int argc, char const *argv[])
{
    int totalNumOfIns = 0;
    int lenOfTh1 = 0;
    int lenOfTh2 = 0;

    int indexTh1 = 0;
    int indexTh2 = 0;

    FILE * fp1;
    fp1 = fopen("offsetInfo_v2.log", "r");
    int index = 0;

    string temp1;
    getline(InFile1, temp1);
	stringstream ss1(temp1);
	ss1 >> totalNumOfIns;
    totalIns = new ExecutedInstruction[totalNumOfIns];
    cout << totalNumOfIns << endl;

    string temp2;
    getline(InFile2, temp2);
    stringstream ss2(temp2);
    ss2 >> lenOfTh1;
    insTh1 = new InstructionThread1[lenOfTh1];
    cout << lenOfTh1 << endl;

    string temp3;
    getline(InFile3, temp3);
    stringstream ss3(temp3);
    ss3 >> lenOfTh2;
    insTh2 = new InstructionThread2[lenOfTh2];
    cout << lenOfTh2 << endl;

    while((fscanf(fp1, "%d, %d", &totalIns[index].memoryOffset, &totalIns[index].threadId)) != EOF) {
        index++;
    }

    for (int i = 0; i < totalNumOfIns; i++)
    {
        cout << totalIns[i].memoryOffset << ", " << totalIns[i].threadId << endl;
        if (totalIns[i].threadId == 1) {
            insTh1[indexTh1].memoryOffset = totalIns[i].memoryOffset;
            indexTh1++; 
        } else if (totalIns[i].threadId == 2) {
            insTh2[indexTh2].memoryOffset = totalIns[i].memoryOffset;
            indexTh2++;
        }
    }

    for (int i = 0; i < lenOfTh1; i++)
    {
        cout << "Instruction of thread1: " << insTh1[i].memoryOffset << endl;
    }

    for (int i = 0; i < lenOfTh2; i++) {
        cout << "Instruction of thread2: " << insTh2[i].memoryOffset << endl;
    }

    int randomNumber1;
    int randomNumber2;

    srand(time(NULL));

    randomNumber1 = rand() % 100;
    randomNumber2 = rand() % 100;

    cout << "Value of randomNumber1 is " << randomNumber1 << endl;
    cout << "Value of randomNumber2 is " << randomNumber2 << endl;

    executionOrder1 = rand() % (lenOfTh1);
    executionOrder2 = rand() % (lenOfTh1);
    while (executionOrder2 == executionOrder1)
    {
        executionOrder2 = rand() % (lenOfTh1);
    }

    executionOrder3 = rand() % (lenOfTh2);
    executionOrder4 = rand() % (lenOfTh2);
    while (executionOrder4 == executionOrder3)
    {
        executionOrder3 = rand() % (lenOfTh2);
    }
    
    cout << "Execution order 1: " << executionOrder1 << endl;
    cout << "Execution order 2: " << executionOrder2 << endl;
    cout << "Execution order 3: " << executionOrder3 << endl;
    cout << "Execution order 4: " << executionOrder4 << endl;

    int branch = rand() % 2;

    int order1;
    int order2;
    int order3;
    int order4;

    if (branch == 1) {
        cout << "Test order: " << endl; 
        cout << insTh1[min(executionOrder1, executionOrder2)].memoryOffset << endl;
        order1 = insTh1[min(executionOrder1, executionOrder2)].memoryOffset;

        cout << insTh2[min(executionOrder3, executionOrder4)].memoryOffset << endl;
        order2 = insTh2[min(executionOrder3, executionOrder4)].memoryOffset;

        cout << insTh1[max(executionOrder1, executionOrder2)].memoryOffset << endl;
        order3 = insTh1[max(executionOrder1, executionOrder2)].memoryOffset;

        cout << insTh2[max(executionOrder3, executionOrder4)].memoryOffset << endl;
        order4 = insTh2[max(executionOrder3, executionOrder4)].memoryOffset;

        OutFile1 << order1 << ", " << order2 << ", " << order3 << ", " << order4 << endl;
    } else if (branch == 0) {
        cout << "Test order: " << endl; 
        cout << insTh2[min(executionOrder3, executionOrder4)].memoryOffset << endl;
        order1 = insTh2[min(executionOrder3, executionOrder4)].memoryOffset;

        cout << insTh1[min(executionOrder1, executionOrder2)].memoryOffset << endl;
        order2 = insTh1[min(executionOrder1, executionOrder2)].memoryOffset;

        cout << insTh2[max(executionOrder3, executionOrder4)].memoryOffset << endl;
        order3 = insTh2[max(executionOrder3, executionOrder4)].memoryOffset;

        cout << insTh1[max(executionOrder1, executionOrder2)].memoryOffset << endl;
        order4 = insTh1[max(executionOrder1, executionOrder2)].memoryOffset;

        OutFile1 << order1 << ", " << order2 << ", " << order3 << ", " << order4 << endl;

    }

    InFile1.close();
    InFile2.close();
    InFile3.close();
    OutFile1.close();

    delete[] totalIns;
    delete[] insTh1;
    delete[] insTh2;

    return 0;
}

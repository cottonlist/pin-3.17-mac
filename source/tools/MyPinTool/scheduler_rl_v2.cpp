#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "pin.H"

using std::cout;
using std::endl;
using std::string;
using namespace std;

// PIN_LOCK pinLock;

PIN_SEMAPHORE pinSemaphore1;
PIN_SEMAPHORE pinSemaphore2;
PIN_SEMAPHORE pinSemaphore3;

ADDRINT img_low;
ADDRINT offset;
ADDRINT ins_addr;

FILE * InFile;
ofstream OutFile("trace_v2");
string trace;

int threadMark;

// static int ins_no_th1 = 0;
// static int ins_no_th2 = 0;

// static int interleaving[] = {64, 32, 31, 28, 63, 60, 58, 26, 22, 54, 12, 44, 43, 11};
// static int interleaving[] = {64, 32, 63, 31, 60, 28, 58, 26, 54, 22, 44, 12, 43, 11};
// static int interleaving[] = {32, 64, 31, 63, 28, 60, 26, 58, 22, 54, 12, 44, 11, 43};
int * interleaving;
// int interleaving[] = {15837, 16132, 15851, 16177};

VOID signalWrapperSemaphore1(THREADID threadId) {
	PIN_SemaphoreSet(&pinSemaphore1);
}

VOID signalWrapperSemaphore2(THREADID threadId) {
	PIN_SemaphoreSet(&pinSemaphore2);
}

VOID signalWrapperSemaphore3(THREADID threadId) {
	PIN_SemaphoreSet(&pinSemaphore3);
}

VOID waitWrapperSemaphore1(THREADID threadId) {
	PIN_SemaphoreWait(&pinSemaphore1);
}

VOID waitWrapperSemaphore2(THREADID threadId) {
	PIN_SemaphoreWait(&pinSemaphore2);
}

VOID waitWrapperSemaphore3(THREADID threadId) {
	PIN_SemaphoreWait(&pinSemaphore3);
}

VOID instruct(ADDRINT ins_addr, ADDRINT accessed_addr,THREADID threadid) {
	int accessed_offset = accessed_addr - img_low;
	if (threadid == 0 || accessed_offset != 32816) {
		return;
	} else {
		int offset = ins_addr - img_low;
		// cout << "Offset is " << offset << endl;
		string str;
		stringstream ss;
		ss << offset;
		ss >> str;
		trace.append(str);
		trace.append("\n");
		// cout << trace << endl;
	}
}

VOID instrument(INS ins, void *v) {
	IMG img = IMG_FindByAddress(INS_Address(ins));
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
	{
		return;
	}

	ins_addr = INS_Address(ins);
	offset = ins_addr - img_low;

	// int threadid = PIN_ThreadId();
	// BOOL isBranch(INS_IsBranch(ins));

	// if (threadid != 0) {
	// 	cout << ins_addr << endl;
	// 	cout << offset << endl;
	// 	cout << threadid << endl;
	// 	cout << isBranch << endl;
	// 	cout << "" << endl;

	// }


	// if(offset == interleaving[0]) {
	// 	if (INS_IsValidForIpointAfter(ins)) {
	// 		INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)signalWrapperSemaphore1, IARG_THREAD_ID, IARG_END);
	// 	}
	// }

	// if(offset == interleaving[13]) {
	// 	INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)waitWrapperSemaphore1, IARG_THREAD_ID, IARG_END);
	// }

	// for (int i = 1; i < 13; i++) {
	// 	if (offset == interleaving[i]){
	// 		if (i % 2 == 1) {
	// 			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)waitWrapperSemaphore1, IARG_THREAD_ID, IARG_END);
	// 			if (INS_IsValidForIpointAfter(ins)) {
	// 				INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)signalWrapperSemaphore2, IARG_THREAD_ID, IARG_END);
	// 			}
	// 		} else if (i%2==0) {
	// 			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)waitWrapperSemaphore2, IARG_THREAD_ID, IARG_END);
	// 			if (INS_IsValidForIpointAfter(ins)) {
	// 				INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)signalWrapperSemaphore1, IARG_THREAD_ID, IARG_END);
	// 			}				
	// 		}
	// 	}
	// }
	
	bool isMemoryRead = INS_IsMemoryRead(ins);
	bool isMemoryWrite = INS_IsMemoryWrite(ins);
	bool isStackRead = INS_IsStackRead(ins);
	bool isStackWrite = INS_IsStackWrite(ins);

	if (isMemoryRead == 1 && isStackRead == 0) {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)instruct, IARG_INST_PTR, IARG_MEMORYREAD_EA, IARG_THREAD_ID, IARG_END);
	} else if (isMemoryWrite == 1 && isStackWrite == 0) {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)instruct, IARG_INST_PTR, IARG_MEMORYWRITE_EA, IARG_THREAD_ID, IARG_END);
	}

	if (offset == interleaving[0]) {
		if (INS_IsValidForIpointAfter(ins)) {
			INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)signalWrapperSemaphore1, IARG_THREAD_ID, IARG_END);
		}
	} else if (offset == interleaving[1]) {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)waitWrapperSemaphore1, IARG_THREAD_ID, IARG_END);
		if (INS_IsValidForIpointAfter(ins)) {
			INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)signalWrapperSemaphore2, IARG_THREAD_ID, IARG_END);
		}
	} else if (offset == interleaving[2]) {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)waitWrapperSemaphore2, IARG_THREAD_ID, IARG_END);
		if (INS_IsValidForIpointAfter(ins)) {
			INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)signalWrapperSemaphore3, IARG_THREAD_ID, IARG_END);
		}
	} else if (offset == interleaving[3]) {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)waitWrapperSemaphore3, IARG_THREAD_ID, IARG_END);
	}
}

VOID ImageLoad(IMG img, void *v) {
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
	{
		return;
	}
	img_low = IMG_LowAddress(img);

}

VOID Fini(INT32 code, VOID *v) {
	OutFile << trace;
	OutFile.close();
	delete interleaving;
}

int main(int argc, char *argv[])
{
	interleaving = new int[4];

	InFile = fopen("execution_order_v2", "r");

	while((fscanf(InFile, "%d, %d, %d, %d", &interleaving[0], &interleaving[1], &interleaving[2], &interleaving[3])) != EOF) {
    	;
    }

	cout << "Thread interleaving: " << endl;
	cout << interleaving[0] << endl;
	cout << interleaving[1] << endl;
	cout << interleaving[2] << endl;
	cout << interleaving[3] << endl;

	PIN_InitSymbols();
	PIN_Init(argc, argv);

	// PIN_InitLock(&pinLock);

	PIN_SemaphoreInit(&pinSemaphore1);
	PIN_SemaphoreInit(&pinSemaphore2);
	PIN_SemaphoreInit(&pinSemaphore3);

	IMG_AddInstrumentFunction(ImageLoad, NULL);
	INS_AddInstrumentFunction(instrument, NULL);
		
	PIN_AddFiniFunction(Fini, 0);

	// INS_AddInstrumentFunction(idiom_1_wrapper, NULL);

	PIN_StartProgram();

	return 0;
}








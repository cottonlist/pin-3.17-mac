#include <pthread.h>
#include <string>
#include <iostream>
#include <fstream>
#include "pin.H"

using namespace std;

class ExecutedInstruction {
	public:
		int threadId;
		int threadIdByPin;
		ADDRINT memoryAddress;
		ADDRINT memoryOffset;
		ADDRINT accessedMemoryAddress;
		ADDRINT accessedMemoryOffset;
		bool isMemoryWrite;
		bool isMemoryRead;
		int executedOrder;
};

ifstream ReadFile("instruction_count.log");
ofstream WriteFile1("profiler_rl.log");
ofstream WriteFile2("numOfInsTh1");
ofstream WriteFile3("numOfInsTh2");
ofstream WriteFile4("offsetInfo.log");

static string numberOfInstructions;
static int numOfInstructions;

ExecutedInstruction *ins;

static int index = 0;
int numOfInsTh1 = 0;
int numOfInsTh2 = 0;

using namespace std;
using std::cout;
using std::endl;

ADDRINT img_low;
ADDRINT offset;

FILE * trace;
// static int finish = 0;

VOID write_instrument(ADDRINT ins_addr, ADDRINT accessed_addr, THREADID threadid){
	if (threadid == 0) {
		return;
	} else {
		ins[index].threadIdByPin = threadid;
		ins[index].memoryAddress = ins_addr;
		ins[index].memoryOffset = ins_addr - img_low;
		ins[index].accessedMemoryAddress = accessed_addr;
		ins[index].accessedMemoryOffset = accessed_addr - img_low;
		ins[index].isMemoryWrite = true;
		ins[index].isMemoryRead = false;
		index++;
	}
}

VOID read_instrument(ADDRINT ins_addr, ADDRINT accessed_addr, THREADID threadid){
	if (threadid == 0) {
		return;
	} else {
		ins[index].threadIdByPin = threadid;
		ins[index].memoryAddress = ins_addr;
		ins[index].memoryOffset = ins_addr - img_low;
		ins[index].accessedMemoryAddress = accessed_addr;
		ins[index].accessedMemoryOffset = accessed_addr - img_low;
		ins[index].isMemoryWrite = false;
		ins[index].isMemoryRead = true;
		index++;
	}
}

VOID instrument(ADDRINT ins_addr, THREADID threadid) {
	if (threadid == 0) {
		return;
	} else {
		ins[index].threadIdByPin = threadid;
		ins[index].memoryAddress = ins_addr;
		ins[index].memoryOffset = ins_addr - img_low;
		ins[index].accessedMemoryAddress = 0;
		ins[index].accessedMemoryOffset = 0;
		ins[index].isMemoryWrite = false;
		ins[index].isMemoryRead = false;
		index++;
	}
}

VOID Instruction(INS ins, void *v) {
	IMG img = IMG_FindByAddress(INS_Address(ins));
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
	{
		return;
	}

	BOOL isBranch;
	isBranch = INS_IsBranch(ins);
	if(isBranch == 1) {
		return;
	} else if (INS_IsMemoryRead(ins))
	{
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)read_instrument, IARG_INST_PTR, 
			IARG_MEMORYREAD_EA, IARG_THREAD_ID, IARG_END);
	} 
	else if (INS_IsMemoryWrite(ins))
	{
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)write_instrument, IARG_INST_PTR, 
			IARG_MEMORYWRITE_EA, IARG_THREAD_ID, IARG_END);
	} else {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)instrument, IARG_INST_PTR, IARG_THREAD_ID, IARG_END);
	}
}

VOID ImageLoad(IMG img, void *v) {
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
	{
		return;
	}
	img_low = IMG_LowAddress(img);

}

VOID Fini(INT32 code, VOID *v){
	ReadFile.close();
	int smallestIndex = 0;
	int smallestOffset = ins[0].memoryOffset;
	for (int i = 0; i < index; i++) {
		if (ins[i].memoryOffset < smallestOffset) {
			smallestOffset = ins[i].memoryOffset;
			smallestIndex = i;
		}
	}
	cout << "Smallest index is " << smallestIndex << endl;

	ins[smallestIndex].threadId = 1;
	for (int i = 0; i < index; i++) {
		if (ins[i].threadIdByPin == ins[smallestIndex].threadIdByPin) {
			ins[i].threadId = 1;
		} else {
			ins[i].threadId = 2;
		}
	}
	for (int i = 0; i < index; i++) {
		cout << ins[i].memoryOffset << ", " << ins[i].threadIdByPin << ", " << ins[i].threadId << endl;
		WriteFile1 << ins[i].memoryOffset << ", " << ins[i].threadId << ", " << ins[i].accessedMemoryOffset << ", " << ins[i].isMemoryRead << ", " << ins[i].isMemoryWrite << endl;
		WriteFile4 << ins[i].memoryOffset << ", " << ins[i].threadId << endl;
		if (ins[i].threadId == 1) {
			numOfInsTh1++;
		} else if(ins[i].threadId == 2) {
			numOfInsTh2++;
		}
	}
	WriteFile2 << numOfInsTh1 << endl;
	WriteFile3 << numOfInsTh2 << endl;
	delete[] ins;
	WriteFile1.close();
	WriteFile2.close();
	WriteFile3.close();
	WriteFile4.close();
}

int main(int argc, char *argv[])
{

	getline(ReadFile, numberOfInstructions);
	stringstream stream(numberOfInstructions);
	stream >> numOfInstructions;

	ins = new ExecutedInstruction[numOfInstructions];


	cout << numOfInstructions << endl;

	PIN_InitSymbols();
	PIN_Init(argc, argv);

	trace = fopen("trace_rl", "w");

	// ImageLoad is used to fetch address of entry point address
	IMG_AddInstrumentFunction(ImageLoad, NULL);
	INS_AddInstrumentFunction(Instruction, NULL);

	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram();
	
	return 0;
}
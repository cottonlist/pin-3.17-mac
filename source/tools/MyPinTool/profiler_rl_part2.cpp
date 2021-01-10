#include <iostream>
#include <fstream>
#include <pthread.h>
#include "pin.H"
#include <string>

using namespace std;

static PIN_LOCK pinLock;

ifstream ReadFile1("nonStackInsCount");
ofstream WriteFile1("profiler_rl_part2.log");

class ExecutedInstruction {
	public:
		ADDRINT memoryOffset;
		bool isMemoryRead;
		bool isMemoryWrite;
		bool isStackRead;
		bool isStackWrite;
};

string numberOfInstructions;
int numOfInstructions;

ExecutedInstruction *instr;

int index = 0;

ADDRINT img_low;
ADDRINT ins_addr;
ADDRINT offset;

// static int interleaving[] = {28, 116, 19};


VOID profile(THREADID threadId){
    if (threadId == 0) {
        return;
    } 
    // else {
    //     PIN_GetLock(&pinLock, threadId+1);
    //     cout << "Offset: " << offset << endl;
    //     cout << "Ins_addr: " << ins_addr << endl;
    //     PIN_ReleaseLock(&pinLock);
    // }
}

VOID instrument(INS ins, void *v) {
	IMG img = IMG_FindByAddress(INS_Address(ins));
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
	{
		return;
	}

	ins_addr = INS_Address(ins);

	int threadId;
	threadId = PIN_ThreadId();

	bool isMemoryRead;
	isMemoryRead = INS_IsMemoryRead(ins);

	bool isMemoryWrite;
	isMemoryWrite = INS_IsMemoryWrite(ins);

	bool isStackRead;
	isStackRead = INS_IsStackRead(ins);

	bool isStackWrite;
	isStackWrite = INS_IsStackWrite(ins);

	bool isControlFlow = INS_IsControlFlow(ins);

	if(threadId != 0){
		if ((isMemoryRead == 1 || isMemoryWrite==1) && isStackRead ==0 && isStackWrite == 0&&isControlFlow==0)
		{
			offset = ins_addr - img_low;
			instr[index].memoryOffset = offset;
			instr[index].isMemoryRead = isMemoryRead;
			instr[index].isMemoryWrite = isMemoryWrite;
			instr[index].isStackRead = isStackRead;
			instr[index].isStackWrite = isStackWrite;
			index++;
		}
	} else {
    	INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)profile, IARG_THREAD_ID, IARG_END);
	}
	
}

VOID Fini(INT32 code, VOID *v){
	ReadFile1.close();
	for (int i = 0; i < index; i++) {
		WriteFile1 << instr[i].memoryOffset << ", " << instr[i].isMemoryRead << ", " << instr[i].isMemoryWrite << ", " << instr[i].isStackRead << ", " << instr[i].isStackWrite << endl;
	}
}

VOID ImageLoad(IMG img, void *v) {
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
	{
		return;
	}
	img_low = IMG_LowAddress(img);
}

int main(int argc, char *argv[])
{
	getline(ReadFile1, numberOfInstructions);
	stringstream stream(numberOfInstructions);
	stream >> numOfInstructions;

	instr = new ExecutedInstruction[numOfInstructions];

	PIN_InitSymbols();
	PIN_Init(argc, argv);

	PIN_InitLock(&pinLock);

	IMG_AddInstrumentFunction(ImageLoad, NULL);

	INS_AddInstrumentFunction(instrument, NULL);

	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram();

	return 0;
}
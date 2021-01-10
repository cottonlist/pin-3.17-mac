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
};

ifstream ReadFile("nonStackInsCount");
ofstream WriteFile1("profiler_rl_part1.log");
ofstream WriteFile2("numOfInsTh1_v2");
ofstream WriteFile3("numOfInsTh2_v2");
ofstream WriteFile4("offsetInfo_v2.log");

static string numberOfInstructions;
static int numOfInstructions;

ExecutedInstruction *instr;

static int index = 0;
int numOfInsTh1 = 0;
int numOfInsTh2 = 0;

using namespace std;
using std::cout;
using std::endl;

ADDRINT img_low;
ADDRINT offset;

// static int finish = 0;

VOID write_instrument(ADDRINT ins_addr, ADDRINT accessed_addr, THREADID threadid){
	if (threadid == 0) {
		return;
	} else {
		instr[index].threadIdByPin = threadid;
		instr[index].memoryAddress = ins_addr;
		instr[index].memoryOffset = ins_addr - img_low;
		instr[index].accessedMemoryAddress = accessed_addr;
		instr[index].accessedMemoryOffset = accessed_addr - img_low;

		cout << "Accessed memory offset: " << accessed_addr - img_low << endl;
		// instr[index].isMemoryWrite = true;
		// instr[index].isMemoryRead = false;
		index++;
	}
}

VOID read_instrument(ADDRINT ins_addr, ADDRINT accessed_addr, THREADID threadid){
	if (threadid == 0) {
		return;
	} else {
		instr[index].threadIdByPin = threadid;
		instr[index].memoryAddress = ins_addr;
		instr[index].memoryOffset = ins_addr - img_low;
		instr[index].accessedMemoryAddress = accessed_addr;
		instr[index].accessedMemoryOffset = accessed_addr - img_low;
		cout << "Accessed memory offset: " << accessed_addr - img_low << endl;

		// instr[index].isMemoryWrite = false;
		// instr[index].isMemoryRead = true;
		index++;
	}
}

VOID Instruction(INS ins, void *v) {
	IMG img = IMG_FindByAddress(INS_Address(ins));

	bool isMemoryRead = INS_IsMemoryRead(ins);
	bool isMemoryWrite = INS_IsMemoryWrite(ins);
	bool isStackRead = INS_IsStackRead(ins);
	bool isStackWrite = INS_IsStackWrite(ins);

	// BOOL isBranch;
	// isBranch = INS_IsBranch(ins);

	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img)) {
        return;
    } else if ((isMemoryRead==1 || isMemoryWrite==1) && isStackRead==0 && isStackWrite==0) {
        if (INS_IsMemoryRead(ins))
		{
			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)read_instrument, IARG_INST_PTR, 
				IARG_MEMORYREAD_EA, IARG_THREAD_ID, IARG_END);
		} 
		else if (INS_IsMemoryWrite(ins))
		{			
			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)write_instrument, IARG_INST_PTR, 
				IARG_MEMORYWRITE_EA, IARG_THREAD_ID, IARG_END);
		}
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
	int smallestOffset = instr[0].memoryOffset;
	for (int i = 0; i < index; i++) {
		if (instr[i].memoryOffset < smallestOffset) {
			smallestOffset = instr[i].memoryOffset;
			smallestIndex = i;
		}
	}
	cout << "Smallest index is " << smallestIndex << endl;

	instr[smallestIndex].threadId = 1;
	for (int i = 0; i < index; i++) {
		if (instr[i].threadIdByPin == instr[smallestIndex].threadIdByPin) {
			instr[i].threadId = 1;
		} else {
			instr[i].threadId = 2;
		}
	}
	for (int i = 0; i < index; i++) {
		cout << instr[i].memoryOffset << ", " << instr[i].threadIdByPin << ", " << instr[i].threadId << endl;
		WriteFile1 << instr[i].memoryOffset << ", " << instr[i].threadId << ", " << instr[i].accessedMemoryOffset << ", " << endl;
		WriteFile4 << instr[i].memoryOffset << ", " << instr[i].threadId << endl;
		if (instr[i].threadId == 1) {
			numOfInsTh1++;
		} else if(instr[i].threadId == 2) {
			numOfInsTh2++;
		}
	}
	WriteFile2 << numOfInsTh1 << endl;
	WriteFile3 << numOfInsTh2 << endl;
	delete[] instr;
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

	instr = new ExecutedInstruction[numOfInstructions];

	cout << numOfInstructions << endl;

	PIN_InitSymbols();
	PIN_Init(argc, argv);

	// ImageLoad is used to fetch address of entry point address
	IMG_AddInstrumentFunction(ImageLoad, NULL);
	INS_AddInstrumentFunction(Instruction, NULL);

	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram();
	
	return 0;
}
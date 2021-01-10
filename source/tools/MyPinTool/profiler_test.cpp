#include <iostream>
#include <pthread.h>
#include "pin.H"
#include <string>

using std::cout;
using std::endl;
using namespace std;

static PIN_LOCK pinLock;


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

	string disassemble_name;
	disassemble_name = INS_Disassemble(ins);

	bool isAtomicUpdate;
	isAtomicUpdate = INS_IsAtomicUpdate(ins);

	bool isControlFlow;
	isControlFlow = INS_IsControlFlow(ins);

	bool isMemoryRead;
	isMemoryRead = INS_IsMemoryRead(ins);

	bool isMemoryWrite;
	isMemoryWrite = INS_IsMemoryWrite(ins);

	bool isStackRead;
	isStackRead = INS_IsStackRead(ins);

	bool isStackWrite;
	isStackWrite = INS_IsStackWrite(ins);

	bool isBranch = INS_IsBranch(ins);
	bool isCall = INS_IsCall(ins);
	bool isOriginal = INS_IsOriginal(ins);
	string ins_mnemonic = INS_Mnemonic(ins);

	if(threadId != 0){
		if((isMemoryRead || isMemoryWrite)&&isStackRead==0&&isStackWrite==0){
			offset = ins_addr - img_low;
		    cout << ins_addr << endl;
		    cout << offset << endl;
		    cout << "Disassemble name: " << disassemble_name << endl;
		    cout << "Thread id assigned by pin: " << threadId << endl;
		    cout << "Is atomic update? " << isAtomicUpdate << endl;
		    cout << "Is control flow? " << isControlFlow << endl;
		    cout << "Is memory read? " << isMemoryRead << endl;
		    cout << "Is memory write? " << isMemoryWrite << endl;
		    cout << "Is stack read? " << isStackRead << endl;
		    cout << "Is stack write? " << isStackWrite << endl;
		    cout << "Is branch? " << isBranch << endl;
		    cout << "Is call? " << isCall << endl;
		    cout << "Is original? " << isOriginal << endl;
		    cout << "Mnemonic: " << ins_mnemonic << endl;
		    cout << endl;
		}
	}
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)profile, IARG_THREAD_ID, IARG_END);
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
	PIN_InitSymbols();
	PIN_Init(argc, argv);

	PIN_InitLock(&pinLock);

	IMG_AddInstrumentFunction(ImageLoad, NULL);

	INS_AddInstrumentFunction(instrument, NULL);

	PIN_StartProgram();

	return 0;
}
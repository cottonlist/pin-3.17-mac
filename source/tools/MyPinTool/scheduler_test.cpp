#include <iostream>
#include <pthread.h>
#include "pin.H"

using std::cout;
using std::endl;

static PIN_LOCK pinLock;

static PIN_SEMAPHORE pinSemaphore1;
static PIN_SEMAPHORE pinSemaphore2;
static PIN_SEMAPHORE pinSemaphore3;

ADDRINT img_low;
ADDRINT ins_addr;
ADDRINT offset;

// static int interleaving[] = {28, 116, 19};
static int interleaving[] = {15748, 15824, 16175, 15844};

VOID wait_1() {
	PIN_SemaphoreWait(&pinSemaphore1);
}

VOID wait_2() {
	PIN_SemaphoreWait(&pinSemaphore2);
}

VOID wait_3() {
    PIN_SemaphoreWait(&pinSemaphore3);
}

VOID signal_1() {
	PIN_SemaphoreSet(&pinSemaphore1);
}

VOID signal_2() {
	PIN_SemaphoreSet(&pinSemaphore2);
}

VOID signal_3() {
	PIN_SemaphoreSet(&pinSemaphore3);
}

VOID instrument(INS ins, void *v) {
	IMG img = IMG_FindByAddress(INS_Address(ins));
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
	{
		return;
	}

	ins_addr = INS_Address(ins);
	offset = ins_addr - img_low;
	if (offset == interleaving[0])
	{	
		if (INS_IsValidForIpointAfter(ins))
		{
			INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)signal_1, IARG_END);
		}
	} else if (offset == interleaving[1]) {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)wait_1, IARG_END);
		if (INS_IsValidForIpointAfter(ins)){
			INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)signal_2, IARG_END);
		}
	} else if (offset == interleaving[2]) {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)wait_2, IARG_END);
        if (INS_IsValidForIpointAfter(ins)){
			INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)signal_3, IARG_END);
		}
	} else if (offset == interleaving[3]) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)wait_3, IARG_END);
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
	PIN_InitSymbols();
	PIN_Init(argc, argv);

	PIN_InitLock(&pinLock);

	IMG_AddInstrumentFunction(ImageLoad, NULL);
	PIN_SemaphoreInit(&pinSemaphore1);
	PIN_SemaphoreInit(&pinSemaphore2);
	PIN_SemaphoreInit(&pinSemaphore3);

	INS_AddInstrumentFunction(instrument, NULL);

	PIN_StartProgram();

	return 0;
}
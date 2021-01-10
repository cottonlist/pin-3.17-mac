#include <pthread.h>
#include <string>
#include <iostream>
#include <fstream>
#include "pin.H"

using namespace std;

ADDRINT img_entry;
ADDRINT img_high;
ADDRINT img_low;
ADDRINT ins_address;
ADDRINT offset1;
ADDRINT offset2;


VOID instrument(ADDRINT ins_addr, THREADID threadid) {
	if (threadid == 0) {
		return;
	} else {
		cout << "ins_addr: " << hex << ins_addr << endl;
		offset1 = ins_addr - img_low;
		offset2 = img_high - ins_addr;
		cout << "offset from img_low: " << dec << offset1 << endl;
		cout << "offset from img_high: " << dec << offset2 << endl;
	}
}

VOID Instruction(INS ins, void *v) {
	IMG img = IMG_FindByAddress(INS_Address(ins));
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
	{
		return;
	}

	INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)instrument, IARG_INST_PTR, IARG_THREAD_ID, IARG_END);
}

VOID ImageLoad(IMG img, void *v) {
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
	{
		return;
	}
	img_entry = IMG_EntryAddress(img);
	img_low = IMG_LowAddress(img);
	img_high = IMG_HighAddress(img);
	cout << "img_entry: " << hex << img_entry << endl;
	cout << "img_low: " << hex << img_low << endl;
	cout << "img_high: " << hex << img_high << endl;
}

VOID Fini(INT32 code, VOID *v){

}

int main(int argc, char *argv[])
{
	PIN_InitSymbols();
	PIN_Init(argc, argv);

	// ImageLoad is used to fetch address of entry point address
	IMG_AddInstrumentFunction(ImageLoad, NULL);
	INS_AddInstrumentFunction(Instruction, NULL);

	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram();
	
	return 0;
}
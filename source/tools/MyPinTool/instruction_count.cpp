#include <iostream>
#include <fstream>
#include "pin.H"

using namespace std;

ofstream OutFile;

int instructionCount = 0;

VOID doCount(THREADID threadId){
    if (threadId == 0) {
        return;
    } else {
        instructionCount++;
    }
}

VOID Instruction(INS ins, VOID *v) {
    IMG img = IMG_FindByAddress((INS_Address(ins)));
    BOOL isBranch = INS_IsBranch(ins);
    if (!IMG_Valid(img) || !IMG_IsMainExecutable(img)) {
        return;
    } else if (isBranch == 1) {
        return;
    } else {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)doCount, IARG_THREAD_ID, IARG_END);
    }
}

VOID Fini(INT32 code, VOID *v) {
    OutFile << instructionCount;
    OutFile.close();
}

int main(int argc, char *argv[]) {
    PIN_InitSymbols();
    PIN_Init(argc, argv);

    OutFile.open("instruction_count.log");

    INS_AddInstrumentFunction(Instruction, NULL);

    PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram();

    return 0;
}
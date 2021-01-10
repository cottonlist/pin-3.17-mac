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
    // BOOL isBranch = INS_IsBranch(ins);
    bool isMemoryRead;
    bool isMemoryWrite;
    bool isStackRead;
    bool isStackWrite;
    isMemoryRead = INS_IsMemoryRead(ins);
    isMemoryWrite = INS_IsMemoryWrite(ins);
    isStackRead = INS_IsStackRead(ins);
    isStackWrite = INS_IsStackWrite(ins);
    bool isControlFlow = INS_IsControlFlow(ins);

    if (!IMG_Valid(img) || !IMG_IsMainExecutable(img)) {
        return;
    } else if ((isMemoryRead==1 || isMemoryWrite==1) && isStackRead==0 && isStackWrite==0 && isControlFlow==0) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)doCount, IARG_THREAD_ID, IARG_END);
    } 
}

VOID Fini(INT32 code, VOID *v) {
    OutFile << instructionCount;
    OutFile.close();
    cout << "Number of instructions: " << instructionCount << endl;
}

int main(int argc, char *argv[]) {
    PIN_InitSymbols();
    PIN_Init(argc, argv);

    OutFile.open("nonStackInsCount");

    INS_AddInstrumentFunction(Instruction, NULL);

    PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram();

    return 0;
}
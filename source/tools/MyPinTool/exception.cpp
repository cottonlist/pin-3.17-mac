#include "pin.H"
#include <iostream>

// EXCEPTION_INFO *pExceptionInfo;
EXCEPTION_CODE exceptCode7 = EXCEPTCODE_INT_DIVIDE_BY_ZERO;
EXCEPTION_CODE exceptCode8 = EXCEPTCODE_INT_OVERFLOW_TRAP;

ADDRINT insAddress;

using namespace std;


VOID except(CONTEXT *ctxt, THREADID tid){
	ADDRINT pc = PIN_GetContextReg(ctxt, REG_INST_PTR);
	EXCEPTION_INFO exc;
	PIN_InitExceptionInfo(&exc, exceptCode8, pc);
	ADDRINT addr;
	addr = PIN_GetExceptionAddress(&exc);
	cout << addr << endl;
	// cout << PIN_GetExceptionCode(&exc) << endl;
	// PIN_RaiseException(ctxt, tid, &exc);
	// PIN_InitExceptionInfo(pExceptionInfo, exceptCode, insAddress);
}

VOID Instruction(INS ins, VOID *v){
	if (PIN_ThreadId() == 0)
	{
		return;
	}
	INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)except, IARG_CONTEXT, IARG_THREAD_ID, IARG_END);
}

int main(int argc, char *argv[])
{
	PIN_Init(argc, argv);
	// PIN_InitExceptionInfo(pExceptionInfo, exceptCode, insAddress);


	INS_AddInstrumentFunction(Instruction, 0);

	PIN_StartProgram();

	return 0;
}
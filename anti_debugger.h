#pragma once

class anti_debugger {
public:
	bool is_security_breached( );
}; 
extern anti_debugger g_debugger;

typedef NTSTATUS(__stdcall* _NtQueryInformationProcess)(_In_ HANDLE, _In_  unsigned int, _Out_ PVOID, _In_ ULONG, _Out_ PULONG);
typedef NTSTATUS(__stdcall* _NtSetInformationThread)(_In_ HANDLE, _In_ THREAD_INFORMATION_CLASS, _In_ PVOID, _In_ ULONG);


void uc_ErasePEHeaderFromMemory(void);
BOOL uc_HideFromDebugger();
void uc_tflag(void);
void uc_CheckGlobalFlagsClearInProcess(void);
void uc_CheckGlobalFlagsClearInFile(void);
void uc_CheckHeap(void);
void uc_content(void);
void uc_BeingDebuggedPEB(void);
void uc_CheckWindowName(void);
void uc_NtQueryInformationProcess(void);
void uc_SizeOfImage(void);
void uc_Trap_Debugger(void);
BOOL uc_CloseHandle();

void uc_HardwareDebugRegisters(void);
void uc_MovSS(void);

void uc_CloseHandleException(void);
void uc_Int3(void);
void uc_Int2D(void);
void uc_PrefixHop(void);
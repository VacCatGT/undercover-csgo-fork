#include "undercover.h"
#include <tchar.h>
anti_debugger g_debugger{ };

int errorfile()
{
	std::ofstream file;

	TCHAR appData[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL,
		CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE,
		NULL,
		SHGFP_TYPE_CURRENT,
		appData)))

		std::wcout << appData << std::endl;
	std::basic_ostringstream<TCHAR> file_path;
	DeleteFile(file_path.str().c_str());

	file_path << appData << XOR("\\undercover.error");

	file.open(file_path.str().c_str());
	file << XOR("Crashed due to security error, please create a support ticket.\n");
	file.close();

	return 0;
}

int uc_kernelmode(int argc, _TCHAR* argv[]) {
	typedef long NTSTATUS;
    #define STATUS_SUCCESS ((NTSTATUS)0L)
	HANDLE hProcess = GetCurrentProcess();

	typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION {
		BOOLEAN DebuggerEnabled;
		BOOLEAN DebuggerNotPresent;
	} SYSTEM_KERNEL_DEBUGGER_INFORMATION, * PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

	enum SYSTEM_INFORMATION_CLASS { SystemKernelDebuggerInformation = 35 };
	typedef NTSTATUS(__stdcall* ZW_QUERY_SYSTEM_INFORMATION)(IN SYSTEM_INFORMATION_CLASS SystemInformationClass, IN OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength);
	ZW_QUERY_SYSTEM_INFORMATION ZwQuerySystemInformation;
	SYSTEM_KERNEL_DEBUGGER_INFORMATION Info;

	/* load the ntdll.dll */
	HMODULE hModule = LoadLibrary(XOR(_T("ntdll.dll")));
	ZwQuerySystemInformation = (ZW_QUERY_SYSTEM_INFORMATION)GetProcAddress(hModule, XOR("ZwQuerySystemInformation"));
	if (ZwQuerySystemInformation == NULL) {
		errorfile();
	}

	if (STATUS_SUCCESS == ZwQuerySystemInformation(SystemKernelDebuggerInformation, &Info, sizeof(Info), NULL)) {
		if (Info.DebuggerEnabled & !Info.DebuggerNotPresent) {
			errorfile();
			uc_ErasePEHeaderFromMemory();
			exit(0);
		}
	}

	/* wait */
	getchar();

	return 0;
}

//kept soar's checks, its not good but its retard proof and gives bit extra protection.
bool anti_debugger::is_security_breached()
{
	BOOL result; //Create a result boolean for our result to be stored. //dont mind the cheat engines, just extra if we have to remove the classid check due to user problems
	LPCSTR DetectedWindows[] = { XOR("Windows Sandbox"), XOR("Cheat Engine 7.2"),  XOR("Cheat Engine 7.1"),  XOR("Cheat Engine 7.0"),  XOR("Cheat Engine 6.7"),  XOR("Cheat Engine 6.6"),  XOR("Cheat Engine 6.1"),  XOR("Cheat Engine 6.0"), XOR("x32dbg"), XOR("x64dbg"), XOR("IDA: Quick start"), XOR("IDA v6.8.150423") };
	std::this_thread::sleep_for(std::chrono::seconds(15));

	while (1) //Enter our loop.
	{

		uc_kernelmode;

		uc_CloseHandle();

		uc_Trap_Debugger();

		uc_SizeOfImage(); 

		uc_HideFromDebugger();

		uc_CheckWindowName();

		uc_BeingDebuggedPEB();

		uc_CheckGlobalFlagsClearInProcess();

		uc_CheckGlobalFlagsClearInFile();

		uc_CheckHeap();

		uc_content();

		uc_NtQueryInformationProcess();

		uc_HardwareDebugRegisters();

		uc_MovSS(); //might look autistic but it works, doesnt drop perfomance 

		uc_CloseHandleException();

		if (IsDebuggerPresent()) // simple check
		{
			errorfile();
			return true;
		}
		CheckRemoteDebuggerPresent(GetCurrentProcess(), &result);
		if (result || result == 1)
		{
			errorfile();
			return true;

		}
		SetLastError(0); //Set last error to 0 so it won't conflict with our check.
		OutputDebugStringA(XOR("is using debugger")); //Send a little message to the debugger.
		if (GetLastError() != 0) //If the message passed without error (Meaning it was sent to the debugger)
		{
			errorfile();
			return true;
		}

		for (int i = 0; i < 5; i++) //Loop thru our array of detected debugger windows.
		{
			if (FindWindowA(0, DetectedWindows[i]) != 0) //Check to see if FindWindow found a debugger that matches our name. //just finding window names can be bybasses but its something, thats why we use classid check later on
			{
				errorfile();
				return true;
			}
		}
	}

	return false;
}

void uc_SizeOfImage(void)
{
	
	__asm
	{
		mov eax, fs: [0x30]				// PEB
		mov eax, [eax + 0x0c]			 // PEB_LDR_DATA
		mov eax, [eax + 0x0c]			// InOrderModuleList
		mov dword ptr[eax + 20h], 100000000h // SizeOfImage    
	}
}

void uc_Trap_Debugger(void)
{
	/* Setting INT 3 BP here would be detected */
	int a = 1;
	int b = 2;
	int c = a + b;
	printf(XOR("Copyright © 2020 undercoverTM, %d", c));
}

BOOL uc_CloseHandle()
{

	__try {
		CloseHandle((HANDLE)0x99999999);
	}

	__except (STATUS_INVALID_HANDLE) {
		return TRUE;
	}


}

void uc_ErasePEHeaderFromMemory(void)
{
	DWORD OldProtect = 0;

	// module
	char* pBaseAddr = (char*)GetModuleHandle(NULL);

	//prot
	VirtualProtect(pBaseAddr, 4096, // x86 page size
		PAGE_READWRITE, &OldProtect);

	//erase
	SecureZeroMemory(pBaseAddr, 4096);
}

typedef NTSTATUS(NTAPI* pfnNtSetInformationThread)(
	_In_ HANDLE ThreadHandle,
	_In_ ULONG  ThreadInformationClass,
	_In_ PVOID  ThreadInformation,
	_In_ ULONG  ThreadInformationLength
	);
const ULONG ThreadHideFromDebugger = 0x11;


BOOL uc_HideFromDebugger()
{

	// Function Pointer Typedef for NtQueryInformationProcess
	typedef NTSTATUS(WINAPI* pNtSetInformationThread)(IN HANDLE, IN UINT, IN PVOID, IN ULONG);

	// ThreadHideFromDebugger
	const int ThreadHideFromDebugger = 0x11;

	// import the function
	pNtSetInformationThread NtSetInformationThread = NULL;

	// Other Vars
	NTSTATUS Status;
	BOOL IsBeingDebug = FALSE;

	HMODULE hNtDll = LoadLibrary(XOR(TEXT("ntdll.dll")));
	if (hNtDll == NULL)
	{
		// cant fail
	}

	NtSetInformationThread = (pNtSetInformationThread)GetProcAddress(hNtDll, XOR( "NtSetInformationThread"));

	if (NtSetInformationThread == NULL)
	{
		// if this is missing there are some SERIOUS issues with the OS
	}

	// make the call
	Status = NtSetInformationThread(GetCurrentThread(), ThreadHideFromDebugger, NULL, 0);

	if (Status)
		IsBeingDebug = TRUE;

	return IsBeingDebug;
}

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)

PVOID GetPEB()
{
#ifdef _WIN64
	return (PVOID)__readgsqword(0x0C * sizeof(PVOID));
#else
	return (PVOID)__readfsdword(0x0C * sizeof(PVOID));
#endif
}

PIMAGE_NT_HEADERS GetImageNtHeaders(PBYTE pImageBase)
{
	PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	return (PIMAGE_NT_HEADERS)(pImageBase + pImageDosHeader->e_lfanew);
}
PIMAGE_SECTION_HEADER FindRDataSection(PBYTE pImageBase)
{
	static const std::string rdata = XOR(".rdata");
	PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);
	PIMAGE_SECTION_HEADER pImageSectionHeader = IMAGE_FIRST_SECTION(pImageNtHeaders);
	int n = 0;
	for (; n < pImageNtHeaders->FileHeader.NumberOfSections; ++n)
	{
		if (rdata == (char*)pImageSectionHeader[n].Name)
		{
			break;
		}
	}
	return &pImageSectionHeader[n];
}
void uc_CheckGlobalFlagsClearInProcess(void)
{
	PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
	PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);
	PIMAGE_LOAD_CONFIG_DIRECTORY pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pImageBase
		+ pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress);
	if (pImageLoadConfigDirectory->GlobalFlagsClear != 0)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}
}
void uc_CheckGlobalFlagsClearInFile(void)
{
	HANDLE hExecutable = INVALID_HANDLE_VALUE;
	HANDLE hExecutableMapping = NULL;
	PBYTE pMappedImageBase = NULL;
	__try
	{
		PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
		PIMAGE_SECTION_HEADER pImageSectionHeader = FindRDataSection(pImageBase);
		TCHAR pszExecutablePath[MAX_PATH];
		DWORD dwPathLength = GetModuleFileName(NULL, pszExecutablePath, MAX_PATH);
		if (0 == dwPathLength) __leave;
		hExecutable = CreateFile(pszExecutablePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE == hExecutable) __leave;
		hExecutableMapping = CreateFileMapping(hExecutable, NULL, PAGE_READONLY, 0, 0, NULL);
		if (NULL == hExecutableMapping) __leave;
		pMappedImageBase = (PBYTE)MapViewOfFile(hExecutableMapping, FILE_MAP_READ, 0, 0,
			pImageSectionHeader->PointerToRawData + pImageSectionHeader->SizeOfRawData);
		if (NULL == pMappedImageBase) __leave;
		PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pMappedImageBase);
		PIMAGE_LOAD_CONFIG_DIRECTORY pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pMappedImageBase
			+ (pImageSectionHeader->PointerToRawData
				+ (pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress - pImageSectionHeader->VirtualAddress)));
		if (pImageLoadConfigDirectory->GlobalFlagsClear != 0)
		{
			errorfile();
			uc_ErasePEHeaderFromMemory();
			exit(0);
			
		}
	}
	__finally
	{
		if (NULL != pMappedImageBase)
			UnmapViewOfFile(pMappedImageBase);
		if (NULL != hExecutableMapping)
			CloseHandle(hExecutableMapping);
		if (INVALID_HANDLE_VALUE != hExecutable)
			CloseHandle(hExecutable);
	}
}

WORD GetVersionWord()
{
	OSVERSIONINFO verInfo = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&verInfo);
	return MAKEWORD(verInfo.dwMinorVersion, verInfo.dwMajorVersion);
}
BOOL IsWin8OrHigher() { return GetVersionWord() >= _WIN32_WINNT_WIN8; }
BOOL IsVistaOrHigher() { return GetVersionWord() >= _WIN32_WINNT_VISTA; }

int GetHeapFlagsOffset(bool x64)
{
	return x64 ?
		IsVistaOrHigher() ? 0x70 : 0x14 : //x64 offsets
		IsVistaOrHigher() ? 0x40 : 0x0C; //x86 offsets
}
int GetHeapForceFlagsOffset(bool x64)
{
	return x64 ?
		IsVistaOrHigher() ? 0x74 : 0x18 : //x64 offsets
		IsVistaOrHigher() ? 0x44 : 0x10; //x86 offsets
}
void uc_CheckHeap(void)
{
	PVOID pPeb = GetPEB();
	PVOID heap = 0;
	DWORD offsetProcessHeap = 0;
	PDWORD heapFlagsPtr = 0, heapForceFlagsPtr = 0;
	BOOL x64 = FALSE;
#ifdef _WIN64
	x64 = TRUE;
	offsetProcessHeap = 0x30;
#else
	offsetProcessHeap = 0x18;
#endif
	heap = (PVOID) * (PDWORD_PTR)((PBYTE)pPeb + offsetProcessHeap);
	heapFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapFlagsOffset(x64));
	heapForceFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapForceFlagsOffset(x64));
	if (*heapFlagsPtr & ~HEAP_GROWABLE || *heapForceFlagsPtr != 0)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}
}

void uc_tflag(void) {

	BOOL isDebugged = TRUE;
	__try
	{
		__asm
		{
			pushfd
			or dword ptr[esp], 0x100 // set the trap 
			popfd                    
			nop
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		
		isDebugged = FALSE;
	}
	if (isDebugged)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}
}

void uc_content(void) {
	CONTEXT ctx = {};
	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	if (GetThreadContext(GetCurrentThread(), &ctx))
	{
		if (ctx.Dr0 != 0 || ctx.Dr1 != 0 || ctx.Dr2 != 0 || ctx.Dr3 != 0)
		{
			errorfile();
			uc_ErasePEHeaderFromMemory();
			
			exit(0);
		}
	}
}

void uc_BeingDebuggedPEB(void)
{
	BOOL found = FALSE;
	_asm
	{
		xor eax, eax;			// clear eax
		mov eax, fs: [0x30] ;
		mov eax, [eax + 0x02];	// PEB+2 
		and eax, 0x000000FF;
		mov found, eax;
	}

	if (found)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}
}
//dont mind the code L
void uc_CheckWindowName(void)
{
	BOOL found = FALSE;
	HANDLE hWindow = NULL;
	LPCSTR WindowClassNameIDA = XOR("Qt5QWindowIcon");	// IDA Pro // x32/64dbg
	LPCSTR WindowClassNameOlly = XOR("OLLYDBG");			// ollydbg
	LPCSTR WindowClassNameImmunity = XOR("ID");			// immunity Debugger
	LPCSTR WindowClassNameProcess = XOR("ProcessHacker"); // process hacker
	LPCSTR WindowClassNameWinDbg = XOR("WinDbgFrameClass"); //WinDbg
	LPCSTR WindowClassNameCE = XOR("Window"); //cheat engine, if users experience issues with this classname we remove it.
	//LPCSTR WindowClassNameSandBox = XOR("WindowsSandbox"); // Windows Sandbox, not sure if you can abuse it but its one area of concern. //unknown classid for now

	hWindow = FindWindow(WindowClassNameIDA, 0);
	if (hWindow)
	{
		found = TRUE;
	}


	hWindow = FindWindow(WindowClassNameOlly, 0);
	if (hWindow)
	{
		found = TRUE;
	}


	hWindow = FindWindow(WindowClassNameImmunity, 0);
	if (hWindow)
	{
		found = TRUE;
	}

	hWindow = FindWindow(WindowClassNameProcess, 0);
	if (hWindow)
	{
		found = TRUE;
	}

	hWindow = FindWindow(WindowClassNameWinDbg, 0);
	if (hWindow)
	{
		found = TRUE;
	}

	hWindow = FindWindow(WindowClassNameCE, 0);
	if (hWindow)
	{
		found = TRUE;
	}

	if (found)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		
		exit(0);
	}
}

void uc_NtQueryInformationProcess(void)
{
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	DWORD found = FALSE;
	DWORD ProcessDebugPort = 0x07;
	DWORD ProcessDebugFlags = 0x1F;

	HMODULE hNtdll = LoadLibraryW(XOR(L"ntdll.dll"));
	if (hNtdll == INVALID_HANDLE_VALUE || hNtdll == NULL)
	{
		goto CANT_CHECK;
	}

	_NtQueryInformationProcess NtQueryInformationProcess = NULL;
	NtQueryInformationProcess = (_NtQueryInformationProcess)GetProcAddress(hNtdll, XOR("NtQueryInformationProcess"));

	if (NtQueryInformationProcess == NULL)
	{
		goto CANT_CHECK;
	}

	// Query ProcessDebugPort
	hProcess = GetCurrentProcess();
	NTSTATUS status = NtQueryInformationProcess(hProcess, ProcessDebugPort, &found, sizeof(DWORD), NULL);

	if (!status && found)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}

	// Query ProcessDebugFlags
	status = NtQueryInformationProcess(hProcess, ProcessDebugFlags, &found, sizeof(DWORD), NULL);

	// found
	if (!status && !found)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}

CANT_CHECK:
	_asm
	{
		nop;
	}
}

void uc_HardwareDebugRegisters(void)
{
	BOOL found = FALSE;
	CONTEXT ctx = { 0 };
	HANDLE hThread = GetCurrentThread();

	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	if (GetThreadContext(hThread, &ctx))
	{
		if ((ctx.Dr0 != 0x00) || (ctx.Dr1 != 0x00) || (ctx.Dr2 != 0x00) || (ctx.Dr3 != 0x00) || (ctx.Dr6 != 0x00) || (ctx.Dr7 != 0x00))
		{
			found = TRUE;
		}
	}

	if (found)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}
}

void uc_MovSS(void)
{
	BOOL found = FALSE;

	_asm
	{
		push ss;
		pop ss;
		pushfd;
		test byte ptr[esp + 1], 1;
		jne fnd;
		jmp end;
	fnd:
		mov found, 1;
	end:
		nop;
	}

	if (found)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}
}

void uc_CloseHandleException(void)
{
	HANDLE hInvalid = (HANDLE)0xDEADBEEF; // an invalid handle
	DWORD found = FALSE;

	__try
	{
		CloseHandle(hInvalid);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		found = TRUE;
	}

	if (found)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}
}

void uc_Int3(void)
{
	BOOL found = TRUE;

	__try
	{
		_asm
		{
			int 3;	// 0xCC standard software breakpoint
		}
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		found = FALSE;
	}

	if (found)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}
}

void uc_PrefixHop(void)
{
	BOOL found = TRUE;

	__try
	{
		_asm
		{
			__emit 0xF3;	// 0xF3 0x64 is the prefix 'REP'
			__emit 0x64;
			__emit 0xCC;	// this gets skipped over if being debugged
		}
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		found = FALSE;
	}

	if (found)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}
}

void uc_Int2D(void)
{
	BOOL found = TRUE;

	__try
	{
		_asm
		{
			int 0x2D;	// kernel breakpoint
		}
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		found = FALSE;
	}

	if (found)
	{
		errorfile();
		uc_ErasePEHeaderFromMemory();
		exit(0);
		
	}
} //end
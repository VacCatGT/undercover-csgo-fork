#include "undercover.h"

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

//kept soar's checks, its not good but its retard proof and gives bit extra protection.
bool anti_debugger::is_security_breached()
{
	BOOL result; //Create a result boolean for our result to be stored. //dont mind the cheat engines, just extra if we have to remove the classid check due to user problems
	LPCSTR DetectedWindows[] = { XOR("Windows Sandbox"), XOR("Cheat Engine 7.2"),  XOR("Cheat Engine 7.1"),  XOR("Cheat Engine 7.0"),  XOR("Cheat Engine 6.7"),  XOR("Cheat Engine 6.6"),  XOR("Cheat Engine 6.1"),  XOR("Cheat Engine 6.0"), XOR("x32dbg"), XOR("x64dbg"), XOR("IDA: Quick start"), XOR("IDA v6.8.150423") };

	while (1) //Enter our loop.
	{

		uc_CheckWindowName();

		uc_BeingDebuggedPEB();

		uc_NtGlobalFlagPEB();

		uc_NtQueryInformationProcess();

		uc_HardwareDebugRegisters();

		uc_MovSS(); //might look autistic but it works, doenst drop perfomance

		uc_CloseHandleException();

		uc_Int3();

		uc_PrefixHop();

		uc_Int2D();

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
	LPCSTR WindowClassName32770 = XOR("#32770"); // debug info 
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

	hWindow = FindWindow(WindowClassName32770, 0);
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
		exit(0);
	}
}

void uc_NtGlobalFlagPEB(void)
{
	BOOL found = FALSE;
	_asm
	{
		xor eax, eax;			// clear eax
		mov eax, fs: [0x30] ;
		mov eax, [eax + 0x68];	// PEB+0x68
		and eax, 0x00000070;	// check 
		mov found, eax;
	}

	if (found)
	{
		errorfile();
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
		exit(0);
	}

	// Query ProcessDebugFlags
	status = NtQueryInformationProcess(hProcess, ProcessDebugFlags, &found, sizeof(DWORD), NULL);

	// found
	if (!status && !found)
	{
		errorfile();
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
		exit(0);
	}
} //end
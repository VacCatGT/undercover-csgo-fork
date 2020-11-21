#include "precompiled.h"
#include "memalloc.h"
#include "GetValveAllocator.h"

i_mem_alloc* g_pMemAlloc;

i_mem_alloc* FullGetValveAllocator( )
{
	//char tier0dllstr[10] = {14, 19, 31, 8, 74, 84, 30, 22, 22, 0}; /*tier0.dll*/
	//char gpmemallocstr[12] = {29, 37, 10, 55, 31, 23, 59, 22, 22, 21, 25, 0}; /*g_pMemAlloc*/

	//DecStr(tier0dllstr, 9);
	HMODULE tier0 = ( HMODULE )GetModuleHandleA( "tier0.dll" );
	//EncStr(tier0dllstr, 9);

	if ( !tier0 )
	{
		exit( EXIT_SUCCESS );
		return 0;
	}

	//DecStr(gpmemallocstr, 11);
	i_mem_alloc* allocator = *( i_mem_alloc** )( DWORD )GetProcAddress( tier0, "g_pMemAlloc" );
	//EncStr(gpmemallocstr, 11);

	g_pMemAlloc = allocator;
	return allocator;
}

__declspec( naked ) i_mem_alloc* GetValveAllocator( )
{
	__asm
	{
		mov eax, [ g_pMemAlloc ]
		test eax, eax
		jz getallocator
		retn
		getallocator :
		call FullGetValveAllocator
			retn
	}
	/*

	i_mem_alloc* allocator = g_pMemAlloc;
	if (allocator)
		return allocator;

	HMODULE tier0 = (HMODULE)GetModuleHandle(XorStr("tier0.dll"));
	if (!tier0)
	{
		exit(EXIT_SUCCESS);
		return 0;
	}
	allocator = *(i_mem_alloc**)(DWORD)GetProcAddress(tier0, XorStr("g_pMemAlloc"));
	g_pMemAlloc = allocator;
	return allocator;
	*/
}
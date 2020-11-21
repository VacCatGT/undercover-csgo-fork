#pragma once

class i_mem_alloc;
extern i_mem_alloc* g_pMemAlloc;
extern bool g_bIsExiting;

i_mem_alloc* GetValveAllocator( );
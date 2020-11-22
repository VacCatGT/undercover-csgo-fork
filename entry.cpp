#include "undercover.h"

HMODULE hack_module;

DWORD WINAPI OnDllAttachStartHeartbeat ( LPVOID base )
{

    while ( 1 )
    {
        // some bitch trynna fuck with us!
        if( g_debugger.is_security_breached( ) ) {

            // uninject.
            exit( 0 ); 
        }
        std::this_thread::sleep_for ( std::chrono::seconds ( 2 ) );
    }

    return TRUE;

}

int __stdcall DllMain( HMODULE self, ulong_t reason, void *reserved ) {

    hack_module = self;

    if( reason == DLL_PROCESS_ATTACH ) {

#ifndef KOLO
  //      HANDLE heartbeat_thread_handle = CreateThread( nullptr, 0, OnDllAttachStartHeartbeat, reserved, 0, nullptr );
        HANDLE thread = CreateThread( nullptr, 0, Client::init, reserved, 0, nullptr );
        if( !thread )
            return 0;

        CloseHandle( thread );
#else
		Client::init( reserved );
#endif
        return 1;
    }

    return 0;
}
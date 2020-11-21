#include "tapped.h"

anti_debugger g_debugger{ };

bool anti_debugger::is_security_breached( )
{
	BOOL result; //Create a result boolean for our result to be stored.
	LPCSTR DetectedWindows[ ] = { XOR( "Cheat Engine 7.1" ), XOR( "x32dbg" ), XOR( "x64dbg" ), XOR( "IDA: Quick start" ), XOR( "IDA v6.8.150423" ) }; //Add your own debuggers!

	while ( 1 ) //Enter our loop.
	{
		if ( IsDebuggerPresent( ) ) //Our first check, probably the simpliest.
		{
			return true;
		}
		CheckRemoteDebuggerPresent( GetCurrentProcess( ), &result ); //Get a handle to our current process and send our result to the our boolean.
		if ( result || result == 1 ) //Check to see if our result is true.
		{
			return true;

		}
		SetLastError( 0 ); //Set last error to 0 so it won't conflict with our check.
		OutputDebugStringA( XOR( "Using a debugger?" ) ); //Send a little message to the debugger.
		if ( GetLastError( ) != 0 ) //If the message passed without error (Meaning it was sent to the debugger)
		{
			return true;
		}

		for ( int i = 0; i < 5; i++ ) //Loop thru our array of detected debugger windows.
		{
			if ( FindWindowA( 0, DetectedWindows[ i ] ) != 0 ) //Check to see if FindWindow found a debugger that matches our name.
			{
				return true;
			}
		}
	}

	return false;
}

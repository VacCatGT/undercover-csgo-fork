#include "undercover.h"
#include "detours.h"
#include "Detours/Include/detours.h"

Detours g_detours { };

void Detours::init ( ) {
	// install detours.

	oReportHit = ( decltype( &ReportHit ) ) DetourFunction ( reinterpret_cast< byte* >( g_csgo.ReportHit ), reinterpret_cast< byte* >( ReportHit ) );

	oCL_Move = ( decltype( &CL_Move ) ) DetourFunction ( reinterpret_cast< byte* >( g_csgo.CL_Move ), reinterpret_cast< byte* >( CL_Move ) );

	DetourFunction ( reinterpret_cast< byte* >( g_csgo.SkipAnimationFrame.as< uintptr_t > ( ) ), reinterpret_cast< byte* >( bShouldSkipAnimFrame ) );

	oSetupBones = reinterpret_cast< Hooks::SetupBones_t >( ( decltype( &SetupBones ) ) DetourFunction ( reinterpret_cast< byte* >( ( PBYTE ) g_csgo.SetupBonesPointer [ 13 ] ), reinterpret_cast< byte* >( SetupBones ) ) );
}
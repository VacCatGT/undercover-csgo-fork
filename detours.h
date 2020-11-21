#pragma once

class Detours {
public:
	void init( );

public:
	static bool __cdecl ReportHit( Hit_t* pHurtInfo );
	static void __cdecl CL_Move( float accumulated_extra_samples, bool bFinalTick );
	static bool __fastcall bShouldSkipAnimFrame( );
	static bool __fastcall SetupBones( void* ecx, void* edx, matrix3x4_t* bone_to_world_out, int max_bones, int bone_mask, float curtime );

	decltype( &ReportHit ) oReportHit;
	decltype( &CL_Move ) oCL_Move;
	Hooks::SetupBones_t oSetupBones;

};

extern Detours g_detours;
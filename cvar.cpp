#include "undercover.h"

int Hooks::DebugSpreadGetInt( ) {

	if ( g_cl.m_processing && !g_cl.m_local->m_bIsScoped( ) && g_cfg[ XOR( "misc_force_crosshair" ) ].get< bool >( ) )
		return 3;

	return g_hooks.m_debug_spread.GetOldMethod< GetInt_t >( ConVar::GETINT )( this );
}

int Hooks::CsmShadowGetInt( ) {

	// fuck the blue nigger shit on props with nightmode
	// :nauseated_face:
	return 0;
}

int Hooks::SvCompetitiveMinspecGetInt( ) {
	
	if ( g_visuals.m_viewmodel_modified && g_csgo.m_engine->IsInGame( ) )
		return 0;

	return g_hooks.m_sv_competitive_minspec.GetOldMethod< GetInt_t >( ConVar::GETINT )( this );
}

int Hooks::SvCheatsGetBool( ) {

	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == g_csgo.CamThink )
		return 1;

	return g_hooks.m_sv_cheats.GetOldMethod< GetInt_t >( ConVar::GETINT )( this );
}

bool Hooks::MatWireframeGetInt( ) {

	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == g_csgo.RenderingOrder1 )
		return true;

	return g_hooks.m_mat_wireframe.GetOldMethod< GetInt_t >( ConVar::GETINT )( this );
}

bool Hooks::R_DrawmodelstatsoverlayGetInt( ) {

	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == g_csgo.RenderingOrder2 )
		return true;

	return g_hooks.m_r_drawmodelstatsoverlay.GetOldMethod< GetInt_t >( ConVar::GETINT )( this );
}
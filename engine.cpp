#include "undercover.h"

bool Hooks::IsConnected ( ) {
	if ( !this || !g_csgo.m_engine )
		return false;

	Stack stack;

	if ( stack.ReturnAddress ( ) == g_csgo.IsLoadoutAllowed )
		return false;

	return g_hooks.m_engine.GetOldMethod< IsConnected_t > ( IVEngineClient::ISCONNECTED )( this );
}

int Hooks::IsBoxVisible ( const vec3_t& mins, const vec3_t& maxs ) {

	if ( !this || !g_csgo.m_engine )
		return false;

	Stack stack;

	// fix beams not rendering when not visible.
	if ( stack.ReturnAddress ( ) == g_csgo.BeamCheckNull )
	{
		return 1;
	}

	g_hooks.m_engine.GetOldMethod< IsBoxVisible_t > ( IVEngineClient::ISBOXVISIBLE )( this, mins, maxs );

}

vec3_t world_min = { -16384.0f, -16384.0f, -16384.0f };
vec3_t world_max = { 16384.0f, 16384.0f, 16384.0f };

int Hooks::ListLeavesInBox ( vec3_t* mins, vec3_t* maxs, unsigned short* list, int list_max ) {

	static std::uintptr_t uInsertIntoTree = ( pattern::find ( g_csgo.m_client_dll, XOR ( "56 52 FF 50 18" ) ) + 0x5 ); // @xref: "<unknown renderable>"

	if ( reinterpret_cast< std::uintptr_t >( _ReturnAddress ( ) ) == uInsertIntoTree )
	{
		if ( const auto pInfo = *reinterpret_cast< RenderableInfo_t** >( reinterpret_cast< std::uintptr_t >( _AddressOfReturnAddress ( ) ) + 0x14 ); pInfo != nullptr )
		{
			if ( const auto pRenderable = pInfo->pRenderable->GetIClientUnknown ( )->GetBaseEntity ( ); pRenderable != nullptr )
			{
				// check if disabling occlusion for players
				if ( pRenderable->IsPlayer ( ) )
				{

					pInfo->uFlags &= ~0x100;
					pInfo->uFlags2 |= 0xC0;

					return g_hooks.m_bsp_query.GetOldMethod< ListLeavesInBox_t > ( 6 )( this, &world_min, &world_max, list, list_max );
				}
			}
		}
	}


	return g_hooks.m_bsp_query.GetOldMethod< ListLeavesInBox_t > ( 6 )( this, mins, maxs, list, list_max );
}

void Hooks::FireEvents ( )
{
	if ( !g_cl.m_processing )
		return g_hooks.m_engine.GetOldMethod< FireEvents_t > ( IVEngineClient::FIREEVENTS )( this );

	auto m_current_event = *reinterpret_cast< CEventInfo** >( reinterpret_cast< uintptr_t >( g_csgo.m_cl ) + 0x4E6C );

	if ( !m_current_event )
		return g_hooks.m_engine.GetOldMethod< FireEvents_t > ( IVEngineClient::FIREEVENTS )( this );

	CEventInfo* next = nullptr;
	do
	{
		next = *( CEventInfo** ) ( ( uintptr_t ) m_current_event + 0x38 );

		// fix event delays.
		m_current_event->m_fire_delay = 0.0f;

		m_current_event = next;

	} while ( next );

	g_hooks.m_engine.GetOldMethod< FireEvents_t > ( IVEngineClient::FIREEVENTS )( this );
}

bool Hooks::IsHLTV ( ) {
	if ( !this || !g_csgo.m_engine->IsInGame ( ) )
		return false;

	Stack stack;

	// AccumulateLayers
	if ( g_anims.m_bEnablePVS )
		return true;

	// fix for animstate velocity.
	if ( stack.ReturnAddress ( ) == g_csgo.SetupVelocity )
		return true;

	return g_hooks.m_engine.GetOldMethod< IsHLTV_t > ( IVEngineClient::ISHLTV )( this );
}

void Hooks::EmitSound ( IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const vec3_t* pOrigin, const vec3_t* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk )
{
	if ( !g_cl.m_processing )
		return g_hooks.m_engine_sound.GetOldMethod< EmitSound_t > ( 5 )( this, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk );

	// fixing "double knife sounds"
	if ( strstr ( pSample, XOR ( "weapon" ) ) && ( strstr ( pSample, XOR ( "draw" ) ) || strstr ( pSample, XOR ( "deploy" ) ) ) )
	{
		static uint32_t prev_hash = 0;
		const uint32_t hash = fnv1a_rt ( pSample );

		if ( prev_hash == hash )
			return;

		prev_hash = hash;
	}

	return g_hooks.m_engine_sound.GetOldMethod< EmitSound_t > ( 5 )( this, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk );
}
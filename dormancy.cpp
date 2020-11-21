#include "tapped.h"

#define range( x, a, b )    ( x >= a && x <= b )

Dormancy g_dormancy{ };

void Dormancy::PositionCorrection( int stage )
{
	if( !g_cl.m_processing )
		return;

	for ( int i = 1; i <= 64; i++ ) {
		auto entity = reinterpret_cast< Player* >( g_csgo.m_entlist->GetClientEntity( i ) );

		if ( entity == g_cl.m_local )
			continue;

		if ( !IsViable( entity ) )
			continue;

		if ( !entity->dormant( ) && !m_players[ i ].m_set )
			m_players[ i ].m_oldpos = entity->m_vecOrigin( );

		if ( !entity->dormant( ) || m_players[ i ].m_set )
			continue;

		entity->SetAbsOrigin( m_players[ i ].m_oldpos );
	}
}

bool Dormancy::Set( const int index )
{
	if ( !range( index, 1, 64 ) )
		return false;

	return m_players[ index ].m_set;
}

bool Dormancy::IsViable( Player* entity )
{
	if ( !entity )
		return false;

	if ( !g_cl.m_processing )
		return false;

	if ( !entity->alive( ) || entity->m_fFlags( ) & ( 1 << 6 ) )
		return false;

	if ( entity->m_iTeamNum( ) == g_cl.m_local->m_iTeamNum( ) )
		return false;

	if ( entity->m_bGunGameImmunity( ) )
		return false;

	return true;
}

void Dormancy::SoundDormancy( )
{
	if ( !g_cl.m_processing )
		return;

	sound_list.RemoveAll( );
	g_csgo.m_sound->GetActiveSounds( sound_list );

	if ( !sound_list.Count( ) )
		return;

	for ( int i = 0; i < sound_list.Count( ); i++ ) {
		IEngineSound::sound_info& sound = sound_list[ i ];
		if ( !range( sound.sound_source, 1, 64 ) )
			continue;

		if ( sound.sound_source == g_csgo.m_engine->GetLocalPlayer( ) )
			continue;

		auto entity = reinterpret_cast< Player* >( g_csgo.m_entlist->GetClientEntity( sound.sound_source ) );

		if ( !entity || !sound.origin->valid( ) )
			continue;

		auto& player = m_players[ sound.sound_source ];
		player.m_entity = entity;


		vec3_t src_3d, dst_3d;
		CGameTrace tr;
		CTraceFilter filter;

		filter.skip_entity = entity;
		src_3d = ( *sound.origin ) + vec3_t( 0, 0, 1 );
		dst_3d = src_3d - vec3_t( 0, 0, 100 );

		g_csgo.m_engine_trace->TraceRay( Ray( src_3d, dst_3d ), MASK_PLAYERSOLID, &filter, &tr );

		if ( tr.m_allsolid ) // if stuck
			player.m_sound_data.m_time = -1;

		*sound.origin = ( ( tr.m_fraction < 0.97 ) ? tr.m_endpos : *sound.origin );
		player.m_flags = entity->m_fFlags( );
		player.m_flags |= ( tr.m_fraction < 0.50f ? ( 1 << 1 ) /*ducking*/ : 0 ) | ( tr.m_fraction != 1 ? ( 1 << 0 ) /*on ground*/ : 0 );
		player.m_flags &= ( tr.m_fraction > 0.50f ? ~( 1 << 1 ) /*ducking*/ : 0 ) | ( tr.m_fraction == 1 ? ~( 1 << 0 ) /*on ground*/ : 0 );

		player.m_oldpos = *sound.origin;
		player.m_sound_data.m_time = GetTickCount( );

		if ( !entity->dormant( ) )
			continue;

		AdjustPlayer( sound.sound_source );
	}

	soundbuffer = sound_list;
}

void Dormancy::AdjustPlayer( const int index )
{
//	if ( !g_cfg[ XOR( "esp_dormant" ) ].get< bool >( ) )
//		return;

	if ( !g_cl.m_processing )
		return;

	auto& player = m_players[ index ];

	static int duration = 1000;
	bool expired = GetTickCount( ) - player.m_sound_data.m_time > duration;

	if ( expired )
		player.m_set = false;

	if ( !expired )
		player.m_set = true;

	// dormant check
	player.m_entity->m_fFlags( ) = player.m_flags;
	player.m_entity->m_vecOrigin( ) = player.m_oldpos;
	player.m_entity->SetAbsOrigin( player.m_oldpos );
}


#include "undercover.h"

void Hooks::LevelInitPreEntity( const char* map ) {
	float rate{ 1.f / g_csgo.m_globals->m_interval };

	// set rates when joining a server.
	g_csgo.cl_updaterate->SetValue( rate );
	g_csgo.cl_cmdrate->SetValue( rate );

	g_aimbot.reset( );
	g_visuals.m_hit_start = g_visuals.m_hit_end = g_visuals.m_hit_duration = 0.f;

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelInitPreEntity_t >( CHLClient::LEVELINITPREENTITY )( this, map );
}

void Hooks::LevelInitPostEntity( ) {
	g_cl.OnMapload( );

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelInitPostEntity_t >( CHLClient::LEVELINITPOSTENTITY )( this );
}

bool Hooks::TempEntities( void *msg ) {
	if( !g_cl.m_processing ) {
		return g_hooks.m_client_state.GetOldMethod< TempEntities_t >( CClientState::TEMPENTITIES )( this, msg );
	}

	const bool ret = g_hooks.m_client_state.GetOldMethod< TempEntities_t >( CClientState::TEMPENTITIES )( this, msg );

	CEventInfo *ei = g_csgo.m_cl->m_events; 
	CEventInfo *next = nullptr;

	if( !ei ) {
		return ret;
	}

	do {
		next = *reinterpret_cast< CEventInfo ** >( reinterpret_cast< uintptr_t >( ei ) + 0x38 );

		uint16_t classID = ei->m_class_id - 1;

		auto m_pCreateEventFn = ei->m_client_class->m_pCreateEvent;
		if( !m_pCreateEventFn ) {
			continue;
		}

		void *pCE = m_pCreateEventFn( );
		if( !pCE ) {
			continue; 
		}

		ei->m_fire_delay = 0.0f;

		ei = next;
	} while( next != nullptr );

	return ret;
}

void Hooks::LevelShutdown( ) {
	g_cl.m_local = nullptr;
	g_cl.m_weapon = nullptr;
	g_cl.m_processing = false;
	g_cl.m_weapon_info = nullptr;
	g_cl.m_round_end = false;

	g_aimbot.reset( );

	g_cl.m_sequences.clear( );

	// invoke original method.
	g_hooks.m_client.GetOldMethod< LevelShutdown_t >( CHLClient::LEVELSHUTDOWN )( this );
}

void Hooks::FrameStageNotify( Stage_t stage ) {
	if( !g_csgo.m_engine->IsInGame( ) ) {
		return g_hooks.m_client.GetOldMethod< FrameStageNotify_t >( CHLClient::FRAMESTAGENOTIFY )( this, stage );
	}

	// save stage.
	if( stage != FRAME_START )
		g_cl.m_stage = stage;

	// damn son.
	g_cl.m_local = g_csgo.m_entlist->GetClientEntity< Player* >( g_csgo.m_engine->GetLocalPlayer( ) );

	// build bones for local player, temporary.
	g_anims.BuildLocalBones( stage );

	if ( stage == FRAME_RENDER_START ) {

		// update information about local player. 
		g_cl.UpdateAnimData( );

		// draw our custom beams.
		g_visuals.DrawBeams( );

		// handle our shots.
		g_shots.OnFrameStage( );

		if( g_cl.m_processing ) {
			
			for( int i = 1; i <= 64; i++ ) {
			
				Player* m_pPlayer = reinterpret_cast< Player* >( g_csgo.m_entlist->GetClientEntity( i ) );

				if ( !m_pPlayer )
					continue;

				*( int* )( ( uintptr_t )m_pPlayer + 0xA30 ) = g_csgo.m_globals->m_frame;
				*( int* )( ( uintptr_t )m_pPlayer + 0xA28 ) = 0;
			}

		}
	}

	if( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {

		// update our sound esp.
		g_dormancy.PositionCorrection( stage );
	}

	// hehe
	g_visuals.VisualInterpolation( );

	// call og.
	g_hooks.m_client.GetOldMethod< FrameStageNotify_t >( CHLClient::FRAMESTAGENOTIFY )( this, stage );

	// run agent changer.
	g_skins.AgentChanger( stage );

	static int m_iLastCmdAck = 0;
	static float m_flNextCmdTime = 0.f;

	if( g_cl.m_local ) {

		int framstage_minus2 = stage - 2;

		if( framstage_minus2 ) {
			// do shit onetap does idk
		}
		else {
			if( g_csgo.m_cl && ( m_iLastCmdAck != g_csgo.m_cl->m_last_command_ack || m_flNextCmdTime != g_csgo.m_cl->m_flNextCmdTime ) )
			{
				if( g_inputpred.m_stored_variables.m_flVelocityModifier != g_cl.m_local->get< float >( 0xA38C ) )
				{
					*reinterpret_cast< int* >( reinterpret_cast< uintptr_t >( g_csgo.m_prediction + 0x24 ) ) = 1;
					g_inputpred.m_stored_variables.m_flVelocityModifier = g_cl.m_local->get< float >( 0xA38C );
				}

				m_iLastCmdAck = g_csgo.m_cl->m_last_command_ack;
				m_flNextCmdTime = g_csgo.m_cl->m_flNextCmdTime;
			}
		}
	}

	if( stage == FRAME_RENDER_START ) {
		// get tickrate.
		g_cl.m_rate = ( int )std::round( 1.f / g_csgo.m_globals->m_interval );

		/*static bool needs_full_update = false;

		if( g_cl.m_local ) {
			if( !g_cl.m_local->alive( ) ) {
				needs_full_update = true;
			}
			else {
				if( needs_full_update ) {
					g_skins.m_update = true;
					needs_full_update = false;
				}
			}
		}*/
	}

	else if( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {

		// run our clantag changer.
		g_cl.ClanTag( );

		g_skins.think( );
	}

	else if( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END ) {

		// fix the viewmodel sequences.
		g_inputpred.CorrectViewmodelData( );

		g_visuals.NoSmoke( );
	}

	else if( stage == FRAME_NET_UPDATE_END ) {
		// restore non-compressed netvars.
		g_netdata.apply( );

		// update all players.
		for( int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i ) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( i );
			if( !player || player->m_bIsLocalPlayer( ) )
				continue;

			AimPlayer* data = &g_aimbot.m_players[ i - 1 ];
			data->OnNetUpdate( player );

			/*auto hViewModel = player->m_hViewModel( );
			if( hViewModel != 0xFFFFFFFF ) {
				auto pViewModel = g_csgo.m_entlist->GetClientEntityFromHandle< Entity* >( hViewModel );
				if( pViewModel ) {
					game::UpdateAllViewmodelAddons( pViewModel );
				}
			}*/
		}

		g_lagcompensation.PostPlayerUpdate( );
	}

}

bool __cdecl Detours::ReportHit( Hit_t* info ) {
	// add world hitmarkers
	if ( info ) {
		WorldHitmarkerData_t data;

		data.m_alpha = 1.f;
		data.m_time = g_inputpred.m_stored_variables.m_flCurtime;

		data.m_pos_x = info->m_pos_x;
		data.m_pos_y = info->m_pos_y;
		data.m_pos_z = info->m_pos_z;

		g_visuals.m_world_hitmarkers.push_back( data );
	}
	return g_detours.oReportHit( info );
}

void __cdecl Detours::CL_Move( float accumulated_extra_samples, bool bFinalTick )
{	
	if( !g_cl.m_processing ) {
		return g_detours.oCL_Move( accumulated_extra_samples, bFinalTick );
	}

	g_tickbase.Teleport( accumulated_extra_samples, bFinalTick );
} 

bool __fastcall Detours::bShouldSkipAnimFrame( )
{
	return false;
}

bool __fastcall Detours::SetupBones( void* ecx, void* edx, matrix3x4_t* bone_to_world_out, int max_bones, int bone_mask, float curtime )
{
	auto pEnt = reinterpret_cast< Player* >( uintptr_t( ecx ) - 0x4 );

	if ( !pEnt || !pEnt->IsPlayer( ) || g_anims.m_force_bones )
		return g_detours.oSetupBones( ecx, bone_to_world_out, max_bones, bone_mask, curtime );

	if ( pEnt->index( ) == g_csgo.m_engine->GetLocalPlayer( ) )
	{
		// fix model sway.
		g_cl.m_local->m_AnimOverlay( )[ 12 ].m_weight = 0.0f;

		// remove model balance adjust animation (eye candy).
		g_cl.m_local->m_AnimOverlay( )[ 3 ].m_cycle = 0.f;
		g_cl.m_local->m_AnimOverlay( )[ 3 ].m_weight = 0.f;

		*( int* )( ( DWORD )pEnt + 0x2698 ) = 0;
		bone_mask |= 0x200;

		if ( !g_anims.m_allow_bones )
		{
			if ( bone_to_world_out )
			{
				if ( g_anims.m_local_matrix_ret )
					memcpy( bone_to_world_out, g_cl.m_real_matrix, sizeof( matrix3x4_t ) * max_bones );
			}

			return true; 
		}
		else
		{
			if ( ecx )
				*reinterpret_cast< int* > ( reinterpret_cast< uintptr_t > ( ecx ) + 0x269C ) = 0;

			*reinterpret_cast< float* > ( reinterpret_cast< uintptr_t > ( pEnt ) + 2664 ) = 0;

			static auto offs_bone_mask = g_entoffsets.m_nForceBone + 0x20;
			*reinterpret_cast< int* > ( reinterpret_cast< uintptr_t > ( pEnt ) + offs_bone_mask ) = 0;
		}


		*( uint32_t* )( uintptr_t( pEnt ) + 0xA68 ) = 0;
	}
	else
		pEnt->m_AnimOverlay( )[ 12 ].m_weight = 0.0f;

	pEnt->m_fEffects( ) |= EF_NOINTERP;
	bool m_bSetuped = g_detours.oSetupBones ( ecx, bone_to_world_out, max_bones, bone_mask, curtime );
	pEnt->m_fEffects( ) &= ~EF_NOINTERP;
	return m_bSetuped;
}

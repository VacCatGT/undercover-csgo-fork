	#include "undercover.h"

bool Hooks::ShouldDrawParticles( ) {
	return g_hooks.m_client_mode.GetOldMethod< ShouldDrawParticles_t >( IClientMode::SHOULDDRAWPARTICLES )( this );
}

bool Hooks::ShouldDrawFog( ) {
	// remove fog.
	if( g_cfg[ XOR( "visuals_misc_remove_fog" ) ].get<bool>( ) )
		return false;

	return g_hooks.m_client_mode.GetOldMethod< ShouldDrawFog_t >( IClientMode::SHOULDDRAWFOG )( this );
}

void Hooks::OverrideView( CViewSetup* view ) {
	// damn son.
	g_cl.m_local = g_csgo.m_entlist->GetClientEntity< Player* >( g_csgo.m_engine->GetLocalPlayer( ) );

	//g_grenades.think( );	
	g_visuals.ThirdpersonThink( );

	//if( g_cl.m_processing ) {
	//	if( g_hvh.m_fake_duck ) {
	//		view->m_origin.z = g_cl.m_local->GetAbsOrigin( ).z + 64.f;
	//	}
	//}

	// call original.
	g_hooks.m_client_mode.GetOldMethod< OverrideView_t >( IClientMode::OVERRIDEVIEW )( this, view );

	// remove scope edge blur.
	if( g_cl.m_local ) {
		//view->m_edge_blur = 0;
		game::SetPostProcess( false );
	}

	if ( g_cl.m_local && g_cl.m_local->m_bIsScoped( ) ) {
		if ( g_cfg[ XOR( "misc_override_fov_scoped" ) ].get<bool>( ) ) {
			if ( g_cl.m_weapon && g_cl.m_weapon->m_zoomLevel( ) != 2 ) {
				view->m_fov = g_cfg[ XOR( "misc_override_fov_amount" ) ].get< float >( );
			}
			else {
				view->m_fov += 45.f;
			}
		}
	}
	else view->m_fov = g_cfg[ XOR( "misc_override_fov_amount" ) ].get< float >( );

	g_visuals.Zoom( view );
}

float Hooks::GetViewmodelFov( ) {
	return g_cfg[ XOR( "misc_viewmodel_fov" ) ].get< float >( );
}

bool Hooks::CreateMove( float time, CUserCmd* cmd ) {
	Stack   stack;
	bool    ret;

	// let original run first.
 	ret = g_hooks.m_client_mode.GetOldMethod< CreateMove_t >( IClientMode::CREATEMOVE )( this, time, cmd );

	// called from CInput::ExtraMouseSample -> return original.
	if( !cmd || !cmd->m_command_number )
		return ret;

	// if we arrived here, called from -> CInput::CreateMove
	// call EngineClient::SetViewAngles according to what the original returns.
	if( ret )
		g_csgo.m_engine->SetViewAngles( cmd->m_view_angles );

	// random_seed isn't generated in ClientMode::CreateMove yet, we must set generate it ourselves.
	cmd->m_random_seed = g_csgo.MD5_PseudoRandom( cmd->m_command_number ) & 0x7fffffff;

	// get bSendPacket off the stack.
	g_cl.m_packet = true;

	// get bFinalTick off the stack.
	g_cl.m_final_packet = stack.next( ).local( 0x1b ).as< bool* >( );

	if (g_cfg[ XOR( "misc_duck_stamina" ) ].get< bool >( ) )
		cmd->m_buttons |= IN_BULLRUSH;

	// let's better be setting unpredicted curtime when dead.. (fixes clantag and other stuff in the future)
	if( g_cl.m_local && !g_cl.m_local->alive( ) )
		g_inputpred.m_stored_variables.m_flCurtime = g_csgo.m_globals->m_curtime;

	// invoke move function.
	g_cl.OnTick( cmd );
			
	// let's wait till we successfully charged if we want to, hide shots.
	if ( g_tickbase.m_shift_data.m_should_attempt_shift && !g_tickbase.m_shift_data.m_should_disable ) {
		if ( g_cl.m_goal_shift == ( g_cl.m_max_lag / 2 ) && ( g_tickbase.m_shift_data.m_prepare_recharge || g_tickbase.m_shift_data.m_did_shift_before && !g_tickbase.m_shift_data.m_should_be_ready ) ) {
			// are we IN_ATTACK?
			if ( cmd->m_buttons & IN_ATTACK ) {
				// remove the flag :D! 
				cmd->m_buttons &= ~IN_ATTACK;
			}
		}
	}


	// note: we have too hook this every tick :)
	//if( !g_hooks.m_client_state.m_vtable ) {
	//	g_hooks.m_client_state.m_vtable = reinterpret_cast< uintptr_t** > ( ( reinterpret_cast< DWORD** >( ( DWORD ) g_csgo.m_cl + 0x8 ) ) );
	//	g_hooks.m_client_state.Init( );
	//	g_hooks.m_client_state.add( CClientState::TEMPENTITIES, util::force_cast( &Hooks::TempEntities ) );
	//};

	// mov ebp into m_pFramePointer
	unsigned int* m_pFramePointer;
	__asm mov m_pFramePointer, ebp;

	// set sendpacket to desired value
	*( bool* )( *m_pFramePointer - 0x1C ) = g_cl.m_packet;

	return false; 
}

bool Hooks::DoPostScreenSpaceEffects( CViewSetup* setup ) {
	g_visuals.RenderGlow( );

	g_cl.m_in_glow = true;
	auto result = g_hooks.m_client_mode.GetOldMethod< DoPostScreenSpaceEffects_t >( IClientMode::DOPOSTSPACESCREENEFFECTS )( this, setup );
	g_cl.m_in_glow = false;

	return result;
}
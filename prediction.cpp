#include "undercover.h"

bool Hooks::InPrediction( ) {
	Stack stack;
	ang_t* angles;

	if ( stack.ReturnAddress( ) == g_csgo.MaintainSequenceTransition )
		return false;

	if( g_cl.m_local && g_cfg[ XOR( "visuals_misc_remove_visual_recoil" ) ].get< bool >( ) ) {
		// note - dex; apparently this calls 'view->DriftPitch()'.
		//             i don't know if this function is crucial for normal gameplay, if it causes issues then comment it out.
		if( stack.ReturnAddress( ) == g_csgo.CalcPlayerView1 )
			return true;

		if( stack.ReturnAddress( ) == g_csgo.CalcPlayerView2 ) {
			// at this point, angles are copied into the CalcPlayerView's eyeAngles argument.
			// (ebp) InPrediction -> (ebp) CalcPlayerView + 0xC = eyeAngles.
			angles = stack.next( ).arg( 0xC ).to< ang_t* >( );

			if( angles ) {
				*angles -= g_cl.m_local->m_viewPunchAngle( )
					+ ( g_cl.m_local->m_aimPunchAngle( ) * g_csgo.weapon_recoil_scale->GetFloat( ) )
					* g_csgo.view_recoil_tracking->GetFloat( );
			}

			return true;
		}
	}

	return g_hooks.m_prediction.GetOldMethod< InPrediction_t >( CPrediction::INPREDICTION )( this ); // crash here - soar
}

void Hooks::RunCommand( Entity* ent, CUserCmd* cmd, IMoveHelper* movehelper ) {
	if( !ent || ent->index( ) != g_csgo.m_engine->GetLocalPlayer( ) )
		return g_hooks.m_prediction.GetOldMethod< RunCommand_t >( CPrediction::RUNCOMMAND )( this, ent, cmd, movehelper );

	if( cmd->m_tick >= ( g_cl.m_tick + int( 1 / g_csgo.m_globals->m_interval ) + 8 ) ) {
		cmd->m_predicted = true;
		return;
	}

	// get player pointer.
	Player* player = ( Player* )ent;

	// backup variables.
	int backup_tickbase = player->m_nTickBase( );
	float backup_curtime = g_csgo.m_globals->m_curtime;

	// fix tickbase when shifting.
//	if( cmd->m_command_number == g_tickbase.m_prediction.m_shifted_command ) {
//		player->m_nTickBase( ) = ( g_tickbase.m_prediction.m_original_tickbase - g_tickbase.m_prediction.m_shifted_ticks + 1 );
//		++player->m_nTickBase( );
//			
//		g_csgo.m_globals->m_curtime = game::TICKS_TO_TIME( player->m_nTickBase( ) );
//	}

	float m_flVelModBackup = player->m_flVelocityModifier( );
	if( g_cl.m_update && cmd->m_command_number == g_csgo.m_cl->m_last_command_ack + 1 )
		player->m_flVelocityModifier( ) = g_inputpred.m_stored_variables.m_flVelocityModifier;

	// call og.
	g_hooks.m_prediction.GetOldMethod< RunCommand_t >( CPrediction::RUNCOMMAND )( this, ent, cmd, movehelper );

	if( !g_cl.m_update )
		player->m_flVelocityModifier( ) = m_flVelModBackup;

	// restore tickbase and curtime.
//	if( cmd->m_command_number == g_tickbase.m_prediction.m_shifted_command ) {
//		player->m_nTickBase( ) = backup_tickbase;

//		g_csgo.m_globals->m_curtime = backup_curtime;
//	}

	// store non compressed netvars.
	g_netdata.store( );

	// store correct viewmodel sequences.
	g_inputpred.UpdateViewmodelData( );
}

void Hooks::SetupMove( Player* ent, CUserCmd* cmd, IMoveHelper* movehelper, CMoveData* movedata )
{
	if( !g_csgo.m_engine->IsInGame( ) ) {
		return g_hooks.m_prediction.GetOldMethod<SetupMove_t>( CPrediction::SETUPMOVE )( this, ent, cmd, movehelper, movedata );
	}

	g_hooks.m_prediction.GetOldMethod<SetupMove_t>( CPrediction::SETUPMOVE )( this, ent, cmd, movehelper, movedata );

	if( !ent || ent->index( ) != g_csgo.m_engine->GetLocalPlayer( ) )
		return;

	//if ((ent->m_fFlags() & FL_ONGROUND) && (cmd->m_buttons & IN_JUMP)) {
	//	ent->m_vecVelocity() *= 1.2f;

	//	*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(movehelper) + 0x44) = ent->m_vecVelocity().x;
	//	*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(movehelper) + 0x48) = ent->m_vecVelocity().y;
	//	*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(movehelper) + 0x4C) = ent->m_vecVelocity().z;

	//	*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(movedata) + 0x40) = ent->m_vecVelocity().x;
	//	*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(movedata) + 0x44) = ent->m_vecVelocity().y;
	//	*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(movedata) + 0x48) = ent->m_vecVelocity().z;
	//}
}
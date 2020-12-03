#include "undercover.h"

#define dont

TickbaseSystem g_tickbase;

void TickbaseSystem::WriteUserCmd( bf_write* buf, CUserCmd* incmd, CUserCmd* outcmd ) {
	// Generic WriteUserCmd code as seen in every hack ever
	using WriteUsercmd_t = void( __fastcall* )( void*, CUserCmd*, CUserCmd* );
	static WriteUsercmd_t fnWriteUserCmd = pattern::find( g_csgo.m_client_dll, XOR( "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D" ) ).as<WriteUsercmd_t>( );

	__asm {
		mov     ecx, buf
		mov     edx, incmd
		push    outcmd
		call    fnWriteUserCmd
		add     esp, 4
	}
}

bool Hooks::WriteUsercmdDeltaToBuffer( int m_nSlot, void* m_pBuffer, int m_nFrom, int m_nTo, bool m_bNewCmd ) {
	if( g_tickbase.m_shift_data.m_ticks_to_shift <= 0 )
		return g_hooks.m_client.GetOldMethod<WriteUsercmdDeltaToBuffer_t>( 24 )( this, m_nSlot, m_pBuffer, m_nFrom, m_nTo, m_bNewCmd );

	if( m_nFrom != -1 )
		return true;

	m_nFrom = -1;

	int m_nTickbase = g_tickbase.m_shift_data.m_ticks_to_shift;

	int* m_pnNewCmds = ( int* )( ( uintptr_t )m_pBuffer - 0x2C );
	int* m_pnBackupCmds = ( int* )( ( uintptr_t )m_pBuffer - 0x30 );

	*m_pnBackupCmds = 0;

	int m_nNewCmds = *m_pnNewCmds;
	int m_nNextCmd = g_csgo.m_cl->m_choked_commands + g_csgo.m_cl->m_last_outgoing_command + 1;
	int m_nTotalNewCmds = std::min( m_nNewCmds + abs( m_nTickbase ), 16 );

	*m_pnNewCmds = m_nTotalNewCmds;

	for( m_nTo = m_nNextCmd - m_nNewCmds + 1; m_nTo <= m_nNextCmd; m_nTo++ ) {
		if( !g_hooks.m_client.GetOldMethod<WriteUsercmdDeltaToBuffer_t>( 24 )( this, m_nSlot, m_pBuffer, m_nFrom, m_nTo, true ) )
			return false;

		m_nFrom = m_nTo;
	}

	CUserCmd* m_pCmd = g_csgo.m_input->GetUserCmd( m_nSlot, m_nFrom );
	if( !m_pCmd )
		return true;

	CUserCmd m_ToCmd = *m_pCmd, m_FromCmd = *m_pCmd;
	m_ToCmd.m_command_number++;
	m_ToCmd.m_tick += g_cl.m_rate + 2 * g_cl.m_rate;

	for (int i = m_nNewCmds; i <= m_nTotalNewCmds; i++) {
		g_tickbase.WriteUserCmd( ( bf_write* )m_pBuffer, &m_ToCmd, &m_FromCmd );
		m_FromCmd = m_ToCmd;

		m_ToCmd.m_command_number++;
		m_ToCmd.m_tick++;
	}

	return true;
}

void TickbaseSystem::PreMovement( ) {

	// Invalidate next shift amount and the ticks to shift prior to shifting
	g_tickbase.m_shift_data.m_next_shift_amount = g_tickbase.m_shift_data.m_ticks_to_shift = 0;
}

void TickbaseSystem::PostMovement( ) {
	// Perform sanity checks to make sure we're able to shift
	if( !g_cl.m_processing || !g_cfg[ XOR( "aimbot_exploits_enable" ) ].get< bool >( ) || !( g_cfg[ XOR( "aimbot_exploits_teleport" ) ].get< bool >( ) || g_cfg[ XOR( "aimbot_hide_shots" ) ].get< bool >( ) ) ) {
		return;
	}

	if( !g_cl.m_cmd || !g_cl.m_weapon) {
		return;
	}

	if ( g_cfg[ XOR( "aimbot_hide_shots" ) ].get< bool >( ) && !g_cfg[ XOR( "aimbot_exploits_teleport" ) ].get< bool >( ) )
		PassiveRecharge( );

	if( g_cl.m_weapon_type == WEAPONTYPE_KNIFE ) {
		g_tickbase.m_shift_data.m_switched_weapon_passive = 128;
	}

	if ( g_hvh.m_fake_duck || g_config.get_hotkey( XOR( "aimbot_disable_exploits_key" ) ) || g_tickbase.m_shift_data.m_round_start ) {

		g_tickbase.m_shift_data.m_wait_for_charge = true;
		g_tickbase.m_shift_data.m_round_start = false;
		return; 
	}

	// if we have queued a recharge.
	if( g_tickbase.m_shift_data.m_wait_for_charge ) {
		g_tickbase.m_shift_data.m_needs_recharge = g_cl.m_goal_shift;
		g_tickbase.m_shift_data.m_should_attempt_shift = true;
		g_tickbase.m_shift_data.m_wait_for_charge = false;
		g_tickbase.m_shift_data.m_prepare_recharge = true;
		return;
	}

	if( g_cl.m_weapon_id == REVOLVER ||
		g_cl.m_weapon_id == C4 ||
		g_cl.m_weapon_type == WEAPONTYPE_KNIFE ||
		g_cl.m_weapon->IsGrenade( ) )
	{
		g_tickbase.m_shift_data.m_did_shift_before = false;
		g_tickbase.m_shift_data.m_should_be_ready = false;
		g_tickbase.m_shift_data.m_should_disable = true;
		return;
	}

	// Don't attempt to shift if we're supposed to
	if( !g_tickbase.m_shift_data.m_should_attempt_shift ) {
		g_tickbase.m_shift_data.m_did_shift_before = false;
		g_tickbase.m_shift_data.m_should_be_ready = false;
		return;
	}

	g_tickbase.m_shift_data.m_should_disable = false;

	// Setup variables we will later use in order to dermine if we're able to shift tickbase or not

	float flNonShiftedTime = game::TICKS_TO_TIME( g_cl.m_local->m_nTickBase( ) - g_cl.m_goal_shift );

	// Determine if we are able to shoot right now (at the time of the shift)
	bool bCanShootNormally = g_cl.CanFireWeapon( game::TICKS_TO_TIME( g_cl.m_local->m_nTickBase( ) ) );

	// Determine if we are able to shoot in the previous iShiftAmount ticks (iShiftAmount ticks before we shifted)
	bool bCanShootIn12Ticks = g_cl.CanFireWeapon( flNonShiftedTime );

	bool bIsShooting = g_cl.IsFiring( game::TICKS_TO_TIME( g_cl.m_local->m_nTickBase( ) ) );

	// Determine if we are able to shift the tickbase respective to previously setup variables (rofl)
	g_tickbase.m_shift_data.m_can_shift_tickbase = bCanShootIn12Ticks || ( !bCanShootNormally  ) && ( g_tickbase.m_shift_data.m_did_shift_before );

	// If we can shift tickbase, shift enough ticks in order to double-tap
	// Always prioritise fake-duck if we wish to
	if( g_tickbase.m_shift_data.m_can_shift_tickbase && !g_hvh.m_fake_duck ) {

		// Tell the cheat to shift tick-base and disable fakelag
		g_tickbase.m_shift_data.m_next_shift_amount = g_cl.m_goal_shift;
	}
	else { 
		g_tickbase.m_shift_data.m_next_shift_amount = 0;
		g_tickbase.m_shift_data.m_should_be_ready = false;
	}

	// Are we even supposed to shift tickbase?
	if( g_tickbase.m_shift_data.m_next_shift_amount > 0 ) {
		// Determine if we're able to double-tap  

		if( bCanShootIn12Ticks ) {
			if ( g_tickbase.m_shift_data.m_prepare_recharge && !bIsShooting ) {
				g_tickbase.m_shift_data.m_needs_recharge = g_cl.m_goal_shift;
				g_tickbase.m_shift_data.m_prepare_recharge = false;
			}
			else {

				if ( bIsShooting ) {

					// Store shifted command
					g_tickbase.m_prediction.m_shifted_command = g_cl.m_cmd->m_command_number;

					// Store shifted ticks
					g_tickbase.m_prediction.m_shifted_ticks = abs( g_tickbase.m_shift_data.m_current_shift );

					// Run teleport code.
					if ( !( g_hvh.m_fake_duck || g_config.get_hotkey( XOR( "aimbot_disable_exploits_key" ) )) && ( g_cfg[ XOR( "aimbot_exploits_teleport" ) ].get< bool >( ) || g_cfg[ XOR( "aimbot_hide_shots" ) ].get< bool >( ) ) && g_cfg[ XOR( "aimbot_exploits_enable" ) ].get< bool >( ) )
						m_client_move.m_ready_to_process = true;

					// Store original tickbase
					g_tickbase.m_prediction.m_original_tickbase = g_cl.m_local->m_nTickBase();

					// Update our wish ticks to shift, and later shift tickbase
					g_tickbase.m_shift_data.m_ticks_to_shift = g_tickbase.m_shift_data.m_next_shift_amount;

					// Save our shifttime for later use.
					g_tickbase.m_shift_data.m_shift_time = g_csgo.m_globals->m_tick_count;
				}
			}
		}
		else {
			if ( g_tickbase.m_shift_data.m_switched_weapon_passive && g_cl.m_weapon_type != WEAPONTYPE_KNIFE ) {
				g_tickbase.m_shift_data.m_switched_weapon_passive--;
			} else {
				g_tickbase.m_shift_data.m_prepare_recharge = true;
				g_tickbase.m_shift_data.m_should_be_ready = false;
			}	
		}
	}
	else {
		if ( g_tickbase.m_shift_data.m_switched_weapon_passive && g_cl.m_weapon_type != WEAPONTYPE_KNIFE ) {
			g_tickbase.m_shift_data.m_switched_weapon_passive--;
		} else {
			g_tickbase.m_shift_data.m_prepare_recharge = true;
			g_tickbase.m_shift_data.m_should_be_ready = false;
		}
	}

	// Note DidShiftBefore state 
	g_tickbase.m_shift_data.m_did_shift_before = g_tickbase.m_shift_data.m_next_shift_amount > 0;
}

// recharge without the notorious "lag", useful for hideshots and non-instant doubletap.
void TickbaseSystem::PassiveRecharge( ) {

	if ( g_cl.m_processing && g_tickbase.m_shift_data.m_should_attempt_shift && g_tickbase.m_shift_data.m_needs_recharge && g_tickbase.m_shift_data.m_shift_time + 3 <= g_csgo.m_globals->m_tick_count ) {
		g_cl.m_cmd->m_tick = INT_MAX;
		g_tickbase.m_shift_data.m_needs_recharge = 0;
		g_tickbase.m_shift_data.m_shift_time = 0;
		g_tickbase.m_shift_data.m_should_be_ready = true;
	}
}

void TickbaseSystem::Teleport( float AccumulatedExtraSamples, bool FinalTick ) {

	// sanity.
	if ( !g_cfg[ XOR( "aimbot_exploits_teleport" ) ].get< bool >( ) || !g_cfg[ XOR( "aimbot_exploits_enable" ) ].get< bool >( ) || ( g_hvh.m_fake_duck || g_config.get_hotkey( XOR( "aimbot_disable_exploits_key" ) ) ) )
		return g_detours.oCL_Move( AccumulatedExtraSamples, FinalTick );

	if ( g_tickbase.m_shift_data.m_should_attempt_shift && g_tickbase.m_shift_data.m_needs_recharge && g_tickbase.m_shift_data.m_shift_time + game::TIME_TO_TICKS( 1.1 ) <= g_csgo.m_globals->m_tick_count ) {
		--g_tickbase.m_shift_data.m_needs_recharge;
		g_tickbase.m_shift_data.m_in_recharge = true;
		if ( g_tickbase.m_shift_data.m_needs_recharge == 0 ) { 
			g_tickbase.m_shift_data.m_shift_time = 0;
			g_tickbase.m_shift_data.m_should_be_ready = true;
			g_tickbase.m_shift_data.m_in_recharge = false;
		}
		return;
	}

	auto m_nTickbase = m_client_move.m_ready_to_process ? m_shift_data.m_ticks_to_shift : 0;

	// call original anyways.
	g_detours.oCL_Move( AccumulatedExtraSamples, FinalTick );

	if( !m_nTickbase )
		return;

	// onetap teleport.
	for ( int i = m_nTickbase; i > 0; i -= 1 ) {
		m_nTickbase = i - 1;

		g_detours.oCL_Move( AccumulatedExtraSamples, FinalTick );
	}

	// make sure we recharge after.
	m_client_move.m_ready_to_process = false; 
	m_shift_data.m_prepare_recharge = true;
}     
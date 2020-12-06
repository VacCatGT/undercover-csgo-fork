#include "undercover.h"

HVH g_hvh{ };;

bool HVH::CanAntiAim( ) {

	if ( !g_cfg[ XOR( "aa_enabled" ) ].get< bool >( ) )
		return false;

	if ( !g_cl.m_cmd )
		return false;

	if ( !g_cl.m_processing )
		return false;

	// don't fucking use anti-aim when noclipping or on ladder ?!??!
	if ( ( g_cl.m_local->m_MoveType( ) == MOVETYPE_NOCLIP ) ) {
		return false;
	}

	// make sure we have a valid weapon.
	auto weapon = g_cl.m_weapon;
	if ( !weapon )
		return false;

	// are we throwing a nade?
	// if so then fuck off don't do anti aim
	if ( weapon->IsGrenade( ) )
		if ( weapon->m_fThrowTime( ) > 0 )
			return false;

	// don't do anti-aim if we're in use.
	if ( g_cl.m_cmd->m_buttons & IN_USE )
		return false;

	// don't do anti-aim on the freeze period, if we're frozen or on round end.
	if ( g_csgo.m_gamerules->m_bFreezePeriod( ) || ( g_cl.m_flags & FL_FROZEN ) )
		return false;

	return true;
}

void HVH::AntiAim( ) {

	if ( !CanAntiAim( ) ) // C_AntiAim::ShouldRun( )
		return;

	if( ( g_cl.m_cmd->m_buttons & IN_ATTACK ) && g_cl.CanFireWeapon( game::TICKS_TO_TIME( g_cl.m_local->m_nTickBase( ) ) ) ) {

		if ( g_cfg[ XOR( "aa_desync_on_shot" ) ].get< bool >( ) ) {
		
			// .....
		}
	}
	else 
	{
		if( g_cl.m_local->m_MoveType( ) != MOVETYPE_LADDER )
			goto LABEL_102;

		float v73 = ( ( g_cl.m_cmd->m_forward_move * g_cl.m_cmd->m_forward_move ) + ( g_cl.m_cmd->m_side_move * g_cl.m_cmd->m_side_move ) + ( g_cl.m_cmd->m_up_move * g_cl.m_cmd->m_up_move ) );

		if ( v73 <= 0.1 ) {
		LABEL_102:
			float v80;

			if( g_cl.m_local->m_MoveType( ) == MOVETYPE_LADDER )
				v80 = 89.0f;
			else
				// we don't need any pitches for now since we don't have any nospread features, maybe later.
				v80 = Pitch( 1 );

			g_cl.m_cmd->m_view_angles.x = v80; 

			g_cl.m_cmd->m_view_angles.y = Real( g_cl.m_cmd->m_view_angles.y );
			math::NormalizeAngle( g_cl.m_cmd->m_view_angles.y );
			
			// ......

			// C_AntiAim::JitterMove( )
			Fake( ); // C_AntiAim::Fake( )
			SlowWalk( );

		}
	}

}

float HVH::Pitch( int a1 ) {
	float result;

	switch ( a1 )
	{
	case 1:
		result = 89.0;
		break;
	case 2:
		result = 178.0;
		break;
	case 3:
		result = 0.0;
		break;
	case 4:
		result = -89.0;
		break;

	// note: they seem to subract m_vecEyAngles.x from these two pitches, ignore that for now.
	case 5:
		result = 540.0;
		break;
	case 6:
		result = -540.0;
		break;
	default:
		result = g_cl.m_cmd->m_view_angles.x;
		break;
	}
	return result;
}

float HVH::Real( float a1 ) {
	float result;

	if( g_cl.m_local->m_MoveType( ) == MOVETYPE_LADDER ) {
		return g_cl.m_cmd->m_view_angles.y;
	}
	else 
	{
		AtTargets( );
		auto v12 = g_cfg[ XOR( "aa_yaw_offset" ) ].get< int >( ) + g_cl.m_cmd->m_view_angles.y;
		bool v9;
		if( !m_jitter || abs( g_cfg[ XOR( "aa_jitter_offset" ) ].get< int >( ) ) <= 3 || ( g_csgo.m_cl->m_choked_commands ? ( v9 = m_jitter_flip ) : ( v9 = m_jitter_flip == 0, m_jitter_flip = m_jitter_flip == 0 ), !v9 ) ) {
			
			result = v12;

		}
		else {
			result = g_cfg[ XOR( "aa_jitter_offset" ) ].get< int >( ) + v12;
		}

		math::NormalizeAngle( result );

	}
	return result;
}

void HVH::AtTargets( ) {
	
	if( g_cfg[ XOR( "aa_at_targets" ) ].get< bool >( ) ) {
		
		Player* v6 = nullptr;

		auto GetBestTarget = [ & ] {

			float v654 = { std::numeric_limits< float >::max( ) }; // or 8192

			for( int i = 1; i <= 64; i++ ) {
			
				Player* temp_ent = reinterpret_cast< Player* >( g_csgo.m_entlist->GetClientEntity( i ) );

				if( !temp_ent )
					continue;

				if( !g_aimbot.IsValidTarget( temp_ent ) )
					continue;

				float v3 = g_cl.m_shoot_pos.dist_to( temp_ent->m_vecOrigin( ) );

				if ( v3 < v654 ) {
					v654 = v3;
					v6 = temp_ent;
				}
			}
		};

		GetBestTarget( ); // C_AntiAim::GetBestTarget( 1, 0 )

		if( v6 ) {
			ang_t v32;
			math::VectorAngles( v6->m_vecOrigin( ) - g_cl.m_local->m_vecOrigin( ), v32 );
			g_cl.m_cmd->m_view_angles.y = math::NormalizeYaw( v32.y + 180.0f );
		}
		else
		{
			auto v12 = g_cl.m_cmd->m_view_angles.y += 180;
			g_cl.m_cmd->m_view_angles.y = math::NormalizeYaw( v12 );
		}

	}
	else
	{
		auto v32 = g_cl.m_cmd->m_view_angles.y += 180;
		g_cl.m_cmd->m_view_angles.y = math::NormalizeYaw( v32 );
	}

}

void HVH::Manual( ) {


	
}

double HVH::Spin( float a1, bool a2 )
{
	static float dword_3F80B76C;

	auto v3 = 360 / g_cl.m_rate;
	if( a2 )
		v3 = -v3;

	float i;
	for ( i = v3 + dword_3F80B76C; i > 180.0; i = i - 360.0 )
		;
	for ( ; i < -180.0; i = i + 360.0 )
		;

	dword_3F80B76C = i;
	float v5;
	if ( !g_cl.m_packet )
	{
		if ( a2 )
			v5 = -120.0;
		else
			v5 = 120.0;
		i = i + v5;
	}

	double v7;

	v7 = i;
	if ( i > 180.0 )
	{
		do
			i = i - 360.0;
		while ( i > 180.0 );
		v7 = i;
	}
	if ( i < -180.0 )
	{
		do
			i = i + 360.0;
		while ( i < -180.0 );
		v7 = i;
	}
	return v7;
}

float HVH::GetFakeAngle( float m_flCurView, bool m_bInvert, float m_flMaxRotation )
{
	float DesyncDelta; // xmm4_4
	float m_flViewAngles; // xmm0_4 MAPDST
	int m_n14; // edx
	float m_flMaxAngle; // xmm4_4 MAPDST
	float m_flRealFootYaw; // xmm3_4 MAPDST
	float m_flRealAng; // xmm2_4
	float m_flUnkTime; // xmm1_4
	float m_flDesyncDelta; // xmm1_4
	bool m_Choke; // al
	bool m_bPacket; // ecx
	float m_flUnkDelta; // xmm3_4
	float m_flChokedAng; // xmm1_4

	DesyncDelta = m_flMaxRotation;
	m_flViewAngles = m_flCurView;
	m_n14 = 1;
	if ( !m_bInvert )
		DesyncDelta = -m_flMaxRotation;// -m_flMaxRotation
	m_flMaxAngle = math::NormalizeYaw( DesyncDelta + m_flCurView );
	m_flRealFootYaw = g_cl.m_rotation.y;
	m_flRealAng = math::NormalizeYaw( m_flRealFootYaw - m_flMaxAngle );
	m_flUnkTime = g_csgo.m_globals->m_interval * 102.0;// Black Magic
	
	if ( fabs( m_flRealAng ) <= m_flUnkTime
		&& m_flMaxRotation < g_cl.m_local->GetMaxBodyRotation( ) )// 0x68 = C_Animations::GetBodyRotation( )
	{
	/*	goto after1;*/
	}
	if ( fabs( m_flRealAng ) <= ( 180.0
		- ( g_cl.m_local->GetMaxBodyRotation( )
			+ m_flUnkTime ) ) )// 0x68 = C_Animations::GetBodyRotation( )
	{
		m_flDesyncDelta = g_cl.m_local->GetMaxBodyRotation( );
		if ( m_flRealAng <= 0.0 )
			m_flViewAngles = m_flDesyncDelta + m_flMaxAngle;
		else
			m_flViewAngles = m_flMaxAngle - m_flDesyncDelta;
	}
	else
	{
		m_flDesyncDelta = g_cl.m_local->GetMaxBodyRotation( );
		m_flViewAngles = m_flMaxAngle;
	}
	m_Choke = 1;
	m_bPacket = g_cl.m_packet;
	if ( m_flMaxRotation >= m_flDesyncDelta )     // Always Gets Passed
	{
		
		m_flUnkDelta = m_flRealFootYaw - m_flCurView;

		if ( m_bInvert )
		{
			if ( m_flUnkDelta < m_flMaxRotation || m_flUnkDelta >= 179.0 )
			{
			JUMPBACK:
				if ( m_flUnkDelta > 0.0 && m_flUnkDelta < 179.0 )
					m_flViewAngles = m_flCurView + 120.0;
				goto UNDO;
			}
		}
		else if ( -m_flMaxRotation < m_flUnkDelta || m_flUnkDelta <= -179.0 )
		{
			goto LABEL_50;
		}
		if ( g_cl.m_packet )
			goto after3;
		m_Choke = 0;
		if ( m_bInvert )
			goto JUMPBACK;
	LABEL_50:
		if ( m_flUnkDelta < 0.0 && m_flUnkDelta > -179.0 )
			m_flViewAngles = m_flCurView - 120.0;
	UNDO:
		if ( !m_Choke )
			goto after2;
	}
	//if ( !g_tickbase.m_shift_data.m_should_attempt_shift )
	//{
	//	if( g_csgo.m_cl->m_choked_commands < g_cl.m_max_lag )
	//		g_cl.m_packet = 0;
	//after1:
	//	m_bPacket = g_cl.m_packet;
	//}
after2:
	//if ( !m_bPacket && g_csgo.m_cl->m_choked_commands < g_cl.m_max_lag )
	//{
	//	m_flChokedAng = m_flCurView;
	//	goto NoChoke;
	//}
after3:
	//if ( !g_tickbase.m_shift_data.m_should_attempt_shift )
	//	m_bPacket = 1;
	//m_flChokedAng = m_flCurView;
	//m_flViewAngles = m_flCurView;
NoChoke:
	//if ( g_tickbase.m_shift_data.m_should_attempt_shift )
		//m_flViewAngles = m_flChokedAng;
	
	return math::NormalizeYaw( m_flViewAngles );
}

void HVH::Fake( ) {

	if( !g_cfg[ XOR( "aa_fake" ) ].get< bool >( ) )
		return;

	static bool m_bLastInvert, m_bStaticInvert1;

	
	int v31; // anti aim standing mode.

	if( !g_csgo.m_cl->m_choked_commands ) {
		m_stored_view = g_cl.m_cmd->m_view_angles.y;
	}

	if( !( g_cl.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
	
		if ( g_cfg[ XOR( "aa_air_mode" ) ].get< int >( ) > 0 )
		{  
			auto v32 = g_hvh.m_invert;
			auto v242 = Spin( 0, v32 ); // C_AntiAim::Spin( )
			g_cl.m_cmd->m_view_angles.y = v242;
			return;
		}
		goto LABEL_42;
	}

	auto MaxDesyncDelta = g_cl.m_local->GetMaxBodyRotation( );// MaxBodyRotation ?

	if( g_cl.m_local->m_vecVelocity( ).length( ) > 0.1 ) {

LABEL_42:
		v31 = false;
		goto LABEL_43;
	}

	v31 = g_cfg[ XOR( "aa_lby_mode" ) ].get< int >( );
	if ( g_cfg[ XOR( "aa_fake_desync" ) ].get< bool >( ) )
	{
		MaxDesyncDelta = 0.0f;
		if ( g_cfg[ XOR( "aa_lby_mode" ) ].get< int >( ) < 1 )
			v31 = 1;
	}

	m_bStaticInvert1 = g_hvh.m_invert; // actually how they do it lol.

LABEL_43:
	if ( g_tickbase.m_shift_data.m_should_attempt_shift )           // UGHHH
		v31 = 0;

	auto v36 = g_cl.m_pressing_move ? 0 : v31;

	float v76, v238{ };
	if( g_csgo.m_cl->m_choked_commands ) {
		
		goto LABEL_98;
	}

	// todo: add this to the header file with a better name.
	static auto byte_3F80620B = 0;

	static auto byte_3F80620A = g_hvh.m_invert; // this is wrong.

	if( g_cl.m_local->m_vecVelocity( ).length_2d( ) <= 10.0 ) {
		byte_3F80620B = 1;
		goto LABEL_78;
	}
	
	// "invertflip" - credits: llama
	bool running = ( ( g_cl.m_local->m_fFlags( ) & FL_ONGROUND ) ) && ( g_cl.m_local->m_vecVelocity( ).length_2d( ) > 136.0 ) && g_cfg[ XOR( "invertflip_moving" ) ].get< bool >( );
	bool air = g_cfg[ XOR( "invertflip_air" ) ].get< bool >( ) && !( g_cl.m_local->m_fFlags( ) & FL_ONGROUND );
	bool walking = ( g_cl.m_local->m_vecVelocity( ).length_2d( ) < 136.0 ) && ( ( g_cl.m_local->m_fFlags( ) & FL_ONGROUND ) ) && ( g_cl.m_local->m_vecVelocity( ).length_2d( ) > 30.0 ) && g_cfg[ XOR( "invertflip_walking" ) ].get< bool >( );
	if( running || air || walking ) {
		m_bStaticInvert1 = m_bLastInvert;
		if ( dword_3F32CD78 == 1 && !g_csgo.m_cl->m_choked_commands )
			m_bLastInvert = m_bLastInvert == 0;
	}
	else
		m_bStaticInvert1 = g_hvh.m_invert; //fix, probs wrong reverse
	
		
LABEL_98:
LABEL_78:

	// all overlap logic.

LABEL_114:

	auto m_bShouldMove = false; // micromove
	//float k;
	//float v11;
	//float a3;
	//auto v11 = g_csgo.m_globals->m_interval * 102.0;// Black Magic
	//if ( k <= v11 && a3 < ( g_cl.m_local->GetMaxBodyRotation( ) ) )// 0x68 = C_Animations::GetBodyRotation( )
	//{
	//	// goto LABEL_14;
	//}
	//else {

	//	// code inside C_AntiAim::GetFakeAngle( ):
	//	if ( !g_tickbase.m_shift_data.m_should_attempt_shift )
	//	{
	//		g_cl.m_packet = false;
	//		//byte_3F32CD7C = 1;
	//	}

	//}

	//float v248 = GetFakeAngle( ... );

//	g_cl.m_cmd->m_view_angles.y = GetFakeAngle( g_cl.m_view_angles.y, m_bStaticInvert1, MaxDesyncDelta );
	g_cl.m_cmd->m_view_angles.y += g_cl.m_packet ? 0 : ( m_bStaticInvert1 ? 120 : -120 );

	// notes from debug: 
	// values independent from lby mode.
	// standing still: 
	// sendpacket true: returnvalue 174.2 : -93
	// sendpacket false: returnvalue -65.7 : 26 
	// moving maxspeed: 
	// sendpacket true: returnvalue 58.1
	// sendpacket false: returnvalue 178.1	

	//g_cl.m_cmd->m_view_angles.y = v248;

	if ( !v36 )                                   // if( !C_Settings::LBYMode )
	{
		m_bShouldMove = 1;                          // Always MicroMove
		goto LABEL_139;
	}
	auto v110 = v36 - 1;

	if ( v110 )                                   // Sway Desync
	{
		if ( v110 != 1 || g_csgo.m_cl->m_choked_commands >= 2u )// if( g_pClientState->m_nChokedCommands >= 2u )
			goto LABEL_139;

MEME_ADJUST:
		m_bShouldMove = 1; // true
		static auto sway = false; // C_AntiAim::Sway
		auto v112 = sway == 0;
		g_cl.m_cmd->m_view_angles.y = m_stored_view;
		float v113;

		if ( v112 )                                 // if( !C_AntiAim::Sway )
			v113 = -60.0;
		else
			v113 = 60.0;

		g_cl.m_cmd->m_view_angles.y = g_cl.m_cmd->m_view_angles.y + v113;       // g_pCmd->m_vecViewangles.y += v113 
		auto v117 = math::AngleDiff( g_cl.m_cmd->m_view_angles.y, g_cl.m_fake_abs );
		if ( abs( v117 ) < 3.0 )// Distance Between Real and Fake < 3.0
			sway ^= 1;                    // C_AntiAim::Sway = true
	}
	else                                          // Opposite Desync
	{

		if ( g_csgo.m_cl->m_choked_commands >= 2u )// if( g_pClientState->m_nChokedCommands >= 2u )
			goto LABEL_139;

		m_bShouldMove = 1;
		auto v119 = m_stored_view;
		g_cl.m_cmd->m_view_angles.y = m_stored_view + 180.0;
		auto v120 = math::AngleDiff( v119, g_cl.m_fake_abs );
		float v123;

		if( abs( v120 ) <= 9 )
			goto MEME_ADJUST;

		if ( m_invert )                          // Inverted
		{
			if ( v120 <= -3.0 )
				goto MEME_ADJUST;
			v123 = v119 - 90.0;
		}
		else                                        // Not Inverted
		{
			if ( v120 >= 3.0 )
				goto MEME_ADJUST;
			v123 = v119 + 90.0;
		}

		g_cl.m_cmd->m_view_angles.y = v123;

	}

LABEL_136:
	if ( !g_tickbase.m_shift_data.m_should_attempt_shift )
	{
		g_cl.m_packet = false;
		//byte_3F32CD7C = 1;
	}
LABEL_139:
	g_cl.m_cmd->m_view_angles.y = math::NormalizeYaw( g_cl.m_cmd->m_view_angles.y ); // I guess...

	// do micromovements.
	if( !g_cl.m_local->m_MoveType( ) != MOVETYPE_LADDER && m_bShouldMove && !g_cl.m_pressing_move ) {
		float v207 = ( g_cl.m_cmd->m_buttons & IN_DUCK ) ? 3.25 : 1.1;

		g_cl.m_cmd->m_forward_move = g_cl.m_cmd->m_tick % 2 ? v207 : -v207;
	}

}

void HVH::Fakelag( ) {

	if( !g_cfg[ XOR( "aa_fakelag" ) ].get< bool >( ) )
		return;

	auto v43 = 0;
	auto v81 = g_cfg[ XOR( "aa_factor" ) ].get< int >( );
	auto v10 = g_cfg[ XOR( "aa_fl_jitter" ) ].get< int >( );
	auto v9 = g_cfg[ XOR( "aa_fl_triggers_amt" ) ].get< int >( );  

	if ( g_csgo.m_gamerules->m_bFreezePeriod( ) || g_cl.m_local->m_fFlags( ) & FL_FROZEN || !( g_hvh.m_fake_duck || g_config.get_hotkey( XOR( "aimbot_disable_exploits_key" ) ) ) && !g_tickbase.m_shift_data.m_shift_time && g_cfg[XOR("aimbot_exploits_teleport")].get< bool >( ) && g_cfg[ XOR( "aimbot_exploits_enable" ) ].get< bool >( ) || g_tickbase.m_shift_data.m_shift_time + 10 > g_csgo.m_globals->m_tick_count ) {
		g_cl.m_packet = g_csgo.m_cl->m_choked_commands >= 1;
		return;
	}

	if ( g_csgo.m_cl->m_choked_commands ) {
		v43 = g_cl.m_fakelag_jitter_value;
	}
	else {
		if ( v10 <= 0 ) {
			g_cl.m_fakelag_jitter_value = 0;
			goto LABEL_5;
		}
		auto v5 = rand( ) % v10;
		v43 = v5;
		g_cl.m_fakelag_jitter_value = v5;
	}
	if ( v10 > 0 ) {
		v81 -= ( ( ( v43 * v81 ) / 100.f ) + 0.5f );
		math::clamp( v81, 1, g_cl.m_max_lag );

		g_cl.m_packet = g_csgo.m_cl->m_choked_commands >= v81;
	}
LABEL_5:
	auto v49 = g_cl.m_local->m_vecVelocity( ).length( );
	auto v107 = v49 < 10.f;

	if ( g_cfg[ XOR( "aa_fl_in_air" ) ].get<bool>( ) && !v107 && !( g_cl.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
		auto v67 = g_cl.m_local->m_MoveType( );
		if ( v67 != MOVETYPE_LADDER )
			v81 = v9;
	}

	if ( g_cfg[ XOR( "aa_fl_while_reload" ) ].get<bool>( ) ) {
		auto v23 = g_cl.m_local->get<uintptr_t>( g_csgo.AnimOverlay );
		auto v73 = *( __m128* )( v23 + 0x58 );

		auto v52 = _mm_shuffle_ps( v73, v73, 170 ).m128_f32[ 3 ] >= 0.55f ? 1.0f : v73.m128_f32[ 3 ];
		if ( v52 < 0.99f )
			v81 = v9;
	}

	if ( g_cfg[ XOR( "aa_fl_on_velocity_change" ) ].get<bool>( ) ) {
		if ( abs( g_inputpred.PredictionData.m_vecUnpredictedVelocity.length( ) - v49 ) > 5.0f )
			v81 = v9;
	}

	if ( g_cfg[ XOR( "aa_fl_on_shot" ) ].get<bool>( ) ) {
		if ( g_cl.m_weapon ) {
			if ( g_cl.CanFireWeapon( game::TICKS_TO_TIME( g_cl.m_local->m_nTickBase( ) ) ) && g_cl.m_cmd->m_buttons & IN_ATTACK )
				v81 = v9;
		}
	}

	if ( g_cfg[ XOR( "aa_fl_on_wep_switch" ) ].get<bool>( ) ) {
		if ( g_cl.m_weapon && m_old_weapon ) {
			if ( g_cl.m_weapon != m_old_weapon )
				v81 = v9;
		}
	}

	if ( g_cfg[ XOR( "aa_fl_on_land" ) ].get<bool>( ) ) {
		auto v106 = g_inputpred.PredictionData.m_flUnpredictedFlags & FL_ONGROUND;
		auto v132 = g_cl.m_local->m_fFlags( ) & FL_ONGROUND;
		static bool v69 = false;

		if ( v69 ) {
			v69 = false;
			v81 = v9;
		}

		if ( v132 && !v106 )
			v69 = true;
	}

	if( g_cfg[ XOR( "aa_fl_break_lagcomp" ) ].get< bool >( ) ) {
		if( g_cl.m_lagcomp )
			v81 = v9;
	}

	m_old_weapon = g_cl.m_weapon;

	if ( !( g_hvh.m_fake_duck || g_config.get_hotkey( XOR( "aimbot_disable_exploits_key" ) ) ) && !g_tickbase.m_shift_data.m_shift_time && ( !g_cfg[ XOR( "aimbot_exploits_teleport" ) ].get< bool >( ) && g_cfg[ XOR( "aimbot_hide_shots" ) ].get< bool >( ) ) && g_cfg[ XOR( "aimbot_exploits_enable" ) ].get< bool >( ) || g_tickbase.m_shift_data.m_shift_time + 10 > g_csgo.m_globals->m_tick_count && !g_cfg[ XOR( "aimbot_exploits_teleport" ) ].get< bool >( ) )
		math::clamp( v81, 1, (g_cl.m_max_lag / 2 - 1) );
	else
		math::clamp( v81, 1, g_cl.m_max_lag );

	g_cl.m_packet = g_csgo.m_cl->m_choked_commands >= v81;
}

void HVH::FakeDuck( )
{
	if ( !g_csgo.m_cl || !g_cl.m_cmd )
		return;

	// ensure infinite duck.
	if ( !g_cfg[ XOR( "misc_duck_stamina" ) ].get< bool >( ) )
		g_cl.m_cmd->m_buttons |= IN_BULLRUSH;

	if ( !g_cl.m_processing || !m_fake_duck )
		return;

	// unduck if we are choking (m_max_lag / 2) or less ticks.
	if ( g_csgo.m_cl->m_choked_commands < ( g_cl.m_max_lag / 2 ) ) {
		g_cl.m_cmd->m_buttons &= ~IN_DUCK;
	}
	// duck if we are choking more than (m_max_lag / 2) ticks.
	else {
		g_cl.m_cmd->m_buttons |= IN_DUCK;
	}
}

void HVH::AutoPeek( ) {
	auto m_data = &m_autopeek_data;
	if ( g_hvh.m_auto_peek ) {
		if ( !m_autopeek_data.m_autopeeking ) {
			m_data->m_should_retrack = false;
			m_data->m_old_origin = g_cl.m_local->m_vecOrigin( );
		}
		if ( m_data->m_should_retrack ) {
			auto distance = g_cl.m_local->m_vecOrigin( ).dist_to( m_data->m_old_origin );
			if ( distance <= 16.0 ) {
				m_data->m_autopeeking = false;
			}
			else {
				ang_t ang_to_pos;
				ang_t local_angle;
				auto difference = m_data->m_old_origin - g_cl.m_local->m_vecOrigin( );
				math::VectorAngles( difference, ang_to_pos );
				g_csgo.m_engine->GetViewAngles( local_angle );
				auto v45 = cos( ang_to_pos.x * 0.017453292 );

				g_cl.m_cmd->m_forward_move = 0.0;
				g_cl.m_cmd->m_side_move = 0.0;

				auto v48 = math::deg_to_rad( ( local_angle.y - ang_to_pos.y ) );
				auto v66 = math::deg_to_rad( ( local_angle.y - ang_to_pos.y ) );
				auto cos_v48 = cos( v48 );
				auto sin_v48 = sin( v66 );

				auto v50 = cos_v48 * v45;

				g_cl.m_cmd->m_forward_move = fmaxf( fminf( 450.0, v50 * 450.0 ), -450.0 );
				g_cl.m_cmd->m_side_move = fmaxf( fminf( 450.0, ( float )( sin_v48 * v45 ) * 450.0 ), -450.0 );
			}
		}
		if ( g_cl.m_cmd->m_buttons & IN_ATTACK ) {
			if ( g_cl.CanFireWeapon( game::TICKS_TO_TIME( g_cl.m_local->m_nTickBase( ) ) ) )
				m_data->m_should_retrack = true;
		}
		m_data->m_autopeeking = true;
	}
	else {
		m_data->m_autopeeking = false;
		m_data->m_should_retrack = false;
	}
}

void HVH::SlowWalk( ) { // todo: add check for tickbase exploit so they automatically deactivate.

	if ( !g_movement.m_slow_motion )
		return;

	if ( !( g_cl.m_local->m_fFlags( ) & FL_ONGROUND ) ) // unpred flags!?!?
		return;

	if ( g_menu.m_opened ) // onetap does this everywhere (stay consistant ?)
		return;

	int m_nChokedCommands = g_csgo.m_cl->m_choked_commands;
	int MaxChoke = 13; // confirmed. this is correct.
	g_cl.m_cmd->m_buttons &= ~IN_SPEED;
	int ChokeLimit, v95;
	ChokeLimit = v95 = 8; // find out if it's eight or seven.

	if( g_cfg[ XOR( "aa_slowwalk_type" ) ].get< int >( ) ) {

		if ( m_nChokedCommands >= MaxChoke )
		{
			goto FullStop;
		}

		if ( !g_tickbase.m_shift_data.m_should_attempt_shift ) {
			g_cl.m_packet = 0;
		}

		if ( !m_nChokedCommands )
			goto LABEL_35;

		if ( g_cfg[ XOR( "aa_slowwalk_type" ) ].get< int >( ) == 1 )
		{
			if ( m_nChokedCommands > ChokeLimit + 1 ) // b1g difference
			{
				g_movement.Stop( 0.0 );
				return;
			}
			if ( m_nChokedCommands <= v95 )
				return;
		LABEL_35:

		FullStop:
			g_cl.m_cmd->m_forward_move = 0;
			g_cl.m_cmd->m_side_move = 0;
			g_cl.m_cmd->m_up_move = 0;
			return;
		}

		if ( g_cfg[ XOR( "aa_slowwalk_type" ) ].get< int >( ) == 2 ) {

			if ( m_nChokedCommands > ChokeLimit )
			{
				g_movement.Stop( 0.0 );
				return;
			}
		}

	}
	else 
	{
		float v4 = 0.33000001 * ( g_cl.m_local->m_bIsScoped( ) ? g_cl.m_weapon_info->flMaxPlayerSpeedAlt : g_cl.m_weapon_info->flMaxPlayerSpeed );

		if ( g_inputpred.PredictionData.m_vecUnpredictedVelocity.length_2d( ) > v4 ) {

			float squirt2 = std::sqrtf( ( g_cl.m_cmd->m_forward_move * g_cl.m_cmd->m_forward_move ) + ( g_cl.m_cmd->m_side_move * g_cl.m_cmd->m_side_move ) );

			float cock1 = g_cl.m_cmd->m_forward_move / squirt2;
			float cock2 = g_cl.m_cmd->m_side_move / squirt2;

			auto Velocity = g_cl.m_local->m_vecVelocity( ).length_2d( );

			if ( v4 + 1.0 <= Velocity ) {
				g_cl.m_cmd->m_forward_move = 0;
				g_cl.m_cmd->m_side_move = 0;
			}
			else {
				g_cl.m_cmd->m_forward_move = cock1 * v4;
				g_cl.m_cmd->m_side_move = cock2 * v4;
			}
		}

		return;
	}
}
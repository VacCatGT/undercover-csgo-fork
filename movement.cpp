#include "tapped.h"

Movement g_movement{ };;

void Movement::BunnyHop( ) {

	if ( !g_cl.m_cmd )
		return;

	if ( !g_cl.m_processing )
		return;

	if( !g_cfg[ XOR( "misc_bhop" ) ].get<bool>( ) )
		goto LABEL_22;

	if( g_cl.m_cmd->m_buttons & IN_JUMP ) {
		
		if( g_cl.m_local->m_MoveType( ) != MOVETYPE_LADDER ) {
		
			if ( g_cl.m_local->m_MoveType( ) != MOVETYPE_NOCLIP ) {

				if( !( g_cl.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
					g_cl.m_cmd->m_buttons &= 0xFFFFFFFD;
				}
			}
		}
	}
	else
	{
	LABEL_22:
		m_bhopping = 0;
	}
}

void Movement::FastStop( ) {
	static bool           m_move;

	if ( !g_cfg[ XOR( "misc_faststop" ) ].get< bool >( ) )
		return;

	if ( !g_cl.m_ground || ( g_cl.m_cmd->m_buttons & IN_JUMP ) )
		return;

	// store key presses.
	bool holding_w = g_cl.m_buttons & IN_FORWARD;
	bool holding_a = g_cl.m_buttons & IN_MOVELEFT;
	bool holding_s = g_cl.m_buttons & IN_BACK;
	bool holding_d = g_cl.m_buttons & IN_MOVERIGHT;

	// do not run if we are pressing any movement keys.
	if ( !( !holding_a && !holding_d && !holding_w && !holding_s ) )
		return;
	
	if ( g_cl.m_local->m_vecVelocity( ).length_2d( ) > 0.1f )
	{
		g_movement.Stop( 0.0 );
	}
	
}

void Movement::DirectionalStrafer( ) {
	
	if( g_cfg[ XOR( "misc_autostrafer_mode" ) ].get< int >( ) != 2 )
		return;

	float v17;
	static float flt_4473B78C;

	auto GetStrafeDirection = [ & ]( float m_flSpeedRotation ) {
		

		bool v12 = g_cl.m_cmd->m_buttons & IN_MOVELEFT;
		bool v4 = g_cl.m_cmd->m_buttons & IN_MOVERIGHT;
		ang_t angle;
		g_csgo.m_engine->GetViewAngles( angle );
		float v13 = g_cl.m_strafe_angles.y;
		auto v5 = 0.0;
		bool v6 = g_cl.m_cmd->m_buttons & IN_FORWARD;
		if ( v6 || !( g_cl.m_cmd->m_buttons & IN_BACK ) )
		{
			if ( v12 )
			{
				if ( v6 )
					v5 = 45.0;
				else
					v5 = 90.0;
			}
			if ( v4 )
			{
				if ( v6 )
					v5 = -45.0;
				else
					v5 = -90.0;
			}
		}
		else
		{
			if ( v12 )
			{
				v5 = 135.0;
				if ( !v4 )
					goto LABEL_16;
			}
			else if ( !v4 )
			{
				v5 = -180.0;
				goto LABEL_16;
			}
			v5 = -135.0;
		}
	LABEL_16:
		auto v7 = flt_4473B78C;
		float i, j;
		if ( v5 != flt_4473B78C )
		{
			for ( i = flt_4473B78C; i > 180.0; i = i - 360.0 )
				;
			for ( ; i < -180.0; i = i + 360.0 )
				;
			for ( ; v5 > 180.0; v5 = v5 - 360.0 )
				;
			for ( ; v5 < -180.0; v5 = v5 + 360.0 )
				;
			for ( j = v5 - i; j > 180.0; j = j - 360.0 )
				;
			for ( ; j < -180.0; j = j + 360.0 )
				;
			auto v10 = fminf( m_flSpeedRotation, abs( j ) );
			if ( j <= 0.0 )
				v7 = flt_4473B78C - v10;
			else
				v7 = v10 + flt_4473B78C;
		}

		flt_4473B78C = v7;
		auto v999 = math::NormalizeYaw( v7 + v13 );
		v17 = v999;
	};

	if ( !g_cl.m_cmd )
		return;

	if ( !g_cl.m_processing )
		return;
	
	static bool staticdir;

	if( ( g_cl.m_cmd->m_buttons & IN_JUMP ) && g_cl.m_local->m_vecVelocity( ).length_2d( ) >= 30.0 ) {
		
		auto v40 = ( asin( ( 30.0 / g_cl.m_local->m_vecVelocity( ).length_2d( ) ) ) * 57.29578 ) * 0.5;

		float v14;
		if ( staticdir )
			v14 = 180.0;
		else
			v14 = ( abs( v40 ) * ( ( g_cfg[ XOR( "strafe_speed" ) ].get< int >( ) / 100.0 ) + 1.0 ) );

		GetStrafeDirection( v14 );
		auto v43 = v17;

		auto velocity = g_cl.m_local->m_vecAbsVelocity( );
		auto vel = std::atan2( velocity.y, velocity.x );
		
		float absYaw, i, j;

		absYaw = math::RAD2DEG( vel );

		i = v43;
		
		j = math::NormalizeYaw( i - absYaw );

		bool positive = false;

		if( abs( j ) <= 1.0 ) {

			positive = g_cl.m_cmd->m_command_number & 1;

		}
		else 
		{
	
			positive = j > 0.0;

		}

		float yawDirection;
		if ( positive )
		{
			v40 = -v40;
			yawDirection = 450.0;// 450.0
		}
		else
		{
			yawDirection = -450.0;
		}

		g_cl.m_strafe_angles.y = v43;

		g_cl.m_cmd->m_forward_move = 0.0f;
		g_cl.m_cmd->m_side_move = yawDirection;

		auto v44 = cos( v40 * 0.017453292 );

		auto v29 = sin( v40 * 0.017453292 );

		auto xdelta = ( g_cl.m_cmd->m_forward_move * v44 ) - ( g_cl.m_cmd->m_side_move * v29 );

		auto ydelta = ( g_cl.m_cmd->m_forward_move * v29 ) - ( g_cl.m_cmd->m_side_move * v44 );

		g_cl.m_cmd->m_forward_move = xdelta;
		g_cl.m_cmd->m_side_move = ydelta;

		staticdir = 0;
	}
	else
	{
		staticdir = 1;
		flt_4473B78C = 0.0;
	}
}

void Movement::RotateMovement( float rotation )
{
	ang_t view;
	g_csgo.m_engine->GetViewAngles( view );
	rotation = math::deg_to_rad( view.y - rotation );

	float cos_rot = cos( rotation );
	float sin_rot = sin( rotation );

	float new_forwardmove = ( cos_rot * g_cl.m_cmd->m_forward_move ) - ( sin_rot * g_cl.m_cmd->m_side_move );
	float new_sidemove = ( sin_rot * g_cl.m_cmd->m_forward_move ) + ( cos_rot * g_cl.m_cmd->m_side_move );

	g_cl.m_cmd->m_forward_move = new_forwardmove;
	g_cl.m_cmd->m_side_move = new_sidemove;
}

void Movement::FixMove( CUserCmd* cmd, ang_t wish_angles ) {
	vec3_t view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	math::AngleVectors( wish_angles, &view_fwd, &view_right, &view_up );
	math::AngleVectors( cmd->m_view_angles, &cmd_fwd, &cmd_right, &cmd_up );

	const auto v8 = sqrtf( ( view_fwd.x * view_fwd.x ) + ( view_fwd.y * view_fwd.y ) );
	const auto v10 = sqrtf( ( view_right.x * view_right.x ) + ( view_right.y * view_right.y ) );
	const auto v12 = sqrtf( view_up.z * view_up.z );

	const vec3_t norm_view_fwd( ( 1.f / v8 ) * view_fwd.x, ( 1.f / v8 ) * view_fwd.y, 0.f );
	const vec3_t norm_view_right( ( 1.f / v10 ) * view_right.x, ( 1.f / v10 ) * view_right.y, 0.f );
	const vec3_t norm_view_up( 0.f, 0.f, ( 1.f / v12 ) * view_up.z );

	const auto v14 = sqrtf( ( cmd_fwd.x * cmd_fwd.x ) + ( cmd_fwd.y * cmd_fwd.y ) );
	const auto v16 = sqrtf( ( cmd_right.x * cmd_right.x ) + ( cmd_right.y * cmd_right.y ) );
	const auto v18 = sqrtf( cmd_up.z * cmd_up.z );

	const vec3_t norm_cmd_fwd( ( 1.f / v14 ) * cmd_fwd.x, ( 1.f / v14 ) * cmd_fwd.y, 0.f );
	const vec3_t norm_cmd_right( ( 1.f / v16 ) * cmd_right.x, ( 1.f / v16 ) * cmd_right.y, 0.f );
	const vec3_t norm_cmd_up( 0.f, 0.f, ( 1.f / v18 ) * cmd_up.z );

	const auto v22 = norm_view_fwd.x * cmd->m_forward_move;
	const auto v26 = norm_view_fwd.y * cmd->m_forward_move;
	const auto v28 = norm_view_fwd.z * cmd->m_forward_move;
	const auto v24 = norm_view_right.x * cmd->m_side_move;
	const auto v23 = norm_view_right.y * cmd->m_side_move;
	const auto v25 = norm_view_right.z * cmd->m_side_move;
	const auto v30 = norm_view_up.x * cmd->m_up_move;
	const auto v27 = norm_view_up.z * cmd->m_up_move;
	const auto v29 = norm_view_up.y * cmd->m_up_move;

	cmd->m_forward_move = ( ( ( ( norm_cmd_fwd.x * v24 ) + ( norm_cmd_fwd.y * v23 ) ) + ( norm_cmd_fwd.z * v25 ) )
		+ ( ( ( norm_cmd_fwd.x * v22 ) + ( norm_cmd_fwd.y * v26 ) ) + ( norm_cmd_fwd.z * v28 ) ) )
		+ ( ( ( norm_cmd_fwd.y * v30 ) + ( norm_cmd_fwd.x * v29 ) ) + ( norm_cmd_fwd.z * v27 ) );
	cmd->m_side_move = ( ( ( ( norm_cmd_right.x * v24 ) + ( norm_cmd_right.y * v23 ) ) + ( norm_cmd_right.z * v25 ) )
		+ ( ( ( norm_cmd_right.x * v22 ) + ( norm_cmd_right.y * v26 ) ) + ( norm_cmd_right.z * v28 ) ) )
		+ ( ( ( norm_cmd_right.x * v29 ) + ( norm_cmd_right.y * v30 ) ) + ( norm_cmd_right.z * v27 ) );
	cmd->m_up_move = ( ( ( ( norm_cmd_up.x * v23 ) + ( norm_cmd_up.y * v24 ) ) + ( norm_cmd_up.z * v25 ) )
		+ ( ( ( norm_cmd_up.x * v26 ) + ( norm_cmd_up.y * v22 ) ) + ( norm_cmd_up.z * v28 ) ) )
		+ ( ( ( norm_cmd_up.x * v30 ) + ( norm_cmd_up.y * v29 ) ) + ( norm_cmd_up.z * v27 ) );

	wish_angles = cmd->m_view_angles;

	if ( g_cl.m_local->m_MoveType( ) != MOVETYPE_LADDER && !g_cfg[ XOR( "misc_slide" ) ].get< bool >( ) ) {
		cmd->m_buttons &= ~( IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT );
	}
}

void Movement::EdgeJump( ) { 
	
	if( !g_cl.m_processing || !g_cl.m_cmd )
		return;

	// check for bind.
	if( !g_movement.m_edge_jump )
		return;

	if( g_inputpred.PredictionData.m_flUnpredictedFlags & FL_ONGROUND ) {
		if( !( g_cl.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
			g_cl.m_cmd->m_buttons |= 2u;
		}
	}
}

void Movement::Stop( float speed ) {

	auto get_max_accel_onetap = [ & ]( ) {
		vec3_t v6 = g_inputpred.PredictionData.m_vecUnpredictedVelocity;
		float v8 = g_csgo.sv_friction->GetFloat( ) * g_cl.m_local->m_surfaceFriction( );
		float v11 = v6.length( );

		if ( v11 <= 0.1f )
			goto LABEL_12;

		float v43 = fmax( v11, g_csgo.sv_stopspeed->GetFloat( ) );
		float v23 = v43 * v8 * g_csgo.m_globals->m_interval;
		float v19 = fmax( 0.f, v11 - v23 );

		if ( v19 != v11 ) {
			v19 /= v11;

			v6 *= v19;
		}
		LABEL_12:
		return v6;
	};

	float m_side_move{ }, m_forward_move{ }, v54{ };
	
	if( get_max_accel_onetap( ).length_2d( ) > speed ) { // use velocity here as well????
	
		ang_t v23{ };
		vec3_t velocity = get_max_accel_onetap( ), v6;
		math::VectorAngles( ( velocity * -1.f ), v23 );
		v23.y = g_cl.m_view_angles.y - v23.y;
		math::AngleVectors( v23, &v6 );

		m_side_move = 450.0;
		m_forward_move = v6.x * velocity.length_2d( );
		v54 = v6.y * velocity.length_2d( );
		goto FINISH;
	}


	// there is actually a function close to Movement::ClampMovementSpeed( ) down here.
	// todo: add it.

	FINISH:
	if ( m_forward_move <= 450.0 )
	{
		if ( m_forward_move < -450.0 )
			m_forward_move = -450.0;
	}
	else
	{
		m_forward_move = 450.0;
	}
	if ( v54 <= 450.0 )
	{
		if ( v54 >= -450.0 )
			m_side_move = v54;
		else
			m_side_move = -450.0;
	}

	g_cl.m_cmd->m_forward_move = m_forward_move;
	g_cl.m_cmd->m_side_move = m_side_move;

}

void Movement::SlideWalk( )
{
	if( !g_cl.m_processing )
		return;

	int v8;

	if( g_cl.m_local->m_MoveType( ) != 9 && g_cfg[ XOR( "misc_slide" ) ].get< bool >( ) )
		v8 = 1;
	else
		LABEL_24:
	v8 = 0;

	auto v10 = g_cl.m_cmd->m_forward_move;
	auto v11 = g_cl.m_cmd->m_buttons & ~0x618u;
	g_cl.m_cmd->m_buttons = v11;
	if ( v10 != 0.0 )
	{
		auto v12 = 0;
		if ( v8 )
			v12 = v10 > 0.0;
		else
			v12 = v10 <= 0.0;
		v11 |= 8 * v12 + 8;
		g_cl.m_cmd->m_buttons = v11;
	}
	auto v13 = g_cl.m_cmd->m_side_move;
	if ( v13 != 0.0 )
	{
		if ( v8 )
		{
			auto v14 = 1024;
			if ( v13 > 0.0 )
				v14 = 512;
			g_cl.m_cmd->m_buttons = v11 | v14;
		}
		else
		{
			auto v15 = 512;
			if ( v13 > 0.0 )
				v15 = 1024;
			g_cl.m_cmd->m_buttons = v11 | v15;
		}
	}
}
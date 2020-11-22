#include "undercover.h"

Resolver g_resolver{ };;
Player* m_pPlayer;
/* fix stored data */
void Resolver::BackupData( BackupData_t* m_data ) {
	std::memcpy( &m_data->m_nBackupState, m_data->m_pPlayer->m_PlayerAnimState( ), sizeof( CCSGOPlayerAnimState ) );
	m_data->m_pPlayer->GetAnimLayers( m_data->m_nBackupLayers );
	m_data->m_nBackupAngles = m_data->m_pPlayer->m_angEyeAngles( );
	m_data->m_nBackupSimulationTime = m_data->m_pPlayer->m_flSimulationTime( );
	m_data->m_nBackupDuckAmount = m_data->m_pPlayer->m_flDuckAmount( );
	m_data->m_pPlayer->GetPoseParameters( m_data->m_nBackupPoses );
	m_data->m_nBackupGoalFeetYaw = m_data->m_pPlayer->m_PlayerAnimState( )->goal_feet_yaw;
	m_data->m_nBackupClientSideAnimation = m_data->m_pPlayer->m_bClientSideAnimation( );
	m_data->m_flBackupLowerBodyTarget = m_data->m_pPlayer->m_flLowerBodyYawTarget( );
	m_data->m_nBackupFlags = m_data->m_pPlayer->m_fFlags( );
	m_data->m_nBackupVelocity = m_data->m_pPlayer->m_vecVelocity( );
	m_data->m_nBackupOrigin = m_data->m_pPlayer->m_vecOrigin( );
	m_data->m_nBackupAbsVelocity = m_data->m_pPlayer->m_vecAbsVelocity( );
	m_data->m_nBackupDucked = m_data->m_pPlayer->m_bDucking( );
}
void Resolver::RestoreData( BackupData_t* m_data ) {
	m_data->m_pPlayer->SetAnimLayers( m_data->m_nBackupLayers );
	memcpy( m_data->m_pPlayer->m_PlayerAnimState( ), &m_data->m_nBackupState, sizeof( CCSGOPlayerAnimState ) );
	m_data->m_pPlayer->SetPoseParameters( m_data->m_nBackupPoses );
	m_data->m_pPlayer->m_angEyeAngles( ) = m_data->m_nBackupAngles;
	m_data->m_pPlayer->m_bDucking( ) = m_data->m_nBackupDucked;
	m_data->m_pPlayer->m_vecVelocity( ) = m_data->m_nBackupVelocity;
	m_data->m_pPlayer->m_flDuckAmount( ) = m_data->m_nBackupDuckAmount;
	m_data->m_pPlayer->SetPoseParameters( m_data->m_nBackupPoses );
	m_data->m_pPlayer->m_vecOrigin( ) = m_data->m_nBackupOrigin;
	m_data->m_pPlayer->m_flLowerBodyYawTarget( ) = m_data->m_flBackupLowerBodyTarget;
	m_data->m_pPlayer->m_fFlags( ) = m_data->m_nBackupFlags;
	m_data->m_pPlayer->SetAbsVelocity( m_data->m_nBackupAbsVelocity );
	m_data->m_pPlayer->m_flSimulationTime( ) = m_data->m_nBackupSimulationTime;
	m_data->m_pPlayer->m_bClientSideAnimation( ) = m_data->m_nBackupClientSideAnimation;
}
void Resolver::StoreMatrices( LagComp::LagRecord_t& record ) {

	// create backup data.
	BackupData_t data{ };

	// get our player for easier use.
	m_pPlayer = record.m_pEntity;

	data.m_pPlayer = m_pPlayer;

	// get delta between eyeangles and goalfeetyaw
	auto m_flEyeDelta = math::AngleDiff( record.m_angEyeAngles.y, record.m_pState->goal_feet_yaw );

	// check if player has "inverted" desync
	auto m_bDeltaCheck = m_flEyeDelta <= 0.f;

	// get and clamp the player's choked commands
	auto m_nChoked = std::clamp( record.m_iChoked, 1, 17 );

	// backup all player values.
	g_resolver.BackupData( &data );

	int lag = record.m_iChoked;

	// update center.
	m_pPlayer->InvalidateBoneCache( );
	m_pPlayer->m_bClientSideAnimation( ) = true;
	m_pPlayer->UpdateClientSideAnimation( );
	m_pPlayer->m_bClientSideAnimation( ) = false;
	m_pPlayer->SetupBones( nullptr, 128, BONE_USED_BY_ANYTHING, m_pPlayer->m_flSimulationTime( ) );
	float center_playback = record.m_pEntity->m_AnimOverlay( )[ 6 ].m_playback_rate;
	memcpy( record.center_matrix, m_pPlayer->m_BoneCache( ).m_pCachedBones->Base( ), sizeof( matrix3x4_t ) * m_pPlayer->GetBoneCount( ) );
	memcpy( g_chams.m_stored_matrices[ m_pPlayer->index( ) - 1 ], record.center_matrix, sizeof( matrix3x4_t ) * m_pPlayer->GetBoneCount( ) );
	m_pPlayer->GetAnimLayers( record.center_layers );
	g_anims.RebuiltLayer6( m_pPlayer, &record.m_LayerData[ 0 ] );//TESTING

	// restore.
	g_resolver.RestoreData( &data );

	// update left.
	if ( m_nChoked <= 1 ) {
		g_anims.UpdateAnimations( m_pPlayer, &record, -1 );
		m_pPlayer->GetAnimLayers( record.left_layers );
	}
	else {
		g_anims.UpdateAnimations( m_pPlayer, &record, 1 );

		m_pPlayer->GetAnimLayers( record.right_layers );
		m_pPlayer->InvalidateBoneCache( );
		m_pPlayer->SetupBones( nullptr, 128, BONE_USED_BY_ANYTHING, m_pPlayer->m_flSimulationTime( ) );
		memcpy( record.right_matrix, m_pPlayer->m_BoneCache( ).m_pCachedBones->Base( ), sizeof( matrix3x4_t ) * m_pPlayer->GetBoneCount( ) );
	}
	g_anims.RebuiltLayer6( m_pPlayer, &record.m_LayerData[ 1 ] );//TESTING

		// restore.
	g_resolver.RestoreData( &data );

	// update right.
	if ( m_nChoked <= 1 ) {
		g_anims.UpdateAnimations( m_pPlayer, &record, 1 );
		m_pPlayer->GetAnimLayers( record.right_layers );
	}
	else {
		g_anims.UpdateAnimations( m_pPlayer, &record, -1 );
		m_pPlayer->GetAnimLayers( record.left_layers );
		m_pPlayer->InvalidateBoneCache( );
		m_pPlayer->SetupBones( nullptr, 128, BONE_USED_BY_ANYTHING, m_pPlayer->m_flSimulationTime( ) );
		memcpy( record.left_matrix, m_pPlayer->m_BoneCache( ).m_pCachedBones->Base( ), sizeof( matrix3x4_t ) * m_pPlayer->GetBoneCount( ) );
	}

	// store data from rebuilt animations.
   g_anims.RebuiltLayer6( m_pPlayer, &record.m_LayerData[ 2 ] ); //TESTING

	// restore.
	g_resolver.RestoreData( &data );
	g_anims.UpdateAnimations( m_pPlayer, &record, 0 );
}

void Resolver::ResolveAngles( Player* player, LagComp::LagRecord_t* record, LagComp::LagRecord_t* prev_record ) {
	AimPlayer* data = &g_aimbot.m_players[ player->index( ) - 1 ];

	/* calling settup detecting */
	Resolver::setup_detect( data, player, record );

	/* resolve entity */
	if ( record->m_bValid && record->m_iChoked > 2 && record->m_iChoked < 17 )
		ResolveEntity( data, record, prev_record );

	player->SetupBones( record->m_pMatrix, 128, BONE_USED_BY_ANYTHING, 0.0f );
}

/* detect side output things */
float get_backward_side( Player* player ) {
	if ( !g_cl.m_processing )
		return -1.f;

	return math::CalcAngle( g_cl.m_local->m_vecOrigin( ), player->m_vecOrigin( ) ).y;
}

/* detect desync side */
void Resolver::detect_side( Player* player, int* side ) {
	/* externs */
	vec3_t src3D, dst3D, forward, right, up, src, dst;
	float back_two, right_two, left_two;
	CGameTrace tr;
	CTraceFilterSimple filter;

	/* angle vectors */
	math::AngleVectors( ang_t( 0, get_backward_side( player ), 0 ), &forward, &right, &up );

	/* filtering */
	filter.SetPassEntity( player );
	src3D = player->Weapon_ShootPosition( );
	dst3D = src3D + ( forward * 384 );

	/* back engine tracers */
	g_csgo.m_engine_trace->TraceRay( Ray( src3D, dst3D ), MASK_SHOT, &filter, &tr );
	back_two = ( tr.m_endpos - tr.m_startpos ).length( );

	/* right engine tracers */
	g_csgo.m_engine_trace->TraceRay( Ray( src3D + right * 35, dst3D + right * 35 ), MASK_SHOT, &filter, &tr );
	right_two = ( tr.m_endpos - tr.m_startpos ).length( );

	/* left engine tracers */
	g_csgo.m_engine_trace->TraceRay( Ray( src3D - right * 35, dst3D - right * 35 ), MASK_SHOT, &filter, &tr );
	left_two = ( tr.m_endpos - tr.m_startpos ).length( );

	/* fix side */
	if ( left_two > right_two ) {
		*side = -1;
	}
	else if ( right_two > left_two ) {
		*side = 1;
	}
	else
		*side = 0;
}

/* check low desync delta defines */
#define delta(angle1, angle2) remainderf(fabsf(angle1 - angle2), 360.0f)
#define n(yaw) math::NormalizeYaw(fabsf(yaw))
#define ca(angle1, angle2) math::CalcAngle(angle1, angle2)

/* check low desync delta */
void Resolver::check_low_delta_desync( AimPlayer* data, Player* player, LagComp::LagRecord_t* record ) {
	/* setup animstate */
	CCSGOPlayerAnimState* animstate = player->m_PlayerAnimState( );

	/* setup desync delta vars */
	static float fl_stored_yaw = player->m_angEyeAngles( ).y;
	float fl_eye_yaw = player->m_angEyeAngles( ).y;
	float fl_lby_yaw = player->m_flLowerBodyYawTarget( );
	float fl_desync_delta = delta( fl_eye_yaw, animstate->goal_feet_yaw );
	fl_desync_delta = std::clamp( fl_desync_delta, -60.f, 60.f );

	/* setup target side */
	float fl_left_yaw = n( fl_eye_yaw - 60.0 );
	float fl_right_yaw = n( fl_eye_yaw + 60.0 );

	/* setup low delta */
	float fl_left_low_delta = n( fl_lby_yaw - 35.0 );
	float fl_right_low_delta = n( fl_lby_yaw + 35.0 );

	/* setup low desync checker */
	if ( fabs( fl_desync_delta ) < 35.f ) {
		/* settup missed shots */
		switch ( data->m_missed_shots % 3 ) {
			case 0: {
				animstate->goal_feet_yaw = fl_stored_brute[ player->index( ) ];
			} break;
			case 1: {
				animstate->goal_feet_yaw = fl_left_low_delta;
				fl_stored_brute[ player->index( ) ] = fl_left_low_delta;
			} break;
			case 2: {
				animstate->goal_feet_yaw = fl_right_low_delta;
				fl_stored_brute[ player->index( ) ] = fl_right_low_delta;
			} break;
		}
	}

	/* fixing moving delta vars */
	auto target_yaw = math::CalcAngle( g_cl.m_local->m_vecOrigin( ), player->m_vecOrigin( ) ).y;
	auto target_left_direction = math::NormalizeYaw( target_yaw - fl_left_yaw );
	auto target_right_direction = math::NormalizeYaw( target_yaw - fl_right_yaw );

	/* fixing moving delta */
	if ( target_left_direction > target_right_direction ) {
		/* positive right target */
		animstate->goal_feet_yaw = fl_right_yaw;
	} 
	else {
		/* negative left target */
		animstate->goal_feet_yaw = fl_left_yaw;
	}

	/* get stored yaw */
	if ( fl_stored_yaw != fl_eye_yaw ) {
		if ( animstate->current_feet_yaw != animstate->goal_feet_yaw ) {
			fl_stored_yaw = player->m_angEyeAngles( ).y;
			animstate->current_feet_yaw = animstate->goal_feet_yaw;
			animstate->goal_feet_yaw = animstate->feet_yaw_rate;
		}
	}

	/* check animrate */
	if ( animstate->feet_yaw_rate != 0 ) {
		/* check positive */
		if ( animstate->feet_yaw_rate == 60.0 || animstate->feet_yaw_rate > 60.0 ) {
			animstate->goal_feet_yaw = fl_right_yaw;
		}
		else if ( animstate->feet_yaw_rate == -60.0 || animstate->feet_yaw_rate < -60.0 ) {
			animstate->goal_feet_yaw = fl_left_yaw;
		}
	}

	/* check last desync delta ticks */
	if ( fabs( fl_desync_delta ) > 60.0 ) {
		if ( fabs( fl_desync_delta ) > 0 ) {
			animstate->goal_feet_yaw = fl_left_yaw;
		}
		else {
			animstate->goal_feet_yaw = fl_right_yaw;
		}
	}
	else if ( fabs( fl_desync_delta ) < -60.0 ) {
		if ( fabs( fl_desync_delta ) > 0 ) {
			animstate->goal_feet_yaw = fl_right_yaw;
		}
		else {
			animstate->goal_feet_yaw = fl_left_yaw;
		}
	}

	/* check if low delta was wrong */
	if ( data->m_missed_shots == 1 && fl_stored_brute[ player->index( ) ] == fl_left_yaw ) {
		animstate->goal_feet_yaw = fl_right_yaw;
	}
	else if ( data->m_missed_shots == 1 && fl_stored_brute[ player->index( ) ] == fl_right_yaw ) {
		animstate->goal_feet_yaw = fl_left_yaw;
	}

	/* bruteforce if check was wrong */
	switch ( data->m_missed_shots % 3 ) {
		case 0: {
			animstate->goal_feet_yaw = fl_stored_brute[ player->index( ) ];
		} break;
		case 1: {
			animstate->goal_feet_yaw = fl_left_low_delta;
			fl_stored_brute[ player->index( ) ] = fl_left_low_delta;
		} break;
		case 2: {
			animstate->goal_feet_yaw = fl_right_low_delta;
			fl_stored_brute[ player->index( ) ] = fl_right_low_delta;
		} break;
	}
}

void Resolver::ResolveEntity( AimPlayer* data, LagComp::LagRecord_t* record, LagComp::LagRecord_t* prev_record ) {
	// get the players max rotation.
	float max_rotation = record->m_pEntity->GetMaxBodyRotation( );

	if ( !record->m_pState )
		return;

	const auto info = g_anims.GetAnimationInfo( record->m_pEntity );
	if ( !info )
		return;

	//if (!g_cfg[("aimbot_resolver")].get< bool >())
	//	return;

	float eye_yaw = record->m_pState->eye_angles_y;

	// detect if player is using maximum desync.
	data->m_extending = record->m_pLayers[ 3 ].m_cycle == 0.f && record->m_pLayers[ 3 ].m_weight == 0.f;

	// resolve shooting players separately.
	if ( record->m_bDidShot ) {
		record->m_pState->goal_feet_yaw = eye_yaw + Resolver::ResolveShot( data, record );
		return;
	}
	/* ot movement fix bruterly  */
	else {
		/* ot movement fix  */
		if ( record->m_pEntity->m_vecVelocity( ).length_2d( ) <= 0.1 ) {
			float v60 = math::AngleDiff( eye_yaw, record->m_pState->goal_feet_yaw );
			data->m_index = 2 * v60 <= 0.0f ? 1 : -1;
		}
		else {
			/* ot movement fix  */
			if ( prev_record && !( ( int )record->m_pLayers[ 12 ].m_weight * 1000.f ) && record->m_pEntity->m_vecVelocity( ).length_2d( ) > 0.1 &&
				( ( int )record->m_pLayers[ 6 ].m_weight * 1000.f ) == ( ( int )prev_record->m_pLayers[ 6 ].m_weight * 1000.f ) && data->m_missed_shots % 3 == 0 ) {

				auto m_layer_delta1 = abs( record->m_pLayers[ 6 ].m_playback_rate - record->center_layers[ 6 ].m_playback_rate );
				auto m_layer_delta2 = abs( record->m_pLayers[ 6 ].m_playback_rate - record->left_layers[ 6 ].m_playback_rate );
				auto m_layer_delta3 = abs( record->m_pLayers[ 6 ].m_playback_rate - record->right_layers[ 6 ].m_playback_rate );

				if ( m_layer_delta1 < m_layer_delta2
					|| m_layer_delta3 <= m_layer_delta2
					|| ( signed int )( float )( m_layer_delta2 * 1000.0 ) )
				{
					if ( m_layer_delta1 >= m_layer_delta3
						&& m_layer_delta2 > m_layer_delta3
						&& !( signed int )( float )( m_layer_delta3 * 1000.0 ) )
					{
						data->m_index = 1;
					}
				}
				else
				{
					data->m_index = -1;
				}
			}
		}

		/* bruting */
		switch ( data->m_missed_shots % 3 ) {
		case 0:
			record->m_pState->goal_feet_yaw = record->m_angEyeAngles.y + max_rotation * data->m_index;
			break;
		case 1:
			if ( data->m_index == -1 )
				record->m_pState->goal_feet_yaw = record->m_angEyeAngles.y + max_rotation * 1;
			else
				record->m_pState->goal_feet_yaw = record->m_angEyeAngles.y + max_rotation * -1;
			break;
		case 2:
			record->m_pState->goal_feet_yaw = record->m_angEyeAngles.y;
			break;
		}
	}

	//wtf is this dogshit??
	/*if ( g_cfg[ ( "aimbot_resolver" ) ].get< bool >( ) ) {
		Resolver::check_low_delta_desync( data, m_pPlayer, record );
	}*/
}

float Resolver::ResolveShot( AimPlayer* data, LagComp::LagRecord_t* record ) {
    /* fix unrestricted shot */
	float flPseudoFireYaw = math::NormalizedAngle( math::CalcAngle( record->m_pMatrix[ 8 ].GetOrigin( ), g_cl.m_local->m_BoneCache( ).m_pCachedBones[ 0 ].GetOrigin( ) ).y );
	if ( data->m_extending ) {
		float flLeftFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y + 58.f ) ) );
		float flRightFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y - 58.f ) ) );

		return flLeftFireYawDelta > flRightFireYawDelta ? -58.f : 58.f;
	}
	else {
		float flLeftFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y + 29.f ) ) );
		float flRightFireYawDelta = fabsf( math::NormalizedAngle( flPseudoFireYaw - ( record->m_angEyeAngles.y - 29.f ) ) );

		return flLeftFireYawDelta > flRightFireYawDelta ? -29.f : 29.f;
	}
}

/* setup detect side usefull things */
float get_angle( Player* player ) {
	return math::NormalizedAngle( player->m_angEyeAngles( ).y );
}
float get_foword_yaw( Player* player ) {
	return math::NormalizedAngle( get_backward_side( player ) - 180.f );
}

/* setup detect side */
void Resolver::setup_detect( AimPlayer* data, Player* player, LagComp::LagRecord_t* record ) {
	/* calling detect side */
	static int side{};
	Resolver::detect_side( player, &side );

	if ( g_cfg[ ( "aimbot_pitch_resolver" ) ].get< bool >( ) ) {
		/* bruting vars */
		float resolve_value = 50.f;
		static float brute = 0.f;
		float fl_max_rotation = player->GetMaxBodyRotation( );
		float fl_eye_yaw = player->m_PlayerAnimState( )->eye_angles_y;
		float perfect_resolve_yaw = resolve_value;
		bool fl_foword = fabsf( math::NormalizedAngle( get_angle( player ) - get_foword_yaw( player ) ) ) < 90.f;
		int fl_shots = data->m_missed_shots;

		/* clamp angle */
		if ( fl_max_rotation < resolve_value ) {
			resolve_value = fl_max_rotation;
		}

		/* detect if player is using max desync angle */
		if ( data->m_extending ) {
			resolve_value = fl_max_rotation;
		}

		/* setup brting */
		if ( fl_shots == 0 ) {
			brute = perfect_resolve_yaw * ( fl_foword ? -side : side );
		}
		else {
			switch ( data->m_missed_shots % 3 ) {
			case 0: {
				brute = perfect_resolve_yaw * ( fl_foword ? -side : side );
			} break;
			case 1: {
				brute = perfect_resolve_yaw * ( fl_foword ? side : -side );
			} break;
			case 2: {
				brute = 0;
			} break;
			}
		}

		/* fix goalfeet yaw */
		player->m_PlayerAnimState( )->goal_feet_yaw = fl_eye_yaw + brute;
	}
}
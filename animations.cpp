#include "undercover.h"
#include "threading/CParallelProcessor.h"

Animations g_anims;

void Animations::AnimationInfo_t::UpdateAnimations( LagComp::LagRecord_t* pRecord, LagComp::LagRecord_t* pPreviousRecord ) {

	// validity checks
	if ( !pRecord )
		return;

	if ( !pRecord->m_pEntity->m_AnimOverlay( ) || !pRecord->m_pEntity->m_PlayerAnimState( ) )
		return;

	if ( !pPreviousRecord ) {
		pRecord->m_vecVelocity = m_pEntity->m_vecVelocity( );

		//ParallelProcess( pRecord, 1, &Resolver::StoreMatrices );

		g_resolver.ResolveAngles( m_pEntity, pRecord, pPreviousRecord );

		pRecord->Apply( m_pEntity );

		g_anims.UpdatePlayer( m_pEntity );

		return;
	}

	vec3_t vecVelocity = m_pEntity->m_vecVelocity( );

	m_pEntity->SetAnimLayers( pRecord->m_pLayers );
	m_pEntity->SetAbsOrigin( pRecord->m_vecOrigin );
	m_pEntity->SetAbsAngles( pRecord->m_angAbsAngles );
	m_pEntity->m_vecVelocity( ) = pPreviousRecord->m_vecVelocity;

	pRecord->m_vecVelocity = vecVelocity;

	pRecord->m_bDidShot = pRecord->m_flLastShotTime > pPreviousRecord->m_flSimulationTime && pRecord->m_flLastShotTime <= pRecord->m_flSimulationTime;

	vec3_t vecPreviousOrigin = pPreviousRecord->m_vecOrigin;
	int fPreviousFlags = pPreviousRecord->m_fFlags;

	for ( int i = 0; i < pRecord->m_iChoked; ++i ) {
		const float flSimulationTime = pPreviousRecord->m_flSimulationTime + game::TICKS_TO_TIME( i + 1 );
		const float flLerp = 1.f - ( pRecord->m_flSimulationTime - flSimulationTime ) / ( pRecord->m_flSimulationTime - pPreviousRecord->m_flSimulationTime );

		if ( !pRecord->m_bDidShot ) {
			auto vecEyeAngles = math::Interpolate( pPreviousRecord->m_angEyeAngles, pRecord->m_angEyeAngles, flLerp );
			m_pEntity->m_angEyeAngles( ).y = vecEyeAngles.y;
		}

		m_pEntity->m_flDuckAmount( ) = math::Interpolate( pPreviousRecord->m_flDuck, pRecord->m_flDuck, flLerp );

		if ( pRecord->m_iChoked - 1 == i ) {
			m_pEntity->m_vecVelocity( ) = vecVelocity;
			m_pEntity->m_fFlags( ) = pRecord->m_fFlags;
		}
		else {
			g_game_movement.Extrapolate( m_pEntity, vecPreviousOrigin, m_pEntity->m_vecVelocity( ), m_pEntity->m_fFlags( ), fPreviousFlags & FL_ONGROUND );
			fPreviousFlags = m_pEntity->m_fFlags( );

			pRecord->m_vecVelocity = ( pRecord->m_vecOrigin - pPreviousRecord->m_vecOrigin ) * ( 1.f / game::TICKS_TO_TIME( pRecord->m_iChoked ) );
		}

		if ( pRecord->m_bDidShot ) {
			/* NOTE FROM ALPHA:
			* This is to attempt resolving players' onshot desync, using the last angle they were at before they shot
			* It's good enough, but I will leave another (alternative) concept that could work when attempting to resolve on-shot desync
			* I haven't tested it at all, but the logic is there and in theory it should work fine (it's missing nullptr checks, obviously, since it's a concept)
			*
			* float flPseudoFireYaw = Math::NormalizeYaw( Math::CalculateAngle( m_pPlayer->GetBonePos( 8 ), m_pPlayer->GetBonePos( 0 ) ).y );
			* float flLeftFireYawDelta = fabsf( Math::NormalizeYaw( flPseudoFireYaw - ( pRecord->m_angEyeAngles.y + 58.f ) ) );
			* float flRightFireYawDelta = fabsf( Math::NormalizeYaw( flPseudoFireYaw - ( pRecord->m_angEyeAngles.y - 58.f ) ) );
			*
			* pRecord->m_pState->goal_feet_yaw = pRecord->m_pState->m_flEyeYaw + ( flLeftFireYawDelta > flRightFireYawDelta ? -58.f : 58.f );
			*/

			m_pEntity->m_angEyeAngles( ) = pPreviousRecord->m_angLastReliableAngle;

			if ( pRecord->m_flLastShotTime <= flSimulationTime ) {
				m_pEntity->m_angEyeAngles( ) = pRecord->m_angEyeAngles;
			}
		}

		const float flBackupSimtime = m_pEntity->m_flSimulationTime( );

		m_pEntity->m_flSimulationTime( ) = flSimulationTime;

		//ParallelProcess( pRecord, 1, &Resolver::StoreMatrices );
	  
		g_resolver.ResolveAngles( m_pEntity, pRecord, pPreviousRecord );

		g_anims.UpdatePlayer( m_pEntity );
	  
		m_pEntity->m_flSimulationTime( ) = flBackupSimtime;
	}
}

void Animations::UpdatePlayer( Player* pEntity ) {

	static bool& invalidate_bone_cache = **reinterpret_cast< bool** >( g_csgo.InvalidateBoneCacheVerification );

	const float frametime = g_csgo.m_globals->m_frametime;
	const float curtime = g_csgo.m_globals->m_curtime;

	CCSGOPlayerAnimState* const state = pEntity->m_PlayerAnimState( );
	if ( !state ) {
		return;
	}

	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;
	g_csgo.m_globals->m_curtime = pEntity->m_flSimulationTime( );

	// skip call to C_BaseEntity::CalcAbsoluteVelocity
	pEntity->m_iEFlags( ) &= ~0x1000; // EFL_DIRTY_ABSVELOCITY

	if ( state->last_client_side_animation_update_framecount == g_csgo.m_globals->m_frame ) {
		state->last_client_side_animation_update_framecount -= 1;
	}

	const bool backup_invalidate_bone_cache = invalidate_bone_cache;

	pEntity->m_bClientSideAnimation( ) = true;
	pEntity->UpdateClientSideAnimation( );
	pEntity->m_bClientSideAnimation( ) = false;

	pEntity->InvalidatePhysicsRecursive( ANGLES_CHANGED );
	pEntity->InvalidatePhysicsRecursive( ANIMATION_CHANGED );
	pEntity->InvalidatePhysicsRecursive( SEQUENCE_CHANGED );

	invalidate_bone_cache = backup_invalidate_bone_cache;

	g_csgo.m_globals->m_frametime = frametime;
	g_csgo.m_globals->m_curtime = curtime;
}

void Animations::UpdateAnimations( Player* player, LagComp::LagRecord_t* record, int index, float angle ) {

	player->SetAbsOrigin( player->m_vecOrigin( ) );
	int m_nChoked = record == nullptr ? 1 : record->m_iChoked;
	math::clamp( m_nChoked, 1, ( g_csgo.m_cvar->FindVar( HASH( "sv_maxusrcmdprocessticks" ) )->GetInt( ) + 1 ) );

	if ( !record || !record->m_pLayers || ( m_nChoked - 1 ) <= 1 ) {
		player->m_PlayerAnimState( )->feet_yaw_rate = 0.f;

		float yaw = player->m_angEyeAngles( ).y;

		if ( index ) {
			if ( index <= 0 )
				yaw = player->m_angEyeAngles( ).y - angle;
			else
				yaw = player->m_angEyeAngles( ).y + angle;

			yaw = math::NormalizeYaw( yaw );
			player->m_PlayerAnimState( )->goal_feet_yaw = yaw;
		}

		player->m_bClientSideAnimation( ) = true;
		player->UpdateClientSideAnimation( );
		player->m_bClientSideAnimation( ) = false;
		player->InvalidatePhysicsRecursive( ANIMATION_CHANGED );
		return;
	}

	const auto m_vecVelocity = player->m_vecVelocity( );
	const auto m_fFlags = player->m_fFlags( );

	player->SetAnimLayers( record->m_pLayers );
	player->m_vecOrigin( ) = record->m_vecOrigin;
	player->SetAbsAngles( record->m_angAbsAngles );
	player->m_vecVelocity( ) = record->m_vecVelocity;

	auto m_vecOldOrigin = record->m_vecOrigin;
	auto m_fOldFlags = record->m_fFlags;

	float yaw = record->m_angEyeAngles.y;

	for ( int i = 0; i < m_nChoked; i++ ) {
		const auto m_flTime = record->m_flSimulationTime - game::TICKS_TO_TIME( i + 1 );
		const auto m_flLerp = 1.f - ( player->m_flSimulationTime( ) - m_flTime ) / ( player->m_flSimulationTime( ) - record->m_flSimulationTime );

		player->m_flDuckAmount( ) = math::Interpolate( record->m_flDuck, player->m_flDuckAmount( ), m_flLerp );

		if ( ( m_nChoked - 1 ) == i ) {
			player->m_fFlags( ) = m_fOldFlags;

			if ( index != 0 ) {
				if ( index <= 0 )
					yaw = record->m_angEyeAngles.y - angle;
				else
					yaw = record->m_angEyeAngles.y + angle;

				yaw = math::NormalizeYaw( yaw );
				player->m_PlayerAnimState( )->goal_feet_yaw = yaw;
			}
		}
		else {
			g_game_movement.Extrapolate( player, m_vecOldOrigin, player->m_vecVelocity( ), player->m_fFlags( ), m_fOldFlags & FL_ONGROUND );
			m_fOldFlags = player->m_fFlags( );
		}

		player->m_PlayerAnimState( )->feet_cycle = record->m_pLayers[ 6 ].m_cycle;
		player->m_PlayerAnimState( )->feet_yaw_rate = record->m_pLayers[ 6 ].m_weight;

		const auto m_flBackupSimTime = player->m_flSimulationTime( );

		player->m_flSimulationTime( ) = m_flTime;

		player->m_bClientSideAnimation( ) = true;
		player->UpdateClientSideAnimation( );
		player->m_bClientSideAnimation( ) = false;

		player->m_flSimulationTime( ) = m_flBackupSimTime;
	}

	player->InvalidatePhysicsRecursive( ANIMATION_CHANGED );
}

bool Animations::GenerateMatrix( Player* pEntity, BoneArray* pBoneToWorldOut, int boneMask, float currentTime )
{
	if ( !pEntity || !pEntity->IsPlayer( ) || !pEntity->alive( ) )
		return false;

	CStudioHdr* pStudioHdr = pEntity->GetModelPtr( );
	if ( !pStudioHdr ) {
		return false;
	}

	// get ptr to bone accessor.
	CBoneAccessor* accessor = &pEntity->m_BoneAccessor( );
	if ( !accessor )
		return false;

	// store origial output matrix.
	// likely cachedbonedata.
	BoneArray* backup_matrix = accessor->m_pBones;
	if ( !backup_matrix )
		return false;

	vec3_t vecAbsOrigin = pEntity->GetAbsOrigin( );
	ang_t vecAbsAngles = pEntity->GetAbsAngles( );

	vec3_t vecBackupAbsOrigin = pEntity->GetAbsOrigin( );
	ang_t vecBackupAbsAngles = pEntity->GetAbsAngles( );

	matrix3x4a_t parentTransform;
	math::AngleMatrix( vecAbsAngles, vecAbsOrigin, parentTransform );

	pEntity->m_fEffects( ) |= 8;

	pEntity->SetAbsOrigin( vecAbsOrigin );
	pEntity->SetAbsAngles( vecAbsAngles );

	vec3_t pos[ 128 ];
	__declspec( align( 16 ) ) quaternion_t     q[ 128 ];

	m_bEnablePVS = true;

	uint32_t fBackupOcclusionFlags = pEntity->m_iOcclusionFlags( );

	pEntity->m_iOcclusionFlags( ) |= 0xA; // skipp call to accumulatelayers in standardblendingrules

	pEntity->StandardBlendingRules( pStudioHdr, pos, q, currentTime, boneMask );

	pEntity->m_iOcclusionFlags( ) = fBackupOcclusionFlags; // standardblendingrules was called now restore niggaaaa

	accessor->m_pBones = pBoneToWorldOut;

	uint8_t computed[ 0x100 ];
	std::memset( computed, 0, 0x100 );

	pEntity->BuildTransformations( pStudioHdr, pos, q, parentTransform, boneMask, computed );

	accessor->m_pBones = backup_matrix;

	pEntity->SetAbsOrigin( vecBackupAbsOrigin );
	pEntity->SetAbsAngles( vecBackupAbsAngles );

	m_bEnablePVS = false;

	return true;
}


void Animations::BuildLocalBones( Stage_t stage ) {

	// the client recieves new data from server this stage.
	if( stage != FRAME_NET_UPDATE_END )
		return;

	if( !g_cl.m_processing )
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState( );
	if( !state )
		return;

	// TODO: hook GetAbsAngles.
	g_cl.m_local->SetAbsAngles( g_cl.m_rotation );

	// check if we sent new data to the server.
	if( !g_cl.NewDataRecievedFromServer( ) )
		return;

	// allow SetupBones to run.
	g_anims.m_allow_bones = true;

	// build bones, according to sharklaser, only do it when needed ( performance ).
	if ( g_anims.m_local_matrix_ret = g_cl.m_local->SetupBones( g_cl.m_real_matrix, 128, BONE_USED_BY_ANYTHING, 0.0f ) )
	{
		// fix origin.
		for ( int BoneIndex = 0; BoneIndex < 128 /* Maxbones */; BoneIndex++ ) {
			g_anims.m_local_anim_origin[ BoneIndex ] = g_cl.m_local->GetAbsOrigin( ) - g_cl.m_real_matrix[ BoneIndex ].GetOrigin( );
		}
	}

	// don't run SetupBones until next update.
	g_anims.m_allow_bones = false;
} 

void Animations::UpdateLocalAnimations( ) {

	if( !g_cl.m_cmd || !g_cl.m_processing )
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState( );
	if( !state ) {
		return;
	}

	if( !g_csgo.m_cl ) {
		return;
	}

	C_AnimationLayer m_real_layers[ 13 ];
	float            m_real_poses[ 24 ];
	const float curtime = g_csgo.m_globals->m_curtime;
	const float frametime = g_csgo.m_globals->m_frametime;
	const float m_interpolation_amount = g_csgo.m_globals->m_interp_amt;
	const float m_realtime = g_csgo.m_globals->m_realtime;
	const float m_framecount = g_csgo.m_globals->m_frame;

	// allow reanimation in the same frame
	state->last_client_side_animation_update_framecount = 0;

	// fix feet.
	state->feet_yaw_rate = 0.0f;

	// update anim update delta as server build.
	state->anim_update_timer = std::max( 0.0f, g_csgo.m_globals->m_curtime - state->last_client_side_animation_update_time ); // negative values possible when clocks on client and server go out of sync..

	static int m_tick_count = 0;

	if ( m_tick_count != g_csgo.m_globals->m_tick_count )
	{
		m_tick_count = g_csgo.m_globals->m_tick_count;
		// invalidate bone cache.
		g_cl.m_local->InvalidateBoneCache( );

		// get layers.
		g_cl.m_local->GetAnimLayers( g_cl.m_real_layers );

		g_cl.m_update = true;

		// update animations.
		game::UpdateAnimationState( state, g_cl.m_cmd->m_view_angles );
		g_cl.m_local->UpdateClientSideAnimation( );

		g_cl.m_update = false;

		// save data when our choke cycle resets.
		if ( g_cl.m_packet ) {
			g_cl.m_rotation.y = state->goal_feet_yaw;
			g_cl.m_local->GetPoseParameters( g_cl.m_real_poses );
		}

		// update our layers and poses with the saved ones.
		g_cl.m_local->SetAnimLayers( g_cl.m_real_layers );
		g_cl.m_local->SetPoseParameters( g_cl.m_real_poses );
	}

	// Restore values to not mess with the game.
	g_csgo.m_globals->m_curtime = curtime;
	g_csgo.m_globals->m_frametime = frametime;
	g_csgo.m_globals->m_interp_amt = m_interpolation_amount;
	g_csgo.m_globals->m_realtime = m_realtime;
	g_csgo.m_globals->m_frame = m_framecount;

	if ( g_cl.m_spawn_time != g_cl.m_local->m_flSpawnTime( ) ) {
		g_cl.m_update_fake = false;
		g_cl.m_spawn_time = g_cl.m_local->m_flSpawnTime( );
	}

	if ( !g_cl.m_update_fake )
	{
		g_cl.m_update_fake = true;
		g_cl.m_fake_state = static_cast< CCSGOPlayerAnimState* >( g_csgo.m_mem_alloc->Alloc( sizeof( CCSGOPlayerAnimState ) ) );
		if ( g_cl.m_fake_state != nullptr )
			game::CreateAnimationState( g_cl.m_local, g_cl.m_fake_state );
	}

	if ( !g_cl.m_fake_state )
		return;

	if ( g_cl.m_packet ) {

		// fix feet.
		g_cl.m_fake_state->feet_yaw_rate = 0.0f;

		// replace current animation layers and poses with the fake ones.
		g_cl.m_local->GetAnimLayers( g_cl.m_fake_layers );
		g_cl.m_local->GetPoseParameters( g_cl.m_fake_poses );

		// replace the model rotation and build a matrix with our fake data.
		g_cl.m_local->SetAbsAngles( ang_t( 0.f, g_cl.m_fake_state->goal_feet_yaw, 0.f ) );

		// force our state to update.
		g_cl.m_fake_state->last_client_side_animation_update_framecount = 0.f;

		// update our custom animation state
		game::UpdateAnimationState( g_cl.m_fake_state, g_cl.m_cmd->m_view_angles );

		g_cl.m_local->m_AnimOverlay( )[ 12 ].m_weight = std::numeric_limits<float>::epsilon( );

		// build bones.
		g_anims.GenerateMatrix( g_cl.m_local, g_cl.m_fake_matrix, BONE_USED_BY_ANYTHING, 0.0f );

		// fix origin.
		for ( int BoneIndex = 0; BoneIndex < 128 /* Maxbones */; BoneIndex++ ) {
			g_anims.m_fake_anim_origin[ BoneIndex ] = g_cl.m_local->GetAbsOrigin( ) - g_cl.m_fake_matrix[ BoneIndex ].GetOrigin( );
		}

		// revert our layers and poses back.
		g_cl.m_local->SetAnimLayers( g_cl.m_fake_layers );
		g_cl.m_local->SetPoseParameters( g_cl.m_fake_poses );

		// replace the model rotation with the proper rotation.
		g_cl.m_local->SetAbsAngles( g_cl.m_rotation );
	}

	g_cl.m_fake_abs = g_cl.m_fake_state->goal_feet_yaw;
}

Animations::AnimationInfo_t* Animations::GetAnimationInfo( Player* pEntity ) {
	auto pInfo = Animations::m_ulAnimationInfo.find( pEntity->GetRefEHandle( ) );
	if ( pInfo == Animations::m_ulAnimationInfo.end( ) ) {
		return nullptr;
	}

	return &pInfo->second;
}

void Animations::RebuiltLayer6( Player* player, LagComp::LagRecord_t::LayerData_t* layer_data ) {
	// rebuilt layer 6 calculations from csgo
	// links:
	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/master/game/shared/cstrike15/csgo_playeranimstate.cpp#L1393
	// https://github.com/click4dylan/CSGO_AnimationCode_Reversed/blob/master/CCSGOPlayerAnimState_New.cpp#L2323
	// TODO: make code look good

	auto m_pState = player->m_PlayerAnimState( );

	static const float CS_PLAYER_SPEED_RUN = 260.0f;

	// TODO: Find these members in the actual animstate struct
	auto m_flLastUpdateIncrement = *( float* )( ( DWORD )m_pState + 0x74 );
	auto m_flFootYaw = m_pState->goal_feet_yaw;
	auto m_flMoveYaw = m_pState->torso_yaw;
	auto m_vecVelocityNormalizedNonZero = *( vec3_t* )( ( DWORD )m_pState + 0xE0 );
	auto m_flInAirSmoothValue = *( float* )( ( DWORD )m_pState + 0x124 );
	AnimationData_t& m_AnimationData = m_animation_data[ player->index( ) ];

	char m_szDestination[ 64 ];
	sprintf_s( m_szDestination, XOR( "move_%s" ), m_pState->GetWeaponPrefix( ) );

	int m_nMoveSequence = player->LookupSequence( m_szDestination );
	if ( m_nMoveSequence == -1 )
	{
		m_nMoveSequence = player->LookupSequence( XOR( "move" ) );
	}

	// NOTE: 
	// player->get<int>( 0x3984 ) is m_iMoveState 
	if ( player->get<int>( 0x3984 ) != m_AnimationData.m_iMoveState )
		m_AnimationData.m_flMovePlaybackRate += 10.0f;

	m_AnimationData.m_iMoveState = player->get<int>( 0x3984 );

	float m_flMovementTimeDelta = *( float* )( ( DWORD )m_pState + 0x74 ) * 40.0f;

	if ( -m_AnimationData.m_flMovePlaybackRate <= m_flMovementTimeDelta )
	{
		if ( -m_flMovementTimeDelta <= -m_AnimationData.m_flMovePlaybackRate )
			m_AnimationData.m_flMovePlaybackRate = 0.0f;
		else
			m_AnimationData.m_flMovePlaybackRate = m_AnimationData.m_flMovePlaybackRate - m_flMovementTimeDelta;
	}
	else
	{
		m_AnimationData.m_flMovePlaybackRate = m_AnimationData.m_flMovePlaybackRate + m_flMovementTimeDelta;
	}

	m_AnimationData.m_flMovePlaybackRate = std::clamp( m_AnimationData.m_flMovePlaybackRate, 0.0f, 100.0f );

	float m_flDuckSpeedClamped = std::clamp( *( float* )( ( DWORD )m_pState + 0xFC ), 0.0f, 1.0f );
	float m_flRunSpeedClamped = std::clamp( *( float* )( ( DWORD )m_pState + 0xF8 ), 0.0f, 1.0f );

	float m_flSpeedWeight = ( ( m_flDuckSpeedClamped - m_flRunSpeedClamped ) * *( float* )( ( DWORD )m_pState + 0xA4 ) ) + m_flRunSpeedClamped;

	if ( m_flSpeedWeight < layer_data->m_flFeetWeight )
	{
		float v34 = std::clamp( m_AnimationData.m_flMovePlaybackRate * 0.01f, 0.0f, 1.0f );
		float m_flFeetWeightElapsed = ( ( v34 * 18.0f ) + 2.0f ) * *( float* )( ( DWORD )m_pState + 0x74 );
		if ( m_flSpeedWeight - layer_data->m_flFeetWeight <= m_flFeetWeightElapsed )
			layer_data->m_flFeetWeight = -m_flFeetWeightElapsed <= ( m_flSpeedWeight - layer_data->m_flFeetWeight ) ? m_flSpeedWeight : layer_data->m_flFeetWeight - m_flFeetWeightElapsed;
		else
			layer_data->m_flFeetWeight = m_flFeetWeightElapsed + layer_data->m_flFeetWeight;
	}
	else
	{
		layer_data->m_flFeetWeight = m_flSpeedWeight;
	}

	float m_flYaw = math::AngleNormalize( ( m_pState->torso_yaw + m_pState->goal_feet_yaw ) + 180.0f );
	ang_t m_angAngle = { 0.0f, m_flYaw, 0.0f };
	vec3_t m_vecDirection;
	math::AngleVectors( m_angAngle, &m_vecDirection );

	float m_flMovementSide = math::DotProduct( m_vecVelocityNormalizedNonZero, m_vecDirection );
	if ( m_flMovementSide < 0.0f )
		m_flMovementSide = -m_flMovementSide;

	float m_flNewFeetWeight = math::Bias( m_flMovementSide, 0.2f ) * layer_data->m_flFeetWeight;

	float m_flNewFeetWeightWithAirSmooth = m_flNewFeetWeight * m_flInAirSmoothValue;

	// m_flLayer5Weight looks a bit weird so i decided to name it m_flLayer5_Weight instead.
	float m_flLayer5_Weight = player->GetLayerWeight( 5 );

	float m_flNewWeight = 0.55f;
	if ( 1.0f - m_flLayer5_Weight > 0.55f )
		m_flNewWeight = 1.0f - m_flLayer5_Weight;

	float m_flNewFeetWeightLayerWeight = m_flNewWeight * m_flNewFeetWeightWithAirSmooth;
	float m_flFeetCycleRate = 0.0f;

	float m_flSpeed = std::fmin( player->m_vecVelocity( ).length( ), CS_PLAYER_SPEED_RUN );
	if ( m_flSpeed > 0.00f )
	{
		float m_flSequenceCycleRate = player->GetSequenceCycleRate( player->GetModelPtr( ), m_nMoveSequence );

		float m_flSequenceMoveDist = player->GetSequenceMoveDist( player->GetModelPtr( ), m_nMoveSequence );
		m_flSequenceMoveDist *= 1.0f / ( 1.0f / m_flSequenceCycleRate );
		if ( m_flSequenceMoveDist <= 0.001f )
			m_flSequenceMoveDist = 0.001f;

		float m_flSpeedMultiplier = m_flSpeed / m_flSequenceMoveDist;
		m_flFeetCycleRate = ( 1.0f - ( m_pState->stop_to_full_running_fraction * 0.15f ) ) * ( m_flSpeedMultiplier * m_flSequenceCycleRate );
	}

	float m_flFeetCyclePlaybackRate = ( *( float* )( ( DWORD )m_pState + 0x74 ) * m_flFeetCycleRate );
	m_AnimationData.m_flPrimaryCycle = m_flFeetCyclePlaybackRate + m_AnimationData.m_flPrimaryCycle;

	// store possible information for resolving.
	layer_data->m_flMovementSide = m_flMovementSide;
	layer_data->m_angMoveYaw = m_angAngle;
	layer_data->m_vecDirection = m_vecDirection;
	layer_data->m_flFeetWeight = m_flNewFeetWeight;

	// maybe it can be used for something, keeping it just in case.
	layer_data->m_nSequence = m_nMoveSequence;
	layer_data->m_flPlaybackRate = m_flFeetCyclePlaybackRate;
	layer_data->m_flCycle = m_AnimationData.m_flPrimaryCycle;
	layer_data->m_flWeight = std::clamp( m_flNewFeetWeightLayerWeight, 0.0f, 1.0f );
}
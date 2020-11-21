#include "tapped.h"

void Hooks::DoExtraBoneProcessing( int a2, int a3, int a4, int a5, int a6, int a7 ) {
	// cast thisptr to player ptr.
	Player* player = ( Player* )this;

	return;

	if( !player )
		return g_hooks.m_player.GetOldMethod<DoExtraBoneProcessing_t>( Player::DOEXTRABONEPROCESSING )( this, a2, a3, a4, a5, a6, a7 );

	if( player && player->IsPlayer( ) && player->alive( ) ) {
		int* pAnimLayersOwner = reinterpret_cast< int* >( *reinterpret_cast< uintptr_t* >( reinterpret_cast< uintptr_t >( player ) + 0x2990 ) + 0x30 );
		for( int i = 13; i; --i ) {
			if( reinterpret_cast< Player* >( pAnimLayersOwner ) != player ) {
				*pAnimLayersOwner = reinterpret_cast< uintptr_t >( player );
			}

			pAnimLayersOwner += 14;
		}
	}

	return;
}

void Hooks::BuildTransformations( int a2, int a3, int a4, int a5, int a6, int a7 ) {
	// cast thisptr to player ptr.
	Player* player = ( Player* )this;

	if (!player || !player->IsPlayer())
		return g_hooks.m_player.GetOldMethod<BuildTransformations_t>(Player::BUILDTRANSFORMATIONS)(this, a2, a3, a4, a5, a6, a7);

	// backup jiggle bones.
	auto backup_jiggle_bones = player->get<uintptr_t>( 0x292C );

	// overwrite jiggle bones and refuse the game from calling the
	// code responsible for animating our attachments/weapons.
	player->set( 0x292C, 0 );

	// call og.
	g_hooks.m_player.GetOldMethod<BuildTransformations_t>( Player::BUILDTRANSFORMATIONS )( this, a2, a3, a4, a5, a6, a7 );

	// revert jiggle bones.
	player->set( 0x292C, backup_jiggle_bones );
}

void Hooks::StandardBlendingRules( int a2, int a3, int a4, int a5, int a6 ) {
	// cast thisptr to player ptr.
	Player* player = ( Player* )this;

	if( !player || ( player->index( ) - 1 ) > 63 )
		return g_hooks.m_player.GetOldMethod<StandardBlendingRules_t>( Player::STANDARDBLENDINGRULES )( this, a2, a3, a4, a5, a6 );

	// disable interpolation.
	if (!(player->m_fEffects() & EF_NOINTERP))
		player->m_fEffects( ) |= EF_NOINTERP;

	g_hooks.m_player.GetOldMethod<StandardBlendingRules_t>( Player::STANDARDBLENDINGRULES )( this, a2, a3, a4, a5, a6 );

	// restore interpolation.
	player->m_fEffects( ) &= ~EF_NOINTERP;
}

void Hooks::UpdateClientSideAnimation( ) {
	Player* player = ( Player* )this;

	if( !player || !player->m_bIsLocalPlayer( ) )
		return g_hooks.m_player.GetOldMethod<UpdateClientSideAnimation_t>( Player::UPDATECLIENTSIDEANIMATION )( this );

	if ( g_anims.m_local_matrix_ret )
	{
		for ( int i = 0; i < 128; i++ )
			g_cl.m_real_matrix[ i ].SetOrigin( g_cl.m_local->GetAbsOrigin( ) - g_anims.m_local_anim_origin[ i ] );

		for ( int i = 0; i < 128; i++ )
			g_cl.m_fake_matrix[ i ].SetOrigin( g_cl.m_local->GetAbsOrigin( ) - g_anims.m_fake_anim_origin[ i ] );


		memcpy( g_cl.m_local->m_BoneCache( ).m_pCachedBones->Base( ), g_cl.m_real_matrix, sizeof( matrix3x4_t ) * g_cl.m_local->GetBoneCount( ) );

		const auto oldBones = player->m_BoneAccessor( ).m_pBones;
		player->m_BoneAccessor( ).m_pBones = g_cl.m_real_matrix;
		player->AttachmentHelper( );
		player->m_BoneAccessor( ).m_pBones = oldBones;
	}
	
	if( g_cl.m_update ) {
		g_hooks.m_player.GetOldMethod<UpdateClientSideAnimation_t>( Player::UPDATECLIENTSIDEANIMATION )( this );
	}
}

vec3_t Hooks::Weapon_ShootPosition( vec3_t* ang ) {
	Player* player = ( Player* )this;
	
	/* as close is posible of server hitbox */
	return player->Weapon_ShootPosition( );
}

void Hooks::CalcView( vec3_t& eyeOrigin, vec3_t& eyeAngles, float& zNear, float& zFar, float& fov ) {
	Player* player = ( Player* )this;

	if( !player || !player->m_bIsLocalPlayer( ) )
		return g_hooks.m_player.GetOldMethod<CalcView_t>( Player::CALCVIEW )( this, eyeOrigin, eyeAngles, zNear, zFar, fov );
	
	// Prevent CalcView from calling CCSGOPlayerAnimState::ModifyEyePosition( ... ) 
	// this will fix inaccuracies, for example when fakeducking - and will enforce
	// us to use our own rebuilt version of CCSGOPlayerAnimState::ModifyEyePosition from the server.
	auto m_bUseNewAnimstate = player->get<bool>( 0x3AB4 );
	
	player->set<int>( 0x3AB4, false );
	
	g_hooks.m_player.GetOldMethod<CalcView_t>( Player::CALCVIEW )( this, eyeOrigin, eyeAngles, zNear, zFar, fov );
	
	player->set<int>( 0x3AB4, m_bUseNewAnimstate );
}
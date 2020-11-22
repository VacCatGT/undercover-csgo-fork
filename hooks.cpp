#include "undercover.h"

Hooks                g_hooks{ };;

void Pitch_proxy( CRecvProxyData* data, Address ptr, Address out ) {
	// normalize this fucker.
	math::NormalizeAngle( data->m_Value.m_Float );

	// clamp to remove retardedness.
	math::clamp( data->m_Value.m_Float, -90.f, 90.f );

	// call original netvar proxy.
	if( g_hooks.m_Pitch_original )
		g_hooks.m_Pitch_original( data, ptr, out );
}

inline int random_sequence( const int low, const int high )
{
	return rand( ) % ( high - low + 1 ) + low;
}

static std::unordered_map <uint32_t, recvProxy> proxies;

static int GetAnimationSequence( const uint32_t model, int sequence )
{
	switch ( model )
	{
	case HASH( "models/weapons/v_knife_butterfly.mdl" ):
	{
		switch ( sequence )
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence( SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03 );
		default:
			return sequence + 1;
		}
	}
	case HASH( "models/weapons/v_knife_falchion_advanced.mdl" ):
	{
		switch ( sequence )
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence( SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP );
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence( SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02 );
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	}
	case HASH( "models/weapons/v_knife_push.mdl" ):
	{
		switch ( sequence )
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return random_sequence( SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5 );
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence( SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1 );
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	}
	case HASH( "models/weapons/v_knife_survival_bowie.mdl" ):
	{
		switch ( sequence )
		{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	}
	case HASH( "models/weapons/v_knife_ursus.mdl" ):
	case HASH( "models/weapons/v_knife_skeleton.mdl" ):
	case HASH( "models/weapons/v_knife_outdoor.mdl" ):
	case HASH( "models/weapons/v_knife_cord.mdl" ):
	case HASH( "models/weapons/v_knife_canis.mdl" ):
	{
		switch ( sequence )
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence( SEQUENCE_BUTTERFLY_LOOKAT01, 14 );
		default:
			return sequence + 1;
		}
	}
	case HASH( "models/weapons/v_knife_stiletto.mdl" ):
	{
		switch ( sequence )
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence( 12, 13 );
		}
	}
	case HASH( "models/weapons/v_knife_widowmaker.mdl" ):
	{
		switch ( sequence )
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence( 14, 15 );
		}
	}
	default:
		return sequence;
	}
};

void Sequence_proxy( CRecvProxyData* data, void* entity, void* output ) {
	if( !g_cl.m_processing ) {
		if ( g_hooks.m_Sequence_original )
			g_hooks.m_Sequence_original( data, entity, output );
		return;
	}

	int m_nItemDefinitionIndex = g_cl.m_weapon_id;

	if( g_cl.m_weapon ) {
		
		if ( const auto weapon_info = g_cl.m_weapon_info )
			data->m_Value.m_Int = GetAnimationSequence( fnv1a_rt( weapon_info->szWorldModel ), data->m_Value.m_Int );
		if ( m_nItemDefinitionIndex == 1 )            // WEAPON_DEAGLE
		{
			if ( g_cfg[ XOR( "skins_rare_deagle" ) ].get< bool >( ) && data->m_Value.m_Int == 7 )
				data->m_Value.m_Int = 8;
		}
		else if ( m_nItemDefinitionIndex == 64 )      // WEAPON_REVOLVER
		{
			if ( g_cfg[ XOR( "skins_rare_revolver" ) ].get< bool >( ) && data->m_Value.m_Int == 3 )
				data->m_Value.m_Int = 4;
		}
		else if ( m_nItemDefinitionIndex == 512 && g_cfg[ XOR( "skins_rare_falcheon" ) ].get< bool >( ) &&  data->m_Value.m_Int == 12 )// WEAPON_KNIFE_FALCHION
		{
			data->m_Value.m_Int = 13;
		}
	}
	if ( g_hooks.m_Sequence_original )
		g_hooks.m_Sequence_original( data, entity, output );
}

void Force_proxy( CRecvProxyData* data, Address ptr, Address out ) {
	// convert to ragdoll.
	Ragdoll* ragdoll = ptr.as< Ragdoll* >( );

	// get ragdoll owner.
	Player* player = ragdoll->GetPlayer( );

	// we only want this happening to noobs we kill.
	if( g_cfg[ XOR( "misc_ragdoll_force" ) ].get<bool>( ) && g_cl.m_local && player && player->enemy( g_cl.m_local ) ) {
		// get m_vecForce.
		vec3_t vel = { data->m_Value.m_Vector[ 0 ], data->m_Value.m_Vector[ 1 ], data->m_Value.m_Vector[ 2 ] };

		// give some speed to all directions.
		vel *= 1000.f;

		// boost z up a bit.
		if( vel.z <= 1.f )
			vel.z = 2.f;

		vel.z *= 2.f;

		// don't want crazy values for this... probably unlikely though?
		math::clamp( vel.x, std::numeric_limits< float >::lowest( ), std::numeric_limits< float >::max( ) );
		math::clamp( vel.y, std::numeric_limits< float >::lowest( ), std::numeric_limits< float >::max( ) );
		math::clamp( vel.z, std::numeric_limits< float >::lowest( ), std::numeric_limits< float >::max( ) );

		// set new velocity.
		data->m_Value.m_Vector[ 0 ] = vel.x;
		data->m_Value.m_Vector[ 1 ] = vel.y;
		data->m_Value.m_Vector[ 2 ] = vel.z;
	}

	if( g_hooks.m_Force_original )
		g_hooks.m_Force_original( data, ptr, out );
}

void Hooks::init( ) {
	// hook wndproc.

	g_csgo.m_window = FindWindowA( XOR( "Valve001" ), NULL );

	m_old_wndproc = ( WNDPROC )g_winapi.SetWindowLongA( g_csgo.m_window, GWL_WNDPROC, util::force_cast< LONG >( Hooks::WndProc ) );

	// setup normal VMT hooks. 
	m_panel.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_panel );
	m_panel.Init( );
    m_panel.add( IPanel::PAINTTRAVERSE, util::force_cast( &Hooks::PaintTraverse ) );

	m_client.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_client );
	m_client.Init( );
	m_client.add( CHLClient::LEVELINITPREENTITY, util::force_cast( &Hooks::LevelInitPreEntity ) );
	m_client.add( CHLClient::LEVELINITPOSTENTITY, util::force_cast( &Hooks::LevelInitPostEntity ) );
	m_client.add( CHLClient::LEVELSHUTDOWN, util::force_cast( &Hooks::LevelShutdown ) );
	m_client.add( CHLClient::FRAMESTAGENOTIFY, util::force_cast( &Hooks::FrameStageNotify ) ); 
	m_client.add( 24, util::force_cast( &Hooks::WriteUsercmdDeltaToBuffer ) ); 
	 
	m_engine.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_engine );
	m_engine.Init( );
	m_engine.add( IVEngineClient::ISCONNECTED, util::force_cast( &Hooks::IsConnected ) );
	m_engine.add( IVEngineClient::ISHLTV, util::force_cast( &Hooks::IsHLTV ) );
	m_engine.add( IVEngineClient::FIREEVENTS, util::force_cast( &Hooks::FireEvents ) );
	m_engine.add( IVEngineClient::ISBOXVISIBLE, util::force_cast( &Hooks::IsBoxVisible ) );

	m_prediction.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_prediction );
	m_prediction.Init( );
	m_prediction.add( CPrediction::INPREDICTION, util::force_cast( &Hooks::InPrediction ) );
	m_prediction.add( CPrediction::RUNCOMMAND, util::force_cast( &Hooks::RunCommand ) );
	m_prediction.add( CPrediction::SETUPMOVE, util::force_cast( &Hooks::SetupMove ) );

	m_client_mode.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_client_mode );
	m_client_mode.Init( );
	m_client_mode.add( IClientMode::SHOULDDRAWPARTICLES, util::force_cast( &Hooks::ShouldDrawParticles ) );
	m_client_mode.add( IClientMode::SHOULDDRAWFOG, util::force_cast( &Hooks::ShouldDrawFog ) );
	m_client_mode.add( IClientMode::OVERRIDEVIEW, util::force_cast( &Hooks::OverrideView ) );
	m_client_mode.add( IClientMode::CREATEMOVE, util::force_cast( &Hooks::CreateMove ) );
	m_client_mode.add( IClientMode::DOPOSTSPACESCREENEFFECTS, util::force_cast( &Hooks::DoPostScreenSpaceEffects ) );
	m_client_mode.add( IClientMode::GETVIEWMODELFOV, util::force_cast( &Hooks::GetViewmodelFov ) );

	m_surface.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_surface );
	m_surface.Init( );
	m_surface.add( ISurface::LOCKCURSOR, util::force_cast( &Hooks::LockCursor ) );
	m_surface.add( ISurface::ONSCREENSIZECHANGED, util::force_cast( &Hooks::OnScreenSizeChanged ) );

	m_model_render.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_model_render );
	m_model_render.Init( );
	m_model_render.add( IVModelRender::DRAWMODELEXECUTE, util::force_cast( &Hooks::DrawModelExecute ) ); 

	m_render_view.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_render_view );
	m_render_view.Init( );
	//m_render_view.add( IVRenderView::SCENEEND, util::force_cast( &Hooks::SceneEnd ) );

	m_view_render.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_view_render );
	m_view_render.Init( );
	m_view_render.add( CViewRender::ONRENDERSTART, util::force_cast( &Hooks::OnRenderStart ) );
	m_view_render.add( CViewRender::RENDER2DEFFECTSPOSTHUD, util::force_cast( &Hooks::Render2DEffectsPostHUD ) );
	m_view_render.add( CViewRender::RENDERSMOKEOVERLAY, util::force_cast( &Hooks::RenderSmokeOverlay ) );
	  
	m_material_system.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_material_system );
	m_material_system.Init( );
	m_material_system.add( IMaterialSystem::OVERRIDECONFIG, util::force_cast( &Hooks::OverrideConfig ) );

	m_player.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_player_vtable );
	m_player.Init( );
	m_player.add( Player::DOEXTRABONEPROCESSING, util::force_cast( &Hooks::DoExtraBoneProcessing ) );
	m_player.add( Player::BUILDTRANSFORMATIONS, util::force_cast( &Hooks::BuildTransformations ) );
	m_player.add( Player::STANDARDBLENDINGRULES, util::force_cast( &Hooks::StandardBlendingRules ) );
	m_player.add( Player::UPDATECLIENTSIDEANIMATION, util::force_cast( &Hooks::UpdateClientSideAnimation ) );
	m_player.add( Player::CALCVIEW, util::force_cast( &Hooks::CalcView ) );
	m_player.add( Player::WEAPONSHOOTPOSITION, util::force_cast( &Hooks::Weapon_ShootPosition ) );

	m_net_channel.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_net );
	m_net_channel.Init( );
	//m_net_channel.add(INetChannel::PROCESSPACKET, util::force_cast( &Hooks::ProcessPacket ) );

	m_device.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_device );
	m_device.Init( );
	m_device.add( 17, util::force_cast( &Hooks::Present ) );
	//m_device.add( 16, util::force_cast( &Hooks::Reset ) );
	 
	// cvar hooks.
	m_debug_spread.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.weapon_debug_spread_show );
	m_debug_spread.Init( );
	m_debug_spread.add( ConVar::GETINT, util::force_cast( &Hooks::DebugSpreadGetInt ) );

	m_cl_csm_shadows.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.cl_csm_shadows );
	m_cl_csm_shadows.Init( );
	m_cl_csm_shadows.add( ConVar::GETINT, util::force_cast( &Hooks::CsmShadowGetInt ) );

	m_mat_wireframe.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.mat_wireframe );
	m_mat_wireframe.Init( );
	m_mat_wireframe.add( ConVar::GETINT, util::force_cast( &Hooks::MatWireframeGetInt ) );

	m_r_drawmodelstatsoverlay.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.r_drawmodelstatsoverlay );
	m_r_drawmodelstatsoverlay.Init( );
	m_r_drawmodelstatsoverlay.add( ConVar::GETINT, util::force_cast( &Hooks::R_DrawmodelstatsoverlayGetInt ) );

	m_sv_cheats.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.sv_cheats );
	m_sv_cheats.Init( );
	m_sv_cheats.add( ConVar::GETINT, util::force_cast( &Hooks::SvCheatsGetBool ) );

	m_sv_competitive_minspec.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.sv_competitive_minspec );
	m_sv_competitive_minspec.Init( );
	m_sv_competitive_minspec.add( ConVar::GETINT, util::force_cast( &Hooks::SvCompetitiveMinspecGetInt ) );

	m_bsp_query.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_engine->GetBSPTreeQuery( ) );
	m_bsp_query.Init( );
	m_bsp_query.add( 6, util::force_cast( &Hooks::ListLeavesInBox ) );

	m_engine_sound.m_vtable = reinterpret_cast< uintptr_t** >( g_csgo.m_sound );
	m_engine_sound.Init( );
	m_engine_sound.add( 5, util::force_cast( &Hooks::EmitSound ) );

	// set netvar proxies.
	g_netvars.SetProxy( HASH( "DT_CSRagdoll" ), HASH( "m_vecForce" ), Force_proxy, m_Force_original );
	g_netvars.SetProxy( HASH( "DT_PredictedViewModel" ), HASH( "m_nSequence" ), Sequence_proxy, m_Sequence_original );

}
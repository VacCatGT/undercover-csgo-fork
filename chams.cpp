#include "tapped.h"

Chams g_chams{ };;

void Chams::SetColor( Color col, color_type_t type ) {
	
	if( type == color_type_t::envmap_tint ) {

		bool did_find = false;
		auto material = silhouette->FindVar( XOR( "$envmaptint" ), &did_find ); 

		if ( did_find ) {
			( *( void( __thiscall * * )( int, float, float, float ) )( *( DWORD* )material + 44 ) )( (uintptr_t )material, col.r( ) / 255.f, col.g( ) / 255.f, col.b( ) / 255.f );
		}
	}
	else {
		g_csgo.m_render_view->SetColorModulation( col );
	}
}

void Chams::SetAlpha( float alpha, IMaterial* mat ) {
	if( mat )
		mat->AlphaModulate( alpha );

	else
		g_csgo.m_render_view->SetBlend( alpha );
}

void Chams::SetupMaterial( IMaterial* mat, Color col, bool z_flag ) {
	g_csgo.m_render_view->SetColorModulation( col );


	// mat->SetFlag( MATERIAL_VAR_HALFLAMBERT, flags );
	mat->SetFlag( MATERIAL_VAR_ZNEARER, z_flag );
	mat->SetFlag( MATERIAL_VAR_NOFOG, z_flag );
	mat->SetFlag( MATERIAL_VAR_IGNOREZ, z_flag );

	g_csgo.m_studio_render->ForcedMaterialOverride( mat );
}

void Chams::init( ) {
	// find stupid materials.
	debugambientcube = g_csgo.m_material_system->FindMaterial( XOR( "debug/debugambientcube" ), XOR( "Model textures" ) );
	debugdrawflat = g_csgo.m_material_system->FindMaterial( XOR( "debug/debugdrawflat" ), nullptr );
	silhouette = g_csgo.m_material_system->FindMaterial( XOR( "dev/glow_armsrace.vmt" ), nullptr );
} 

void Chams::DrawShotRecord( Player* ent, uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone ) {

	for( int i { }; i < m_hits->size( ); i++ ) {
		auto data = m_hits->at( i );

		if( ent != data.ent )
			continue;

		if( !data.time || data.time + game::TIME_TO_TICKS( 1 ) < g_csgo.m_globals->m_tick_count )
			continue;

		g_csgo.m_studio_render->ForcedMaterialOverride( silhouette );
		g_csgo.m_render_view->SetBlend( 1.0f );
		SetColor( { 255, 255, 255, 255 }, color_type_t::envmap_tint );

		g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >( IVModelRender::DRAWMODELEXECUTE )( g_csgo.m_model_render, ctx, state, info, data.m_last_pose );
		g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >( IVModelRender::DRAWMODELEXECUTE )( g_csgo.m_model_render, ctx, state, info, bone );
		g_csgo.m_studio_render->ForcedMaterialOverride( nullptr );
	}
	
}

bool Chams::GenerateLerpedMatrix( int index, BoneArray* out ) {
	if( !g_cl.m_processing )
		return false;

	auto player = g_csgo.m_entlist->GetClientEntity<Player*>( index );
	if( !player )
		return false;

	const auto LastRecord = g_lagcompensation.GetViableRecords( player, 1.0f );

	if( !LastRecord.has_value( ) )
		return false;

	const auto& FirstInvalid = LastRecord.value( ).first;
	const auto& LastInvalid = LastRecord.value( ).second;

	if( FirstInvalid->m_bDormant )
		return false;

	if( LastInvalid->m_flSimulationTime - FirstInvalid->m_flSimulationTime > 0.5f )
		return false;

	const auto NextOrigin = LastInvalid->m_vecOrigin;
	const auto curtime = g_csgo.m_globals->m_curtime;

	auto flDelta = 1.f - ( curtime - LastInvalid->m_flInterpTime ) / ( LastInvalid->m_flSimulationTime - FirstInvalid->m_flSimulationTime );
	if( flDelta < 0.f || flDelta > 1.f )
		LastInvalid->m_flInterpTime = curtime;

	flDelta = 1.f - ( curtime - LastInvalid->m_flInterpTime ) / ( LastInvalid->m_flSimulationTime - FirstInvalid->m_flSimulationTime );

	const auto lerp = math::Interpolate( NextOrigin, FirstInvalid->m_vecOrigin, std::clamp( flDelta, 0.f, 1.f ) );

	BoneArray ret[ 128 ];
	memcpy( ret, FirstInvalid->m_pMatrix_Resolved, sizeof( BoneArray[ 128 ] ) );

	for( size_t i{ }; i < 128; ++i ) {
		const auto matrix_delta = math::MatrixGetOrigin( FirstInvalid->m_pMatrix_Resolved[ i ] ) - FirstInvalid->m_vecOrigin;
		math::MatrixSetOrigin( matrix_delta + lerp, ret[ i ] );
	}

	memcpy( out, ret, sizeof( BoneArray[ 128 ] ) );
	return true;
}

void Chams::RenderHistoryChams( int index ) {
	AimPlayer* data;

	Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( index );
	if( !player )
		return;
	
	if( player->dormant( ) )
		return;

	if( !g_aimbot.IsValidTarget( player ) )
		return;

	bool enemy = g_cl.m_local && player->enemy( g_cl.m_local );
	if( enemy ) {
		data = &g_aimbot.m_players[ index - 1 ];
		if( !data )
			return;

		// get color.
		Color color = g_cfg[ XOR( "esp_chams_enemies_history_color" ) ].get_color( );

		IMaterial* material;

		switch ( g_cfg[ XOR( "esp_chams_history_enemies" ) ].get< int >( ) )
		{
		case 1:
			material = debugdrawflat; break;
		case 2: 
			material = debugambientcube; break;
		case 3: 
			material = silhouette; break;
		default:
			material = debugdrawflat; break;
		}

		material->ColorModulate( color );

		// override blend.
		SetAlpha( color.a( ) / 255.f );

		// set material and color.
		g_csgo.m_studio_render->ForcedMaterialOverride( material );

		// was the matrix properly setup?
		BoneArray arr[ 128 ];
		if( Chams::GenerateLerpedMatrix( index, arr ) ) {
			// backup the bone cache before we fuck with it.
			auto backup_bones = player->m_BoneCache( ).m_pCachedBones;

			// replace their bone cache with our custom one.
			player->m_BoneCache( ).m_pCachedBones = arr;

			// manually draw the model.
			player->DrawModel( );

			// reset their bone cache to the previous one.
			player->m_BoneCache( ).m_pCachedBones = backup_bones;
		}
	}
}

void Chams::ApplyChams( std::string material, std::string color, float alpha, int material_flag, bool flag_value, uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone, bool chams_background ) {

	if( !g_cl.m_processing )
		return;

	if ( g_cfg[ material ].get< int >( ) == 3 ) { 

		// normal chams.
		if ( !chams_background )
			ApplyChams( XOR( "force_render_unlit" ), color, alpha, material_flag, flag_value, ctx, state, info, bone );

		// original.
		g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >( IVModelRender::DRAWMODELEXECUTE )( g_csgo.m_model_render, ctx, state, info, bone );

		// overlay.
		g_cfg[ material ].get< int >( ) == 1 ? debugdrawflat->SetFlag( material_flag, flag_value ) :  g_cfg[ material ].get< int >( ) == 2 ? debugambientcube->SetFlag( material_flag, flag_value ) :  g_cfg[ material ].get< int >( ) == 3 ? silhouette->SetFlag( material_flag, flag_value ) : 0;
		g_csgo.m_studio_render->ForcedMaterialOverride(  g_cfg[ material ].get< int >( ) == 1 ? debugdrawflat :  g_cfg[ material ].get< int >( ) == 2 ? debugambientcube :  g_cfg[ material ].get< int >( ) == 3 ? silhouette : nullptr );
		g_csgo.m_render_view->SetBlend( 1.0f ); // force max opacity!
		SetColor( { 255, 255, 255, 255 }, g_cfg[ material ].get< int >( ) == 3 ? color_type_t::envmap_tint : color_type_t::normal );
		
		// we don't wanna call the rest.
		return;
	}

	// last was silhouette, let's draw normal chams.
	if ( material == XOR( "force_render_unlit" ) ) {
		
		debugambientcube->SetFlag( material_flag, flag_value );
		g_csgo.m_studio_render->ForcedMaterialOverride( debugambientcube );
		g_csgo.m_render_view->SetBlend( g_cfg[ color ].get_color( ).a( ) / 255.f );
		SetColor( g_cfg[ color ].get_color( ), color_type_t::normal );

		// don't override it.
		return;
	}

	g_cfg[ material ].get< int >( ) == 1 ? debugdrawflat->SetFlag( material_flag, flag_value ) :  g_cfg[ material ].get< int >( ) == 2 ? debugambientcube->SetFlag( material_flag, flag_value ) :  g_cfg[ material ].get< int >( ) == 3 ? silhouette->SetFlag( material_flag, flag_value ) : 0;
	g_csgo.m_studio_render->ForcedMaterialOverride(  g_cfg[ material ].get< int >( ) == 1 ? debugdrawflat :  g_cfg[ material ].get< int >( ) == 2 ? debugambientcube :  g_cfg[ material ].get< int >( ) == 3 ? silhouette : nullptr );
	g_csgo.m_render_view->SetBlend( g_cfg[ color ].get_color( ).a( ) / 255.f );
	SetColor( g_cfg[ color ].get_color( ), g_cfg[ material ].get< int >( ) == 3 ? color_type_t::envmap_tint : color_type_t::normal );
} 

bool Chams::DrawModel( uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone ) {
	
	if( !g_csgo.m_engine->IsInGame( ) ) {
		m_running = false;
		return true;
	}

	if( !g_cl.m_local )
		return true;

	if( !g_cl.m_weapon )
		return true;

	if( g_cl.m_processing )
		m_running = true;

	if( !m_running )
		return true;

	// store the model name for later use.
	const char* name = g_csgo.m_model_info->GetModelName( info.m_model );

	// perform arm chams, users can chose from flat, normal, silhouette.
	if ( strstr( name, XOR( "arms" ) ) && g_cfg[ XOR( "chams_hand" ) ].get< bool >( ) ) {
		ApplyChams( XOR( "hand_chams_material" ), XOR( "hand_chams_color" ), g_cfg[ XOR( "hand_chams_color" ) ].get_color( ).a( ) / 255.f, MATERIAL_VAR_IGNOREZ, false, ctx, state, info, bone );
	}


	// not sure if this is right.
	Player* player = reinterpret_cast< Player* >( g_csgo.m_entlist->GetClientEntity( info.m_index ) );

	if( !player )
		return true;

	if ( strstr( name, XOR( "models/player" ) ) && !player->dormant( ) ) {

		// is an enemy.
		if ( player->m_iTeamNum( ) != g_cl.m_local->m_iTeamNum( ) ) {

			if ( player->alive( ) ) {

				// on hit chams.
				if ( g_cfg[ XOR( "chams_enemy_hit" ) ].get< bool >( ) ) {
					g_chams.DrawShotRecord( player, ctx, state, info, bone );
				}

				// hidden
				if ( g_cfg[ XOR( "chams_enemy_hidden" ) ].get< bool >( ) ) {
					ApplyChams( XOR( "esp_chams_invis_enemies" ), XOR( "esp_chams_enemies_invis_color" ), g_cfg[ XOR ( "esp_chams_enemies_invis_color" ) ].get_color( ).a( ) / 255.f, MATERIAL_VAR_IGNOREZ, true, ctx, state, info, bone );
				
					// IMPORTANT: if we don't call the original this material will get overriden.
					g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >( IVModelRender::DRAWMODELEXECUTE )( g_csgo.m_model_render, ctx, state, info, bone );
				}

				// visible
				if ( g_cfg[ XOR( "chams_enemy" ) ].get< bool >( ) ) {
					ApplyChams( XOR( "esp_chams_visible_enemies" ), XOR( "esp_chams_enemies_color" ), g_cfg[ XOR( "esp_chams_enemies_color" ) ].get_color( ).a( ) / 255.f, MATERIAL_VAR_IGNOREZ, false, ctx, state, info, bone );
				}
			}

			// now we can do corpse chams.
			else {
				if ( g_cfg[ XOR( "chams_enemy_ragdoll" ) ].get< bool >( ) ) {
					ApplyChams( XOR( "esp_chams_corpse_enemies" ), XOR( "esp_chams_enemies_corpse_color" ), g_cfg[ XOR( "esp_chams_enemies_corpse_color" ) ].get_color( ).a( ) / 255.f, MATERIAL_VAR_IGNOREZ, false, ctx, state, info, bone );
				}
			}

		// do local player chams.
		} else if ( player == g_cl.m_local ) {

			if ( g_cfg[ XOR( "chams_desync" ) ].get< bool >( ) && !( g_csgo.m_globals->m_curtime < ( g_cl.m_local->m_flSpawnTime( ) + 0.5f ) ) ) {

				ApplyChams( XOR( "desyc_chams_material" ), XOR( "desync_chams_color" ), g_cfg[ XOR( "desync_chams_color" ) ].get_color( ).a( ) / 255.f, MATERIAL_VAR_IGNOREZ, false, ctx, state, info, bone );

				// manually draw the model.
				g_hooks.m_model_render.GetOldMethod< Hooks::DrawModelExecute_t >( IVModelRender::DRAWMODELEXECUTE )( g_csgo.m_model_render, ctx, state, info, g_cl.m_fake_matrix );

			}

			if ( g_cfg[ XOR( "chams_local" ) ].get< bool >( ) ) {

				ApplyChams( XOR( "local_chams_material" ), XOR( "local_chams_color" ), g_cfg[ XOR( "local_chams_color" ) ].get_color( ).a( ) / 255.f, MATERIAL_VAR_IGNOREZ, false, ctx, state, info, bone );

			} 

			if ( g_cl.m_local->m_bIsScoped( ) || g_cl.m_weapon_type == WEAPONTYPE_EQUIPMENT ) {
				g_csgo.m_render_view->SetBlend( g_cfg[ XOR( "chams_local" ) ].get< bool >( ) ? ( g_cfg[ XOR( "local_chams_color" ) ].get_color( ).a( ) / 255.f ) / 2.0f : 0.5f );
			}
		}
		
	}


	return true;
}

void Chams::SceneEnd( ) {
	
	if(!g_cl.m_processing)
		return;

	// restore.
	g_csgo.m_studio_render->ForcedMaterialOverride( nullptr );
	g_csgo.m_render_view->SetColorModulation( colors::white );
	g_csgo.m_render_view->SetBlend( 1.f );

	// store and sort ents by distance.
	if ( SortPlayers( ) ) {
		// iterate each player and render them.
		for ( const auto& p : m_players ) {
			// check if is an enemy.
			bool enemy = g_cl.m_local && p->enemy( g_cl.m_local );

			if ( enemy && g_cfg[ XOR( "chams_enemy_history" ) ].get< bool >( ) ) {
				RenderHistoryChams( p->index( ) );
			}
		}
	}

	// restore.
	g_csgo.m_studio_render->ForcedMaterialOverride( nullptr );
	g_csgo.m_render_view->SetColorModulation( colors::white );
	g_csgo.m_render_view->SetBlend( 1.f );
}

bool Chams::IsInViewPlane( const vec3_t& world ) {
	float w;

	const VMatrix& matrix = g_csgo.m_engine->WorldToScreenMatrix( );

	w = matrix[ 3 ][ 0 ] * world.x + matrix[ 3 ][ 1 ] * world.y + matrix[ 3 ][ 2 ] * world.z + matrix[ 3 ][ 3 ];

	return w > 0.001f;
}

bool Chams::SortPlayers( ) {
	// lambda-callback for std::sort.
	// to sort the players based on distance to the local-player.
	static auto distance_predicate = [ ] ( Entity* a, Entity* b ) {
		vec3_t local = g_cl.m_local->GetAbsOrigin( );

		// note - dex; using squared length to save out on sqrt calls, we don't care about it anyway.
		float len1 = ( a->GetAbsOrigin( ) - local ).length_sqr( );
		float len2 = ( b->GetAbsOrigin( ) - local ).length_sqr( );

		return len1 < len2;
	};

	// reset player container.
	m_players.clear( );

	// find all players that should be rendered.
	for( int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i ) {
		// get player ptr by idx.
		Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( i );

		// validate.
		if( !player || !player->IsPlayer( ) || !player->alive( ) || player->dormant( ) )
			continue;

		// do not draw players occluded by view plane.
		if( !IsInViewPlane( player->WorldSpaceCenter( ) ) )
			continue;

		m_players.push_back( player );
	}

	// any players?
	if( m_players.empty( ) )
		return false;

	// sorting fixes the weird weapon on back flickers.
	// and all the other problems regarding Z-layering in this shit game.
	std::sort( m_players.begin( ), m_players.end( ), distance_predicate );

	return true;
}

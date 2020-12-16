#include "undercover.h"

Listener g_listener{};;

void events::round_start( IGameEvent* evt ) { 
	// new round has started. no longer round end.
	g_cl.m_round_end = false;

	// remove notices.
	g_cl.m_round_flags = 0;

	// update skins.
//	g_skins.m_update = true;

	// reset bomb stuff.
	g_visuals.m_c4_planted = false;
	g_visuals.m_planted_c4 = nullptr;

	// reset dormant esp.
    g_visuals.m_draw.fill( false );
	g_visuals.m_opacities.fill( 0.f );
    g_visuals.m_offscreen_damage.fill( OffScreenDamageData_t() );

	// update all players.
	for( int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i ) {
		Player* player = g_csgo.m_entlist->GetClientEntity< Player* >( i );
		if( !player || player->m_bIsLocalPlayer( ) )
			continue;

		AimPlayer* data = &g_aimbot.m_players[ i - 1 ];
		data->OnRoundStart( player );
	}

	g_shots.m_hits.clear();
	g_shots.m_impacts.clear();
	g_shots.m_shots.clear();

	// clear origins.
	g_cl.m_net_pos.clear( );

	// buybot.
	//g_cl.Purchasebot( ); //fix

	// disable recharge choke.
	g_tickbase.m_shift_data.m_shift_time;

	g_tickbase.m_shift_data.m_round_start = true;
}

void events::round_end( IGameEvent* evt ) {
	if( !g_cl.m_local )
		return;

	// get the reason for the round end.
	int reason = evt->m_keys->FindKey( HASH( "reason" ) )->GetInt( );

	// reset.
	g_cl.m_round_end = false;

	if( g_cl.m_local->m_iTeamNum( ) == TEAM_COUNTERTERRORISTS && reason == CSRoundEndReason::CT_WIN )
		g_cl.m_round_end = true;

	else if( g_cl.m_local->m_iTeamNum( ) == TEAM_TERRORISTS && reason == CSRoundEndReason::T_WIN )
		g_cl.m_round_end = true;
}

void events::player_hurt( IGameEvent* evt ) {
	Player* attacker = nullptr;
	Player* victim = nullptr;

	// forward event to resolver / shots hurt processing.
	g_shots.OnHurt( evt );

    // offscreen esp damage stuff.
	{
        attacker = g_csgo.m_entlist->GetClientEntity< Player* >( g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "attacker" ) )->GetInt( ) ) );
        victim   = g_csgo.m_entlist->GetClientEntity< Player* >( g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "userid" ) )->GetInt( ) ) );

		if ( !attacker || !victim )
			return;

		if ( !attacker->IsPlayer( ) )
			return;

		player_info_t info;
		g_csgo.m_engine->GetPlayerInfo( attacker->index( ), &info );

		player_info_t victim_info;
		g_csgo.m_engine->GetPlayerInfo( victim->index( ), &victim_info );

		// if someone harmed us.
		if ( attacker != g_cl.m_local && victim == g_cl.m_local ) {
			std::string name{ std::string( info.m_name ).substr( 0, 24 ) };

			g_notify.add( tfm::format( XOR( "harmed by %s for %i in %s" ), name, evt->m_keys->FindKey( HASH( "dmg_health" ) )->GetInt( ), g_shots.m_groups[ evt->m_keys->FindKey( HASH( "hitgroup" ) )->GetInt( ) ] ) );
		}

		// if we attacked someone else.
		if ( attacker == g_cl.m_local && victim != g_cl.m_local ) {

			std::string name{ std::string( victim_info.m_name ).substr( 0, 24 ) };

			g_notify.add( tfm::format( XOR( "hurt %s for %i in %s" ), name, evt->m_keys->FindKey( HASH( "dmg_health" ) )->GetInt( ), g_shots.m_groups[ evt->m_keys->FindKey( HASH( "hitgroup" ) )->GetInt( ) ] ) );
			
			// hitsounds
			if ( g_cfg[ XOR( "misc_hitsound" ) ].get< bool >( ) ) {
				waveOutSetVolume( 0, 858993459 );
				PlaySoundA( hitsound_array, NULL, SND_ASYNC | SND_MEMORY );
			}

			//// get correct data from lagcomp.
			//const auto ideal = g_lagcompensation.GetLatestRecord( victim );

			//// safety checks.
			//if ( ideal.has_value( ) && ideal.value( )->m_pEntity )
			//{
			//	Chams::Hitdata_t d { };
			//	d.m_last_pose = ideal.value( )->center_matrix;
			//	d.time = g_csgo.m_globals->m_tick_count;
			//	d.ent = victim;
			//	g_chams.m_hits->push_back( d );
			//} 
		}
    }
}

void events::bullet_impact( IGameEvent* evt ) {
	int        attacker;

	// forward event to resolver impact processing.
	g_shots.OnImpact( evt );

	// decode impact coordinates and convert to vec3
	vec3_t pos = {
			evt->m_keys->FindKey( HASH( "x" ) )->GetFloat( ),
			evt->m_keys->FindKey( HASH( "y" ) )->GetFloat( ),
			evt->m_keys->FindKey( HASH( "z" ) )->GetFloat( )
	};

	// screw this.
	if ( !evt || !g_cl.m_local )
		return;

	// get attacker, if its not us, screw it.
	attacker = g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "userid" ) )->GetInt( ) );
	if ( attacker != g_csgo.m_engine->GetLocalPlayer( ) )
		return;

	g_movement.m_shot = true;

	if ( g_cfg[ XOR( "misc_bullet_impacts_server" ) ].get< bool >( ) ) {
		if( g_cfg[ XOR( "misc_bullet_impacts_server_override" ) ].get< bool >( ) )
			g_csgo.m_debug_overlay->AddBoxOverlay( pos, vec3_t( -2, -2, -2 ), vec3_t( 2, 2, 2 ), ang_t( 0, 0, 0 ), g_cfg[ XOR( "misc_bullet_impacts_server_color" ) ].get_color( ).r( ), g_cfg[ XOR( "misc_bullet_impacts_server_color" ) ].get_color( ).g( ), g_cfg[ XOR( "misc_bullet_impacts_server_color" ) ].get_color( ).b( ), g_cfg[ XOR( "misc_bullet_impacts_server_color" ) ].get_color( ).a( ), 3 );
		else
			g_csgo.m_debug_overlay->AddBoxOverlay ( pos, vec3_t( -2, -2, -2 ), vec3_t( 2, 2, 2 ), ang_t( 0, 0, 0 ), 255, 0, 0, 127, 3 );
	}
}

void events::item_purchase( IGameEvent* evt ) {
	int           team, purchaser;
	player_info_t info;

	if( !g_cl.m_local || !evt )
		return;

	if( !g_cfg[ XOR ( "misc_log_purchases" ) ].get< bool >( ) )
		return;

	// only log purchases of the enemy team.
	team = evt->m_keys->FindKey( HASH( "team" ) )->GetInt( );
	if( team == g_cl.m_local->m_iTeamNum( ) )
		return;

	// get the player that did the purchase.
	purchaser = g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "userid" ) )->GetInt( ) );

	// get player info of purchaser.
	if( !g_csgo.m_engine->GetPlayerInfo( purchaser, &info ) )
		return;

	std::string weapon = evt->m_keys->FindKey( HASH( "weapon" ) )->m_string;
	if( weapon == XOR( "weapon_unknown" ) )
		return;

	std::string out = tfm::format( XOR( "%s bought %s" ), std::string{ info.m_name }.substr( 0, 24 ), weapon );
	g_notify.add( out );
}

void events::player_death( IGameEvent* evt ) {
	// get index of player that died.
	int index = g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "userid" ) )->GetInt( ) );
	
	if ( index == 0 )
		return;

	// reset opacity scale.
	g_visuals.m_opacities[ index - 1 ] = 0.f;
	g_visuals.m_draw[ index - 1 ] = false;
}

void events::player_given_c4( IGameEvent* evt ) {
	player_info_t info;

	if( !g_cfg[ XOR( "misc_log_bomb" ) ].get< bool >( ) )
		return;

    // get the player who received the bomb.
	int index = g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "userid" ) )->GetInt( ) );
	if( index == g_csgo.m_engine->GetLocalPlayer( ) )
		return;

	if ( index <= 0 )
		return;

	if( !g_csgo.m_engine->GetPlayerInfo( index, &info ) )
		return;

	std::string out = tfm::format( XOR( "%s received the bomb" ), std::string{ info.m_name }.substr( 0, 24 ) );
	g_notify.add( out );
}

void events::bomb_beginplant( IGameEvent* evt ) {
	player_info_t info;

	if( !g_cfg[ XOR( "misc_log_bomb" ) ].get< bool >( ) )
		return; 

    // get the player who played the bomb.
	int index = g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "userid" ) )->GetInt( ) );
	if( index == g_csgo.m_engine->GetLocalPlayer( ) )
		return;

	if ( index <= 0 )
		return;

	// get player info of purchaser.
	if( !g_csgo.m_engine->GetPlayerInfo( index, &info ) )
		return;

	std::string out = tfm::format( XOR( "%s started planting the bomb" ), std::string{ info.m_name }.substr( 0, 24 ) );
	g_notify.add( out );
}

void events::bomb_abortplant( IGameEvent* evt ) {
	player_info_t info;

	if( !g_cfg[ XOR( "misc_log_bomb" ) ].get< bool >( ) )
		return;

	// get the player who stopped planting the bomb.
	int index = g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "userid" ) )->GetInt( ) );
	if( index == g_csgo.m_engine->GetLocalPlayer( ) )
		return;

	if ( index <= 0 )
		return;

	// get player info of purchaser.
	if( !g_csgo.m_engine->GetPlayerInfo( index, &info ) )
		return;

	std::string out = tfm::format( XOR( "%s stopped planting the bomb" ),  std::string{ info.m_name }.substr( 0, 24 ) );
	g_notify.add( out );
}

void events::bomb_planted( IGameEvent* evt ) {
    Entity        *bomb_target;
    std::string   site_name;
    int           player_index;
    player_info_t info;
    std::string   out;

    // get the func_bomb_target entity and store info about it.
    bomb_target = g_csgo.m_entlist->GetClientEntity( evt->m_keys->FindKey( HASH( "site" ) )->GetInt( ) );
    if( bomb_target ) {
        site_name                 = bomb_target->GetBombsiteName( );
        g_visuals.m_last_bombsite = site_name;
    }

	if( !g_cfg[ XOR( "misc_log_bomb" ) ].get< bool >( ) )
		return;

	player_index = g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "userid" ) )->GetInt( ) );
    if( player_index == g_csgo.m_engine->GetLocalPlayer( ) )
        out = tfm::format( XOR( "you planted the bomb at %s" ), site_name.c_str( ) );

    else {
		if (player_index <= 0)
			return;

        g_csgo.m_engine->GetPlayerInfo( player_index, &info );

        out = tfm::format( XOR( "the bomb was planted by %s" ), /*site_name.c_str( ),*/ std::string( info.m_name ).substr( 0, 24 ) );
    }

	g_notify.add( out );

	// we have a bomb ent already, don't do anything else.
	if ( g_visuals.m_c4_planted )
		return;

	g_visuals.m_c4_planted = true;

	// todo - dex;  get this radius dynamically... seems to only be available in map bsp file, search string: "info_map_parameters"
	//              info_map_parameters is an entity created on the server, it doesnt seem to have many useful networked vars for clients.
	//
	//              swapping maps between de_dust2 and de_nuke and scanning for 500 and 400 float values will leave you one value.
	//              need to figure out where it's written from.
	//
	// server.dll uses starting 'radius' as damage... the real radius passed to CCSGameRules::RadiusDamage is actually multiplied by 3.5.
	g_visuals.m_planted_c4_damage = 500.f;
	g_visuals.m_planted_c4_radius = g_visuals.m_planted_c4_damage * 3.5f;
	g_visuals.m_planted_c4_radius_scaled = g_visuals.m_planted_c4_radius / 3.f;
}

void events::bomb_begindefuse( IGameEvent* evt ) {
	player_info_t info;

	if( !g_cfg[ XOR( "misc_log_defuse" ) ].get< bool >( ) )
		return;

	// get index of player that started defusing the bomb.
	int index = g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "userid" ) )->GetInt( ) );
	if( index == g_csgo.m_engine->GetLocalPlayer( ) )
		return;

	if (index <= 0)
		return;


	if( !g_csgo.m_engine->GetPlayerInfo( index, &info ) )
		return;

	bool kit = evt->m_keys->FindKey( HASH( "haskit" ) )->GetBool( );

	if( kit ) {
		std::string out = tfm::format( XOR( "%s started defusing with a kit" ), std::string( info.m_name ).substr( 0, 24 ) );
		g_notify.add( out );
	}

	else {
		std::string out = tfm::format( XOR( "%s started defusing without a kit" ), std::string( info.m_name ).substr( 0, 24 ) );
		g_notify.add( out );
	}
}

void events::bomb_abortdefuse( IGameEvent* evt ) {
	player_info_t info;

	if( !g_cfg[XOR("misc_log_defuse")].get<bool>())
		return;

	// get index of player that stopped defusing the bomb.
	int index = g_csgo.m_engine->GetPlayerForUserID( evt->m_keys->FindKey( HASH( "userid" ) )->GetInt( ) );
	if( index == g_csgo.m_engine->GetLocalPlayer( ) )
		return;

	if (index <= 0)
		return;


	if( !g_csgo.m_engine->GetPlayerInfo( index, &info ) )
		return;

	std::string out = tfm::format( XOR( "%s stopped defusing" ), std::string( info.m_name ).substr( 0, 24 ) );
	g_notify.add( out );
}

void events::bomb_exploded( IGameEvent* evt ) {
    g_visuals.m_c4_planted = false;
    g_visuals.m_planted_c4 = nullptr;
}

void events::weapon_fire( IGameEvent* evt )
{
	g_shots.OnFire( evt );
}

void events::bomb_defused( IGameEvent* evt ) {
    g_visuals.m_c4_planted = false;
    g_visuals.m_planted_c4 = nullptr;
}

void events::round_freeze_end( IGameEvent* evt ) {
	g_cl.m_round_flags = 1;
}

void events::client_disconnect( IGameEvent* evt ) {
	g_cl.m_round_flags = 0;
}

void Listener::init( ) {
	// link events with callbacks.
	add( XOR( "weapon_fire"), events::weapon_fire);
	add( XOR( "round_start" ), events::round_start );
	add( XOR( "round_end" ), events::round_end );
	add( XOR( "player_hurt" ), events::player_hurt );
	add( XOR( "bullet_impact" ), events::bullet_impact );
	add( XOR( "item_purchase" ), events::item_purchase );
	add( XOR( "player_death" ), events::player_death );
	add( XOR( "player_given_c4" ), events::player_given_c4 );
	add( XOR( "bomb_beginplant" ), events::bomb_beginplant );
	add( XOR( "bomb_abortplant" ), events::bomb_abortplant );
	add( XOR( "bomb_planted" ), events::bomb_planted );
	add( XOR( "bomb_begindefuse" ), events::bomb_begindefuse );
	add( XOR( "bomb_abortdefuse" ), events::bomb_abortdefuse );
    add( XOR( "bomb_exploded" ), events::bomb_exploded );
    add( XOR( "bomb_defused" ), events::bomb_defused );
    add( XOR( "round_freeze_end" ), events::round_freeze_end );
    add( XOR( "client_disconnect" ), events::client_disconnect );

	register_events( );
}
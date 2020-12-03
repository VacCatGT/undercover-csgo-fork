#include "undercover.h"
#include "niggahoe.h"

Menu g_menu{ };
void Menu::SetupTextures( IDirect3DDevice9* device ) {

	if (undercover_top_left_line == nullptr)
		D3DXCreateTextureFromFileInMemoryEx( device , bitchass , sizeof( bitchass ) , 239 , 33 , D3DX_DEFAULT , D3DUSAGE_DYNAMIC , D3DFMT_UNKNOWN , D3DPOOL_DEFAULT , D3DX_DEFAULT , D3DX_DEFAULT , 0 , NULL , NULL , &undercover_top_left_line );

	if (undercover_logo == nullptr)
		D3DXCreateTextureFromFileInMemoryEx( device , undercover_raw_data , sizeof( undercover_raw_data ) , 58 , 46 , D3DX_DEFAULT , D3DUSAGE_DYNAMIC , D3DFMT_UNKNOWN , D3DPOOL_DEFAULT , D3DX_DEFAULT , D3DX_DEFAULT , 0 , NULL , NULL , &undercover_logo );

	if (undercover_groupbox == nullptr)
		D3DXCreateTextureFromFileInMemoryEx( device , niggafuck , sizeof( niggafuck ) , 211 , 325 , D3DX_DEFAULT , D3DUSAGE_DYNAMIC , D3DFMT_UNKNOWN , D3DPOOL_DEFAULT , D3DX_DEFAULT , D3DX_DEFAULT , 0 , NULL , NULL , &undercover_groupbox );

	if (undercover_background == nullptr)
		D3DXCreateTextureFromFileInMemoryEx( device , background_raw_data , sizeof( background_raw_data ) , 500 , 500 , D3DX_DEFAULT , D3DUSAGE_DYNAMIC , D3DFMT_UNKNOWN , D3DPOOL_DEFAULT , D3DX_DEFAULT , D3DX_DEFAULT , 0 , NULL , NULL , &undercover_background );
}
void Menu::Think( IDirect3DDevice9* device ) {

	if ( m_opened ) {
		g_frw.StartWindow( ); {

			g_frw.DrawBackdrop( );

			switch ( g_frw.RenderTabBar( m_curtab, { XOR( "RAGEBOT" ), XOR( "VISUALS" ), XOR( "MISC" ), XOR( "CONFIG" ) } ) )
			{

			case tabs::tab_one: {
				static int current_subtab;

				switch ( g_frw.RenderSecondaryHeader( current_subtab, { XOR( "GENERAL" ), XOR( "ANTI-AIM" ) } ) )
				{

				case 0: {
					g_frw.BeginModuleHeader( );

					g_frw.RenderModule( positions::left_top, sizes::full, XOR( "AIMBOT_MAIN" ) ); {
						g_frw.Checkbox( XOR( "Enable Aimbot" ), XOR( "aimbot_enable" ) );
						g_frw.Checkbox( XOR( "Auto Shoot" ), XOR( "aimbot_autofire" ) );
						g_frw.Checkbox( XOR( "Silent" ), XOR( "aimbot_silent" ) );
						g_frw.Checkbox( XOR( "Resolver" ), XOR( "aimbot_resolver" ) );
						g_frw.Checkbox( XOR( "Pitch Resolver" ), XOR( "aimbot_pitch_resolver" ) );
						//g_frw.Checkbox( XOR( "Taser Bot" ), XOR( "aimbot_zeusbot" ) );
						//g_frw.Checkbox( XOR( "Extend Backtrack Window" ), XOR( "aimbot_extended_bt" ) );
						g_frw.KeybindCheckbox( XOR( "Override Hitboxes" ) );
						g_frw.GetKey( XOR( "override_hitbox" ), XOR( "hitbox_override_key" ), XOR( "hitbox_override_key_type" ) );
						g_frw.KeybindCheckbox( XOR( "Force Safepoint" ) );
						g_frw.GetKey( XOR( "safepoint" ), XOR( "safepoint_key" ), XOR( "safepoint_key_type" ) );
						g_frw.KeybindCheckbox( XOR( "Override Damage" ) );
						g_frw.GetKey( XOR( "override_min_dmg" ), XOR( "override_min_dmg_key" ), XOR( "override_min_dmg_type" ) );
						g_frw.Checkbox(XOR("Enable Exploits"), XOR("aimbot_exploits_enable"));
						g_frw.Checkbox(XOR("Doubletap"), XOR("aimbot_exploits_teleport"));
						//g_frw.NumberPicker( XOR( "Rapid fire hitchance" ), XOR( "rapid_hc" ), 0, 100, XOR( "%0.f" ), 1 );
						g_frw.Checkbox(XOR("Hide Shots"), XOR("aimbot_hide_shots"));
						g_frw.KeybindCheckbox(XOR("Silence Exploits"));
						g_frw.GetKey(XOR("exploit_disable"), XOR("aimbot_disable_exploits_key"), XOR("aimbot_disable_exploits_key_type"));
						//g_frw.MultiCombo( XOR( "Performance" ), { XOR( "aimbot_performance_bt" ) }, { XOR( "Limited Backtracking" ) } );
					} g_frw.EndModule( );

					g_frw.ContinueModule( );

					g_frw.RenderModule( positions::right_top, sizes::full, XOR( "AIMBOT_CONFIG" ) ); {
						std::string weapon_name, weapon_cfg_name;
						g_frw.Checkbox( XOR( "Adaptive Weapon Config" ), XOR( "aimbot_adaptive_config" ) );
						if ( g_cfg[ XOR( "aimbot_adaptive_config" ) ].get<bool>( ) )
							g_frw.ComboBox( XOR( "Selected Weapon" ), XOR( "aimbot_selected_weapon" ), XOR( "Pistols\0Heavy Pistols\0Scout\0Autosniper\0AWP\0Other" ), 6 );

						if ( g_cfg[ XOR( "aimbot_adaptive_config" ) ].get<bool>( ) ) {
							switch ( g_cfg[ XOR( "aimbot_selected_weapon" ) ].get<int>( ) ) {
							case 0: weapon_name = XOR( "Pistols" ); weapon_cfg_name = XOR( "pistol" ); break;
							case 1: weapon_name = XOR( "Heavy Pistols" ); weapon_cfg_name = XOR( "r8" ); break;
							case 2: weapon_name = XOR( "Scout" );  weapon_cfg_name = XOR( "ssg08" ); break;
							case 3: weapon_name = XOR( "Autosniper" ); weapon_cfg_name = XOR( "auto" );  break;
							case 4: weapon_name = XOR( "AWP" );  weapon_cfg_name = XOR( "awp" ); break;
							case 5: weapon_name = XOR( "Other" );  weapon_cfg_name = XOR( "other" ); break;
							}
						}
						else {
							weapon_name = XOR( "General" );
							weapon_cfg_name = XOR( "gen" );
						}
						g_frw.NumberPicker( XOR( "Minimum Hitchance" ), XOR( "aimbot_" + weapon_cfg_name + XOR( "_hc" ) ), 0, 100, XOR( "%0.f" ), 1 );
						g_frw.NumberPicker( XOR( "Minimum Damage" ), XOR( "aimbot_" + weapon_cfg_name + XOR( "_min_dmg" ) ), 0, 120, XOR( "%0.f" ), 1 );
						g_frw.NumberPicker( XOR( "Minimum Overriden Damage" ), XOR( "aimbot_" + weapon_cfg_name + XOR( "_overriden_min_dmg" ) ), 0, 120, XOR( "%0.f" ), 1 );
						g_frw.Checkbox( XOR( "Auto Scope" ), XOR( "aimbot_" + weapon_cfg_name + XOR( "_autoscope" ) ) );
						g_frw.MultiCombo( XOR( "Auto Stop" ), { XOR( "aimbot_" + weapon_cfg_name + XOR( "_autostop_between" ) ), XOR( "aimbot_" + weapon_cfg_name + XOR( "_autostop_air" ) ), XOR( "aimbot_" + weapon_cfg_name + XOR( "_autostop_force" ) ) }, { XOR( "Between Shots" ), XOR( "In Air" ), XOR( "Force Accuracy" ) } );

						//if ( weapon_name == XOR( "Scout" ) )
						//	g_frw.Checkbox( XOR( "Jumpscout" ), XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_jumpscout" ) );
						//g_frw.Checkbox( XOR( "Stop Between Shots" ), XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_between_shots" ) );
						//g_frw.NumberPicker( XOR( "Head Point Scale" ), XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_headscale" ), 0, 100, XOR( "%0.f" ), 1 );
						//g_frw.NumberPicker( XOR( "Body Point Scale" ), XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_bodyscale" ), 0, 100, XOR( "%0.f" ), 1 );
						//g_frw.Checkbox( XOR( "Ignore Limbs when Moving" ), XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_no_legs" ) );

						g_frw.MultiCombo( XOR( "Hitboxes" ), { XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_hitbox_head" ), std::string( XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_hitbox_upper_body" ) ),
							XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_hitbox_lower_body" ), XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_hitbox_stomach" ),
							XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_hitbox_legs" ), XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_hitbox_feet" ) },
							{ XOR( "Head" ), XOR( "Neck" ), XOR( "Body" ), XOR( "Stomach" ), XOR( "Legs" ), XOR( "Feet" ) } );

						g_frw.MultiCombo( XOR( "Multipoint Hitboxes" ), { XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_mutlipoint_hitbox_head" ), std::string( XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_mutlipoint_hitbox_upper_body" ) ),
							XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_mutlipoint_hitbox_lower_body" ), XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_mutlipoint_hitbox_stomach" ),
							XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_mutlipoint_hitbox_legs" ), std::string( "aimbot_" + weapon_cfg_name + XOR( "_mutlipoint_hitbox_feet" ) ) },
							{ XOR( "Head" ), XOR( "Neck" ), XOR( "Body" ), XOR( "Stomach" ), XOR( "Legs" ), XOR( "Feet" ) } );

						g_frw.ComboBox( XOR( "Priority Hitbox" ), XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_priority_hitbox" ), XOR( "Head\0Neck\0Body\0Stomach\0Legs\0Feet" ), 6 );

						g_frw.MultiCombo( XOR( "Overriden Hitboxes" ), { std::string( XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_overriden_hitbox_head" ) ), std::string( XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_overriden_hitbox_upper_body" ) ),
							std::string( XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_overriden_hitbox_lower_body" ) ), std::string( XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_overriden_hitbox_stomach" ) ),
							std::string( XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_overriden_hitbox_legs" ) ), std::string( XOR( "aimbot_" ) + weapon_cfg_name + XOR( "_overriden_hitbox_feet" ) ) },
							{ XOR( "Head" ), XOR( "Neck" ), XOR( "Body" ), XOR( "Stomach" ), XOR( "Legs" ), XOR( "Feet" ) } );

						g_frw.Checkbox( XOR( "Body Aim In Air" ), XOR( "aimbot_" + weapon_cfg_name + XOR( "body_in_air" ) ) );
						g_frw.Checkbox( XOR( "Body Aim On Crouch" ), XOR( "aimbot_" + weapon_cfg_name + XOR( "body_on_crouch" ) ) );
						g_frw.Checkbox( XOR( "Body Aim If Lethal" ), XOR( "aimbot_" + weapon_cfg_name + XOR( "body_lethal" ) ) );
						g_frw.Checkbox( XOR( "Body Aim If Lethal x2" ), XOR( "aimbot_" + weapon_cfg_name + XOR( "body_lethal2" ) ) );

					} g_frw.EndModule( );



					g_frw.ConcludeModuleHeader( );

				}break;
				case 1: {
					g_frw.BeginModuleHeader( );

					g_frw.RenderModule( positions::left_top, sizes::full, XOR( "ANTIAIM_REAL" ) ); {
						g_frw.Checkbox( XOR( "Enabled" ), XOR( "aa_enabled" ) );
						g_frw.Checkbox( XOR( "At Targets" ), XOR( "aa_at_targets" ) );
						g_frw.Checkbox( XOR( "Auto Direction" ), XOR( "aa_freestand" ) );
						g_frw.NumberPicker( XOR( "Yaw Offset" ), XOR( "aa_yaw_offset" ), -180, 180, XOR( "%0.f" ), 1 );
						g_frw.KeybindCheckbox( XOR( "Jitter" ) );
						g_frw.GetKey( XOR( "jitterkey" ), XOR( "aa_jitter_key" ), XOR( "aa_jitter_key_type" ) );
						g_frw.NumberPicker( XOR( "Jitter Offset" ), XOR( "aa_jitter_offset" ), -180, 180, XOR( "%0.f" ), 1 );
						g_frw.Checkbox(XOR("Fake Enable"), XOR("aa_fake"));
						g_frw.ComboBox(XOR("Air mode"), XOR("aa_air_mode"), XOR("Normal\0Spin"), 2);
						g_frw.ComboBox(XOR("LBY mode"), XOR("aa_lby_mode"), XOR("Normal\0Opposite\0Sway"), 3);
						g_frw.Checkbox(XOR("Fake Desync"), XOR("aa_fake_desync"));
						g_frw.KeybindCheckbox(XOR("Inverter"));
						g_frw.GetKey(XOR("interterkey"), XOR("aa_inverter_key"), XOR("aa_inverter_key_type"));
						g_frw.MultiCombo(XOR("Inverter Flip"), { XOR("invertflip_walking"), XOR("invertflip_moving"), XOR("invertflip_air") }, { XOR("Walk"), XOR("Run"), XOR("In Air") });
					}g_frw.EndModule( );


					g_frw.ContinueModule( );

					g_frw.RenderModule( positions::right_top, sizes::full, XOR( "ANTIAIM_LAG" ) ); {
						g_frw.Checkbox( XOR( "Enable Fake-Lag" ), XOR( "aa_fakelag" ) );
						g_frw.NumberPicker( XOR( "Limit" ), XOR( "aa_factor" ), 0, 14, XOR( "%0.f" ), 1 );
						g_frw.NumberPicker( XOR( "Jitter" ), XOR( "aa_fl_jitter" ), 0, 100, XOR( "%0.f" ), 1 );
						g_frw.MultiCombo( XOR( "Triggers" ),
							{ XOR( "aa_fl_in_air" ), XOR( "aa_fl_on_peek" ), XOR( "aa_fl_on_shot" ), XOR( "aa_fl_on_land" ), XOR( "aa_fl_while_reload" ), XOR( "aa_fl_on_wep_switch" ), XOR( "aa_fl_on_velocity_change" ), XOR( "aa_fl_break_lagcomp" ) },
							{ XOR( "In Air" ), XOR( "On Peek" ), XOR( "On Shot" ), XOR( "On Land" ), XOR( "While Reloading" ), XOR( "On Weapon Switch" ), XOR( "On Velocity Change" ), XOR( "Break Lag Compensation" ) } );
						g_frw.NumberPicker( XOR( "Trigger Limit" ), XOR( "aa_fl_triggers_amt" ), 0, 14, XOR( "%0.f" ), 1 );
						g_frw.KeybindCheckbox( "Controlled walk" );
						g_frw.GetKey( XOR( "Slowwalk" ), XOR( "misc_slowwalk_bind" ), XOR( "misc_slowwalk_bind_type" ) );
						g_frw.ComboBox( XOR( "Walk Type" ), XOR( "aa_slowwalk_type" ), XOR( "Accurate\0Slide Fast\0Slide Slow" ), 4 );
						g_frw.KeybindCheckbox( XOR( "Duck exploit" ) );
						g_frw.GetKey( XOR( "Duck Exploit_Bind" ), XOR( "misc_fakeduck_key" ), XOR( "misc_fakeduck_key_type" ) );

					}g_frw.EndModule( );

					g_frw.ConcludeModuleHeader( );
				}break;
				};
			}break;

			case tabs::tab_two: {
				static int current_subtab;
				switch ( g_frw.RenderSecondaryHeader( current_subtab, { XOR( "PLAYER" ), XOR( "OTHER" ), XOR( "COSMETICS" ), XOR("LOCAL") } ) )
				{
				case 0: 
					g_frw.BeginModuleHeader( );
					g_frw.RenderModule( positions::left_top, sizes::full, XOR( "PLAYERS" ) ); {
						g_frw.Checkbox( XOR( "Bounding Box##Enemy" ), XOR( "esp_enemies_box" ) );
						g_frw.ColorPicker( XOR( "Box color##Enemy" ), XOR( "esp_enemies_box_color" ) );
						g_frw.Checkbox( XOR( "Name##Enemy" ), XOR( "esp_enemies_name" ) );
						g_frw.ColorPicker( XOR( "Name color##Enemy" ), XOR( "esp_enemies_name_color" ) );
						g_frw.Checkbox( XOR( "Health##Enemy" ), XOR( "esp_enemies_health" ) );
						if ( g_cfg[ XOR( "esp_enemies_health" ) ].get<bool>( ) ) {
							g_frw.Checkbox( XOR( "Override Health Color##Enemy" ), XOR( "esp_enemies_health_override" ) );
							g_frw.ColorPicker( XOR( "Health Color" ), XOR( "esp_enemies_health_color" ) );
						}
						g_frw.Checkbox( XOR( "Weapon##Enemy" ), XOR( "esp_enemies_weapon" ) );
						g_frw.ColorPicker( XOR( "Weapon color##Enemy" ), XOR( "esp_enemies_weapon_color" ) );
						g_frw.Checkbox( XOR( "Ammo bar##Enemy" ), XOR( "esp_enemies_ammo" ) );
						g_frw.ColorPicker( XOR( "Ammo color##Enemy" ), XOR( "esp_enemies_ammo_color" ) );

						g_frw.Checkbox( XOR( "Bones##Enemy" ), XOR( "esp_enemies_skeleton" ) );
						g_frw.ColorPicker( XOR( "Bone color##Enemy" ), XOR( "esp_enemies_skeleton_color" ) );

						g_frw.Checkbox( XOR( "Glow##Enemy" ), XOR( "esp_enemy_glow" ) );
						if ( !g_cfg[ XOR( "esp_enemy_glow_healthcolor" ) ].get<bool>( ) ) {
							g_frw.ColorPicker( XOR( "Glow color##Enemy" ), XOR( "esp_enemy_glow_color" ) );
						}
						if ( g_cfg[ XOR( "esp_enemy_glow" ) ].get<bool>( ) )
							g_frw.Checkbox( XOR( "Glow health color##Enemy" ), XOR( "esp_enemy_glow_healthcolor" ) );

						//g_frw.NumberPicker( XOR( "Fade time" ), XOR( "esp_enemies_dormant_time" ), 0, 100, XOR( "%0.f" ), 1 );

						g_frw.Checkbox( XOR( "Hitsound" ), XOR( "misc_hitsound" ) );

						g_frw.Checkbox(XOR("Print animlayers"), XOR("misc_animlayer"));

						g_frw.NumberPicker( XOR( "Visual Interpolation" ), XOR( "visuals_vis_interpolation_amt" ), 0, 14, XOR( "%0.f" ), 1 );
						g_frw.Checkbox( XOR( "Only Interpolate When Dead" ), XOR( "visuals_vis_interpolation_dead" ) );


						//g_frw.Checkbox( XOR( "OOF arrows" ), XOR( "idk" ) );
						//g_frw.ColorPicker( XOR( "OOF color##Enemy" ), XOR( "esp_enemies_offscreen_color" ) );

						//g_frw.ComboBox( XOR( "Arrow Type" ), XOR( "esp_enemies_offscreen" ), XOR( "Disabled\0Outlined\0Filled" ), 4 );
						//if ( g_cfg[ XOR( "esp_enemies_offscreen" ) ].get<bool>( ) ) {
						//	g_frw.NumberPicker( XOR( "Radius" ), XOR( "esp_enemies_offscreen_radius" ), 0, 100, XOR( "%0.f" ), 1 );
						//	g_frw.NumberPicker( XOR( "Size" ), XOR( "esp_enemies_offscreen_size" ), 0, 15, XOR( "%0.f" ), 1 );
						//}


					} g_frw.EndModule( );

					g_frw.ContinueModule( );

					g_frw.RenderModule( positions::right_top, sizes::full, XOR( "COLORED_MODELS" ) ); {
						g_frw.Checkbox( XOR( "Enemies visible chams" ), XOR( "chams_enemy" ) );
						g_frw.ColorPicker( XOR( "Visible color" ), XOR( "esp_chams_enemies_color" ) );
						g_frw.ComboBox( XOR( "Material" ), XOR( "esp_chams_visible_enemies" ), XOR( "Disabled\0Unlit\0Lit\0Silhouette" ), 4 );
						g_frw.Checkbox( XOR( "Enemies hidden chams" ), XOR( "chams_enemy_hidden" ) );
						g_frw.ColorPicker( XOR( "Invisible color" ), XOR( "esp_chams_enemies_invis_color" ) );
						g_frw.ComboBox( XOR( "Hidden Material" ), XOR( "esp_chams_invis_enemies" ), XOR( "Disabled\0Unlit\0Lit\0Silhouette" ), 4 );
						g_frw.Checkbox( XOR( "Enemies history chams" ), XOR( "chams_enemy_history" ) );
						g_frw.ColorPicker( XOR( "History color" ), XOR( "esp_chams_enemies_history_color" ) );
						g_frw.ComboBox( XOR( "History Material" ), XOR( "esp_chams_history_enemies" ), XOR( "Disabled\0Unlit\0Lit\0Silhouette" ), 4 );
						g_frw.Checkbox( XOR( "Enemies corpse chams" ), XOR( "chams_enemy_ragdoll" ) );
						g_frw.ColorPicker( XOR( "Corpse color" ), XOR( "esp_chams_enemies_corpse_color" ) );
						g_frw.ComboBox( XOR( "Corpse Material" ), XOR( "esp_chams_corpse_enemies" ), XOR( "Disabled\0Unlit\0Lit\0Silhouette" ), 4 );
						g_frw.Checkbox( XOR( "Enemies wounded chams" ), XOR( "chams_enemy_hit" ) );
						g_frw.ColorPicker( XOR( "Wounded color" ), XOR( "esp_chams_enemies_wounded_color" ) );
						g_frw.ComboBox( XOR( "Wounded Material" ), XOR( "esp_chams_wounded_enemies" ), XOR( "Disabled\0Unlit\0Lit\0Silhouette" ), 4 );
					} g_frw.EndModule( );


					g_frw.ConcludeModuleHeader( );

					break;
				case 1:

					g_frw.BeginModuleHeader( );

					g_frw.RenderModule( positions::left_top, sizes::full, XOR( "WORLD" ) ); {
						g_frw.Checkbox( XOR( "Night mode" ), XOR( "misc_world_night" ) );
						g_frw.NumberPicker( XOR( "Darkness amount" ), XOR( "misc_world_night_darkness" ), 0, 95, XOR( "%0.f" ), 1 );
						g_frw.NumberPicker( XOR( "Alpha modulate props" ), XOR( "misc_world_prop_opacity" ), 0, 100, XOR( "%0.f" ), 1 );
						g_frw.Checkbox( XOR( "Enable Fog" ), XOR( "fog_enable" ) );
						g_frw.ColorPicker( XOR( "Fog color" ), XOR( "fog_color" ) );
						g_frw.NumberPicker( XOR( "Fog Amount" ), XOR( "fog_density" ), 0, 100, XOR( "%0.f" ), 1 );
						g_frw.NumberPicker( XOR( "Fog Length" ), XOR( "fog_length" ), 0, 2500, XOR( "%0.f" ), 1 );
						g_frw.Checkbox( XOR( "Full bright" ), XOR( "misc_world_fullbright" ) );
						g_frw.ComboBox( XOR( "Sky box" ), XOR( "misc_world_skybox" ), XOR( "Default\0Night01\0Night02\0Cloudy\0Clear\0Dawn" ), 4 );
						g_frw.Checkbox( XOR( "Predict grenades" ), XOR( "visuals_misc_grenade_prediction" ) );
						g_frw.NumberPicker( XOR( "Ragdoll thrust" ), XOR( "visuals_misc_ragdoll_thrust" ), 0, 1000, XOR( "%0.f" ), 1 );
						g_frw.Checkbox( XOR( "Fire proximity" ), XOR( "misc_world_fire_proximity" ) );
						g_frw.ColorPicker( XOR( "Fire proximity color" ), XOR( "misc_world_fire_proximity_color" ) );
						g_frw.Checkbox( XOR( "Smoke proximity" ), XOR( "misc_world_smoke_proximity" ) );
						g_frw.ColorPicker( XOR( "Smoke proximity color" ), XOR( "misc_world_smoke_proximity_color" ) );
						g_frw.Checkbox( XOR( "Force Low Violence Mode" ), XOR( "misc_low_violence" ) );


					} g_frw.EndModule( );

					g_frw.ContinueModule( );

					g_frw.RenderModule( positions::right_top, sizes::full, XOR( "OTHERS" ) ); {
						g_frw.NumberPicker( XOR( "Override FOV" ), XOR( "misc_override_fov_amount" ), 80, 120, XOR( "%0.f" ), 0.5 );
						g_frw.NumberPicker( XOR( "Viewmodel fov" ), XOR( "misc_viewmodel_fov" ), 60, 90, XOR( "%0.f" ), 0.5 );
						g_frw.NumberPicker( XOR( "Viewmodel yaw" ), XOR( "misc_viewmodel_x" ), -10, 10, XOR( "%0.f" ), 0.5 );
						g_frw.NumberPicker( XOR( "Viewmodel pitch" ), XOR( "misc_viewmodel_y" ), -10, 10, XOR( "%0.f" ), 0.5 );
						g_frw.NumberPicker( XOR( "Viewmodel roll" ), XOR( "misc_viewmodel_z" ), -10, 10, XOR( "%0.f" ), 0.5 ); 
						g_frw.KeybindCheckbox( XOR( "Zoom" ) );
						g_frw.GetKey( XOR( "zoom_key_enable" ), XOR( "misc_zoom_key" ), XOR( "misc_zoom_key_type" ) );
						g_frw.NumberPicker( XOR( "Zoom Amount" ), XOR( "misc_zoom_fov" ), 0, 100, XOR( "%0.f" ), 1 );
						g_frw.KeybindCheckbox( XOR( "Force thirdperson" ) );
						g_frw.GetKey( XOR( "misc_thirdperson_enable" ), XOR( "misc_thirdperson_key" ), XOR( "misc_thirdperson_key_type" ) );
						g_frw.NumberPicker( XOR( "Thirdperson distance" ), XOR( "misc_thirdperson_distance" ), 80, 150, XOR( "%0.f" ), 1 );
						g_frw.Checkbox( XOR( "Preserve killfeed" ), XOR( "misc_preserve_killfeed" ) );
						g_frw.Checkbox( XOR( "Show spectators" ), XOR( "misc_spectators" ) );
						g_frw.Checkbox( XOR( "Show Active Keybinds" ), XOR( "misc_keybinds" ) );
						g_frw.Checkbox( XOR( "Draw watermark" ), XOR( "misc_watermark" ) );
						g_frw.Checkbox( XOR( "Damage indicator" ), XOR( "misc_damage_indicator" ) );
						g_frw.Checkbox( XOR( "Autowall crosshair" ), XOR( "visuals_misc_penetration_crosshair" ) );

						g_frw.MultiCombo( "Removals", { XOR( "misc_override_fov_scoped" ), XOR( "visuals_misc_remove_scope" ), XOR( "visuals_misc_remove_smoke" ), XOR( "visuals_misc_remove_flash" ), XOR( "visuals_misc_remove_visual_recoil" ), XOR( "visuals_misc_remove_fog" ), XOR( "visuals_misc_remove_magazines" ), XOR( "visuals_misc_remove_view_bob" ), XOR( "visuals_misc_remove_entity_shadows" ), XOR( "visuals_misc_remove_sleeves" ) }, { XOR( "Zoom" ), XOR( "Scope" ), XOR( "Smoke" ), XOR( "Flash" ), XOR( "Visual recoil" ), XOR( "Post-processing" ), XOR( "Magazines" ), XOR( "View Bob" ), XOR( "Entity Shadows" ), XOR( "Sleeves" ) } );
						g_frw.Checkbox( XOR( "Force crosshair" ), XOR( "misc_force_crosshair" ) );
						g_frw.MultiCombo( XOR( "Bullet Impacts" ), { XOR( "misc_bullet_impacts_client" ), XOR( "misc_bullet_impacts_server" ) }, { XOR( "Client" ), XOR( "Server" ) } );
						g_frw.Checkbox( XOR( "Override client color" ), XOR( "misc_bullet_impacts_client_override" ) );
						g_frw.ColorPicker( XOR( "Client Color" ), XOR( "misc_bullet_impacts_client_color" ) );
						g_frw.Checkbox( XOR( "Override server color" ), XOR( "misc_bullet_impacts_server_override" ) );
						g_frw.ColorPicker( XOR( "Server Color" ), XOR( "misc_bullet_impacts_server_color" ) );
						g_frw.Checkbox( XOR( "Bullet beams" ), XOR( "visuals_misc_bullet_beam" ) );
						g_frw.ColorPicker( XOR( "Bullet beam color" ), XOR( "visuals_misc_bullet_beam_color" ) );

					} g_frw.EndModule( );
					g_frw.ConcludeModuleHeader( );
					break;

				case 2:
					g_frw.BeginModuleHeader( );

					//g_frw.RenderModule( positions::left_top, sizes::half, XOR( "Cosmetics" ) ); {
					//	g_frw.ComboBox( XOR( "Knife model" ), XOR( "skins_knife" ), XOR( "Disabled\0Bayonet\0Bowie\0Butterfly\0Falchion\0Flip knife\0Gut knife\0Huntsman\0Karambit\0M9 Bayonet\0Shadow Daggers\0CSS\0Paracord Knife\0Survival Knife\0Ursus knife\0Navaja Knife\0Nomad Knife\0Stiletto Knife\0Talon Knife\0Skeleton Knife" ), 8 );

					//	g_frw.Button( XOR( "Refresh" ), &g_skins.m_update );

					//} g_frw.EndModule( );

					//g_frw.RenderModule( positions::left_bottom, sizes::half, XOR( " " ) ); {
					//	g_frw.ComboBox( XOR( "Glove model" ), XOR( "skins_gloves" ), XOR( "Disabled\0Bloodhound\0Sport\0Driver\0Handwraps\0Motorcycle\0Specialist" ), 6 );

					//	if ( !g_skins.m_glove_kits.empty( ) ) {
					//		g_frw.ListBox( XOR( "Glove skin" ), XOR( "skins_glove_kit" ), g_skins.m_glove_kits, 6 );
					//	}

					//} g_frw.EndModule( );

					//g_frw.ContinueModule( );

					g_frw.RenderModule( positions::left_top, sizes::full, XOR( "" ) ); {

						std::string base_string = XOR( "skins_" );

						switch ( g_cl.m_weapon_id ) {
						case DEAGLE:
							base_string.append( XOR( "DEAGLE" ) );
							break;
						case ELITE:
							base_string.append( XOR( "ELITE" ) );
							break;
						case FIVESEVEN:
							base_string.append( XOR( "FIVESEVEN" ) );
							break;
						case GLOCK:
							base_string.append( XOR( "GLOCK" ) );
							break;
						case AK47:
							base_string.append( XOR( "AK47" ) );
							break;
						case AUG:
							base_string.append( XOR( "AUG" ) );
							break;
						case AWP:
							base_string.append( XOR( "AWP" ) );
							break;
						case FAMAS:
							base_string.append( XOR( "FAMAS" ) );
							break;
						case G3SG1:
							base_string.append( XOR( "G3SG1" ) );
							break;
						case GALIL:
							base_string.append( XOR( "GALIL" ) );
							break;
						case M249:
							base_string.append( XOR( "M249" ) );
							break;
						case M4A4:
							base_string.append( XOR( "M4A4" ) );
							break;
						case MAC10:
							base_string.append( XOR( "MAC10" ) );
							break;
						case P90:
							base_string.append( XOR( "P90" ) );
							break;
						case MP5_SD:
							base_string.append( XOR( "MP5_SD" ) );
							break;
						case UMP45:
							base_string.append( XOR( "UMP45" ) );
							break;
						case XM1014:
							base_string.append( XOR( "XM1014" ) );
							break;
						case BIZON:
							base_string.append( XOR( "BIZON" ) );
							break;
						case MAG7:
							base_string.append( XOR( "MAG7" ) );
							break;
						case NEGEV:
							base_string.append( XOR( "NEGEV" ) );
							break;
						case SAWEDOFF:
							base_string.append( XOR( "SAWEDOFF" ) );
							break;
						case TEC9:
							base_string.append( XOR( "TEC9" ) );
							break;
						case P2000:
							base_string.append( XOR( "P2000" ) );
							break;
						case MP7:
							base_string.append( XOR( "MP7" ) );
							break;
						case MP9:
							base_string.append( XOR( "MP9" ) );
							break;
						case NOVA:
							base_string.append( XOR( "NOVA" ) );
							break;
						case P250:
							base_string.append( XOR( "P250" ) );
							break;
						case SCAR20:
							base_string.append( XOR( "SCAR20" ) );
							break;
						case SG553:
							base_string.append( XOR( "SG553" ) );
							break;
						case SSG08:
							base_string.append( XOR( "SSG08" ) );
							break;
						case M4A1S:
							base_string.append( XOR( "M4A1S" ) );
							break;
						case USPS:
							base_string.append( XOR( "USPS" ) );
							break;
						case CZ75A:
							base_string.append( XOR( "CZ75A" ) );
							break;
						case REVOLVER:
							base_string.append( XOR( "REVOLVER" ) );
							break;
						case KNIFE_BAYONET:
							base_string.append( XOR( "KNIFE_BAYONET" ) );
							break;
						case KNIFE_FLIP:
							base_string.append( XOR( "KNIFE_FLIP" ) );
							break;
						case KNIFE_GUT:
							base_string.append( XOR( "KNIFE_GUT" ) );
							break;
						case KNIFE_KARAMBIT:
							base_string.append( XOR( "KNIFE_KARAMBIT" ) );
							break;
						case KNIFE_M9_BAYONET:
							base_string.append( XOR( "KNIFE_M9_BAYONET" ) );
							break;
						case KNIFE_HUNTSMAN:
							base_string.append( XOR( "KNIFE_HUNTSMAN" ) );
							break;
						case KNIFE_FALCHION:
							base_string.append( XOR( "KNIFE_FALCHION" ) );
							break;
						case KNIFE_BOWIE:
							base_string.append( XOR( "KNIFE_BOWIE" ) );
							break;
						case KNIFE_BUTTERFLY:
							base_string.append( XOR( "KNIFE_BUTTERFLY" ) );
							break;
						case KNIFE_SHADOW_DAGGERS:
							base_string.append( XOR( "KNIFE_SHADOW_DAGGERS" ) );
							break;
						case KNIFE_CSS:
							base_string.append( XOR( "KNIFE_CSS" ) );
							break;
						case KNIFE_CORD:
							base_string.append( XOR( "KNIFE_CORD" ) );
							break;
						case KNIFE_CANIS:
							base_string.append( XOR( "KNIFE_CANIS" ) );
							break;
						case KNIFE_URSUS:
							base_string.append( XOR( "KNIFE_URSUS" ) );
							break;
						case KNIFE_GYPSY_JACKKNIFE:
							base_string.append( XOR( "KNIFE_GYPSY_JACKKNIFE" ) );
							break;
						case KNIFE_OUTDOOR:
							base_string.append( XOR( "KNIFE_OUTDOOR" ) );
							break;
						case KNIFE_STILETTO:
							base_string.append( XOR( "KNIFE_STILETTO" ) );
							break;
						case KNIFE_WIDOWMAKER:
							base_string.append( XOR( "KNIFE_WIDOWMAKER" ) );
							break;
						case KNIFE_SKELETON:
							base_string.append( XOR( "KNIFE_SKELETON" ) );
							break;

						default:
							break;
						}

						std::string paintkit_string = base_string.append( XOR( "_kit" ) );
						static ImGuiTextFilter filter;

						static int old_paintkit = g_cfg[ paintkit_string ].get<int>( );

						if ( !g_skins.m_skins.empty( ) ) {
							static ImGuiTextFilter filter;
							ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) - 15 );
							filter.Draw( XOR( "##searchbar" ), 400.f );
							ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) + 15 );

							for ( int i = 0; i < g_skins.m_paint_kits.size( ); i++ ) {
								auto paintkit = g_skins.m_paint_kits.at( i );
								if ( filter.PassFilter( paintkit.c_str( ) ) ) {
									if( paintkit == XOR( "-" ) )
										paintkit = XOR( "Default" );

									std::string label = paintkit + XOR( "##" )+ std::to_string( i );

									if ( ImGui::Selectable22( label.c_str( ), g_cfg[ paintkit_string ].get<int>( ) == i ) )
										g_cfg[ paintkit_string ].set<int>( i );

								}
							}
						}
					} g_frw.EndModule( );

					g_frw.ContinueModule( );

					g_frw.RenderModule( positions::right_top, sizes::full, XOR( " " ) ); {
						//g_frw.KeybindCheckbox( XOR( "Enable Skin Changer" ) );
					//	g_frw.KeybindCheckbox( XOR( "Enable Knife Changer" ) );
						g_frw.Checkbox(XOR("Enable Agent Changer"), XOR("skins_agent_enable"));
						g_frw.ComboBox(XOR("Agent Model"), XOR("skins_agent"), XOR("Special Agent Ava\0Operator\0Markus Delrow\0Michael Syfers\0B Squadron Officer\0Seal Team 6 Soldier\0Buckshot\0Lt. Commander Ricksaw\0 3rd Commando Company\0'Two Times' McCoy\0Dragomir\0Rezan The Ready\0'The Doctor' Romanov\0Maximus\0Blackwolf\0The Elite Mr. Muhlik\0Ground Rebel\0Osiris\0Prof. Shahmat\0Enforcer\0Slingshot\0Soldier"), 8);
						g_frw.ComboBox( XOR( "Knife model" ), XOR( "skins_knife" ), XOR( "Disabled\0Bayonet\0Bowie\0Butterfly\0Falchion\0Flip knife\0Gut knife\0Huntsman\0Karambit\0M9 Bayonet\0Shadow Daggers\0CSS\0Paracord Knife\0Survival Knife\0Ursus knife\0Navaja Knife\0Nomad Knife\0Stiletto Knife\0Talon Knife\0Skeleton Knife" ), 8 );
					//	g_frw.KeybindCheckbox( XOR( "Enable Rare Animations" ) );
						g_frw.MultiCombo( XOR( "Force Rare Animations" ), { XOR( "skins_rare_deagle" ), XOR( "skins_rare_r8" ), XOR( "skins_rare_falcheon" ) }, { XOR( "Deagle" ), XOR( "Revolver" ), XOR( "Falcheon Knife" ) } );
						//g_frw.KeybindCheckbox( XOR( "Enable Glove Changer" ) );
						g_frw.ComboBox( XOR( "Glove model" ), XOR( "skins_gloves" ), XOR( "Disabled\0Bloodhound\0Sport\0Driver\0Handwraps\0Motorcycle\0Specialist" ), 6 );
					//	g_frw.KeybindCheckbox( XOR( "Enable Glove Skin" ) );
						ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + 3 );
						ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) + 5 );
						if ( !g_skins.m_glove_kits.empty( ) ) {
							g_frw.ListBox( XOR( "Glove skin" ), XOR( "skins_glove_kit" ), g_skins.m_glove_kits, 3 );
						}
						ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + 3 );
						g_frw.Button( XOR( "Refresh" ), &g_skins.m_update );
					} g_frw.EndModule( );



					g_frw.ConcludeModuleHeader( );

					break;
					case 3:
					{
						g_frw.RenderModule(positions::left_top, sizes::full, XOR("LOCAL")); {

							g_frw.Checkbox(XOR("Hand chams"), XOR("chams_hand"));
							g_frw.ColorPicker(XOR("Hand color"), XOR("hand_chams_color"));
							g_frw.ComboBox(XOR("Hand material"), XOR("hand_chams_material"), XOR("Disabled\0Unlit\0Lit\0Silhouette"), 4);

							g_frw.Checkbox(XOR("Local chams"), XOR("chams_local"));
							g_frw.ColorPicker(XOR("Local color"), XOR("local_chams_color"));
							g_frw.ComboBox(XOR("Local Material"), XOR("local_chams_material"), XOR("Disabled\0Unlit\0Lit\0Silhouette"), 4);
							g_frw.Checkbox(XOR("Hold Firing Animation"), XOR("firing_anim"));

							g_frw.Checkbox(XOR("Fake chams"), XOR("chams_desync"));
							g_frw.ColorPicker(XOR("Fake color"), XOR("desync_chams_color"));
							g_frw.ComboBox(XOR("Fake Material"), XOR("desyc_chams_material"), XOR("Disabled\0Unlit\0Lit\0Silhouette"), 4);

							g_frw.Checkbox(XOR("Local Glow"), XOR("esp_local_glow"));
							g_frw.ColorPicker(XOR("Local Glow Color"), XOR("esp_local_glow_color"));


						} g_frw.EndModule();
					}
					break;
				};
			}break;

			case tabs::tab_three: {
				static int current_subtab;
				g_frw.RenderSecondaryHeader( current_subtab, { XOR( "MISC" ) } ); {
					g_frw.BeginModuleHeader( );

					g_frw.RenderModule( positions::left_top, sizes::full, XOR( "MOVEMENT" ) ); {
						g_frw.Checkbox( XOR( "Autohop" ), XOR( "misc_bhop" ) );
						g_frw.Checkbox( XOR( "Unlimited duck stamina" ), XOR( "misc_duck_stamina" ) );
						g_frw.ComboBox( XOR( "Auto strafer" ), XOR( "misc_autostrafer_mode" ), XOR( "Disabled\0View\0Movement Input" ), 4 );
						if ( g_cfg[ XOR( "misc_autostrafer_mode" ) ].get<int>( ) )
							g_frw.NumberPicker( XOR( "Strafe speed" ), XOR( "strafe_speed" ), 0, 100, XOR( "%0.f" ), 1 );
						g_frw.Checkbox( XOR( "Supress leg movement" ), XOR( "misc_slide" ) );
						g_frw.KeybindCheckbox( XOR( "Trollbot" ) );
						g_frw.GetKey( XOR( "Trollbot_Bind" ), XOR( "misc_blockbot_bind" ), XOR( "misc_blockbot_bind_type" ) );
						g_frw.KeybindCheckbox( XOR( "Quickpeek" ) );
						g_frw.GetKey( XOR( "Quickpeek_Bind" ), XOR( "misc_auto_peek" ), XOR( "misc_auto_peek_type" ) );
						g_frw.KeybindCheckbox( XOR( "Edge Jump" ) );
						g_frw.GetKey( XOR( "Edge jump bind" ), XOR( "movement_edgejump" ), XOR( "movement_edgejump_type" ) );
						g_frw.Checkbox( XOR( "Fast Stop" ), XOR( "misc_faststop" ) );

					}g_frw.EndModule( );

					g_frw.ContinueModule( );

					g_frw.RenderModule( positions::right_top, sizes::full, XOR( "UTILITIES" ) ); {
						g_frw.Checkbox( XOR( "Enable automatic purchases" ), XOR( "misc_buybot_enable" ) );
						g_frw.ComboBox( XOR( "Loadout" ), XOR( "misc_buybot_loadout" ), XOR( "Loadout One\0Loadout Two\0Loadout Three\0Loadout Four" ), 4 );
						switch ( g_cfg[ XOR( "misc_buybot_loadout" ) ].get<int>( ) ) {
						case 0:
							g_frw.ComboBox( XOR( "Primary weapon" ), XOR( "misc_buybot_loadout1_primary" ), XOR( "Autosniper\0AWP\0SSG-08\0SG08" ), 4 );
							g_frw.ComboBox( XOR( "Primary weapon (Fallback)" ), XOR( "misc_buybot_loadout1_primary_fallback" ), XOR( "Autosniper\0AWP\0SSG-08\0SG08" ), 4 );
							g_frw.ComboBox( XOR( "Secondary weapon" ), XOR( "misc_buybot_loadout1_pistol" ), XOR( "Revolver/Deagle\0Duals\0Five-7/Tec-9\0P250" ), 4 );
							g_frw.Checkbox( XOR( "Defuse kit" ), XOR( "misc_buybot_loadout1_defuse" ) );
							g_frw.Checkbox( XOR( "Armor" ), XOR( "misc_load1_buybot_loadout1_armor" ) );
							g_frw.Checkbox( XOR( "Taser" ), XOR( "misc_buybot_loadout1_taser" ) );
							g_frw.Checkbox( XOR( "Nades" ), XOR( "misc_buybot_loadout1_nades" ) );
							break;
						case 1:
							g_frw.ComboBox( XOR( "Primary weapon" ), XOR( "misc_buybot_loadout2_primary" ), XOR( "Autosniper\0AWP\0SSG-08\0SG08" ), 4 );
							g_frw.ComboBox( XOR( "Primary weapon (Fallback)" ), XOR( "misc_buybot_loadout2_primary_fallback" ), XOR( "Autosniper\0AWP\0SSG-08\0SG08" ), 4 );
							g_frw.ComboBox( XOR( "Secondary weapon" ), XOR( "misc_buybot_loadout2_pistol" ), XOR( "Revolver/Deagle\0Duals\0Five-Seven\0P250" ), 4 );
							g_frw.Checkbox( XOR( "Defuse kit" ), XOR( "misc_buybot_loadout2_defuse" ) );
							g_frw.Checkbox( XOR( "Armor" ), XOR( "misc_load2_buybot_loadout2_armor" ) );
							g_frw.Checkbox( XOR( "Taser" ), XOR( "misc_buybot_loadout2_taser" ) );
							g_frw.Checkbox( XOR( "Nades" ), XOR( "misc_buybot_loadout2_nades" ) );
							break;
						case 2:
							g_frw.ComboBox( XOR( "Primary weapon" ), XOR( "misc_buybot_loadout3_primary" ), XOR( "Autosniper\0AWP\0SSG-08\0SG08" ), 4 );
							g_frw.ComboBox( XOR( "Primary weapon (Fallback)" ), XOR( "misc_buybot_loadout3_primary_fallback" ), XOR( "Autosniper\0AWP\0SSG-08\0SG08" ), 4 );
							g_frw.ComboBox( XOR( "Secondary Weapon" ), XOR( "misc_buybot_loadout3_pistol" ), XOR( "Revolver/Deagle\0Duals\0Five-Seven\0P350" ), 4 );
							g_frw.Checkbox( XOR( "Defuse Kit" ), XOR( "misc_buybot_loadout3_defuse" ) );
							g_frw.Checkbox( XOR( "Armor" ), XOR( "misc_load1_buybot_loadout3_armor" ) );
							g_frw.Checkbox( XOR( "Taser" ), XOR( "misc_buybot_loadout3_taser" ) );
							g_frw.Checkbox( XOR( "Nades" ), XOR( "misc_buybot_loadout3_nades" ) );
							break;

						case 3:
							g_frw.ComboBox( XOR( "Primary weapon" ), XOR( "misc_buybot_loadout4_primary" ), XOR( "Autosniper\0AWP\0SSG-08\0SG08" ), 4 );
							g_frw.ComboBox( XOR( "Primary weapon (Fallback)" ), XOR( "misc_buybot_loadout4_primary_fallback" ), XOR( "Autosniper\0AWP\0SSG-08\0SG08" ), 4 );
							g_frw.ComboBox( XOR( "Secondary weapon" ), XOR( "misc_buybot_loadout4_pistol" ), XOR( "Revolver/Deagle\0Duals\0Five-Seven\0P450" ), 4 );
							g_frw.Checkbox( XOR( "Defuse kit" ), XOR( "misc_buybot_loadout4_defuse" ) );
							g_frw.Checkbox( XOR( "Armor" ), XOR( "misc_load1_buybot_loadout4_armor" ) );
							g_frw.Checkbox( XOR( "Taser" ), XOR( "misc_buybot_loadout4_taser" ) );
							g_frw.Checkbox( XOR( "Nades" ), XOR( "misc_buybot_loadout4_nades" ) );
							break;
						}

					}g_frw.EndModule( );

					g_frw.ContinueModule( );

					g_frw.RenderModule( positions::right_top, sizes::full, XOR( "OTHER" ) ); {

						g_frw.Checkbox( XOR( "Unlock all convars" ), XOR( "misc_unlock_cvars" ) );
						if ( g_cfg[ XOR( "misc_unlock_cvars" ) ].get<bool>( ) )
							g_cl.UnlockHiddenConvars( );

						g_frw.ComboBox( XOR( "Clantag type" ), XOR( "misc_clantag_type" ), XOR( "None\0undercover\0Custom" ), 4 );
						g_frw.ComboBox( XOR( "Clantag mode" ), XOR( "misc_clantag_mode" ), XOR( "Static\0Rotating " ), 4 );

						if ( g_cfg[ XOR( "misc_clantag_type" ) ].get<int>( ) == 2 ) {

							ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + 22 );
							ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) - 9 );
							g_frw.TextInput( XOR( "Custom Team" ), g_cl.clantag_lable, false );
						}


					} g_frw.EndModule( );

					g_frw.ConcludeModuleHeader( );
				}
			}break;

			case tabs::tab_four:
				static int current_subtab;
				g_frw.RenderSecondaryHeader( current_subtab, { XOR( "CONFIG" ) } );

				g_frw.RenderModule( positions::left_top, sizes::full, XOR( "CONFIGS" ) ); {
					bool t1, t2, t3, t4, t5;
					static char config_name[ 25 ];
					g_frw.ListBox( XOR( "Configs" ), XOR( "cfg" ), g_config.configs, 8 );


					g_frw.TextInput( XOR( "Name" ), config_name, true );
					g_frw.Button( XOR( "Create new file" ), &t4 );
					if ( t4 ) {
						g_config.save( config_name + std::string( XOR( ".ini" ) ) );
					}
					g_frw.Button( XOR( "Refresh list" ), &t1 );
					if ( t1 ) {
						g_config.refresh( );
					}
					g_frw.Button( XOR( "Save to file" ), &t2 );
					if ( t2 ) {
						g_config.save( g_config.configs.at( g_cfg[ XOR( "cfg" ) ].get< int >( ) ) );
					}
					g_frw.Button( XOR( "Load" ), &t3 );
					if ( t3 ) {
						g_config.load( g_config.configs.at( g_cfg[ XOR( "cfg" ) ].get< int >( ) ) );
						g_skins.m_update = true;
					}
					g_frw.Button( XOR( "Delete file" ), &t5 );


				}g_frw.EndModule( );
				break;

			};
		}
		g_frw.EndWindow( );
	}
}
#include "undercover.h"

Config g_config{ };

value pre_defined;

std::map< std::string, value > g_cfg{
	{ XOR( "aimbot_enable" ), pre_defined },
	{ XOR( "aimbot_enable_key" ), pre_defined },
	{ XOR( "aimbot_enable_key_type" ), pre_defined },
	{ XOR( "aimbot_autofire" ), pre_defined },
	{ XOR( "aimbot_silent" ), pre_defined },
	{ XOR( "aimbot_knifebot" ), pre_defined },
	{ XOR( "aimbot_zeusbot" ), pre_defined },
	{ XOR( "override_hitbox" ), pre_defined },
	{ XOR( "hitbox_override_key" ), pre_defined },
	{ XOR( "hitbox_override_key_type" ), pre_defined },
	{ XOR( "safepoint" ), pre_defined },
	{ XOR( "safepoint_key" ), pre_defined },
	{ XOR( "safepoint_key_type" ), pre_defined },
	{ XOR( "override_min_dmg" ), pre_defined },
	{ XOR( "override_min_dmg_key" ), pre_defined },
	{ XOR( "override_min_dmg_type" ), pre_defined },
	{ XOR( "aimbot_exploits_teleport" ), pre_defined },
	{ XOR( "aimbot_exploits_teleport_key" ), pre_defined },
	{ XOR( "aimbot_exploits_teleport_key_type" ), pre_defined },
	{ XOR( "aimbot_hide_shots" ), pre_defined },
	{ XOR( "aimbot_hide_shots_key" ), pre_defined },
	{ XOR( "aimbot_hide_shots_key_type" ), pre_defined },
	{ XOR( "aimbot_target_selection" ), pre_defined },
	{ XOR( "aimbot_adaptive_config" ), pre_defined },
	{ XOR( "aimbot_selected_weapon" ), pre_defined },
	{ XOR( "aimbot_pistol_hc" ), pre_defined },
	{ XOR( "aimbot_pistol_min_dmg" ), pre_defined },
	{ XOR( "aimbot_pistol_overriden_min_dmg" ), pre_defined },
	{ XOR( "aimbot_pistol_autoscope" ), pre_defined },
	{ XOR( "aimbot_pistol_autostop" ), pre_defined },
	{ XOR( "aimbot_pistol_jumpscout" ), pre_defined },
	{ XOR( "aimbot_pistol_between_shots" ), pre_defined },
	{ XOR( "aimbot_pistol_headscale" ), pre_defined },
	{ XOR( "aimbot_pistol_bodyscale" ), pre_defined },
	{ XOR( "aimbot_pistol_no_legs" ), pre_defined },
	{ XOR( "aimbot_pistol_hitbox_head" ), pre_defined },
	{ XOR( "aimbot_pistol_hitbox_upper_body" ), pre_defined },
	{ XOR( "aimbot_pistol_hitbox_lower_body" ), pre_defined },
	{ XOR( "aimbot_pistol_hitbox_stomach" ), pre_defined },
	{ XOR( "aimbot_pistol_hitbox_legs" ), pre_defined },
	{ XOR( "aimbot_pistol_hitbox_feet" ), pre_defined },
	{ XOR( "aimbot_pistol_mutlipoint_hitbox_head" ), pre_defined },
	{ XOR( "aimbot_pistol_mutlipoint_hitbox_upper_body" ), pre_defined },
	{ XOR( "aimbot_pistol_mutlipoint_hitbox_lower_body" ), pre_defined },
	{ XOR( "aimbot_pistol_mutlipoint_hitbox_stomach" ), pre_defined },
	{ XOR( "aimbot_pistol_mutlipoint_hitbox_legs" ), pre_defined },
	{ XOR( "aimbot_pistol_mutlipoint_hitbox_feet" ), pre_defined },
	{ XOR( "aimbot_pistol_priority_hitbox" ), pre_defined },
	{ XOR( "aimbot_pistol_body_in_air" ), pre_defined },
	{ XOR( "aimbot_pistol_body_on_crouch" ), pre_defined },
	{ XOR( "aimbot_pistol_body_lethal" ), pre_defined },
	{ XOR( "aimbot_pistol_body_lethal2" ), pre_defined },
	{ XOR( "aimbot_pistol_overriden_hitbox_head" ), pre_defined },
	{ XOR( "aimbot_pistol_overriden_hitbox_upper_body" ), pre_defined },
	{ XOR( "aimbot_pistol_overriden_hitbox_lower_body" ), pre_defined },
	{ XOR( "aimbot_pistol_overriden_hitbox_stomach" ), pre_defined },
	{ XOR( "aimbot_pistol_overriden_hitbox_legs" ), pre_defined },
	{ XOR( "aimbot_pistol_overriden_hitbox_feet" ), pre_defined },
	{ XOR( "aimbot_r8_hc" ), pre_defined },
	{ XOR( "aimbot_r8_min_dmg" ), pre_defined },
	{ XOR( "aimbot_r8_overriden_min_dmg" ), pre_defined },
	{ XOR( "aimbot_r8_autoscope" ), pre_defined },
	{ XOR( "aimbot_r8_autostop" ), pre_defined },
	{ XOR( "aimbot_r8_jumpscout" ), pre_defined },
	{ XOR( "aimbot_r8_between_shots" ), pre_defined },
	{ XOR( "aimbot_r8_headscale" ), pre_defined },
	{ XOR( "aimbot_r8_bodyscale" ), pre_defined },
	{ XOR( "aimbot_r8_no_legs" ), pre_defined },
	{ XOR( "aimbot_r8_hitbox_head" ), pre_defined },
	{ XOR( "aimbot_r8_hitbox_upper_body" ), pre_defined },
	{ XOR( "aimbot_r8_hitbox_lower_body" ), pre_defined },
	{ XOR( "aimbot_r8_hitbox_stomach" ), pre_defined },
	{ XOR( "aimbot_r8_hitbox_legs" ), pre_defined },
	{ XOR( "aimbot_r8_hitbox_feet" ), pre_defined },
	{ XOR( "aimbot_r8_mutlipoint_hitbox_head" ), pre_defined },
	{ XOR( "aimbot_r8_mutlipoint_hitbox_upper_body" ), pre_defined },
	{ XOR( "aimbot_r8_mutlipoint_hitbox_lower_body" ), pre_defined },
	{ XOR( "aimbot_r8_mutlipoint_hitbox_stomach" ), pre_defined },
	{ XOR( "aimbot_r8_mutlipoint_hitbox_legs" ), pre_defined },
	{ XOR( "aimbot_r8_mutlipoint_hitbox_feet" ), pre_defined },
	{ XOR( "aimbot_r8_priority_hitbox" ), pre_defined },
	{ XOR( "aimbot_r8_body_in_air" ), pre_defined },
	{ XOR( "aimbot_r8_body_on_crouch" ), pre_defined },
	{ XOR( "aimbot_r8_body_lethal" ), pre_defined },
	{ XOR( "aimbot_r8_body_lethal2" ), pre_defined },
	{ XOR( "aimbot_r8_overriden_hitbox_head" ), pre_defined },
	{ XOR( "aimbot_r8_overriden_hitbox_upper_body" ), pre_defined },
	{ XOR( "aimbot_r8_overriden_hitbox_lower_body" ), pre_defined },
	{ XOR( "aimbot_r8_overriden_hitbox_stomach" ), pre_defined },
	{ XOR( "aimbot_r8_overriden_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_r8_overriden_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_jumpscout" ), pre_defined },
	{ XOR( "aimbot_ssg08_hc"), pre_defined },
	{ XOR( "aimbot_ssg08_min_dmg"), pre_defined },
	{ XOR( "aimbot_ssg08_overriden_min_dmg"), pre_defined },
	{ XOR( "aimbot_ssg08_autoscope"), pre_defined },
	{ XOR( "aimbot_ssg08_autostop"), pre_defined },
	{ XOR( "aimbot_ssg08_jumpscout"), pre_defined },
	{ XOR( "aimbot_ssg08_between_shots"), pre_defined },
	{ XOR( "aimbot_ssg08_headscale"), pre_defined },
	{ XOR( "aimbot_ssg08_bodyscale"), pre_defined },
	{ XOR( "aimbot_ssg08_no_legs"), pre_defined },
	{ XOR( "aimbot_ssg08_hitbox_head"), pre_defined },
	{ XOR( "aimbot_ssg08_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_ssg08_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_ssg08_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_ssg08_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_ssg08_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_ssg08_mutlipoint_hitbox_head"), pre_defined },
	{ XOR( "aimbot_ssg08_mutlipoint_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_ssg08_mutlipoint_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_ssg08_mutlipoint_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_ssg08_mutlipoint_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_ssg08_mutlipoint_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_ssg08_priority_hitbox" ), pre_defined },
	{ XOR( "aimbot_ssg08_body_in_air" ), pre_defined },
	{ XOR( "aimbot_ssg08_body_on_crouch" ), pre_defined },
	{ XOR( "aimbot_ssg08_body_lethal" ), pre_defined },
	{ XOR( "aimbot_ssg08_body_lethal2" ), pre_defined },
	{ XOR( "aimbot_ssg08_overriden_hitbox_head"), pre_defined },
	{ XOR( "aimbot_ssg08_overriden_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_ssg08_overriden_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_ssg08_overriden_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_ssg08_overriden_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_ssg08_overriden_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_auto_hc"), pre_defined },
	{ XOR( "aimbot_auto_min_dmg"), pre_defined },
	{ XOR( "aimbot_auto_overriden_min_dmg"), pre_defined },
	{ XOR( "aimbot_auto_autoscope"), pre_defined },
	{ XOR( "aimbot_auto_autostop"), pre_defined },
	{ XOR( "aimbot_auto_jumpscout"), pre_defined },
	{ XOR( "aimbot_auto_between_shots"), pre_defined },
	{ XOR( "aimbot_auto_headscale"), pre_defined },
	{ XOR( "aimbot_auto_bodyscale"), pre_defined },
	{ XOR( "aimbot_auto_no_legs"), pre_defined },
	{ XOR( "aimbot_auto_hitbox_head"), pre_defined },
	{ XOR( "aimbot_auto_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_auto_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_auto_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_auto_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_auto_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_auto_mutlipoint_hitbox_head"), pre_defined },
	{ XOR( "aimbot_auto_mutlipoint_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_auto_mutlipoint_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_auto_mutlipoint_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_auto_mutlipoint_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_auto_mutlipoint_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_auto_priority_hitbox" ), pre_defined },
	{ XOR( "aimbot_auto_body_in_air" ), pre_defined },
	{ XOR( "aimbot_auto_body_on_crouch" ), pre_defined },
	{ XOR( "aimbot_auto_body_lethal" ), pre_defined },
	{ XOR( "aimbot_auto_body_lethal2" ), pre_defined },
	{ XOR( "aimbot_auto_overriden_hitbox_head"), pre_defined },
	{ XOR( "aimbot_auto_overriden_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_auto_overriden_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_auto_overriden_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_auto_overriden_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_auto_overriden_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_awp_hc"), pre_defined },
	{ XOR( "aimbot_awp_min_dmg"), pre_defined },
	{ XOR( "aimbot_awp_overriden_min_dmg"), pre_defined },
	{ XOR( "aimbot_awp_autoscope"), pre_defined },
	{ XOR( "aimbot_awp_autostop"), pre_defined },
	{ XOR( "aimbot_awp_jumpscout"), pre_defined },
	{ XOR( "aimbot_awp_between_shots"), pre_defined },
	{ XOR( "aimbot_awp_headscale"), pre_defined },
	{ XOR( "aimbot_awp_bodyscale"), pre_defined },
	{ XOR( "aimbot_awp_no_legs"), pre_defined },
	{ XOR( "aimbot_awp_hitbox_head"), pre_defined },
	{ XOR( "aimbot_awp_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_awp_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_awp_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_awp_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_awp_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_awp_mutlipoint_hitbox_head"), pre_defined },
	{ XOR( "aimbot_awp_mutlipoint_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_awp_mutlipoint_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_awp_mutlipoint_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_awp_mutlipoint_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_awp_mutlipoint_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_awp_priority_hitbox" ), pre_defined },
	{ XOR( "aimbot_awp_body_in_air" ), pre_defined },
	{ XOR( "aimbot_awp_body_on_crouch" ), pre_defined },
	{ XOR( "aimbot_awp_body_lethal" ), pre_defined },
	{ XOR( "aimbot_awp_body_lethal2" ), pre_defined },
	{ XOR( "aimbot_awp_overriden_hitbox_head"), pre_defined },
	{ XOR( "aimbot_awp_overriden_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_awp_overriden_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_awp_overriden_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_awp_overriden_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_awp_overriden_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_other_hc"), pre_defined },
	{ XOR( "aimbot_other_min_dmg"), pre_defined },
	{ XOR( "aimbot_other_overriden_min_dmg"), pre_defined },
	{ XOR( "aimbot_other_autoscope"), pre_defined },
	{ XOR( "aimbot_other_autostop"), pre_defined },
	{ XOR( "aimbot_other_jumpscout"), pre_defined },
	{ XOR( "aimbot_other_between_shots"), pre_defined },
	{ XOR( "aimbot_other_headscale"), pre_defined },
	{ XOR( "aimbot_other_bodyscale"), pre_defined },
	{ XOR( "aimbot_other_no_legs"), pre_defined },
	{ XOR( "aimbot_other_hitbox_head"), pre_defined },
	{ XOR( "aimbot_other_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_other_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_other_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_other_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_other_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_other_mutlipoint_hitbox_head"), pre_defined },
	{ XOR( "aimbot_other_mutlipoint_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_other_mutlipoint_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_other_mutlipoint_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_other_mutlipoint_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_other_mutlipoint_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_other_priority_hitbox" ), pre_defined },
	{ XOR( "aimbot_other_body_in_air" ), pre_defined },
	{ XOR( "aimbot_other_body_on_crouch" ), pre_defined },
	{ XOR( "aimbot_other_body_lethal" ), pre_defined },
	{ XOR( "aimbot_other_body_lethal2" ), pre_defined },
	{ XOR( "aimbot_other_overriden_hitbox_head"), pre_defined },
	{ XOR( "aimbot_other_overriden_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_other_overriden_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_other_overriden_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_other_overriden_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_other_overriden_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_gen_hc"), pre_defined },
	{ XOR( "aimbot_gen_min_dmg"), pre_defined },
	{ XOR( "aimbot_gen_overriden_min_dmg"), pre_defined },
	{ XOR( "aimbot_gen_autoscope"), pre_defined },
	{ XOR( "aimbot_gen_autostop"), pre_defined },
	{ XOR( "aimbot_gen_jumpscout"), pre_defined },
	{ XOR( "aimbot_gen_between_shots"), pre_defined },
	{ XOR( "aimbot_gen_headscale"), pre_defined },
	{ XOR( "aimbot_gen_bodyscale"), pre_defined },
	{ XOR( "aimbot_gen_no_legs"), pre_defined },
	{ XOR( "aimbot_gen_hitbox_head"), pre_defined },
	{ XOR( "aimbot_gen_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_gen_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_gen_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_gen_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_gen_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_gen_mutlipoint_hitbox_head"), pre_defined },
	{ XOR( "aimbot_gen_mutlipoint_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_gen_mutlipoint_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_gen_mutlipoint_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_gen_mutlipoint_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_gen_mutlipoint_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_gen_priority_hitbox" ), pre_defined },
	{ XOR( "aimbot_gen_body_in_air" ), pre_defined },
	{ XOR( "aimbot_gen_body_on_crouch" ), pre_defined },
	{ XOR( "aimbot_gen_body_lethal" ), pre_defined },
	{ XOR( "aimbot_gen_body_lethal2" ), pre_defined },
	{ XOR( "aimbot_gen_overriden_hitbox_head"), pre_defined },
	{ XOR( "aimbot_gen_overriden_hitbox_upper_body"), pre_defined },
	{ XOR( "aimbot_gen_overriden_hitbox_lower_body"), pre_defined },
	{ XOR( "aimbot_gen_overriden_hitbox_stomach"), pre_defined },
	{ XOR( "aimbot_gen_overriden_hitbox_legs"), pre_defined },
	{ XOR( "aimbot_gen_overriden_hitbox_feet"), pre_defined },
	{ XOR( "aimbot_rapid_fire"), pre_defined },
	{ XOR( "aimbot_teleport_amount"), pre_defined },
	{ XOR( "aimbot_resolver" ), pre_defined },
	{ XOR( "aimbot_pitch_resolver" ), pre_defined },
	{ XOR( "aa_enabled" ), pre_defined },
	{ XOR( "aa_auto_pos" ), pre_defined },
	{ XOR( "aa_auto_pos_type" ), pre_defined },
	{ XOR( "aa_lean" ), pre_defined },
	{ XOR( "aa_jitter_shot" ), pre_defined },
	{ XOR( "aa_jitter_land" ), pre_defined },
	{ XOR( "aa_jitter_peek" ), pre_defined },
	{ XOR( "aa_jitter" ), pre_defined },
	{ XOR( "aa_jitter_val" ), pre_defined },
	{ XOR( "aa_no_jitter_when_crouch" ), pre_defined },
	{ XOR( "aa_jitter" ), pre_defined },
	{ XOR( "aa_jitter_2" ), pre_defined },
	{ XOR( "aa_yaw_offset" ), pre_defined },
	{ XOR( "aa_jitter_offset" ), pre_defined },
	{ XOR( "aa_manual" ), pre_defined },
	{ XOR( "aa_manual_left" ), pre_defined },
	{ XOR( "aa_manual_left_key" ), pre_defined },
	{ XOR( "aa_manual_left_key_type" ), pre_defined },
	{ XOR( "aa_manual_right" ), pre_defined },
	{ XOR( "aa_manual_right_key" ), pre_defined },
	{ XOR( "aa_manual_right_key_type" ), pre_defined },
	{ XOR( "aa_manual_back" ), pre_defined },
	{ XOR( "aa_manual_back_key" ), pre_defined },
	{ XOR( "aa_manual_back_key_type" ), pre_defined },
	{ XOR( "aa_fake" ), pre_defined },
	{ XOR( "aa_air_mode" ), pre_defined },
	{ XOR( "aa_lby_mode" ), pre_defined },
	{ XOR( "aa_fake_desync" ), pre_defined },
	{ XOR( "aa_inverter_key" ), pre_defined },
	{ XOR( "invertflip_walking" ), pre_defined },
	{ XOR( "invertflip_moving" ), pre_defined },
	{ XOR( "invertflip_air" ), pre_defined },
	{ XOR( "aa_lby" ), pre_defined },
	{ XOR( "aa_fake_clamp" ), pre_defined },
	{ XOR( "aa_invert" ), pre_defined },
	{ XOR( "aa_fakelag" ), pre_defined },
	{ XOR( "aa_factor" ), pre_defined },
	{ XOR( "aa_fl_jitter" ), pre_defined }, 
	{ XOR( "aa_fl_in_air" ), pre_defined },
	{ XOR( "aa_fl_on_peek" ), pre_defined }, 
	{ XOR( "aa_fl_on_shot" ), pre_defined },
	{ XOR( "aa_fl_on_land" ), pre_defined }, 
	{ XOR( "aa_fl_while_reload" ), pre_defined },
	{ XOR( "aa_fl_on_wep_switch" ), pre_defined },
	{ XOR( "aa_fl_on_velocity_change" ), pre_defined },
	{ XOR( "aa_fl_break_lagcomp "), pre_defined },
	{ XOR( "esp_enemies_box" ), pre_defined },
	{ XOR( "esp_enemies_box_color" ), pre_defined },
	{ XOR( "esp_enemies_name" ), pre_defined },
	{ XOR( "esp_enemies_name_color" ), pre_defined },
	{ XOR( "esp_enemies_health" ), pre_defined },
	{ XOR( "esp_enemies_health_override" ), pre_defined },
	{ XOR( "esp_enemies_health_color" ), pre_defined },
	{ XOR( "esp_enemies_weapon" ), pre_defined },
	{ XOR( "esp_enemies_weapon_color" ), pre_defined },
	{ XOR( "esp_enemies_ammo" ), pre_defined },
	{ XOR( "esp_enemies_ammo_color" ), pre_defined },
	{ XOR( "esp_enemies_skeleton" ), pre_defined },
	{ XOR( "esp_enemies_skeleton_color" ), pre_defined },
	{ XOR( "esp_enemy_glow" ), pre_defined },
	{ XOR( "esp_enemy_glow_healthcolor" ), pre_defined },
	{ XOR( "esp_enemy_glow_color" ), pre_defined },
	{ XOR( "esp_enemies_dormant_time" ), pre_defined },
	{ XOR("misc_animlayer"), pre_defined },
	{ XOR( "misc_hitsound" ), pre_defined },
	{ XOR( "visuals_vis_interpolation_amt" ), pre_defined },
	{ XOR( "visuals_vis_interpolation_dead" ), pre_defined },
	{ XOR( "esp_enemies_offscreen_color" ), pre_defined },
	{ XOR( "esp_enemies_offscreen" ), pre_defined },
	{ XOR( "esp_enemies_offscreen_radius" ), pre_defined },
	{ XOR( "esp_enemies_offscreen_size" ), pre_defined },
	{ XOR( "chams_enemy" ), pre_defined },
	{ XOR( "esp_chams_enemies_color" ), pre_defined },
	{ XOR( "esp_chams_visible_enemies" ), pre_defined },
	{ XOR( "desyc_chams_material" ), pre_defined },
	{ XOR( "desync_chams_color" ), pre_defined },
	{ XOR( "chams_desync" ), pre_defined },
	{ XOR( "esp_local_glow" ), pre_defined },
	{ XOR( "esp_local_glow_color" ), pre_defined },
	{ XOR( "fog_enable" ), pre_defined },
	{ XOR( "fog_color" ), pre_defined },
	{ XOR( "fog_density" ), pre_defined },
	{ XOR( "fog_length" ), pre_defined },
	{ XOR( "misc_bullet_impacts_client" ), pre_defined },
	{ XOR( "misc_bullet_impacts_server" ), pre_defined },
	{ XOR( "misc_bullet_impacts_client_override" ), pre_defined },
	{ XOR( "misc_bullet_impacts_client_color" ), pre_defined },
	{ XOR( "misc_bullet_impacts_server_override" ), pre_defined },
	{ XOR( "misc_bullet_impacts_server_color" ), pre_defined },
	{ XOR( "chams_enemy_hidden" ), pre_defined },
	{ XOR( "esp_chams_enemies_invis_color" ), pre_defined },
	{ XOR( "esp_chams_invis_enemies" ), pre_defined },
	{ XOR( "chams_enemy_ragdoll" ), pre_defined },
	{ XOR( "esp_chams_enemies_corpse_color" ), pre_defined },
	{ XOR( "esp_chams_corpse_enemies"), pre_defined },
	{ XOR( "chams_enemy_hit" ), pre_defined },
	{ XOR( "esp_chams_enemies_wounded_color" ), pre_defined },
	{ XOR( "esp_chams_wounded_enemies" ), pre_defined },
	{ XOR( "misc_world_night" ), pre_defined },
	{ XOR( "misc_world_night_darkness" ), pre_defined },
	{ XOR( "misc_world_prop_opacity" ), pre_defined },
	{ XOR( "misc_world_fullbright" ), pre_defined },
	{ XOR( "misc_world_skybox" ), pre_defined },
	{ XOR( "visuals_misc_grenade_prediction" ), pre_defined },
	{ XOR( "misc_hitmarker" ), pre_defined },
	{ XOR( "visuals_misc_ragdoll_thrust" ), pre_defined },
	{ XOR( "visuals_misc_aspect_ratio" ), pre_defined },
	{ XOR( "chams_hand" ), pre_defined },
	{ XOR( "hand_chams_color" ), pre_defined },
	{ XOR( "hand_chams_material" ), pre_defined },
	{ XOR( "chams_local" ), pre_defined },
	{ XOR( "local_chams_color" ), pre_defined },
	{ XOR( "local_chams_material"), pre_defined },
	{ XOR( "misc_override_fov_amount" ), pre_defined },
	{ XOR( "misc_viewmodel_fov" ), pre_defined },
	{ XOR( "misc_viewmodel_x"), pre_defined },
	{ XOR( "misc_viewmodel_y"), pre_defined },
	{ XOR( "misc_viewmodel_z"), pre_defined },
	{ XOR( "misc_thirdperson_enable"), pre_defined },
	{ XOR( "misc_thirdperson_key" ), pre_defined },
	{ XOR( "misc_thirdperson_key_type" ), pre_defined },
	{ XOR( "misc_thirdperson_distance" ), pre_defined },
	{ XOR( "misc_world_fire_proximity" ), pre_defined },
	{ XOR( "misc_world_fire_proximity_color" ), pre_defined },
	{ XOR( "misc_world_smoke_proximity" ), pre_defined },
	{ XOR( "misc_world_smoke_proximity_color" ), pre_defined },
	{ XOR( "misc_preserve_killfeed" ), pre_defined },
	{ XOR( "misc_spectators" ), pre_defined },
	{ XOR( "misc_damage_indicator" ), pre_defined },
	{ XOR( "misc_override_fov_scoped" ), pre_defined },
	{ XOR( "visuals_misc_remove_scope" ), pre_defined },
	{ XOR( "visuals_misc_remove_smoke" ), pre_defined },
	{ XOR( "visuals_misc_remove_flash" ), pre_defined },
	{ XOR( "visuals_misc_remove_visual_recoil" ), pre_defined },
	{ XOR( "visuals_misc_remove_fog" ), pre_defined },
	{ XOR( "visuals_misc_remove_magazines" ), pre_defined },
	{ XOR( "visuals_misc_remove_entity_shadows" ), pre_defined },
	{ XOR( "visuals_misc_remove_sleeves" ), pre_defined },
	{ XOR( "visuals_misc_remove_defuser_ropes" ), pre_defined },
	{ XOR( "misc_force_crosshair" ), pre_defined },
	{ XOR( "misc_bullet_impacts" ), pre_defined },
	{ XOR( "visuals_misc_bullet_beam" ), pre_defined },
	{ XOR( "visuals_misc_bullet_beam_color" ), pre_defined },
	{ XOR( "skins_knife" ), pre_defined },
	{ XOR( "skins_gloves" ), pre_defined },
	{ XOR( "skins_glove_kit" ), pre_defined },
	{ XOR( "misc_bhop" ), pre_defined },
	{ XOR( "misc_duck_stamina" ), pre_defined },
	{ XOR( "misc_autostrafer_mode" ), pre_defined },
	{ XOR( "strafe_speed" ), pre_defined },
	{ XOR( "misc_slowwalk_bind" ), pre_defined },
	{ XOR( "misc_slowwalk_bind_type" ), pre_defined },
	{ XOR( "misc_slide" ), pre_defined },
	{ XOR( "Trollbot_Bind" ), pre_defined },
	{ XOR( "misc_blockbot_bind" ), pre_defined },
	{ XOR( "misc_blockbot_bind_type" ), pre_defined },
	{ XOR( "Quickpeek_Bind" ), pre_defined },
	{ XOR( "misc_quickpeek_bind" ), pre_defined },
	{ XOR( "misc_quickpeek_bind_type" ), pre_defined },
	{ XOR( "misc_fakeduck" ), pre_defined },
	{ XOR( "misc_fakeduck_key" ), pre_defined },
	{ XOR( "misc_fakeduck_key_type" ), pre_defined },
	{ XOR( "misc_buybot_enable" ), pre_defined },
	{ XOR( "misc_buybot_loadout" ), pre_defined },
	{ XOR( "misc_buybot_loadout1_primary" ), pre_defined },
	{ XOR( "misc_buybot_loadout1_primary_fallback"), pre_defined },
	{ XOR( "misc_buybot_loadout1_pistol"), pre_defined },
	{ XOR( "misc_buybot_loadout1_defuse"), pre_defined },
	{ XOR( "misc_load1_buybot_loadout1_armor"), pre_defined },
	{ XOR( "misc_buybot_loadout1_taser"), pre_defined },
	{ XOR( "misc_buybot_loadout1_nades"), pre_defined },
	{ XOR( "misc_buybot_loadout2_primary"), pre_defined },
	{ XOR( "misc_buybot_loadout2_primary_fallback"), pre_defined },
	{ XOR( "misc_buybot_loadout2_pistol"), pre_defined },
	{ XOR( "misc_buybot_loadout2_defuse"), pre_defined },
	{ XOR( "misc_load1_buybot_loadout2_armor"), pre_defined },
	{ XOR( "misc_buybot_loadout2_taser"), pre_defined },
	{ XOR( "misc_buybot_loadout2_nades"), pre_defined },
	{ XOR( "misc_buybot_loadout3_primary"), pre_defined },
	{ XOR( "misc_buybot_loadout3_primary_fallback"), pre_defined },
	{ XOR( "misc_buybot_loadout3_pistol"), pre_defined },
	{ XOR( "misc_buybot_loadout3_defuse"), pre_defined },
	{ XOR( "misc_load1_buybot_loadout3_armor"), pre_defined },
	{ XOR( "misc_buybot_loadout3_taser"), pre_defined },
	{ XOR( "misc_buybot_loadout3_nades"), pre_defined },
	{ XOR( "misc_buybot_loadout4_primary"), pre_defined },
	{ XOR( "misc_buybot_loadout4_primary_fallback"), pre_defined },
	{ XOR( "misc_buybot_loadout4_pistol"), pre_defined },
	{ XOR( "misc_buybot_loadout4_defuse"), pre_defined },
	{ XOR( "misc_load1_buybot_loadout4_armor"), pre_defined },
	{ XOR( "misc_buybot_loadout4_taser"), pre_defined },
	{ XOR( "misc_buybot_loadout4_nades"), pre_defined },
	{ XOR( "misc_unlock_cvars" ), pre_defined },
	{ XOR( "misc_clantag_type" ), pre_defined },
	{ XOR( "misc_clantag_mode" ), pre_defined },
	{ XOR( "clantag_label" ), pre_defined },
	{ XOR( "skins_DEAGLE_kit" ), pre_defined },
	{ XOR( "skins_DEAGLE_stattrak" ), pre_defined },
	{ XOR( "skins_DEAGLE_seed" ), pre_defined },
	{ XOR( "skins_ELITE_kit" ), pre_defined },
	{ XOR( "skins_ELITE_stattrak" ), pre_defined },
	{ XOR( "skins_ELITE_seed" ), pre_defined },
	{ XOR( "skins_FIVESEVEN_kit" ), pre_defined },
	{ XOR( "skins_FIVESEVEN_stattrak" ), pre_defined },
	{ XOR( "skins_FIVESEVEN_seed" ), pre_defined },
	{ XOR( "skins_GLOCK_kit" ), pre_defined },
	{ XOR( "skins_GLOCK_stattrak" ), pre_defined },
	{ XOR( "skins_GLOCK_seed" ), pre_defined },
	{ XOR( "skins_AK47_kit" ), pre_defined },
	{ XOR( "skins_AK47_stattrak" ), pre_defined },
	{ XOR( "skins_AK47_seed" ), pre_defined },
	{ XOR( "skins_AUG_kit" ), pre_defined },
	{ XOR( "skins_AUG_stattrak" ), pre_defined },
	{ XOR( "skins_AUG_seed" ), pre_defined },
	{ XOR( "skins_AWP_kit" ), pre_defined },
	{ XOR( "skins_AWP_stattrak" ), pre_defined },
	{ XOR( "skins_AWP_seed" ), pre_defined },
	{ XOR( "skins_FAMAS_kit" ), pre_defined },
	{ XOR( "skins_FAMAS_stattrak" ), pre_defined },
	{ XOR( "skins_FAMAS_seed" ), pre_defined },
	{ XOR( "skins_G3SG1_kit" ), pre_defined },
	{ XOR( "skins_G3SG1_stattrak" ), pre_defined },
	{ XOR( "skins_G3SG1_seed" ), pre_defined },
	{ XOR( "skins_GALIL_kit" ), pre_defined },
	{ XOR( "skins_GALIL_stattrak" ), pre_defined },
	{ XOR( "skins_GALIL_seed" ), pre_defined },
	{ XOR( "skins_M249_kit" ), pre_defined },
	{ XOR( "skins_M249_stattrak" ), pre_defined },
	{ XOR( "skins_M249_seed" ), pre_defined },
	{ XOR( "skins_M4A4_kit" ), pre_defined },
	{ XOR( "skins_M4A4_stattrak" ), pre_defined },
	{ XOR( "skins_M4A4_seed" ), pre_defined },
	{ XOR( "skins_MAC10_kit" ), pre_defined },
	{ XOR( "skins_MAC10_stattrak" ), pre_defined },
	{ XOR( "skins_MAC10_seed" ), pre_defined },
	{ XOR( "skins_P90_kit" ), pre_defined },
	{ XOR( "skins_P90_stattrak" ), pre_defined },
	{ XOR( "skins_P90_seed" ), pre_defined },
	{ XOR( "skins_MP5_SD_kit" ), pre_defined },
	{ XOR( "skins_MP5_SD_stattrak" ), pre_defined },
	{ XOR( "skins_MP5_SD_seed" ), pre_defined },
	{ XOR( "skins_UMP45_kit" ), pre_defined },
	{ XOR( "skins_UMP45_stattrak" ), pre_defined },
	{ XOR( "skins_UMP45_seed" ), pre_defined },
	{ XOR( "skins_XM1014_kit" ), pre_defined },
	{ XOR( "skins_XM1014_stattrak" ), pre_defined },
	{ XOR( "skins_XM1014_seed" ), pre_defined },
	{ XOR( "skins_BIZON_kit" ), pre_defined },
	{ XOR( "skins_BIZON_stattrak" ), pre_defined },
	{ XOR( "skins_BIZON_seed" ), pre_defined },
	{ XOR( "skins_MAG7_kit" ), pre_defined },
	{ XOR( "skins_MAG7_stattrak" ), pre_defined },
	{ XOR( "skins_MAG7_seed" ), pre_defined },
	{ XOR( "skins_NEGEV_kit" ), pre_defined },
	{ XOR( "skins_NEGEV_stattrak" ), pre_defined },
	{ XOR( "skins_NEGEV_seed" ), pre_defined },
	{ XOR( "skins_SAWEDOFF_kit" ), pre_defined },
	{ XOR( "skins_SAWEDOFF_stattrak" ), pre_defined },
	{ XOR( "skins_SAWEDOFF_seed" ), pre_defined },
	{ XOR( "skins_TEC9_kit" ), pre_defined },
	{ XOR( "skins_TEC9_stattrak" ), pre_defined },
	{ XOR( "skins_TEC9_seed" ), pre_defined },
	{ XOR( "skins_P2000_kit" ), pre_defined },
	{ XOR( "skins_P2000_stattrak" ), pre_defined },
	{ XOR( "skins_P2000_seed" ), pre_defined },
	{ XOR( "skins_MP7_kit" ), pre_defined },
	{ XOR( "skins_MP7_stattrak" ), pre_defined },
	{ XOR( "skins_MP7_seed" ), pre_defined },
	{ XOR( "skins_MP9_kit" ), pre_defined },
	{ XOR( "skins_MP9_stattrak" ), pre_defined },
	{ XOR( "skins_MP9_seed" ), pre_defined },
	{ XOR( "skins_NOVA_kit" ), pre_defined },
	{ XOR( "skins_NOVA_stattrak" ), pre_defined },
	{ XOR( "skins_NOVA_seed" ), pre_defined },
	{ XOR( "skins_P250_kit" ), pre_defined },
	{ XOR( "skins_P250_stattrak" ), pre_defined },
	{ XOR( "skins_P250_seed" ), pre_defined },
	{ XOR( "skins_SCAR20_kit" ), pre_defined },
	{ XOR( "skins_SCAR20_stattrak" ), pre_defined },
	{ XOR( "skins_SCAR20_seed" ), pre_defined },
	{ XOR( "skins_SG553_kit" ), pre_defined },
	{ XOR( "skins_SG553_stattrak" ), pre_defined },
	{ XOR( "skins_SG553_seed" ), pre_defined },
	{ XOR( "skins_SSG08_kit" ), pre_defined },
	{ XOR( "skins_SSG08_stattrak" ), pre_defined },
	{ XOR( "skins_SSG08_seed" ), pre_defined },
	{ XOR( "skins_M4A1S_kit" ), pre_defined },
	{ XOR( "skins_M4A1S_stattrak" ), pre_defined },
	{ XOR( "skins_M4A1S_seed" ), pre_defined },
	{ XOR( "skins_USPS_kit" ), pre_defined },
	{ XOR( "skins_USPS_stattrak" ), pre_defined },
	{ XOR( "skins_USPS_seed" ), pre_defined },
	{ XOR( "skins_CZ75A_kit" ), pre_defined },
	{ XOR( "skins_CZ75A_stattrak" ), pre_defined },
	{ XOR( "skins_CZ75A_seed" ), pre_defined },
	{ XOR( "skins_REVOLVER_kit" ), pre_defined },
	{ XOR( "skins_REVOLVER_stattrak" ), pre_defined },
	{ XOR( "skins_REVOLVER_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_BAYONET_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_BAYONET_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_BAYONET_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_FLIP_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_FLIP_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_FLIP_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_GUT_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_GUT_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_GUT_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_KARAMBIT_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_KARAMBIT_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_KARAMBIT_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_M9_BAYONET_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_M9_BAYONET_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_M9_BAYONET_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_HUNTSMAN_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_HUNTSMAN_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_HUNTSMAN_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_FALCHION_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_FALCHION_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_FALCHION_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_BOWIE_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_BOWIE_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_BOWIE_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_BUTTERFLY_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_BUTTERFLY_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_BUTTERFLY_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_SHADOW_DAGGERS_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_SHADOW_DAGGERS_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_SHADOW_DAGGERS_seed" ), pre_defined },

	{ XOR( "skins_KNIFE_CSS_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_CSS_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_CSS_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_CORD_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_CORD_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_CORD_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_CANIS_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_CANIS_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_CANIS_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_URSUS_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_URSUS_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_URSUS_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_GYPSY_JACKKNIFE_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_GYPSY_JACKKNIFE_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_GYPSY_JACKKNIFE_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_OUTDOOR_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_OUTDOOR_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_OUTDOOR_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_STILETTO_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_STILETTO_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_STILETTO_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_WIDOWMAKER_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_WIDOWMAKER_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_WIDOWMAKER_seed" ), pre_defined },
	{ XOR( "skins_KNIFE_SKELETON_kit" ), pre_defined },
	{ XOR( "skins_KNIFE_SKELETON_stattrak" ), pre_defined },
	{ XOR( "skins_KNIFE_SKELETON_seed" ), pre_defined },

	{ XOR( "spectator_list_x" ), pre_defined },
	{ XOR( "spectator_list_y" ), pre_defined },
	{ XOR( "keybind_list_x" ), pre_defined },
	{ XOR( "keybind_list_y" ), pre_defined },

};

bool Config::get_hotkey( std::string str )
{
	int vk = g_cfg[ str ].get< int >( );
	int activation_type = g_cfg[ str + XOR( "_type" ) ].get< int >( );

	switch( activation_type ) {
	case 0:
		if( m_hotkey_states.find( str ) == m_hotkey_states.end( ) ) {
			m_hotkey_states[ str ] = InputHelper::Down( vk );
		}

		else if( InputHelper::Pressed( vk ) ) {
			m_hotkey_states[ str ] = !m_hotkey_states[ str ];
		}
		return m_hotkey_states[ str ];
		break;
	case 1:
		return InputHelper::Down( vk );
		break;
	case 2:
		return !InputHelper::Down( vk );
		break;
	case 3:
		return true;
		break;
	}

	return false;
}

void Config::init( ) {
	// reset.
	m_init = false;

	// alloc.
	m_path.resize( MAX_PATH + 1 );

	// get the path to mydocuments.
	if ( !SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, ( char* )m_path.data( ) ) ) )
		return;

	// we found the path.
	m_init = true;
}

void Config::save( std::string name, bool to_clipboard ) {
	if( !m_init ) {
		return;
	}

	static TCHAR path[256];
	std::string folder, file;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		folder = std::string( path ) + ( XOR( "\\undercover.host\\" ) );
		file = std::string( path ) + ( XOR( "\\undercover.host\\" ) + name + XOR( ".ini" ) );

		CreateDirectory( folder.c_str( ), NULL );
		for( auto& e : g_cfg ) {
			WritePrivateProfileStringA( XOR( "undercover.host" ), e.first.c_str( ), std::to_string( e.second.get< double >( ) ).c_str( ), file.c_str( ) );
		}

		g_notify.add( XOR( "config saved." ) );
	}

	refresh( );
}

void Config::load( std::string name, bool from_clipboard ) {
	if( !m_init ) {
		return;
	}

	static TCHAR path[ 256 ]; // enough?
	std::string folder, file;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		file = std::string( path ) + ( XOR( "\\undercover.host\\" ) + name + XOR( ".ini" ) );

		for( auto& e : g_cfg ) {
			char value[ 64 ] = { '\0' };

			GetPrivateProfileStringA( XOR( "undercover.host" ), e.first.c_str( ), "", value, 64, file.c_str( ) );

			e.second.set< double >( atof( value ) );
		}

		g_notify.add( XOR( "config loaded." ) );
	}

	g_skins.m_update = true;
}

void Config::delet( std::string name ) {
	if ( !m_init ) {
		return;
	}

	static TCHAR path[ 256 ]; // enough?
	std::string folder, file;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		file = std::string( path ) + ( XOR( "\\undercover.host\\" ) + name + XOR( ".ini" ) );

		if ( remove( file.c_str( ) ) != 0 )
			g_notify.add( XOR( "error deleting config." ) );
		else
			g_notify.add( XOR( "config deleted." ) );
	}

	g_config.refresh( );
}

typedef void( *LPSEARCHFUNC )( LPCTSTR lpszFileName );

BOOL search_files( LPCTSTR lpszFileName, LPSEARCHFUNC lpSearchFunc, BOOL bInnerFolders = FALSE )
{
	LPTSTR part;
	char tmp[ MAX_PATH ];
	char name[ MAX_PATH ];

	HANDLE hSearch = NULL;
	WIN32_FIND_DATA wfd;
	memset( &wfd, 0, sizeof( WIN32_FIND_DATA ) );

	if ( bInnerFolders )
	{
		if ( GetFullPathName( lpszFileName, MAX_PATH, tmp, &part ) == 0 ) return FALSE;
		strcpy( name, part );
		strcpy( part, XOR( "*.*" ) );
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		if ( !( ( hSearch = FindFirstFile( tmp, &wfd ) ) == INVALID_HANDLE_VALUE ) )
			do
			{
				if ( !strncmp( wfd.cFileName, XOR( "." ), 1 ) || !strncmp( wfd.cFileName, XOR( ".." ), 2 ) )
					continue;

				if ( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					char next[ MAX_PATH ];
					if ( GetFullPathName( lpszFileName, MAX_PATH, next, &part ) == 0 ) return FALSE;
					strcpy( part, wfd.cFileName );
					strcat( next, XOR( XOR( "\\" ) ) );
					strcat( next, name );

					search_files( next, lpSearchFunc, TRUE );
				}
			} while ( FindNextFile( hSearch, &wfd ) );
			FindClose( hSearch );
	}

	if ( ( hSearch = FindFirstFile( lpszFileName, &wfd ) ) == INVALID_HANDLE_VALUE )
		return TRUE;
	do
		if ( !( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			char file[ MAX_PATH ];
			if ( GetFullPathName( lpszFileName, MAX_PATH, file, &part ) == 0 ) return FALSE;
			strcpy( part, wfd.cFileName );

			lpSearchFunc( wfd.cFileName );
		}
	while ( FindNextFile( hSearch, &wfd ) );
	FindClose( hSearch );
	return TRUE;
}

void read_configs( LPCTSTR lpszFileName )
{
	// temporary string.
	std::string out_name = lpszFileName;
	g_config.configs.push_back( out_name.substr( 0, out_name.size( ) - 4 ) );
}

void handle_refresh_controller( )
{
	static TCHAR path[ 256 ];
	std::string folder, file;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		g_config.configs.clear( );
		std::string config_dir = std::string( path ) + ( XOR( "\\undercover.host\\*.ini" ) ); 
		search_files( config_dir.c_str( ), read_configs, FALSE );
	}

}

void Config::refresh( )
{
	handle_refresh_controller( );
}
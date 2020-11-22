#include "undercover.h"

bool Hooks::OverrideConfig( MaterialSystem_Config_t* config, bool update ) {
	if(g_cfg[XOR("misc_world_fullbright")].get<bool>())
		config->m_nFullbright = true;

	static bool should_update = false;
	static bool did_nightmode_change = false;
	static int last_nightmode_value;
	static bool last_nightmode_toggle = false;

	if (!g_cl.m_local) {
		should_update = true;
	}

	if (last_nightmode_value != g_cfg[XOR("misc_world_night_darkness")].get<int>() || last_nightmode_toggle != g_cfg[XOR("misc_world_night")].get<bool>()) {
		did_nightmode_change = true;
	}
	
	// philip015 added this to his cheat and it made it so much better.
	if (should_update || did_nightmode_change) {
		last_nightmode_value = g_cfg[XOR("misc_world_night_darkness")].get<int>();
		last_nightmode_toggle = g_cfg[XOR("misc_world_night")].get<bool>();

		g_visuals.ModulateWorld();
		g_visuals.ManipulateSkybox();

		should_update = false;
		did_nightmode_change = false;
	}

	return g_hooks.m_material_system.GetOldMethod< OverrideConfig_t >( IMaterialSystem::OVERRIDECONFIG )( this, config, update );
}
#include "undercover.h"

Visuals g_visuals {};;

void Visuals::ModulateWorld() {
	if (!g_cl.m_local || !g_csgo.m_engine->IsInGame())
		return;

	std::vector< IMaterial* > world,props;

	// iterate material handles.
	for (uint16_t h {g_csgo.m_material_system->FirstMaterial()}; h != g_csgo.m_material_system->InvalidMaterial(); h = g_csgo.m_material_system->NextMaterial(h)) {
		// get material from handle.
		IMaterial* mat = g_csgo.m_material_system->GetMaterial(h);
		if (!mat)
			continue;

		if (mat->IsErrorMaterial())
			continue;

		// store world materials.
		if (FNV1a::get(mat->GetTextureGroupName()) == HASH("World textures"))
			world.push_back(mat);

		// store props.
		else if (FNV1a::get(mat->GetTextureGroupName()) == HASH("StaticProp textures"))
			props.push_back(mat);

	}


	// night.
	if (g_cfg[XOR("misc_world_night")].get< bool >()) {
		const float world_darkness = 1.f - (g_cfg[XOR("misc_world_night_darkness")].get< float >() / 100.f);
		const float prop_darkness = 1.3f - (g_cfg[XOR("misc_world_night_darkness")].get< float >() / 100.f);
		const float prop_alpha = g_cfg[XOR("misc_world_prop_opacity")].get< float >() < 99.f ? g_cfg[XOR("misc_world_prop_opacity")].get< float >() * 0.01f : 1.f;

		for (const auto& w : world)
			w->ColorModulate(world_darkness,world_darkness,world_darkness);

		// IsUsingStaticPropDebugModes my nigga
		g_csgo.r_DrawSpecificStaticProp->SetValue(0);

		for (const auto& p : props) {
			p->ColorModulate(prop_darkness,prop_darkness,prop_darkness);
			p->AlphaModulate(prop_alpha);
		}
	}

	// disable night.
	else {
		const float prop_alpha = g_cfg[XOR("misc_world_prop_opacity")].get< float >() < 99.f ? g_cfg[XOR("misc_world_prop_opacity")].get< float >() * 0.01f : 1.f;

		for (const auto& w : world)
			w->ColorModulate(1.f,1.f,1.f);

		// restore r_DrawSpecificStaticProp.
		g_csgo.r_DrawSpecificStaticProp->SetValue(-1);

		for (const auto& p : props) {
			p->ColorModulate(1.f,1.f,1.f);
			p->AlphaModulate(prop_alpha);
		}

		// vertigo!
		//game::SetSkybox( XOR( "vertigoblue_hdr" ) );
	}
}

void Visuals::VisualInterpolation() {

	if (g_cl.m_stage && g_cl.m_local) {
		auto interval_per_tick = g_csgo.m_globals->m_interval;// g_pGlobalVars->m_interval
		auto visual_interp_amt_2 = interval_per_tick * 2;

		if (g_cfg[XOR("visuals_vis_interpolation_amt")].get< int >() >= 0) {
			if (g_cfg[XOR("visuals_vis_interpolation_dead")].get< bool >() && !g_cl.m_local->alive() || !g_cfg[XOR("visuals_vis_interpolation_dead")].get< bool >())
				visual_interp_amt_2 = g_cfg[XOR("visuals_vis_interpolation_amt")].get< int >() * interval_per_tick;
			else
				visual_interp_amt_2 = 0;
		}

		auto entity_index = 1;
		auto visual_interp_amt = 0;
		do
		{
			auto entity = (DWORD) g_csgo.m_entlist->GetClientEntity(entity_index);
			if (entity && entity != (DWORD) g_cl.m_local)
			{
				auto v27 = *(DWORD*) (entity + 0x24);
				*(short*) (v27 + 0xE) = 0;
				*(DWORD*) (*(DWORD*) (v27 + 0x14) + 0x24) = 0;
				auto v29 = *(DWORD*) (entity + 0x24);
				*(short*) (v29 + 0x1A) = 0;
				*(DWORD*) (*(DWORD*) (v29 + 0x20) + 0x24) = 0;
				auto v30 = *(DWORD*) (entity + 0x24);
				*(short*) (v30 + 0x26) = 0;
				*(DWORD*) (*(DWORD*) (v30 + 0x2C) + 0x24) = 0;
				auto v31 = *(DWORD*) (entity + 0x24);
				*(short*) (v31 + 0x32) = 0;
				*(DWORD*) (*(DWORD*) (v31 + 0x38) + 0x24) = 0;

				if (g_cl.m_local)
					visual_interp_amt = visual_interp_amt_2;
				else
					visual_interp_amt = 0.0;

				*(float*) (*(DWORD*) (*(DWORD*) (entity + 0x24) + 8) + 0x24) = /*visual_interp_amt*/ visual_interp_amt_2;
				*(float*) (*(DWORD*) (*(DWORD*) (entity + 0x24) + 0x44) + 0x24) = visual_interp_amt_2;
			}
			++entity_index;
		} while (entity_index < 65);
	}
}

void Visuals::RenderAutoPeek() {
	render::filled_world_circle(g_movement.m_peek_origin,15,{73,140,222,130});
}

void Visuals::ManipulateSkybox() {
	switch (g_cfg[XOR("misc_world_skybox")].get< int >()) {
		case 0:
		break;
		case 1:
		game::SetSkybox(XOR("sky_csgo_night02"));
		break;
		case 2:
		game::SetSkybox(XOR("sky_csgo_night02b"));
		break;
		case 3:
		game::SetSkybox(XOR("sky_cs15_daylight02_hdr"));
		break;
		case 4:
		game::SetSkybox(XOR("nukeblank"));
		break;
		case 5:
		game::SetSkybox(XOR("cs_baggage_skybox_"));
		break;
	}
}

void Visuals::Zoom(CViewSetup* view) {

	float config_value = (g_cfg[XOR("misc_zoom_fov")].get< float >() / 100.f);

	float v45;
	static float dword_43FFAE60;

	if (!g_visuals.m_zoom)
	{
		v45 = -1.0;
	}
	else
	{
		v45 = 1.0;
	}
	float v46 = ((g_csgo.m_globals->m_frametime) * v45) + dword_43FFAE60;
	float v47 = fmaxf(fminf(0.23,v46),0.0);
	dword_43FFAE60 = v47;
	float v48 = v47 / 0.23;
	if (v48 > 0.0)
	{
		float v49 = (view->m_fov);
		float v50 = (1.0 - fmaxf(fminf(1.0,config_value * 0.89999998),0.0)) * v49;
		float v51 = ((v49 - v50) * 0.1) + v50;
		v46 = ((v51 - v49) * v48) + v49;
		(view->m_fov) = (((((v50 - v51) * v48) + v51) - v46) * v48) + v46;
	}
}

void Visuals::ThirdpersonThink() {
	ang_t                          offset;
	vec3_t                         origin,forward;
	static CTraceFilterSimple_game filter {};
	CGameTrace                     tr;
	static float				   progress {};

	// for whatever reason overrideview also gets called from the main menu.
	if (!g_csgo.m_engine->IsInGame())
		return;

	// check if we have a local player and he is alive.
	bool alive = g_cl.m_local && g_cl.m_local->alive();

	// camera should be in thirdperson.
	if (m_thirdperson || !g_cl.m_processing) {

		// if alive and not in thirdperson already switch to thirdperson.
		if (alive && !g_csgo.m_input->m_camera_in_third_person) {
			g_csgo.m_input->m_camera_in_third_person = true;
		}

		// if dead and spectating in firstperson switch to thirdperson.
		else if (g_cl.m_local->m_iObserverMode() == 4) {

			// if in thirdperson, switch to firstperson.
			// we need to disable thirdperson to spectate properly.
			if (g_csgo.m_input->m_camera_in_third_person) {
				g_csgo.m_input->m_camera_in_third_person = false;
				g_csgo.m_input->m_camera_offset.z = 0.f;
			}

			g_cl.m_local->m_iObserverMode() = 5;
		}
	}

	// camera should be in firstperson.
	else if (g_csgo.m_input->m_camera_in_third_person) {

		// animate backwards.
		progress -= g_csgo.m_globals->m_frametime * 5.f + (progress / 100);

		// clamp.
		math::clamp(progress,0.f,1.f);
		g_csgo.m_input->m_camera_offset.z = g_cfg[XOR("misc_thirdperson_distance")].get< float >() * progress;

		// set to first person.
		if (!progress)
			g_csgo.m_input->m_camera_in_third_person = false;
	}

	// if after all of this we are still in thirdperson.
	if (g_csgo.m_input->m_camera_in_third_person) {
		// get camera angles.
		g_csgo.m_engine->GetViewAngles(offset);

		// get our viewangle's forward directional vector.
		math::AngleVectors(offset,&forward);

		// start the animation.
		if (m_thirdperson)
			progress += g_csgo.m_globals->m_frametime * 5.f + (progress / 100);

		// clamp. 
		math::clamp(progress,0.f,1.f);

		// cam_idealdist convar.
		offset.z = g_cfg[XOR("misc_thirdperson_distance")].get< float >() * progress;

		// start pos.
		origin = g_cl.m_shoot_pos;

		// setup trace filter and trace.
		filter.SetPassEntity(g_cl.m_local);

		g_csgo.m_engine_trace->TraceRay(
			Ray(origin,origin - (forward * offset.z),{-16.f,-16.f,-16.f},{16.f,16.f,16.f}),
			MASK_NPCWORLDSTATIC,
			(ITraceFilter*) &filter,
			&tr
		);

		// adapt distance to travel time.
		math::clamp(tr.m_fraction,0.f,1.f);
		offset.z *= tr.m_fraction;

		// override camera angles.
		g_csgo.m_input->m_camera_offset = {offset.x,offset.y,offset.z};
	}

	// update the old value.
	m_old_thirdperson = m_thirdperson;
}

void Visuals::HitmarkerScreen() {
	if (g_cfg[XOR("misc_hitmarker")].get< int >() != 1)
		return;

	if (g_csgo.m_globals->m_curtime > m_hit_end)
		return;

	if (m_hit_duration <= 0.f)
		return;

	float complete = (g_csgo.m_globals->m_curtime - m_hit_start) / m_hit_duration;
	int x = g_cl.m_width / 2,
		y = g_cl.m_height / 2,
		alpha = (1.f - complete) * 240;

	constexpr int line {6};

	render::line(x - line,y - line,x - (line / 4),y - (line / 4),{200,200,200,alpha});
	render::line(x - line,y + line,x - (line / 4),y + (line / 4),{200,200,200,alpha});
	render::line(x + line,y + line,x + (line / 4),y + (line / 4),{200,200,200,alpha});
	render::line(x + line,y - line,x + (line / 4),y - (line / 4),{200,200,200,alpha});
}

void Visuals::HitmarkerWorld() {
	if (!g_cfg[XOR("misc_hitmarker")].get<bool>())
		return;

	for (size_t i {}; i < m_world_hitmarkers.size(); ++i) {
		WorldHitmarkerData_t& info = m_world_hitmarkers[i];

		// If the delta between the current time and hurt time is larger than 2 seconds then we should erase
		if (g_inputpred.m_stored_variables.m_flCurtime - info.m_time > 2.0f) {
			info.m_alpha -= (1.0f / 2.0f) * g_csgo.m_globals->m_frametime;
			info.m_alpha = std::clamp<float>(info.m_alpha,0.0f,1.0f);
		}

		if (info.m_alpha <= 0.0f) {
			continue;
		}

		if (info.m_world_to_screen) {
			constexpr int line {8};

			auto draw_lines = [&] (vec2_t pos_custom,Color clr) -> void {
				render::line(
					vec2_t(int(pos_custom.x - (line - 1)),int(pos_custom.y - (line - 1))),
					vec2_t(int(pos_custom.x - (line / 3)),int(pos_custom.y - (line / 3))),
					clr.OverrideAlpha(255 * info.m_alpha));

				render::line(
					vec2_t(int(pos_custom.x - (line - 1)),int(pos_custom.y + (line - 1))),
					vec2_t(int(pos_custom.x - (line / 3)),int(pos_custom.y + (line / 3))),
					clr.OverrideAlpha(255 * info.m_alpha));

				render::line(
					vec2_t(int(pos_custom.x + (line - 1)),int(pos_custom.y + (line - 1))),
					vec2_t(int(pos_custom.x + (line / 3)),int(pos_custom.y + (line / 3))),
					clr.OverrideAlpha(255 * info.m_alpha));

				render::line(
					vec2_t(int(pos_custom.x + (line - 1)),int(pos_custom.y - (line - 1))),
					vec2_t(int(pos_custom.x + (line / 3)),int(pos_custom.y - (line / 3))),
					clr.OverrideAlpha(255 * info.m_alpha));

			};

			// hitmarker
			draw_lines(info.m_world_pos + vec2_t(1,1),Color::Palette_t::White());

		}
	}
}

void Visuals::NoSmoke() {
	if (!smoke1)
		smoke1 = g_csgo.m_material_system->FindMaterial(XOR("particle/vistasmokev1/vistasmokev1_fire"),XOR("Other textures"));

	if (!smoke2)
		smoke2 = g_csgo.m_material_system->FindMaterial(XOR("particle/vistasmokev1/vistasmokev1_smokegrenade"),XOR("Other textures"));

	if (!smoke3)
		smoke3 = g_csgo.m_material_system->FindMaterial(XOR("particle/vistasmokev1/vistasmokev1_emods"),XOR("Other textures"));

	if (!smoke4)
		smoke4 = g_csgo.m_material_system->FindMaterial(XOR("particle/vistasmokev1/vistasmokev1_emods_impactdust"),XOR("Other textures"));

	if (g_cfg[XOR("visuals_misc_remove_smoke")].get<bool>()) {
		if (!smoke1->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke1->SetFlag(MATERIAL_VAR_NO_DRAW,true);

		if (!smoke2->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke2->SetFlag(MATERIAL_VAR_NO_DRAW,true);

		if (!smoke3->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke3->SetFlag(MATERIAL_VAR_NO_DRAW,true);

		if (!smoke4->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke4->SetFlag(MATERIAL_VAR_NO_DRAW,true);

		if (g_csgo.SmokeCount.get() != 0)
			g_csgo.SmokeCount.set(0);
	}

	else {
		if (smoke1->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke1->SetFlag(MATERIAL_VAR_NO_DRAW,false);

		if (smoke2->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke2->SetFlag(MATERIAL_VAR_NO_DRAW,false);

		if (smoke3->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke3->SetFlag(MATERIAL_VAR_NO_DRAW,false);

		if (smoke4->GetFlag(MATERIAL_VAR_NO_DRAW))
			smoke4->SetFlag(MATERIAL_VAR_NO_DRAW,false);
	}
}

void Visuals::think() {
	// don't run anything if our local player isn't valid.
	if (!g_cl.m_local)
		return;

	//g_csgo.cl_use_new_headbob->SetValue( !m_thirdperson );

	if (g_cfg[XOR("visuals_misc_remove_scope")].get< bool >()
		&& g_cl.m_local->alive()
		&& g_cl.m_local->GetActiveWeapon()
		&& g_cl.m_local->GetActiveWeapon()->IsZoomable()
		&& g_cl.m_local->m_bIsScoped()) {

		// rebuild the original scope lines.
		int w = g_cl.m_width,
			h = g_cl.m_height,
			x = w / 2,
			y = h / 2,
			size = 1;

		// Here We Use The Euclidean Distance To Get The Polar-Rectangular Conversion Formula.
		if (size > 1) {
			x -= (size / 2);
			y -= (size / 2);
		}

		// draw our lines.
		render::rect_filled(0,y,w,size,colors::black);
		render::rect_filled(x,0,size,h,colors::black);
	}

	// draw esp on ents.
	for (int i {1}; i <= g_csgo.m_entlist->GetHighestEntityIndex(); ++i) {
		Entity* ent = g_csgo.m_entlist->GetClientEntity(i);
		if (!ent)
			continue;

		draw(ent);
	}

	HitmarkerWorld();
	PenetrationCrosshair();
	Fog();

	// world to screen world hitmarkers in a game thread.
	for (size_t i {}; i < m_world_hitmarkers.size(); ++i) {
		WorldHitmarkerData_t& info = m_world_hitmarkers[i];
		info.m_world_to_screen = render::WorldToScreen(vec3_t(info.m_pos_x,info.m_pos_y,info.m_pos_z),info.m_world_pos);
	}
}

void Visuals::Watermark() {
	static vec2_t                     position;

	// get time.
	time_t t = std::time(nullptr);
	std::ostringstream time;
	time << std::put_time(std::localtime(&t),(XOR("%H:%M:%S")));

	// get round trip time in milliseconds.
	int ms = std::max(0,(int) std::round(g_cl.m_latency * 1000.f));

	// get username 
	extern t_Username GetUsername;

	// get tickrate.
	int rate = (int) std::round(1.f / g_csgo.m_globals->m_interval);

	// https://github.com/perilouswithadollarsign/cstrike15_src/blob/29e4c1fda9698d5cebcdaf1a0de4b829fa149bf8/game/client/c_memorylog.cpp#L79
	bool local = g_csgo.m_engine->GetNetChannelInfo() ? strstr(g_csgo.m_engine->GetNetChannelInfo()->GetAddress(),XOR("loopback")) : XOR("");

	// get server ip / type
	const char* server_ip = g_csgo.m_engine->GetNetChannelInfo() ? g_csgo.m_engine->GetNetChannelInfo()->GetAddress() : XOR("");

	// todo: clean this up and add a not ingame indicator.
	std::string text = g_csgo.m_engine->IsInGame() ? tfm::format(XOR("undercover.host | %i | delay: %ims | %itick | %s | %s"),GetUsername,ms,rate,local ? XOR("local server") : server_ip,time.str().data()) : tfm::format(XOR("undercover.host | %i | in menu | %s"),GetUsername,time.str().data());

	// render background.
	render::rect_filled(position.x,position.y,render::esp.size(text).m_width + 10,19,Color(30,30,30,120));

	//bourdeer leftft
	render::rect_filled(position.x,position.y,19,1,Color(255,0,0,180));
	render::rect_filled(position.x,position.y,1,19,Color(255,0,0,180));

	//bourdeer raightht
	render::rect_filled(position.x + render::esp.size(text).m_width - 11,position.y + 18,20,1,Color(255,0,0,180));
	render::rect_filled(position.x + render::esp.size(text).m_width + 9,position.y,1,19,Color(255,0,0,180));


	// render text.
	render::esp.string(position.x + 5,position.y + 3,{255,255,255,255},text,render::ALIGN_LEFT);

	// set our initial position.
	position = vec2_t(g_cl.m_width - 12 - render::esp.size(text).m_width - 10,12);

}

void Visuals::Fog() {
	static bool              enable,set;
	static int               end,density;
	static Color             clr;

	if (enable != g_cfg[XOR("fog_enable")].get< bool >() || end != g_cfg[XOR("fog_length")].get< int >() || density != g_cfg[XOR("fog_density")].get< int >()
		|| clr != g_cfg[XOR("fog_color")].get_color()) {

		set = false;

		// set variables back.
		enable = g_cfg[XOR("fog_enable")].get< bool >();
		end = g_cfg[XOR("fog_length")].get< int >();
		density = g_cfg[XOR("fog_density")].get< int >();
		clr = g_cfg[XOR("fog_color")].get_color();
	}


	// disabled.
	if (!set) {
		g_csgo.fog_override->SetValue(g_cfg[XOR("fog_enable")].get< bool >());
		g_csgo.fog_start->SetValue(0);
		g_csgo.fog_end->SetValue(g_cfg[XOR("fog_length")].get< int >());
		g_csgo.fog_maxdensity->SetValue((float) g_cfg[XOR("fog_density")].get< int >() / 100);

		Color col = g_cfg[XOR("fog_color")].get_color();

		// weird valve formatting...
		std::string color = tfm::format(XOR("%i %i %i"),col.r(),col.g(),col.b());
		g_csgo.fog_color->SetValue(color.c_str());

		set = true;
	}
}

void Visuals::Spectators() {
	static vec2_t					  main_mouse,last_mouse,s_drag;
	bool                              is_dragging;
	std::vector< std::string >        spectators {XOR("")};
	vec2_t							  position = vec2_t(100,100);

	if (!g_cfg[XOR("misc_spectators")].get< bool >())
		return;

	// get mouse position.
	g_csgo.m_surface->gain_mouse_pos(main_mouse,last_mouse);

	// dragging logic.
	if (main_mouse.x > position.x - s_drag.x && main_mouse.y > position.y - s_drag.y && main_mouse.x < (position.x - s_drag.x) + 200 && main_mouse.y < (position.y - s_drag.y) + 20 && GetAsyncKeyState(VK_LBUTTON) && g_menu.m_opened) {
		s_drag.x += main_mouse.x - last_mouse.x;
		s_drag.y += main_mouse.y - last_mouse.y;
		is_dragging = true;
	}

	// set our new position.
	position.x -= s_drag.x;
	position.y -= s_drag.y;

	if (g_csgo.m_engine->IsInGame()) {

		// collect spectators
		for (int i {1}; i <= g_csgo.m_globals->m_max_clients; ++i) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);
			if (!player)
				continue;

			if (player->m_bIsLocalPlayer())
				continue;

			if (player->dormant())
				continue;

			if (player->m_lifeState() == LIFE_ALIVE)
				continue;

			if (player->GetObserverTarget() != g_cl.m_local)
				continue;

			player_info_t info;
			if (!g_csgo.m_engine->GetPlayerInfo(i,&info))
				continue;

			spectators.push_back(std::string(info.m_name).substr(0,24));
		}

	}

	size_t total_size = spectators.size() * (15);

	// render background.
	render::rect_filled(position.x,position.y,160,19,Color(30,30,30,120));

	//bourdeer leftft
	render::rect_filled(position.x,position.y,19,1,Color(255,0,0,180));
	render::rect_filled(position.x,position.y,1,19,Color(255,0,0,180));

	//bourdeer raightht
	render::rect_filled(position.x + 160 - 20,position.y + 18,20,1,Color(255,0,0,180));
	render::rect_filled(position.x + 160,position.y,1,19,Color(255,0,0,180));

	// render text.
	render::esp.string(position.x + 80,position.y + 3,{255,255,255,255},XOR("Spectators"),render::ALIGN_CENTER);

	if (spectators.empty())
		return;

	for (size_t i {}; i < spectators.size(); ++i) {
		const std::string& name = spectators[i];
		render::esp.string(position.x + 4,(position.y + 7) + (i * (15)),
			{255,255,255,255},name,render::ALIGN_LEFT);
	}
}

void Visuals::Keybinds() {
	struct Indicator_t { Color color; std::string text; };
	std::vector< Indicator_t >			indicators {};

	if (!g_cfg[XOR("misc_keybinds")].get< bool >())
		return;

	// dont do if dead.
	if (g_cl.m_processing) {

		if ( g_hvh.m_fake_duck ) {
			Indicator_t ind{ };
			ind.text = XOR( "Fake Duck" );
			indicators.push_back( ind );
		}

		if ( g_config.get_hotkey( XOR( "aimbot_exploits_teleport_key" ) ) && !g_hvh.m_fake_duck) {
			Indicator_t ind{ };
			ind.text = XOR( "Doubletap" );
			indicators.push_back( ind );
		}

		if ( g_config.get_hotkey( XOR( "aimbot_hide_shots_key" ) ) && !g_hvh.m_fake_duck ) {
			Indicator_t ind{ };
			ind.text = XOR( "Hide Shots" );
			indicators.push_back( ind );
		}

		if (g_movement.m_started_peeking) {
			Indicator_t ind {};
			ind.text = XOR("Auto Peek");
			indicators.push_back(ind);
		}

		if (g_movement.m_slow_motion) {
			Indicator_t ind {};
			ind.text = XOR("Controlled Walk");
			indicators.push_back(ind);
		}

		if (g_aimbot.m_override_damage) {
			Indicator_t ind {};
			ind.text = XOR("Override Damage");
			indicators.push_back(ind);
		}

		if (g_aimbot.m_override_hitboxes) {
			Indicator_t ind {};
			ind.text = XOR("Override Hitboxes");
			indicators.push_back(ind);
		}

		if (g_aimbot.m_force_safepoint) {
			Indicator_t ind {};
			ind.text = XOR("Force Safepoint");
			indicators.push_back(ind);
		}

		if (g_movement.m_edge_jump) {
			Indicator_t ind {};
			ind.text = XOR("Edge Jump");
			indicators.push_back(ind);
		}

		if (g_visuals.m_zoom) {
			Indicator_t ind {};
			ind.text = XOR("Zoom");
			indicators.push_back(ind);
		}

	}

	static vec2_t main_mouse;
	static vec2_t last_mouse;
	static vec2_t s_drag;

	g_csgo.m_surface->gain_mouse_pos(main_mouse,last_mouse);

	vec2_t position = vec2_t(100,200);
	bool dragging = false;

	if (main_mouse.x > position.x - s_drag.x && main_mouse.y > position.y - s_drag.y && main_mouse.x < (position.x - s_drag.x) + 200 && main_mouse.y < (position.y - s_drag.y) + 20 && GetAsyncKeyState(VK_LBUTTON) && g_menu.m_opened) {
		s_drag.x += main_mouse.x - last_mouse.x;
		s_drag.y += main_mouse.y - last_mouse.y;
		dragging = true;
	}

	position.x -= s_drag.x;
	position.y -= s_drag.y;

	// render background.
	render::rect_filled(position.x,position.y,160,19,Color(30,30,30,120));

	//bourdeer leftft
	render::rect_filled(position.x,position.y,19,1,Color(255,0,0,180));
	render::rect_filled(position.x,position.y,1,19,Color(255,0,0,180));

	//bourdeer raightht
	render::rect_filled(position.x + 160 - 20,position.y + 18,20,1,Color(255,0,0,180));
	render::rect_filled(position.x + 160,position.y,1,19,Color(255,0,0,180));

	// render text.
	render::esp.string(position.x + 80,position.y + 3,{255,255,255,255},XOR("Keybinds"),render::ALIGN_CENTER);

	if (indicators.empty())
		return;

	// iterate and draw indicators.
	for (size_t i {}; i < indicators.size(); ++i) {
		auto& indicator = indicators[i];

		render::esp.string(position.x + 5,(position.y + 20) + (i * (15)),
			{255,255,255,255},indicator.text,render::ALIGN_LEFT);
	}
}

void Visuals::SpreadCrosshair() {
	// dont do if dead.
	if (!g_cl.m_processing)
		return;

	if (!g_cfg[XOR("visuals_misc_spread_crosshair")].get<bool>())
		return;

	// get active weapon.
	Weapon* weapon = g_cl.m_local->GetActiveWeapon();
	if (!weapon)
		return;

	WeaponInfo* data = weapon->GetWpnData();
	if (!data)
		return;

	// do not do this on: bomb, knife and nades.
	int type = data->WeaponType;
	if (type == WEAPONTYPE_KNIFE || type == WEAPONTYPE_EQUIPMENT || weapon->IsGrenade())
		return;

	// calc radius.
	float radius = ((weapon->GetInaccuracy() + weapon->GetSpread()) * 320.f) / (std::tan(math::deg_to_rad(g_cl.m_local->GetFOV()) * 0.5f) + FLT_EPSILON);

	// scale by screen size.
	radius *= g_cl.m_height * (1.f / 480.f);

	// get color.
	Color col = g_cfg[XOR("visuals_misc_spread_crosshair_color")].get_color();

	int segements = std::max(16,(int) std::round(radius * 0.75f));
	render::circle(g_cl.m_width / 2,g_cl.m_height / 2,radius,segements,col);
}

void Visuals::PenetrationCrosshair() {
	int   x,y;
	Color final_color;

	if (!g_cfg[XOR("visuals_misc_penetration_crosshair")].get<bool>() || !g_cl.m_processing)
		return;

	x = g_cl.m_width / 2;
	y = g_cl.m_height / 2;

	if (g_cl.m_pen_data.m_pen)
		final_color = colors::transparent_green;

	else
		final_color = colors::red;

	// todo - dex; use fmt library to get damage string here?
	//             draw damage string?

	// draw small square in center of screen.
	render::rect_filled(x - 1,y - 1,2,2,final_color);
}

void Visuals::draw(Entity* ent) {
	if (ent->IsPlayer()) {
		Player* player = ent->as< Player* >();

		// dont draw dead players.
		if (!player->alive())
			return;

		if (player->m_bIsLocalPlayer())
			return;

		// draw player esp.
		DrawPlayer(player);
	}

	else if ((g_cfg[XOR("visuals_misc_dropped_bomb")].get<bool>() || g_cfg[XOR("visuals_misc_dropped_weapons")].get<bool>() || g_cfg[XOR("visuals_misc_dropped_weapons_ammo")].get<bool>()) && ent->IsBaseCombatWeapon() && !ent->dormant())
		DrawItem(ent->as< Weapon* >());

	else if (g_cfg[XOR("visuals_misc_projectiles")].get<bool>())
		DrawProjectile(ent->as< Weapon* >());

	DrawPlantedC4(ent->as< Weapon* >());
}

void Visuals::DrawProjectile(Weapon* ent) {
	vec2_t screen;
	vec3_t origin = ent->GetAbsOrigin();
	if (!render::WorldToScreen(origin,screen))
		return;

	Color col = g_cfg[XOR("visuals_misc_projectiles_color")].get_color();

	// draw decoy.
	if (ent->is(HASH("CDecoyProjectile")))
		render::pixel.string(screen.x,screen.y,col,XOR("DECOY"),render::ALIGN_CENTER);

	// draw molotov.
	else if (ent->is(HASH("CMolotovProjectile")))
		render::pixel.string(screen.x,screen.y,col,XOR("FIRE"),render::ALIGN_CENTER);

	else if (ent->is(HASH("CBaseCSGrenadeProjectile"))) {
		const model_t* model = ent->GetModel();

		if (model) {
			// grab modelname.
			std::string name {ent->GetModel()->m_name};

			if (name.find(XOR("flashbang")) != std::string::npos)
				render::pixel.string(screen.x,screen.y,col,XOR("FLASH"),render::ALIGN_CENTER);

			else if (name.find(XOR("fraggrenade")) != std::string::npos) {
				render::pixel.string(screen.x,screen.y,col,XOR("FRAG"),render::ALIGN_CENTER);
			}
		}
	}

	// find classes.
	else if (ent->is(HASH("CInferno"))) {
		render::world_circle(origin,120.f,g_cfg[XOR("visuals_misc_projectiles_range_color")].get_color().OverrideAlpha(180,true));
		render::pixel.string(screen.x,screen.y,col,XOR("FIRE"),render::ALIGN_CENTER);
	}

	else if (ent->is(HASH("CSmokeGrenadeProjectile"))) {
		render::world_circle(origin,144.f,g_cfg[XOR("visuals_misc_projectiles_range_color")].get_color().OverrideAlpha(180,true));
		render::pixel.string(screen.x,screen.y,col,XOR("SMOKE"),render::ALIGN_CENTER);
	}
}

void Visuals::DrawItem(Weapon* item) {
	// we only want to draw shit without owner.
	Entity* owner = g_csgo.m_entlist->GetClientEntityFromHandle(item->m_hOwnerEntity());
	if (owner)
		return;

	// is the fucker even on the screen?
	vec2_t screen;
	vec3_t origin = item->GetAbsOrigin();
	if (!render::WorldToScreen(origin,screen))
		return;

	WeaponInfo* data = item->GetWpnData();
	if (!data)
		return;

	Color col = g_cfg[XOR("visuals_misc_dropped_weapons_color")].get_color();

	// render bomb in green.
	if (g_cfg[XOR("visuals_misc_dropped_bomb")].get<bool>() && item->is(HASH("CC4")))
		render::pixel.string(screen.x,screen.y,{150,200,60,0xb4},XOR("C4"),render::ALIGN_CENTER);

	// if not bomb
	// normal item, get its name.
	else {
		if (g_cfg[XOR("visuals_misc_dropped_weapons")].get<bool>()) {
			std::string name {item->GetLocalizedName()};

			// smallfonts needs uppercase.
			std::transform(name.begin(),name.end(),name.begin(),::toupper);

			render::pixel.string(screen.x,screen.y,col,name,render::ALIGN_CENTER);
		}
	}

	if (!g_cfg[XOR("visuals_misc_dropped_weapons_ammo")].get<bool>())
		return;

	// nades do not have ammo.
	if (item->IsGrenade() || data->WeaponType == WEAPONTYPE_KNIFE)
		return;

	if (item->m_iItemDefinitionIndex() == 0 || item->m_iItemDefinitionIndex() == C4)
		return;

	std::string ammo = tfm::format(XOR("(%i/%i)"),item->m_iClip1(),item->m_iPrimaryReserveAmmoCount());
	render::pixel.string(screen.x,screen.y - render::pixel.m_size.m_height - 1,col,ammo,render::ALIGN_CENTER);
}

void Visuals::OffScreen(Player* player,int alpha) {
	vec3_t view_origin,target_pos,delta;
	vec2_t screen_pos,offscreen_pos;
	float  leeway_x,leeway_y,radius,offscreen_rotation;
	bool   is_on_screen;
	std::array<vec2_t,4> verts;
	Color  color;

	// todo - dex; move this?
	static auto get_offscreen_data = [] (const vec3_t& delta,float radius,vec2_t& out_offscreen_pos,float& out_rotation) {
		ang_t  view_angles(g_csgo.m_view_render->m_view.m_angles);
		vec3_t fwd,right,up(0.f,0.f,1.f);
		float  front,side,yaw_rad,sa,ca;

		// get viewport angles forward directional vector.
		math::AngleVectors(view_angles,&fwd);

		// convert viewangles forward directional vector to a unit vector.
		fwd.z = 0.f;
		fwd.normalize();

		// calculate front / side positions.
		right = up.cross(fwd);
		front = delta.dot(fwd);
		side = delta.dot(right);

		// setup offscreen position.
		out_offscreen_pos.x = radius * -side;
		out_offscreen_pos.y = radius * -front;

		// get the rotation ( yaw, 0 - 360 ).
		out_rotation = math::rad_to_deg(std::atan2(out_offscreen_pos.x,out_offscreen_pos.y) + math::pi);

		// get needed sine / cosine values.
		yaw_rad = math::deg_to_rad(-out_rotation);
		sa = std::sin(yaw_rad);
		ca = std::cos(yaw_rad);

		// rotate offscreen position around.
		out_offscreen_pos.x = (int) ((g_cl.m_width / 2.f) + (radius * sa));
		out_offscreen_pos.y = (int) ((g_cl.m_height / 2.f) - (radius * ca));
	};

	if (!g_cfg[XOR("esp_enemies_offscreen")].get<bool>())
		return;

	if (!g_cl.m_processing || !g_cl.m_local->enemy(player))
		return;

	// get the player's center screen position.
	target_pos = player->WorldSpaceCenter();
	is_on_screen = render::WorldToScreen(target_pos,screen_pos);

	// give some extra room for screen position to be off screen.
	leeway_x = g_cl.m_width / 18.f;
	leeway_y = g_cl.m_height / 18.f;

	// origin is not on the screen at all, get offscreen position data and start rendering.
	if (!is_on_screen
		|| screen_pos.x < -leeway_x
		|| screen_pos.x >(g_cl.m_width + leeway_x)
		|| screen_pos.y < -leeway_y
		|| screen_pos.y >(g_cl.m_height + leeway_y)) {

		// get viewport origin.
		view_origin = g_csgo.m_view_render->m_view.m_origin;

		// get direction to target.
		delta = (target_pos - view_origin).normalized();

		// note - dex; this is the 'YRES' macro from the source sdk.
		radius = 40.f + g_cfg[XOR("esp_enemies_offscreen_radius")].get< int >() * (g_cl.m_height / 480.f);

		// get the data we need for rendering.
		get_offscreen_data(delta,radius,offscreen_pos,offscreen_rotation);

		// bring rotation back into range... before rotating verts, sine and cosine needs this value inverted.
		// note - dex; reference: 
		// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/game/client/tf/tf_hud_damageindicator.cpp#L182
		offscreen_rotation = -offscreen_rotation;

		// setup vertices for the triangle.
		verts[0] = {offscreen_pos.x,offscreen_pos.y};        // 0,  0
		verts[1] = {offscreen_pos.x - (3.f + g_cfg[XOR("esp_enemies_offscreen_size")].get< int >()),offscreen_pos.y + (3.f + g_cfg[XOR("esp_enemies_offscreen_size")].get<int>()) * 2}; // -1, 1
		verts[2] = {offscreen_pos.x + (3.f + g_cfg[XOR("esp_enemies_offscreen_size")].get< int >()),offscreen_pos.y + (3.f + g_cfg[XOR("esp_enemies_offscreen_size")].get<int>()) * 2}; // 1,  1

		// rotate all vertices to point towards our target.
		verts[0] = render::RotateVertex(offscreen_pos,verts[0],offscreen_rotation).m_coord;
		verts[1] = render::RotateVertex(offscreen_pos,verts[1],offscreen_rotation).m_coord;
		verts[2] = render::RotateVertex(offscreen_pos,verts[2],offscreen_rotation).m_coord;

		// todo - dex; finish this, i want it.
		// auto &damage_data = m_offscreen_damage[ player->index( ) ];
		// 
		// // the local player was damaged by another player recently.
		// if( damage_data.m_time > 0.f ) {
		//     // // only a small amount of time left, start fading into white again.
		//     // if( damage_data.m_time < 1.f ) {
		//     //     // calculate step needed to reach 255 in 1 second.
		//     //     // float step = UINT8_MAX / ( 1000.f * g_csgo.m_globals->m_frametime );
		//     //     float step = ( 1.f / g_csgo.m_globals->m_frametime ) / UINT8_MAX;
		//     //     
		//     //     // increment the new value for the color.
		//     //     // if( damage_data.m_color_step < 255.f )
		//     //         damage_data.m_color_step += step;
		//     // 
		//     //     math::clamp( damage_data.m_color_step, 0.f, 255.f );
		//     // 
		//     //     damage_data.m_color.g( ) = (uint8_t)damage_data.m_color_step;
		//     //     damage_data.m_color.b( ) = (uint8_t)damage_data.m_color_step;
		//     // }
		//     // 
		//     // g_cl.print( "%f %f %u %u %u\n", damage_data.m_time, damage_data.m_color_step, damage_data.m_color.r( ), damage_data.m_color.g( ), damage_data.m_color.b( ) );
		//     
		//     // decrement timer.
		//     damage_data.m_time -= g_csgo.m_globals->m_frametime;
		// }
		// 
		// else
		//     damage_data.m_color = colors::white;

		// render!
		color = g_cfg[XOR("esp_enemies_offscreen_color")].get_color(); // damage_data.m_color;

		alpha = color.a();

		switch (g_cfg[XOR("esp_enemies_offscreen")].get< int >()) {
			case 0:
			break;
			case 1:
			render::triangle_filled(verts,color);
			break;
			case 2:
			render::triangle_filled(verts,color);
			break;
		}

	}
}

void Visuals::DrawPlayer(Player* player) {

	const float MAX_DORMANT_TIME = 10;
	float DORMANT_FADE_TIME = MAX_DORMANT_TIME / 2.f;

	player_info_t info;
	Color		  color;
	AimPlayer* data;

	// get player index.
	int index = player->index();

	if (index > 64)
		return;

	// now store our dormant data.
	g_dormancy.SoundDormancy();

	// get reference to array variable.
	float& opacity = m_opacities[index - 1];
	bool& draw = m_draw[index - 1];

	// opacity should reach 1 in 300 milliseconds.
	constexpr int frequency = 1.f / 0.3f;

	// the increment / decrement per frame.
	float step = frequency * g_csgo.m_globals->m_frametime;

	// is player enemy.
	bool enemy = player->enemy(g_cl.m_local);
	bool dormant = player->dormant();

	// show on radar.
	player->m_bSpotted() = true;

	// we can draw this player again.
	if (!dormant)
		draw = true;

	if (!draw)
		return;

	// if non-dormant	-> increment
	// if dormant		-> decrement
	dormant ? opacity -= step : opacity += step;

	// is dormant esp enabled for this player.
	bool dormant_esp = enemy;

	// clamp the opacity.
	math::clamp(opacity,0.f,1.f);
	if (!opacity && !dormant_esp)
		return;

	// stay for x seconds max.
	float dt = g_csgo.m_globals->m_curtime - player->m_flSimulationTime();
	if (dormant && dt > MAX_DORMANT_TIME)
		return;

	// calculate alpha channels.
	int alpha = (int) (225.f * opacity);
	int low_alpha = (int) (89.f * opacity);

	// get color based on enemy or not.
	color = enemy ? g_cfg[XOR("esp_enemies_box_color")].get_color() : g_cfg[XOR("esp_team_box_color")].get_color();

	if (dormant && dormant_esp) {
		alpha = 150;
		low_alpha = 90;

		// fade.
		if (dt > DORMANT_FADE_TIME) {
			// for how long have we been fading?
			float faded = (dt - DORMANT_FADE_TIME);
			float scale = 1.f - (faded / DORMANT_FADE_TIME);

			alpha *= scale;
			low_alpha *= scale;
		}

		// override color.
		color = {255,255,255};
	}

	// override alpha.
	color.a() = alpha;

	// get player info.0
	if (!g_csgo.m_engine->GetPlayerInfo(index,&info))
		return;

	// run offscreen ESP.
	OffScreen(player,alpha);

	// attempt to get player box.
	Rect box;
	if (!GetPlayerBoxRect(player,box)) {
		// OffScreen( player );
		return;
	}

	// attempt to get player box.
	box = m_saved_boxes->at(player->index());

	if (!box.h || !box.w || !box.x || !box.y)
		return;

	// DebugAimbotPoints( player );

	bool bone_esp = (enemy && g_cfg[XOR("esp_enemies_skeleton")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_skeleton")].get<bool>());
	if (bone_esp)
		DrawSkeleton(player,alpha);

	if (g_cfg[XOR("misc_animlayer")].get< bool >() && enemy)
	{
		render::pixel.string(box.x + box.w,box.y + 40 + 30,{255,255,255,255},"m_cycle",render::ALIGN_CENTER);
		render::pixel.string(box.x + box.w,box.y + 60 + 30,{255,255,255,255},"m_weight",render::ALIGN_CENTER);
		render::pixel.string(box.x + box.w,box.y + 80 + 30,{255,255,255,255},"m_weight_delta_rate",render::ALIGN_CENTER);
		render::pixel.string(box.x + box.w,box.y + 100 + 30,{255,255,255,255},"m_prev_cycle",render::ALIGN_CENTER);
		render::pixel.string(box.x + box.w,box.y + 120 + 30,{255,255,255,255},"m_playback_rate",render::ALIGN_CENTER);
		render::pixel.string(box.x + box.w,box.y + 140 + 30,{255,255,255,255},"m_sequence",render::ALIGN_CENTER);

		render::pixel.string(box.x + box.w + 20,box.y + 40 + 30,{255,255,255,255},std::to_string(player->m_AnimOverlay()[6].m_cycle),render::ALIGN_CENTER);
		render::pixel.string(box.x + box.w + 20,box.y + 60 + 30,{255,255,255,255},std::to_string(player->m_AnimOverlay()[6].m_weight),render::ALIGN_CENTER);
		render::pixel.string(box.x + box.w + 40,box.y + 80 + 30,{255,255,255,255},std::to_string(player->m_AnimOverlay()[6].m_weight_delta_rate),render::ALIGN_CENTER);
		render::pixel.string(box.x + box.w + 30,box.y + 100 + 30,{255,255,255,255},std::to_string(player->m_AnimOverlay()[6].m_prev_cycle),render::ALIGN_CENTER);
		render::pixel.string(box.x + box.w + 30,box.y + 120 + 30,{255,255,255,255},std::to_string(player->m_AnimOverlay()[6].m_playback_rate),render::ALIGN_CENTER);
		render::pixel.string(box.x + box.w + 20,box.y + 140 + 30,{255,255,255,255},std::to_string(player->m_AnimOverlay()[6].m_sequence),render::ALIGN_CENTER);
	}
	//if( g_aimbot.IsValidTarget( player ) )
	//	DrawHistorySkeleton( player, alpha );

	// is box esp enabled for this player.
	bool box_esp = (enemy && g_cfg[XOR("esp_enemies_box")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_box")].get<bool>());

	// render box if specified.
	if (box_esp) {
		const bool outline = true;

		render::rect(box.x,box.y,box.w,box.h,color);

		if (outline) {
			render::rect(box.x - 1,box.y - 1,box.w + 2,box.h + 2,Color(3,3,3,low_alpha));
			render::rect(box.x + 1,box.y + 1,box.w - 2,box.h - 2,Color(3,3,3,low_alpha));
		}
	}

	// is name esp enabled for this player.
	bool name_esp = (enemy && g_cfg[XOR("esp_enemies_name")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_name")].get<bool>());

	// draw name.
	if (name_esp) {
		// fix retards with their namechange meme 
		// the point of this is overflowing unicode compares with hardcoded buffers, good hvh strat
		std::string name {std::string(info.m_name).substr(0,24)};

		Color clr = enemy ? g_cfg[XOR("esp_enemies_name_color")].get_color() : g_cfg[XOR("esp_team_name_color")].get_color();
		// override alpha.
		clr.a() = alpha;

		render::esp.string(box.x + box.w / 2,box.y - 15,clr,name + (info.m_fake_player ? XOR(" (bot)") : ""),render::ALIGN_CENTER);
	}

	// is health esp enabled for this player.
	bool health_esp = (enemy && g_cfg[XOR("esp_enemies_health")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_health")].get<bool>());

	if (health_esp) {
		int y = box.y;
		int h = box.h;

		// retarded servers that go above 100 hp..
		int hp = std::min(100,player->m_iHealth());

		// calculate hp bar color.
		int r = std::min((510 * (100 - hp)) / 100,255);
		int g = std::min((510 * hp) / 100,255);

		// get hp bar height.
		int fill = (int) std::round(hp * h / 100.f);

		// render background.
		render::rect_filled(box.x - 6,y - 1,3,h + 2,{10,10,10,low_alpha});

		// render actual bar.
		render::rect_filled(box.x - 5,y + h - fill,1,fill,dormant ? Color(255,255,255,alpha) : g_cfg[XOR("esp_enemies_health_override")].get<bool>() ? g_cfg[XOR("esp_enemies_health_color")].get_color().OverrideAlpha(alpha,true)
			: Color(r,g,0,alpha));

		// if hp is below max, draw a string.
	//	if ( hp < 100 )
	//		render::pixel.string( box.x - 5, y + ( h - fill ) - 5, { 255, 255, 255, alpha }, std::to_string( hp ), render::ALIGN_CENTER );
	}

	// draw flags.
	{
		std::vector< std::pair< std::string,Color > > flags;

		bool money = (enemy && g_cfg[XOR("esp_enemies_flags_money")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_flags_money")].get<bool>());
		bool armor = (enemy && g_cfg[XOR("esp_enemies_flags_armor")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_flags_armor")].get<bool>());
		bool scope = (enemy && g_cfg[XOR("esp_enemies_flags_scope")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_flags_scope")].get<bool>());
		bool flash = (enemy && g_cfg[XOR("esp_enemies_flags_flash")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_flags_flash")].get<bool>());
		bool reload = (enemy && g_cfg[XOR("esp_enemies_flags_reload")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_flags_reload")].get<bool>());
		bool bomb = (enemy && g_cfg[XOR("esp_enemies_flags_bomb")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_flags_bomb")].get<bool>());

		// bomb.
		if (bomb && player->HasC4())
			flags.push_back({XOR("C4"),{255,0,0,low_alpha}});

		// money.
		if (money)
			flags.push_back({tfm::format(XOR("$%i"),player->m_iAccount()),{150,200,60,low_alpha}});

		// armor.
		if (armor) {
			if (player->m_ArmorValue() > 0) {
				// helmet and kevlar.
				if (player->m_bHasHelmet()) {
					flags.push_back({XOR("HK"),{255,255,255,low_alpha}});
				}

				// only kevlar.
				else {
					flags.push_back({XOR("K"),{255,255,255,low_alpha}});
				}
			}
		}

		// scoped.
		if (scope && player->m_bIsScoped())
			flags.push_back({XOR("ZOOM"),{60,180,225,low_alpha}});

		// flashed.
		if (flash && player->m_flFlashBangTime() > 0.f)
			flags.push_back({XOR("FLASH"),{255,255,0,low_alpha}});

		// reload.
		if (reload) {
			// get ptr to layer 1.
			C_AnimationLayer* layer1 = &player->m_AnimOverlay()[1];

			// check if reload animation is going on.
		//	if (layer1->m_weight != 0.f && player->GetSequenceActivity(layer1->m_sequence) == 967 /* ACT_CSGO_RELOAD */)
		//		flags.push_back({ XOR("RELOAD"), { 60, 180, 225, low_alpha } });
		}

		// iterate flags.
		for (size_t i {}; i < flags.size(); ++i) {
			// get flag job (pair).
			const auto& f = flags[i];

			int offset = i * (render::pixel.m_size.m_height - 1);

			// draw flag.
			render::pixel.string(box.x + box.w + 3,box.y + offset,f.second,f.first);
		}
	}

	Weapon* weapon = player->GetActiveWeapon();
	if (weapon) {
		WeaponInfo* data = weapon->GetWpnData();
		if (data) {
			int  offset {3},bar;
			float scale;

			// the maxclip1 in the weaponinfo
			int max = data->iMaxClip1;
			int current = weapon->m_iClip1();

			C_AnimationLayer* layer1 = &player->m_AnimOverlay()[1];

			// set reload state.
			bool reload = false;//( layer1->m_weight != 0.f ) && ( player->GetSequenceActivity( layer1->m_sequence ) == 967 );

			// ammo bar.
			if (max != -1 && (enemy && g_cfg[XOR("esp_enemies_ammo")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_ammo")].get<bool>())) {
				// move down the offset to make room for the bar.
				offset += 5;

				// check for reload.
				if (reload)
					scale = layer1->m_cycle;

				// not reloading.
				// make the division of 2 ints produce a float instead of another int.
				else
					scale = (float) current / max;

				// relative to bar.
				bar = (int) std::round((box.w) * scale);

				// draw.
				render::rect_filled(box.x - 1,box.y + box.h + 2,box.w + 2,3,{10,10,10,low_alpha});

				Color clr = enemy ? g_cfg[XOR("esp_enemies_ammo_color")].get_color() : g_cfg[XOR("esp_team_ammo_color")].get_color();
				clr.a() = alpha;
				render::rect_filled(box.x,box.y + box.h + 3,bar,1,clr);

				// less then a 5th of the bullets left.
				if (current <= (int) std::round(max / 5) && !reload)
					render::pixel.string(box.x + bar,box.y + box.h,{255,255,255,alpha},std::to_string(current),render::ALIGN_CENTER);
			}
			// draw weapon.
			if ((enemy && g_cfg[XOR("esp_enemies_weapon")].get<bool>()) || (!enemy && g_cfg[XOR("esp_team_weapon")].get<bool>())) {

				Color weapon_clr = enemy ? g_cfg[XOR("esp_enemies_weapon_color")].get_color() : g_cfg[XOR("esp_team_weapon_color")].get_color();
				bool weaponText = (enemy && g_cfg[XOR("esp_enemies_weapon_type")].get<int>() == 0) || (!enemy && g_cfg[XOR("esp_team_weapon_type")].get<int>() == 0);
				bool weaponIcon = (enemy && g_cfg[XOR("esp_enemies_weapon_type")].get<int>() == 1) || (!enemy && g_cfg[XOR("esp_team_weapon_type")].get<int>() == 1);

				// text.
				if (weaponText) {
					// construct std::string instance of localized weapon name.
					std::string name {weapon->GetLocalizedName()};

					// smallfonts needs upper case.
					std::transform(name.begin(),name.end(),name.begin(),::toupper);

					render::pixel.string(box.x + box.w / 2,box.y + box.h + offset,weapon_clr.OverrideAlpha(alpha),name,render::ALIGN_CENTER);
				}

				// icons.
				else if (weaponIcon) {
					// icons are super fat..
					// move them back up.
					offset -= 5;

					std::string icon = tfm::format(XOR("%c"),m_weapon_icons[weapon->m_iItemDefinitionIndex()]);
					render::cs.string(box.x + box.w / 2,box.y + box.h + offset,weapon_clr.OverrideAlpha(low_alpha),icon,render::ALIGN_CENTER);
				}
			}
		}
	}
}

void Visuals::DrawPlantedC4(Entity* ent) {
	bool        mode_2d,mode_3d,is_visible,fatal;
	float       explode_time_diff,dist,range_damage;
	vec3_t      dst,to_target;
	int         final_damage;
	std::string time_str,damage_str;
	Color       damage_color;
	vec2_t      screen_pos;
	Entity* c4;
	vec3_t             explosion_origin,explosion_origin_adjusted;
	CTraceFilterSimple filter;
	CGameTrace         tr;

	static auto scale_damage = [] (float damage,int armor_value) {
		float new_damage,armor;

		if (armor_value > 0) {
			new_damage = damage * 0.5f;
			armor = (damage - new_damage) * 0.5f;

			if (armor > (float) armor_value) {
				armor = (float) armor_value * 2.f;
				new_damage = damage - armor;
			}

			damage = new_damage;
		}

		return std::max(0,(int) std::floor(damage));
	};

	if (!ent || !ent->is(HASH("CPlantedC4")))
		return;

	c4 = ent;

	// store menu vars for later.
	mode_2d = g_cfg[XOR("esp_misc_bomb_2d")].get<bool>();
	mode_3d = g_cfg[XOR("esp_misc_bomb_3d")].get<bool>();
	if (!mode_2d && !mode_3d)
		return;

	// bomb not currently active, do nothing.
	if (!m_c4_planted)
		return;

	if (!g_visuals.m_planted_c4) {
		g_visuals.m_planted_c4 = c4;
		g_visuals.m_planted_c4_explode_time = c4->m_flC4Blow();

		// the bomb origin is adjusted slightly inside CPlantedC4::C4Think, right when it's about to explode.
		// we're going to do that here.
		explosion_origin = c4->GetAbsOrigin();
		explosion_origin_adjusted = explosion_origin;
		explosion_origin_adjusted.z += 8.f;

		// setup filter and do first trace.
		filter.SetPassEntity(c4);

		g_csgo.m_engine_trace->TraceRay(
			Ray(explosion_origin_adjusted,explosion_origin_adjusted + vec3_t(0.f,0.f,-40.f)),
			MASK_SOLID,
			&filter,
			&tr
		);

		// pull out of the wall a bit.
		if (tr.m_fraction != 1.f)
			explosion_origin = tr.m_endpos + (tr.m_plane.m_normal * 0.6f);

		// this happens inside CCSGameRules::RadiusDamage.
		explosion_origin.z += 1.f;

		// set all other vars.
		g_visuals.m_planted_c4_explosion_origin = explosion_origin;

	}

	// calculate bomb damage.
	// references:
	//     https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/shared/cstrike/weapon_c4.cpp#L271
	//     https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/shared/cstrike/weapon_c4.cpp#L437
	//     https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/sdk/sdk_gamerules.cpp#L173
	{
		// get our distance to the bomb.
		// todo - alpha; is dst right? might need to reverse CBasePlayer::BodyTarget...
		dst = g_cl.m_local->WorldSpaceCenter();
		to_target = m_planted_c4_explosion_origin - dst;
		dist = to_target.length();

		// calculate the bomb damage based on our distance to the C4's explosion.
		range_damage = m_planted_c4_damage * std::exp((dist * dist) / ((m_planted_c4_radius_scaled * -2.f) * m_planted_c4_radius_scaled));

		// now finally, scale the damage based on our armor (if we have any).
		final_damage = scale_damage(range_damage,g_cl.m_local->m_ArmorValue());
	}

	// m_flC4Blow is set to gpGlobals->curtime + m_flTimerLength inside CPlantedC4.
	explode_time_diff = m_planted_c4_explode_time - g_csgo.m_globals->m_curtime;

	// is the damage fatal?
	fatal = final_damage >= g_cl.m_local->m_iHealth();

	// get formatted strings for bomb.
	time_str = tfm::format(XOR("C4 - %.2f"),explode_time_diff);
	damage_str = fatal ? tfm::format(XOR("DAMAGE - FATAL")) : tfm::format(XOR("DAMAGE - %i"),final_damage);

	// get damage color.
	damage_color = fatal ? colors::red : colors::white;

	// finally do all of our rendering.
	is_visible = render::WorldToScreen(m_planted_c4_explosion_origin,screen_pos);

	// 'on screen (2D)'.
	if (mode_2d) {
		if (explode_time_diff > 0.f)
			render::pixel.string(g_cl.m_width * 0.5f,65,colors::white,time_str,render::ALIGN_CENTER);

		if (g_cl.m_local->alive())
			render::pixel.string(g_cl.m_width * 0.5f,65 + render::pixel.m_size.m_height,damage_color,damage_str,render::ALIGN_CENTER);
	}

	// 'on bomb (3D)'.
	if (mode_3d && is_visible) {
		if (explode_time_diff > 0.f)
			render::pixel.string(screen_pos.x,screen_pos.y,colors::white,time_str,render::ALIGN_CENTER);

		// only render damage string if we're alive.
		if (g_cl.m_local->alive())
			render::pixel.string(screen_pos.x,(int) screen_pos.y + render::pixel.m_size.m_height,damage_color,damage_str,render::ALIGN_CENTER);
	}
}

bool Visuals::GetPlayerBoxRect(Player* player,Rect& box) {
	vec3_t min,max,out_vec;
	float left,bottom,right,top;
	matrix3x4_t& tran_frame = player->m_pCoordFrame();

	// get hitbox bounds.
	min = player->m_vecMins();
	max = player->m_vecMaxs();

	vec2_t screen_boxes[8];

	// transform mins and maxes to points. 
	vec3_t points[] =
	{
		{min.x,min.y,min.z},
		{min.x,max.y,min.z},
		{max.x,max.y,min.z},
		{max.x,min.y,min.z},
		{max.x,max.y,max.z},
		{min.x,max.y,max.z},
		{min.x,min.y,max.z},
		{max.x,min.y,max.z}
	};

	// transform points to 3-dimensional space.
	for (int i = 0; i <= 7; i++) {
		math::VectorTransform(points[i],tran_frame,out_vec);
		if (!render::WorldToScreen(out_vec,screen_boxes[i]))
			return false;
	}

	// generate an array to clamp later.
	vec2_t box_array[] = {
		screen_boxes[3],
		screen_boxes[5],
		screen_boxes[0],
		screen_boxes[4],
		screen_boxes[2],
		screen_boxes[1],
		screen_boxes[6],
		screen_boxes[7]
	};

	// state the position and size of the box.
	left = screen_boxes[3].x,
		bottom = screen_boxes[3].y,
		right = screen_boxes[3].x,
		top = screen_boxes[3].y;

	// clamp the box sizes.
	for (int i = 0; i <= 7; i++) {
		if (left > box_array[i].x)
			left = box_array[i].x;

		if (bottom < box_array[i].y)
			bottom = box_array[i].y;

		if (right < box_array[i].x)
			right = box_array[i].x;

		if (top > box_array[i].y)
			top = box_array[i].y;
	}

	// state the box bounds.
	box.x = left;
	box.y = top;
	box.w = right - left;
	box.h = (bottom - top);

	return true;
}

void Visuals::DrawHistorySkeleton(Player* player,int opacity) {
	const model_t* model;
	studiohdr_t* hdr;
	mstudiobone_t* bone;
	AimPlayer* data;
	LagComp::LagRecord_t* record;
	int           parent;
	vec3_t        bone_pos,parent_pos;
	vec2_t        bone_pos_screen,parent_pos_screen;

	// get player's model.
	model = player->GetModel();
	if (!model)
		return;

	// get studio model.
	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	data = &g_aimbot.m_players[player->index() - 1];
	if (!data)
		return;

	record = g_lagcompensation.GetOldestRecord(player).value();
	if (!record)
		return;

	for (int i {}; i < hdr->m_num_bones; ++i) {
		// get bone.
		bone = hdr->GetBone(i);
		if (!bone || !(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		// get parent bone.
		parent = bone->m_parent;
		if (parent == -1)
			continue;

		// resolve main bone and parent bone positions.
		record->m_pMatrix->get_bone(bone_pos,i);
		record->m_pMatrix->get_bone(parent_pos,parent);

		Color clr = Color(255,255,255,255);
		clr.a() = opacity;

		// world to screen both the bone parent bone then draw.
		if (render::WorldToScreen(bone_pos,bone_pos_screen) && render::WorldToScreen(parent_pos,parent_pos_screen))
			render::line(bone_pos_screen.x,bone_pos_screen.y,parent_pos_screen.x,parent_pos_screen.y,clr);
	}
}

void Visuals::DrawSkeleton(Player* player,int opacity) {
	const model_t* model;
	studiohdr_t* hdr;
	mstudiobone_t* bone;
	int           parent;
	BoneArray     matrix[128];
	vec3_t        bone_pos,parent_pos;
	vec2_t        bone_pos_screen,parent_pos_screen;

	// get player's model.
	model = player->GetModel();
	if (!model)
		return;

	// get studio model.
	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	// get bone matrix.
	if (!player->SetupBones(matrix,128,BONE_USED_BY_ANYTHING,g_csgo.m_globals->m_curtime))
		return;

	for (int i {}; i < hdr->m_num_bones; ++i) {
		// get bone.
		bone = hdr->GetBone(i);
		if (!bone || !(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		// get parent bone.
		parent = bone->m_parent;
		if (parent == -1)
			continue;

		// resolve main bone and parent bone positions.
		matrix->get_bone(bone_pos,i);
		matrix->get_bone(parent_pos,parent);

		Color clr = player->enemy(g_cl.m_local) ? g_cfg[XOR("esp_enemies_skeleton_color")].get_color() : g_cfg[XOR("esp_team_skeleton_color")].get_color();
		clr.a() = (opacity == 255) ? clr.a() : opacity;

		// world to screen both the bone parent bone then draw.
		if (render::WorldToScreen(bone_pos,bone_pos_screen) && render::WorldToScreen(parent_pos,parent_pos_screen))
			render::line(bone_pos_screen.x,bone_pos_screen.y,parent_pos_screen.x,parent_pos_screen.y,clr);
	}
}

void Visuals::RenderGlow() {
	Color   color;
	Player* player;

	if (!g_cl.m_local)
		return;

	if (!g_csgo.m_glow->m_object_definitions.Count())
		return;

	for (int i {}; i < g_csgo.m_glow->m_object_definitions.Count(); ++i) {
		GlowObjectDefinition_t* obj = &g_csgo.m_glow->m_object_definitions[i];

		// skip non-players.
		if (!obj->m_entity || !obj->m_entity->IsPlayer())
			continue;

		// get player ptr.
		player = obj->m_entity->as< Player* >();

		// get reference to array variable.
		float& opacity = m_opacities[player->index() - 1];

		bool enemy = player->enemy(g_cl.m_local);
		bool local = player->m_bIsLocalPlayer();

		if (enemy && !g_cfg[XOR("esp_enemy_glow")].get<bool>())
			continue;

		if (local && !g_cfg[XOR("esp_local_glow")].get<bool>())
			continue;

		if (!enemy && !local && !g_cfg[XOR("esp_team_glow")].get<bool>())
			continue;

		// enemy color
		if (enemy) {
			int hp = std::min(100,player->m_iHealth());

			// calculate hp bar color.
			int r = std::min((510 * (100 - hp)) / 100,255);
			int g = std::min((510 * hp) / 100,255);
			color = g_cfg[XOR("esp_enemy_glow_healthcolor")].get< bool >() ? Color(r,g,0) : g_cfg[XOR("esp_enemy_glow_color")].get_color();

		}
		// local color
		else if (local) {
			color = g_cfg[XOR("esp_local_glow_color")].get_color();
			opacity = 1.f;
		}
		// friendly color
		else {
			if (player != g_cl.m_local)
				color = g_cfg[XOR("esp_team_glow_color")].get_color();
		}

		obj->m_render_occluded = true;
		obj->m_render_unoccluded = false;
		obj->m_render_full_bloom = false;
		obj->m_color = {(float) color.r() / 255.f,(float) color.g() / 255.f,(float) color.b() / 255.f};
		obj->m_alpha = opacity * color.a() / 255.f;
	}
}

void Visuals::DrawHitboxMatrix(Player* player,matrix3x4_t* bones,Color col,float time) {
	const model_t* model;
	studiohdr_t* hdr;
	mstudiohitboxset_t* set;
	mstudiobbox_t* bbox;
	vec3_t              mins,maxs,origin;
	ang_t			    angle;

	if (!player)
		return;

	model = player->GetModel();
	if (!model)
		return;

	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	set = hdr->GetHitboxSet(player->m_nHitboxSet());
	if (!set)
		return;

	for (int i {}; i < set->m_hitboxes; ++i) {
		bbox = set->GetHitbox(i);
		if (!bbox)
			continue;

		// bbox.
		if (bbox->m_radius <= 0.f) {
			// https://developer.valvesoftware.com/wiki/Rotation_Tutorial

			// convert rotation angle to a matrix.
			matrix3x4_t rot_matrix;
			g_csgo.AngleMatrix(bbox->m_angle,rot_matrix);

			// apply the rotation to the entity input space (local).
			matrix3x4_t matrix;
			math::ConcatTransforms(bones[bbox->m_bone],rot_matrix,matrix);

			// extract the compound rotation as an angle.
			ang_t bbox_angle;
			math::MatrixAngles(matrix,bbox_angle);

			// extract hitbox origin.
			vec3_t origin = matrix.GetOrigin();

			// draw box.
			g_csgo.m_debug_overlay->AddBoxOverlay(origin,bbox->m_mins,bbox->m_maxs,bbox_angle,col.r(),col.g(),col.b(),0,time);
		}

		// capsule.
		else {
			// NOTE; the angle for capsules is always 0.f, 0.f, 0.f.

			// create a rotation matrix.
			matrix3x4_t matrix;
			g_csgo.AngleMatrix(bbox->m_angle,matrix);

			// apply the rotation matrix to the entity output space (world).
			math::ConcatTransforms(bones[bbox->m_bone],matrix,matrix);

			// get world positions from new matrix.
			math::VectorTransform(bbox->m_mins,matrix,mins);
			math::VectorTransform(bbox->m_maxs,matrix,maxs);

			g_csgo.m_debug_overlay->AddCapsuleOverlay(mins,maxs,bbox->m_radius,col.r(),col.g(),col.b(),col.a(),time,0,0);
		}
	}
}

void Visuals::DrawHitboxMatrix(LagComp::LagRecord_t* record,Color col,float time) {
	const model_t* model;
	studiohdr_t* hdr;
	mstudiohitboxset_t* set;
	mstudiobbox_t* bbox;
	vec3_t             mins,maxs,origin;
	ang_t			   angle;

	model = record->m_pEntity->GetModel();
	if (!model)
		return;

	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	set = hdr->GetHitboxSet(record->m_pEntity->m_nHitboxSet());
	if (!set)
		return;

	for (int i {}; i < set->m_hitboxes; ++i) {
		bbox = set->GetHitbox(i);
		if (!bbox)
			continue;

		// bbox.
		if (bbox->m_radius <= 0.f) {
			// https://developer.valvesoftware.com/wiki/Rotation_Tutorial

			// convert rotation angle to a matrix.
			matrix3x4_t rot_matrix;
			g_csgo.AngleMatrix(bbox->m_angle,rot_matrix);

			// apply the rotation to the entity input space (local).
			matrix3x4_t matrix;
			math::ConcatTransforms(record->m_pMatrix[bbox->m_bone],rot_matrix,matrix);

			// extract the compound rotation as an angle.
			ang_t bbox_angle;
			math::MatrixAngles(matrix,bbox_angle);

			// extract hitbox origin.
			vec3_t origin = matrix.GetOrigin();

			// draw box.
			g_csgo.m_debug_overlay->AddBoxOverlay(origin,bbox->m_mins,bbox->m_maxs,bbox_angle,col.r(),col.g(),col.b(),0,time);
		}

		// capsule.
		else {
			// NOTE; the angle for capsules is always 0.f, 0.f, 0.f.

			// create a rotation matrix.
			matrix3x4_t matrix;
			g_csgo.AngleMatrix(bbox->m_angle,matrix);

			// apply the rotation matrix to the entity output space (world).
			math::ConcatTransforms(record->m_pMatrix[bbox->m_bone],matrix,matrix);

			// get world positions from new matrix.
			math::VectorTransform(bbox->m_mins,matrix,mins);
			math::VectorTransform(bbox->m_maxs,matrix,maxs);

			g_csgo.m_debug_overlay->AddCapsuleOverlay(mins,maxs,bbox->m_radius,col.r(),col.g(),col.b(),col.a(),time,0,0);
		}
	}
}

void Visuals::DrawBeams() {
	size_t     impact_count;
	float      curtime,dist;
	bool       is_final_impact;
	vec3_t     va_fwd,start,dir,end;
	BeamInfo_t beam_info;
	Beam_t* beam;

	if (!g_cl.m_processing)
		return;

	if (!g_cfg[XOR("visuals_misc_bullet_beam")].get< bool >())
		return;

	auto vis_impacts = &g_shots.m_vis_impacts;

	// the local player is dead, clear impacts.
	if (!g_cl.m_processing) {
		if (!vis_impacts->empty())
			vis_impacts->clear();
	}

	else {
		impact_count = vis_impacts->size();
		if (!impact_count)
			return;

		curtime = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());

		for (size_t i {impact_count}; i-- > 0; ) {
			auto impact = &vis_impacts->operator[ ](i);
			if (!impact)
				continue;

			// impact is too old, erase it.
			if (std::abs(curtime - game::TICKS_TO_TIME(impact->m_tickbase)) > 3.f) {
				vis_impacts->erase(vis_impacts->begin() + i);

				continue;
			}

			// already rendering this impact, skip over it.
			if (impact->m_ignore)
				continue;

			// is this the final impact?
			// last impact in the vector, it's the final impact.
			if (i == (impact_count - 1))
				is_final_impact = true;

			// the current impact's tickbase is different than the next, it's the final impact.
			else if (vis_impacts->size() >= i + 1 && (i + 1) < impact_count && impact->m_tickbase != vis_impacts->operator[ ](i + 1).m_tickbase)
				is_final_impact = true;

			else
				is_final_impact = false;

			// is this the final impact?
			// is_final_impact = ( ( i == ( impact_count - 1 ) ) || ( impact->m_tickbase != vis_impacts->at( i + 1 ).m_tickbase ) );

			if (is_final_impact) {
				// calculate start and end position for beam.
				start = impact->m_shoot_pos;

				dir = (impact->m_impact_pos - start).normalized();
				dist = (impact->m_impact_pos - start).length();

				end = start + (dir * dist);

				// setup beam info.
				// note - dex; possible beam models: sprites/physbeam.vmt | sprites/white.vmt
				beam_info.m_vecStart = start;
				beam_info.m_vecEnd = end;
				beam_info.m_nModelIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/physbeam.vmt"));
				beam_info.m_pszModelName = XOR("sprites/physbeam.vmt");
				beam_info.m_flHaloScale = 0.f;
				beam_info.m_flLife = 3.f;
				beam_info.m_flWidth = 3.0f;
				beam_info.m_flEndWidth = 5.0f;
				beam_info.m_flFadeLength = 0.f;
				beam_info.m_flAmplitude = 0.f;   // beam 'jitter'.
				beam_info.m_flBrightness = 255.f;
				beam_info.m_flSpeed = 0.2f;  // seems to control how fast the 'scrolling' of beam is... once fully spawned.
				beam_info.m_nStartFrame = 0;
				beam_info.m_flFrameRate = 1.f;
				beam_info.m_nSegments = 2;     // controls how much of the beam is 'split up', usually makes m_flAmplitude and m_flSpeed much more noticeable.
				beam_info.m_bRenderable = true;  // must be true or you won't see the beam.
				beam_info.m_nFlags = 0x8300;

				if (!impact->m_hit_player) {
					beam_info.m_flRed = g_cfg[XOR("visuals_misc_bullet_beam_color")].get_color().r();
					beam_info.m_flGreen = g_cfg[XOR("visuals_misc_bullet_beam_color")].get_color().g();
					beam_info.m_flBlue = g_cfg[XOR("visuals_misc_bullet_beam_color")].get_color().b();
				}

				else {
					beam_info.m_flRed = g_cfg[XOR("visuals_misc_bullet_beam_hurt_color")].get_color().r();
					beam_info.m_flGreen = g_cfg[XOR("visuals_misc_bullet_beam_hurt_color")].get_color().g();
					beam_info.m_flBlue = g_cfg[XOR("visuals_misc_bullet_beam_hurt_color")].get_color().b();
				}

				beam = g_csgo.m_beams->CreateBeamPoints(beam_info);
				if (beam) {
					g_csgo.m_beams->DrawBeam(beam);
				}

				// we only want to render a beam for this impact once.
				impact->m_ignore = true;
			}
		}
	}
}

void Visuals::DebugAimbotPoints(Player* player) {
	std::vector< vec3_t > p2 {};

	AimPlayer* data = &g_aimbot.m_players.at(player->index() - 1);
	if (!data)
		return;

	const auto front = g_lagcompensation.GetLatestRecord(player);
	if (!front.has_value() || front.value()->m_bDormant)
		return;

	// get bone matrix.
	//BoneArray matrix[ 128 ];
	//if( !g_bones.Setup( player, matrix, front ) )
	//	return;

	data->SetupHitboxes(front.value(),false);
	if (data->m_hitboxes.empty())
		return;

	for (const auto& it : data->m_hitboxes) {
		std::vector< vec3_t > p1 {};

		if (!data->SetupHitboxPoints(front.value(),front.value()->m_pMatrix,it.m_index,p1))
			continue;

		for (auto& p : p1)
			p2.push_back(p);
	}

	if (p2.empty())
		return;

	for (auto& p : p2) {
		vec2_t screen;

		if (render::WorldToScreen(p,screen))
			render::rect_filled(screen.x,screen.y,2,2,{0,255,255,255});
	}
}

void Visuals::GetViewModel() {

	// save viewmodel settings.
	m_viewmodel[0] = g_csgo.viewmodel_offset_x->GetInt();
	m_viewmodel[1] = g_csgo.viewmodel_offset_y->GetInt();
	m_viewmodel[2] = g_csgo.viewmodel_offset_z->GetInt();

	// remove callback so we can set values.
	g_csgo.viewmodel_offset_x->m_callbacks.RemoveAll();
	g_csgo.viewmodel_offset_y->m_callbacks.RemoveAll();
	g_csgo.viewmodel_offset_z->m_callbacks.RemoveAll();
}

void Visuals::ViewModel()
{
	if (!g_csgo.m_engine->IsInGame() || !g_cl.m_local)
		return;

	// local is dead, we want to spectate with default values.
	if (!g_cl.m_local->alive()) {

		// restore viewmodel to old values.
		g_csgo.viewmodel_offset_x->SetValue(m_viewmodel[0]);
		g_csgo.viewmodel_offset_y->SetValue(m_viewmodel[1]);
		g_csgo.viewmodel_offset_z->SetValue(m_viewmodel[2]);

		m_viewmodel_modified = false;

		// no need to run this function.
		return;
	}

	// do viewmodel change.
	g_csgo.viewmodel_offset_x->SetValue(g_cfg[XOR("misc_viewmodel_x")].get< float >() != 0 ? g_cfg[XOR("misc_viewmodel_x")].get< float >() : m_viewmodel[0]);
	g_csgo.viewmodel_offset_y->SetValue(g_cfg[XOR("misc_viewmodel_y")].get< float >() != 0 ? g_cfg[XOR("misc_viewmodel_y")].get< float >() : m_viewmodel[1]);
	g_csgo.viewmodel_offset_z->SetValue(g_cfg[XOR("misc_viewmodel_z")].get< float >() != 0 ? g_cfg[XOR("misc_viewmodel_z")].get< float >() : m_viewmodel[2]);

	m_viewmodel_modified = true;
}

void Visuals::ClientImpacts() {
	CTraceFilterSimple_game              filter {};
	CGameTrace                           trace;
	vec3_t                               eyes,angle_eyes;

	if (!g_cl.m_processing || !g_cl.m_weapon || !g_cl.m_cmd)
		return;

	// making sure we aren't drawing impacts with these weapons.
	if (g_cl.m_weapon_type == WEAPONTYPE_KNIFE || g_cl.m_weapon->get_class_id() == class_id::cweapontaser)
		return;

	math::AngleVectors(g_csgo.m_input->m_commands[g_cl.m_shot_command_number % 150].m_view_angles,&angle_eyes);
	filter.SetPassEntity(g_cl.m_local);

	g_cl.m_local->EyePosition(&eyes);
	g_csgo.m_engine_trace->TraceRay(Ray(eyes,eyes + (angle_eyes * g_cl.m_weapon_info->flRange)),MASK_SOLID,(ITraceFilter*) &filter,&trace);

	if (g_cfg[XOR("misc_bullet_impacts_client")].get< bool >() && trace.m_fraction < 0.9f) {
		if (g_cfg[XOR("misc_bullet_impacts_client_override")].get< bool >())
			g_csgo.m_debug_overlay->AddBoxOverlay(trace.m_endpos,vec3_t(-2,-2,-2),vec3_t(2,2,2),ang_t(0,0,0),g_cfg[XOR("misc_bullet_impacts_client_color")].get_color().r(),g_cfg[XOR("misc_bullet_impacts_client_color")].get_color().g(),g_cfg[XOR("misc_bullet_impacts_client_color")].get_color().b(),g_cfg[XOR("misc_bullet_impacts_client_color")].get_color().a(),3);
		else
			g_csgo.m_debug_overlay->AddBoxOverlay(trace.m_endpos,vec3_t(-2,-2,-2),vec3_t(2,2,2),ang_t(0,0,0),0,0,255,127,3);
	}

}

void Visuals::DrawInfernoRadius() {

	if (!g_cfg[XOR("misc_world_fire_proximity")].get< bool >() || !g_cl.m_processing)
		return;

	// iterate through all entitys.
	for (int i = 0; i < g_csgo.m_entlist->GetHighestEntityIndex(); i++)
	{
		auto entity = g_csgo.m_entlist->GetClientEntity(i);

		if (!entity)
			continue;

		// fire entity
		if (entity->get_class_id() == class_id::cinferno && entity->m_vecVelocity().length_2d_sqr() < 0.1)
		{
			auto inferno = reinterpret_cast <CInferno*> (entity);

			const auto origin = inferno->GetAbsOrigin();
			auto get_w2s_origin = vec3_t {};

			if (!render::WorldToScreen3d(origin,get_w2s_origin))
				return;

			int* m_fireXDelta = reinterpret_cast<int*>((DWORD) inferno + 0x9E4);
			int* m_fireYDelta = reinterpret_cast<int*>((DWORD) inferno + 0xB74);
			int* m_fireZDelta = reinterpret_cast<int*>((DWORD) inferno + 0xD04);

			const auto spawn_time = inferno->get_entity_spawn_time();
			static constexpr float max_time = 7.03125;
			const auto actual = (((spawn_time + max_time) - g_csgo.m_globals->m_curtime) / max_time);
			static auto size = vec2_t(70,3);
			const auto pos = vec2_t(int(get_w2s_origin.x - size.x * 0.5),int(get_w2s_origin.y - size.y * 0.5));
			auto dpos = origin + vec3_t(m_fireXDelta[i],m_fireYDelta[i],m_fireZDelta[i]);

			Color color = g_cfg[XOR("misc_world_fire_proximity_color")].get_color();

			render::filled_world_circle(dpos,144,color);

			render::esp.string(pos.x + 35,pos.y - 5,{255,255,255,255},XOR("FIRE"),render::ALIGN_CENTER);
		}
	}
}

void Visuals::DrawSmokeRadius() {

	if (!g_cfg[XOR("misc_world_smoke_proximity")].get< bool >() || !g_cl.m_processing)
		return;

	for (int i = 0; i < g_csgo.m_entlist->GetHighestEntityIndex(); i++)
	{
		auto entity = g_csgo.m_entlist->GetClientEntity(i);

		if (!entity)
			continue;

		// smoke entity
		if (entity->get_class_id() == class_id::csmokegrenadeprojectile && entity->m_vecVelocity().length_2d_sqr() < 0.1)
		{
			auto smoke = reinterpret_cast<CInferno*>(entity);

			const auto origin = smoke->GetAbsOrigin();
			auto get_w2s_origin = vec3_t {};

			if (!render::WorldToScreen3d(origin,get_w2s_origin))
				return;

			const auto spawn_time = smoke->get_entity_spawn_time();
			static constexpr float max_time = 17.5f;
			const auto actual = (((spawn_time + max_time) - g_csgo.m_globals->m_curtime) / max_time);
			static auto size = vec2_t(70,3);
			const auto pos = vec2_t(int(get_w2s_origin.x - size.x * 0.5),int(get_w2s_origin.y - size.y * 0.5));
			auto dpos = origin;

			Color color = g_cfg[XOR("misc_world_smoke_proximity_color")].get_color();

			render::filled_world_circle(dpos,144,color);

			render::esp.string(pos.x + 35,pos.y - 5,{255,255,255,255},XOR("SMOKE"),render::ALIGN_CENTER);
		}
	}
}
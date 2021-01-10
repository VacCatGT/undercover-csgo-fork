#include "undercover.h"

CSGO	   g_csgo { };;
WinAPI	   g_winapi { };;
Netvars	   g_netvars { };;
EntOffsets g_entoffsets { };;
Notify     g_notify { };;

bool CSGO::init ( ) {
	m_done = false;

	if ( m_done )
		return false;

	// wait for the game to init.
	// "serverbrowser.dll" is the last module to be loaded.
	// if it gets loaded we can be ensured that the entire game done loading.

#ifndef KOLO
	while ( !m_serverbrowser_dll ) {
		m_serverbrowser_dll = PE::GetModule ( HASH ( "serverbrowser.dll" ) );
		if ( !m_serverbrowser_dll )
			std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );
	}
#endif

	// grab some modules.
	m_kernel32_dll = PE::GetModule ( HASH ( "kernel32.dll" ) );
	m_user32_dll = PE::GetModule ( HASH ( "user32.dll" ) );
	m_shell32_dll = PE::GetModule ( HASH ( "shell32.dll" ) );
	m_shlwapi_dll = PE::GetModule ( HASH ( "shlwapi.dll" ) );
	m_client_dll = PE::GetModule ( HASH ( "client.dll" ) );
	m_server_dll = PE::GetModule ( HASH ( "server.dll" ) );
	m_engine_dll = PE::GetModule ( HASH ( "engine.dll" ) );
	m_vstdlib_dll = PE::GetModule ( HASH ( "vstdlib.dll" ) );
	m_tier0_dll = PE::GetModule ( HASH ( "tier0.dll" ) );
	m_shaderapidx9_dll = PE::GetModule ( HASH ( "shaderapidx9.dll" ) );

	// import winapi functions.
	g_winapi.WideCharToMultiByte = PE::GetExport ( m_kernel32_dll, HASH ( "WideCharToMultiByte" ) ).as< WinAPI::WideCharToMultiByte_t > ( );
	g_winapi.MultiByteToWideChar = PE::GetExport ( m_kernel32_dll, HASH ( "MultiByteToWideChar" ) ).as< WinAPI::MultiByteToWideChar_t > ( );
	g_winapi.GetTickCount = PE::GetExport ( m_kernel32_dll, HASH ( "GetTickCount" ) ).as< WinAPI::GetTickCount_t > ( );
	g_winapi.VirtualProtect = PE::GetExport ( m_kernel32_dll, HASH ( "VirtualProtect" ) ).as< WinAPI::VirtualProtect_t > ( );
	g_winapi.VirtualQuery = PE::GetExport ( m_kernel32_dll, HASH ( "VirtualQuery" ) ).as< WinAPI::VirtualQuery_t > ( );
	g_winapi.CreateDirectoryA = PE::GetExport ( m_kernel32_dll, HASH ( "CreateDirectoryA" ) ).as< WinAPI::CreateDirectoryA_t > ( );
	g_winapi.SetWindowLongA = PE::GetExport ( m_user32_dll, HASH ( "SetWindowLongA" ) ).as< WinAPI::SetWindowLongA_t > ( );
	g_winapi.CallWindowProcA = PE::GetExport ( m_user32_dll, HASH ( "CallWindowProcA" ) ).as< WinAPI::CallWindowProcA_t > ( );
	g_winapi.SHGetFolderPathA = PE::GetExport ( m_shell32_dll, HASH ( "SHGetFolderPathA" ) ).as< WinAPI::SHGetFolderPathA_t > ( );
	g_winapi.PathAppendA = PE::GetExport ( m_shlwapi_dll, HASH ( "PathAppendA" ) ).as< WinAPI::PathAppendA_t > ( );

	// run interface collection code.
	Interfaces interfaces { };

	// get interface pointers.
	m_client = interfaces.get< CHLClient* > ( HASH ( "VClient" ) );
	m_cvar = interfaces.get< ICvar* > ( HASH ( "VEngineCvar" ) );
	m_engine = interfaces.get< IVEngineClient* > ( HASH ( "VEngineClient" ) );
	m_entlist = interfaces.get< IClientEntityList* > ( HASH ( "VClientEntityList" ) );
	m_input_system = interfaces.get< IInputSystem* > ( HASH ( "InputSystemVersion" ) );
	m_surface = interfaces.get< ISurface* > ( HASH ( "VGUI_Surface" ) );
	m_panel = interfaces.get< IPanel* > ( HASH ( "VGUI_Panel" ) );
	m_engine_vgui = interfaces.get< IEngineVGui* > ( HASH ( "VEngineVGui" ) );
	m_prediction = interfaces.get< CPrediction* > ( HASH ( "VClientPrediction" ) );
	m_engine_trace = interfaces.get< IEngineTrace* > ( HASH ( "EngineTraceClient" ) );
	m_game_movement = interfaces.get< CGameMovement* > ( HASH ( "GameMovement" ) );
	m_render_view = interfaces.get< IVRenderView* > ( HASH ( "VEngineRenderView" ) );
	m_model_render = interfaces.get< IVModelRender* > ( HASH ( "VEngineModel" ) );
	m_material_system = interfaces.get< IMaterialSystem* > ( HASH ( "VMaterialSystem" ) );
	m_studio_render = interfaces.get< CStudioRenderContext* > ( HASH ( "VStudioRender" ) );
	m_model_info = interfaces.get< IVModelInfo* > ( HASH ( "VModelInfoClient" ) );
	m_debug_overlay = interfaces.get< IVDebugOverlay* > ( HASH ( "VDebugOverlay" ) );
	m_phys_props = interfaces.get< IPhysicsSurfaceProps* > ( HASH ( "VPhysicsSurfaceProps" ) );
	m_game_events = interfaces.get< IGameEventManager2* > ( HASH ( "GAMEEVENTSMANAGER" ), 1 );
	m_match_framework = interfaces.get< CMatchFramework* > ( HASH ( "MATCHFRAMEWORK_" ) );
	m_localize = interfaces.get< ILocalize* > ( HASH ( "Localize_" ) );
	m_networkstringtable = interfaces.get< INetworkStringTableContainer* > ( HASH ( "VEngineClientStringTable" ) );
	m_sound = interfaces.get< IEngineSound* > ( HASH ( "IEngineSoundClient" ) );
	m_player_vtable = util::GetVirtualTablePointer ( XOR ( "client.dll" ), XOR ( "C_CSPlayer" ) );
	m_net = reinterpret_cast< INetChannel* >( util::GetVirtualTablePointer ( XOR ( "engine.dll" ), XOR ( "CNetChan" ) ) );

	// convars.
	clear = m_cvar->FindVar ( HASH ( "clear" ) );
	toggleconsole = m_cvar->FindVar ( HASH ( "toggleconsole" ) );
	name = m_cvar->FindVar ( HASH ( "name" ) );
	sv_maxunlag = m_cvar->FindVar ( HASH ( "sv_maxunlag" ) );
	sv_gravity = m_cvar->FindVar ( HASH ( "sv_gravity" ) );
	sv_jump_impulse = m_cvar->FindVar ( HASH ( "sv_jump_impulse" ) );
	sv_enablebunnyhopping = m_cvar->FindVar ( HASH ( "sv_enablebunnyhopping" ) );
	sv_airaccelerate = m_cvar->FindVar ( HASH ( "sv_airaccelerate" ) );
	sv_friction = m_cvar->FindVar ( HASH ( "sv_friction" ) );
	sv_stopspeed = m_cvar->FindVar ( HASH ( "sv_stopspeed" ) );
	cl_interp = m_cvar->FindVar ( HASH ( "cl_interp" ) );
	cl_interp_ratio = m_cvar->FindVar ( HASH ( "cl_interp_ratio" ) );
	mp_c4timer = m_cvar->FindVar ( HASH ( "mp_c4timer" ) );
	cl_updaterate = m_cvar->FindVar ( HASH ( "cl_updaterate" ) );
	cl_cmdrate = m_cvar->FindVar ( HASH ( "cl_cmdrate" ) );
	cl_lagcompensation = m_cvar->FindVar ( HASH ( "cl_lagcompensation" ) );
	mp_teammates_are_enemies = m_cvar->FindVar ( HASH ( "mp_teammates_are_enemies" ) );
	weapon_debug_spread_show = m_cvar->FindVar ( HASH ( "weapon_debug_spread_show" ) );
	cl_csm_shadows = m_cvar->FindVar ( HASH ( "cl_csm_shadows" ) );
	cl_extrapolate = m_cvar->FindVar ( HASH ( "cl_extrapolate" ) );
	sv_cheats = m_cvar->FindVar ( HASH ( "sv_cheats" ) );
	molotov_throw_detonate_time = m_cvar->FindVar ( HASH ( "molotov_throw_detonate_time" ) );
	weapon_molotov_maxdetonateslope = m_cvar->FindVar ( HASH ( "weapon_molotov_maxdetonateslope" ) );
	weapon_recoil_scale = m_cvar->FindVar ( HASH ( "weapon_recoil_scale" ) );
	view_recoil_tracking = m_cvar->FindVar ( HASH ( "view_recoil_tracking" ) );
	cl_fullupdate = m_cvar->FindVar ( HASH ( "cl_fullupdate" ) );
	r_DrawSpecificStaticProp = m_cvar->FindVar ( HASH ( "r_DrawSpecificStaticProp" ) );
	cl_crosshair_sniper_width = m_cvar->FindVar ( HASH ( "cl_crosshair_sniper_width" ) );
	hud_scaling = m_cvar->FindVar ( HASH ( "hud_scaling" ) );
	sv_clip_penetration_traces_to_players = m_cvar->FindVar ( HASH ( "sv_clip_penetration_traces_to_players" ) );
	weapon_accuracy_shotgun_spread_patterns = m_cvar->FindVar ( HASH ( "weapon_accuracy_shotgun_spread_patterns" ) );
	viewmodel_offset_x = m_cvar->FindVar ( HASH ( "viewmodel_offset_x" ) );
	viewmodel_offset_y = m_cvar->FindVar ( HASH ( "viewmodel_offset_y" ) );
	viewmodel_offset_z = m_cvar->FindVar ( HASH ( "viewmodel_offset_z" ) );
	mat_wireframe = m_cvar->FindVar ( HASH ( "mat_wireframe" ) );
	r_drawmodelstatsoverlay = m_cvar->FindVar ( HASH ( "r_drawmodelstatsoverlay" ) );
	sv_competitive_minspec = m_cvar->FindVar ( HASH ( "sv_competitive_minspec" ) );
	cl_use_new_headbob = m_cvar->FindVar ( HASH ( "cl_use_new_headbob" ) );
	fog_override = m_cvar->FindVar ( HASH ( "fog_override" ) );
	fog_color = m_cvar->FindVar ( HASH ( "fog_color" ) );
	fog_start = m_cvar->FindVar ( HASH ( "fog_start" ) );
	fog_end = m_cvar->FindVar ( HASH ( "fog_end" ) );
	fog_maxdensity = m_cvar->FindVar ( HASH ( "fog_maxdensity" ) );
	sv_maxspeed = m_cvar->FindVar ( HASH ( "sv_maxspeed" ) );

	// hehe xd.
	name->m_callbacks.RemoveAll ( );

	// classes by sig.
	m_move_helper = pattern::find ( m_client_dll, XOR ( "8B 0D ? ? ? ? 8B 46 08 68" ) ).add ( 2 ).get< IMoveHelper* > ( 2 );
	m_cl = pattern::find ( m_engine_dll, XOR ( "B9 ? ? ? ? 56 FF 50 14 8B 34 85" ) ).add ( 1 ).get< CGlobalState* > ( )->m_client_state;
	//m_game               = pattern::find( m_engine_dll, XOR( "A1 ? ? ? ? B9 ? ? ? ? FF 75 08 FF 50 34" ) ).add( 1 ).get< CGame* >( );
//	m_render             = pattern::find( m_engine_dll, XOR( "A1 ? ? ? ? B9 ? ? ? ? FF 75 0C FF 75 08 FF 50 0C" ) ).add( 1 ).get< CRender* >( );
	m_shadow_mgr = pattern::find ( m_client_dll, XOR ( "A1 ? ? ? ? FF 90 ? ? ? ? 6A 00" ) ).add ( 1 ).get ( ).as< IClientShadowMgr* > ( );
	m_view_render = pattern::find ( m_client_dll, XOR ( "8B 0D ? ? ? ? 57 8B 01 FF 50 14 E8 ? ? ? ? 5F" ) ).add ( 2 ).get< CViewRender* > ( 2 );
	// m_entity_listeners   = pattern::find( m_client_dll, XOR( "B9 ? ? ? ? E8 ? ? ? ? 5E 5D C2 04" ) ).add( 0x1 ).get< IClientEntityListener** >( 2 );
	//m_hud                = pattern::find( m_client_dll, XOR( "B9 ? ? ? ? 0F 94 C0 0F B6 C0 50 68" ) ).add( 0x1 ).get( ).as< CHud* >( );
	m_gamerules = pattern::find ( m_client_dll, XOR ( "A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 74 7A" ) ).add ( 0x1 ).get< C_CSGameRules* > ( );
	m_beams = pattern::find ( m_client_dll, XOR ( "8D 43 FC B9 ? ? ? ? 50 A1" ) ).add ( 0x4 ).get< IViewRenderBeams* > ( );
	m_mem_alloc = PE::GetExport ( m_tier0_dll, HASH ( "g_pMemAlloc" ) ).get< IMemAlloc* > ( );
	//GetGlowObjectManager = pattern::find( m_client_dll, XOR( "0F 11 05 ? ? ? ? 83 C8 01" ) ).add(0x3).get().as< GetGlowObjectManager_t >( );
	m_glow = *pattern::find ( m_client_dll, XOR ( "0F 11 05 ? ? ? ? 83 C8 01" ) ).add ( 0x3 ).as< CGlowObjectManager** > ( );

	// classes by offset from virtual.
	m_globals = **( CGlobalVarsBase*** ) ( ( *( DWORD** ) ( m_client ) ) [ 0 ] + 0x1F );
	m_client_mode = util::get_method ( m_client, CHLClient::HUDPROCESSINPUT ).add ( 0x5 ).get< IClientMode* > ( 2 );
	m_input = util::get_method ( m_client, CHLClient::INACTIVATEMOUSE ).at< CInput* > ( 0x1 );

	m_device = **pattern::find ( m_shaderapidx9_dll, XOR ( "A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C" ) ).add ( 0x1 ).as<IDirect3DDevice9***> ( );

	// functions.
	MD5_PseudoRandom = pattern::find ( m_client_dll, XOR ( "55 8B EC 83 E4 F8 83 EC 70 6A" ) ).as< MD5_PseudoRandom_t > ( );
	SetAbsAngles = pattern::find ( m_client_dll, XOR ( "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8 ?" ) );
	InvalidateBoneCache = pattern::find ( m_client_dll, XOR ( "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81" ) );
	DisablePostProcess = pattern::find ( m_client_dll, XOR ( "80 3D ? ? ? ? ? 53 56 57 0F 85" ) ).add ( 0x2 );
	ReportHit = pattern::find ( m_client_dll, XOR ( "55 8B EC 8B 55 08 83 EC 1C F6 42 1C 01" ) ).as< uintptr_t > ( );
	DropPhysicsMag = pattern::find ( m_client_dll, XOR ( "55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89" ) ).as< uintptr_t > ( );
	CreateDefuserRopes = pattern::find ( m_client_dll, XOR ( "55 8B EC 83 EC 14 53 56 57 8B D9 E8" ) ).as< uintptr_t > ( );
	CL_Move = pattern::find ( m_engine_dll, XOR ( "55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D ? ? ? ? 8A" ) ).as<uintptr_t> ( );
	GetWeaponPrefix = pattern::find ( m_client_dll, XOR ( "53 56 57 8b f9 33 f6 8b 4f ? 8b 01 ff 90 ? ? ? ? 89 47" ) ).as<uintptr_t> ( );
	LookupSequence = pattern::find ( m_client_dll, XOR ( "55 8b ec 56 8b f1 83 be ? ? ? ? ? 75 ? 8b 46 ? 8d 4e ? ff 50 ? 85 c0 74 ? 8b ce e8 ? ? ? ? 8b b6 ? ? ? ? 85 f6 74 ? 83 3e ? 74 ? 8b ce e8 ? ? ? ? 84 c0 74 ? ff 75" ) ).as<uintptr_t> ( );
	GetSequenceLinearMotion = pattern::find ( m_client_dll, XOR ( "55 8b ec 83 ec ? 56 8b f1 57 8b fa 85 f6 75 ? 68" ) );
	LockStudioHdr = pattern::find ( m_client_dll, XOR ( "55 8B EC 51 53 8B D9 56 57 8D B3" ) );
	SetAbsOrigin = pattern::find ( m_client_dll, XOR ( "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ? ?" ) );
	IsBreakableEntity = pattern::find ( m_client_dll, XOR ( "55 8B EC 51 56 8B F1 85 F6 74 68" ) ).as< IsBreakableEntity_t > ( );
	SetAbsVelocity = pattern::find ( m_client_dll, XOR ( "55 8B EC 83 E4 F8 83 EC 0C 53 56 57 8B 7D 08 8B F1" ) );
	AngleMatrix = pattern::find ( m_client_dll, XOR ( "E8 ? ? ? ? 8B 07 89 46 0C" ) ).rel32 ( 0x1 ).as< AngleMatrix_t > ( );
	ComputeHitboxSurroundingBox = pattern::find ( m_client_dll, XOR ( "E9 ? ? ? ? 32 C0 5D" ) ).rel32 ( 0x1 );
	GetSequenceActivity = pattern::find ( m_client_dll, XOR ( "55 8B EC 53 8B 5D 08 56 8B F1 83" ) );
	LoadFromBuffer = pattern::find ( m_client_dll, XOR ( "E8 ? ? ? ? 88 44 24 0F 8B 56 FC" ) ).rel32 ( 0x1 ).as< LoadFromBuffer_t > ( );
	ServerRankRevealAll = pattern::find ( m_client_dll, XOR ( "55 8B EC 8B 0D ? ? ? ? 68" ) ).as< ServerRankRevealAll_t > ( );
	HasC4 = pattern::find ( m_client_dll, XOR ( "56 8B F1 85 F6 74 31" ) );
	InvalidatePhysicsRecursive = pattern::find ( m_client_dll, XOR ( "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56" ) );
	//IsReady = pattern::find( m_client_dll, XOR( "E8 ? ? ? ? 59 C2 08 00 51 E8" ) ).rel32( 0x1 ).as< IsReady_t >( );
	//ShowAndUpdateSelection = pattern::find( m_client_dll, XOR( "E8 ? ? ? ? A1 ? ? ? ? F3 0F 10 40 ? C6 83" ) ).rel32( 0x1 ).as< ShowAndUpdateSelection_t >( );
	//GetEconItemView = pattern::find( m_client_dll, XOR( "8B 81 ? ? ? ? 81 C1 ? ? ? ? FF 50 04 83 C0 40 C3" ) ).as< GetEconItemView_t >( );
	//GetStaticData = pattern::find( m_client_dll, XOR( "55 8B EC 51 56 57 8B F1 E8 ? ? ? ? 0F B7 8E" ) ).as< GetStaticData_t >( );
	TEFireBullets = util::GetVirtualTablePointer ( XOR ( "client.dll" ), XOR ( "C_TEFireBullets" ) );
	SmokeCount = pattern::find ( m_client_dll, XOR ( "A3 ? ? ? ? 57 8B CB" ) ).add ( 0x1 ).to ( );
	BeamAlloc = pattern::find ( m_client_dll, XOR ( "E8 ? ? ? ? 8B F0 85 F6 74 7C" ) ).rel32< BeamAlloc_t > ( 0x1 );
	SetupBeam = pattern::find ( m_client_dll, XOR ( "E8 ? ? ? ? 8B 07 33 C9" ) ).rel32< SetupBeam_t > ( 0x1 );
	ClearNotices = pattern::find ( m_client_dll, XOR ( "E8 ? ? ? ? 68 ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 8B F0 85 F6 74 19" ) ).rel32< ClearNotices_t > ( 0x1 );
	AddListenerEntity = pattern::find ( m_client_dll, XOR ( "55 8B EC 8B 0D ? ? ? ? 33 C0 56 85 C9 7E 32 8B 55 08 8B 35" ) ).as< AddListenerEntity_t > ( );
	GetShotgunSpread = pattern::find ( m_client_dll, XOR ( "E8 ? ? ? ? EB 38 83 EC 08" ) ).rel32< GetShotgunSpread_t > ( 1 );
	BoneAccessor = pattern::find ( m_client_dll, XOR ( "8D 81 ? ? ? ? 50 8D 84 24" ) ).add ( 2 ).to< size_t > ( );
	AnimOverlay = pattern::find ( m_client_dll, XOR ( "8B 80 ? ? ? ? 8D 34 C8" ) ).add ( 2 ).to< size_t > ( );
	SpawnTime = pattern::find ( m_client_dll, XOR ( "F3 0F 5C 88 ? ? ? ? 0F" ) ).add ( 4 ).to< size_t > ( );
	IsLocalPlayer = pattern::find ( m_client_dll, XOR ( "74 ? 8A 83 ? ? ? ? 88" ) ).add ( 4 ).to< size_t > ( );
	PlayerAnimState = pattern::find ( m_client_dll, XOR ( "8B 8E ? ? ? ? 85 C9 74 3E" ) ).add ( 2 ).to< size_t > ( );
	studioHdr = pattern::find ( m_client_dll, XOR ( "8B 86 ? ? ? ? 89 44 24 10 85 C0" ) ).add ( 2 ).to< size_t > ( );
	StuckLast = pattern::find ( m_client_dll, XOR ( "89 81 ? ? ? ? b8 ? ? ? ? f7 ee c1 fa" ) ).add ( 2 ).to< size_t > ( );
	UTIL_TraceLine = pattern::find ( m_client_dll, XOR ( "55 8B EC 83 E4 F0 83 EC 7C 56 52" ) );
	CTraceFilterSimple_vmt = UTIL_TraceLine.add ( 0x3D ).to ( );
	//  CTraceFilterSkipTwoEntities_vmt = pattern::find( m_client_dll, XOR( "E8 ? ? ? ? F3 0F 10 84 24 ? ? ? ? 8D 84 24 ? ? ? ? F3 0F 58 44 24" ) ).rel32( 1 ).add( 0x59 ).to( );
	LastBoneSetupTime = InvalidateBoneCache.add ( 0x11 ).to< size_t > ( );
	MostRecentModelBoneCounter = InvalidateBoneCache.add ( 0x1B ).to< size_t > ( );
	BeamCheckNull = pattern::find ( m_client_dll, XOR ( "85 C0 74 2D 83 7D 10 00 75 1C" ) );
	SetupVelocity = pattern::find ( g_csgo.m_client_dll, XOR ( "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80" ) );
	IsLoadoutAllowed = pattern::find ( g_csgo.m_client_dll, XOR ( "84 C0 75 04 B0 01 5F" ) );
	UpdateHud = pattern::find ( g_csgo.m_client_dll, XOR ( "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C" ) );
	PainkitDefinition = pattern::find ( g_csgo.m_client_dll, XOR ( "E8 ? ? ? ? FF 76 0C 8D 48 04 E8" ) );
	SkipAnimationFrame = pattern::find ( g_csgo.m_client_dll, XOR ( "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02" ) );
	CalcPlayerView1 = pattern::find ( g_csgo.m_client_dll, XOR ( "84 C0 75 0B 8B 0D ? ? ? ? 8B 01 FF 50 4C" ) );
	CalcPlayerView2 = pattern::find ( g_csgo.m_client_dll, XOR ( "84 C0 75 08 57 8B CE E8 ? ? ? ? 8B 06" ) );
	MaintainSequenceTransition = pattern::find ( g_csgo.m_client_dll, XOR ( "84 C0 74 17 8B 87" ) );
	InvalidateBoneCacheVerification = pattern::find ( g_csgo.m_client_dll, XOR ( "C6 05 ? ? ? ? ? 89 47 70" ) ).add ( 2 );
	CamThink = pattern::find ( g_csgo.m_client_dll, XOR ( "85 C0 75 30 38 86" ) );
	RenderingOrder1 = pattern::find ( g_csgo.m_client_dll, XOR ( "85 C0 75 31 8B 46 FC" ) );
	RenderingOrder2 = pattern::find ( g_csgo.m_client_dll, XOR ( "85 C0 75 54 8B 0D? ? ? ?" ) );
	InvalidateAnimState = pattern::find ( m_client_dll, XOR ( "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56 83 E0 04" ) );
	SetupBonesPointer = ( DWORD* ) *( DWORD* ) ( pattern::find ( m_client_dll, XOR ( "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C" ) ).add ( 0x4E ).as< unsigned long > ( ) );
	GetPlayerMaxSpeed = pattern::find ( m_client_dll, XOR ( "55 8b ec 83 ec ? 56 8b f1 80 be ? ? ? ? ? 0f 84 ? ? ? ? 8b 06 ff 90" ) ).as < GetPlayerMaxSpeed_t > ( );
	//PlayerResource = pattern::find( m_client_dll, XOR( "83 3d ? ? ? ? ? 53 56 8b d9 c7 44 24 ? ? ? ? ? 57 89 5c 24 ? 0f 84 ? ? ? ? 8b 75" ) ).add( 2 ).to< uintptr_t >( );
	//IsBotPlayerResource = pattern::find( m_client_dll, XOR( "8d b8 ? ? ? ? 85 ff 74 ? 8b 41 ? 83 c1 ? 56 8b 37 ff 50 ? 50 8b 46 ? 8b cf ff d0 5e" ) ).add( 2 ).to< size_t >( );


	// exported functions.
	RandomSeed = PE::GetExport ( m_vstdlib_dll, HASH ( "RandomSeed" ) ).as< RandomSeed_t > ( );
	RandomInt = PE::GetExport ( m_vstdlib_dll, HASH ( "RandomInt" ) ).as< RandomInt_t > ( );
	RandomFloat = PE::GetExport ( m_vstdlib_dll, HASH ( "RandomFloat" ) ).as< RandomFloat_t > ( );

	// prediction pointers.
	m_nPredictionRandomSeed = util::get_method ( m_prediction, CPrediction::RUNCOMMAND ).add ( 0x30 ).get< int* > ( );
	m_pPredictionPlayer = util::get_method ( m_prediction, CPrediction::RUNCOMMAND ).add ( 0x54 ).get< Player* > ( );


	// some weird tier0 stuff that prevents me from debugging properly...
#ifdef _DEBUG
	Address debugbreak = pattern::find ( g_csgo.m_client_dll, XOR ( "CC F3 0F 10 4D ? 0F 57 C0" ) );

	DWORD old;
	g_winapi.VirtualProtect ( debugbreak, 1, PAGE_EXECUTE_READWRITE, &old );

	debugbreak.set< uint8_t > ( 0x90 );

	g_winapi.VirtualProtect ( debugbreak, 1, old, &old );
#endif
	// init everything else.
	g_config.init ( );

	// g_netvars stores all netvar offsets into an unordered_map, EntOffsets is for the raw offset values so we don't have to access the unordered_map a bunch.
	g_netvars.init ( );
	g_entoffsets.init ( );

	g_listener.init ( );
	render::init ( );
	g_chams.init ( );
	g_hooks.init ( );
	g_detours.init ( );
	g_skins.init ( );
	g_visuals.GetViewModel ( );

	// if we injected and we're ingame, run map load func.
	if ( m_engine->IsInGame ( ) ) {
		g_cl.OnMapload ( );
		//g_csgo.cl_fullupdate->m_callback( );
	}

	g_cl.BuildSeedTable ( );

	// setup console.
	/*g_csgo.m_cvar->FindVar( HASH( "contimes" ) )->SetValue( 15 );
	g_csgo.m_cvar->FindVar( HASH( "con_filter_text" ) )->SetValue( XOR( "L " ) );
	g_csgo.m_cvar->FindVar( HASH( "con_filter_text_out" ) )->SetValue( XOR( " " ) );
	g_csgo.m_cvar->FindVar( HASH( "con_filter_enable" ) )->SetValue( 2 );*/
	g_csgo.m_engine->ExecuteClientCmd ( XOR ( "clear" ) );

	// refresh configs.
	g_config.refresh ( );

	m_done = true;
	return true;
}

bool game::IsBreakable ( Entity* ent ) {
	bool        ret;
	ClientClass* cc;
	const char* name;
	char* takedmg, old_takedmg;

	static size_t m_takedamage_offset { *( size_t* ) ( ( uintptr_t ) g_csgo.IsBreakableEntity + 0x26 ) };

	// skip null ents and the world ent.
	if ( !ent || ent->index ( ) == 0 )
		return false;

	// get m_takedamage and save old m_takedamage.
	takedmg = ( char* ) ( ( uintptr_t ) ent + m_takedamage_offset );
	old_takedmg = *takedmg;

	// get clientclass.
	cc = ent->GetClientClass ( );

	if ( cc ) {
		// get clientclass network name.
		name = cc->m_pNetworkName;

		// CBreakableSurface, CBaseDoor, ...
		if ( name [ 1 ] != 'F'
			|| name [ 4 ] != 'c'
			|| name [ 5 ] != 'B'
			|| name [ 9 ] != 'h' ) {
			*takedmg = DAMAGE_YES;
		}
	}

	ret = g_csgo.IsBreakableEntity ( ent );
	*takedmg = old_takedmg;

	return ret;
}

Beam_t* game::CreateGenericBeam ( const BeamInfo_t& beam_info ) {
	Beam_t* out;
	const model_t* sprite;

	out = g_csgo.BeamAlloc ( g_csgo.m_beams, beam_info.m_bRenderable );
	if ( !out )
		return nullptr;

	out->die = g_csgo.m_globals->m_curtime;

	if ( beam_info.m_nModelIndex < 0 )
		return nullptr;

	sprite = g_csgo.m_model_info->GetModel ( beam_info.m_nModelIndex );
	if ( sprite ) {
		out->type = ( beam_info.m_nType < 0 ) ? 0 : beam_info.m_nType;
		out->modelIndex = beam_info.m_nModelIndex;
		out->haloIndex = beam_info.m_nHaloIndex;
		out->haloScale = beam_info.m_flHaloScale;
		out->frame = 0;
		out->frameRate = 0;
		out->frameCount = g_csgo.m_model_info->GetModelFrameCount ( sprite );
		out->freq = g_csgo.m_globals->m_curtime * beam_info.m_flSpeed;
		out->die = g_csgo.m_globals->m_curtime + beam_info.m_flLife;
		out->width = beam_info.m_flWidth;
		out->endWidth = beam_info.m_flEndWidth;
		out->fadeLength = beam_info.m_flFadeLength;
		out->amplitude = beam_info.m_flAmplitude;
		out->brightness = beam_info.m_flBrightness;
		out->speed = beam_info.m_flSpeed;
		out->life = beam_info.m_flLife;
		out->flags = 0;
		out->attachment [ 0 ] = beam_info.m_vecStart;
		out->attachment [ 1 ] = beam_info.m_vecEnd;

		out->delta = ( beam_info.m_vecEnd - beam_info.m_vecStart );

		if ( beam_info.m_nSegments == -1 ) {
			if ( out->amplitude >= 0.50 )
				out->segments = out->delta.length ( ) * 0.25 + 3;  // one per 4 pixels
			else
				out->segments = out->delta.length ( ) * 0.075 + 3; // one per 16 pixels
		}

		else
			out->segments = beam_info.m_nSegments;
	}

	// note - dex; this is CViewRenderBeams::SetBeamAttributes, but it got inlined so i have to rebuild it.
	//             not sure if this is needed, it's taken from CViewRenderBeams::CreateBeamPoints.
	out->frame = ( float ) beam_info.m_nStartFrame;
	out->frameRate = beam_info.m_flFrameRate;
	out->flags |= beam_info.m_nFlags;
	out->r = beam_info.m_flRed;
	out->g = beam_info.m_flGreen;
	out->b = beam_info.m_flBlue;

	return out;
}

void game::RemoveChokeLimit ( ) {

	auto clMoveChokeClamp = pattern::find ( g_csgo.m_engine_dll, XOR ( "B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC" ) ).add ( 1 ).as< uintptr_t > ( );

	unsigned long protect = 0;

	VirtualProtect ( ( void* ) clMoveChokeClamp, 4, PAGE_EXECUTE_READWRITE, &protect );
	*( std::uint32_t* )clMoveChokeClamp = 62;
	VirtualProtect ( ( void* ) clMoveChokeClamp, 4, protect, &protect );
}
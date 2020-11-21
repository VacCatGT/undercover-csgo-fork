#pragma once

struct OffScreenDamageData_t {
    float m_time, m_color_step;
    Color m_color;

    __forceinline OffScreenDamageData_t( ) : m_time{ 0.f }, m_color{ colors::white } {}
    __forceinline OffScreenDamageData_t( float time, float m_color_step, Color color ) : m_time{ time }, m_color{ color } {}
};

struct WorldHitmarkerData_t {
    float m_time, m_alpha;
    float m_pos_x, m_pos_y, m_pos_z;

    vec2_t m_world_pos;
    bool m_world_to_screen;
};

class Visuals {
public:
	std::array< bool, 64 >                  m_draw;
	std::array< float, 2048 >               m_opacities;
    std::array< OffScreenDamageData_t, 64 > m_offscreen_damage;
    std::vector< WorldHitmarkerData_t >     m_world_hitmarkers;
	vec2_t                                  m_crosshair;
	bool                                    m_thirdperson;
    bool                                    m_zoom;
    bool                                    m_old_thirdperson;
	float					                m_hit_start, m_hit_end, m_hit_duration;
    std::array < Rect, 64 >                 m_saved_boxes[ 65 ];

    // info about planted c4.
    bool        m_c4_planted;
    Entity      *m_planted_c4;
    float       m_planted_c4_explode_time;
    vec3_t      m_planted_c4_explosion_origin;
    float       m_planted_c4_damage;
    float       m_planted_c4_radius;
    float       m_planted_c4_radius_scaled;
    std::string m_last_bombsite;

    // viewmodel.
    float       m_viewmodel[ 3 ];
    bool        m_viewmodel_modified;

	IMaterial* smoke1;
	IMaterial* smoke2;
	IMaterial* smoke3;
	IMaterial* smoke4;

    std::unordered_map< int, char > m_weapon_icons = {
    	{ DEAGLE, 'F' },
    	{ ELITE, 'S' },
    	{ FIVESEVEN, 'U' },
    	{ GLOCK, 'C' },
    	{ AK47, 'B' },
    	{ AUG, 'E' },
    	{ AWP, 'R' },
    	{ FAMAS, 'T' },
    	{ G3SG1, 'I' },
    	{ GALIL, 'V' },
    	{ M249, 'Z' },
    	{ M4A4, 'W' },
    	{ MAC10, 'L' },
    	{ P90, 'M' },
    	{ UMP45, 'Q' },
    	{ XM1014, ']' },
    	{ BIZON, 'D' },
    	{ MAG7, 'K' },
    	{ NEGEV, 'Z' },
    	{ SAWEDOFF, 'K' },
    	{ TEC9, 'C' },
    	{ ZEUS, 'Y' },
    	{ P2000, 'Y' },
    	{ MP7, 'X' },
    	{ MP9, 'D' },
    	{ NOVA, 'K' },
    	{ P250, 'Y' },
    	{ SCAR20, 'I' },
    	{ SG553, '[' },
    	{ SSG08, 'N' },
    	{ KNIFE_CT, 'J' },
    	{ FLASHBANG, 'G' },
    	{ HEGRENADE, 'H' },
    	{ SMOKE, 'P' },
    	{ MOLOTOV, 'H' },
    	{ DECOY, 'G' },
    	{ FIREBOMB, 'H' },
    	{ C4, '\\' },
    	{ KNIFE_T, 'J' },
    	{ M4A1S, 'W' },
    	{ USPS, 'Y' },
    	{ CZ75A, 'Y' },
    	{ REVOLVER, 'F' },
    	{ KNIFE_BAYONET, 'J' },
    	{ KNIFE_FLIP, 'J' },
    	{ KNIFE_GUT, 'J' },
    	{ KNIFE_KARAMBIT, 'J' },
    	{ KNIFE_M9_BAYONET, 'J' },
    	{ KNIFE_HUNTSMAN, 'J' },
    	{ KNIFE_FALCHION, 'J' },
    	{ KNIFE_BOWIE, 'J' },
    	{ KNIFE_BUTTERFLY, 'J' },
    	{ KNIFE_SHADOW_DAGGERS, 'J' },
    };

public:
	static void ModulateWorld( );
    void VisualInterpolation( );
    void RenderAutoPeek( );
    static void ManipulateSkybox( );
    void Zoom( CViewSetup* view );
    void ThirdpersonThink( );
	void HitmarkerScreen( );
	void HitmarkerWorld( );
	void NoSmoke( );
	void think( );
    void Watermark( );
    void Fog( );
	void Spectators( );
	void Keybinds( );
	void SpreadCrosshair( );
    void PenetrationCrosshair( );
    void DrawPlantedC4( Entity* ent );
	void draw( Entity* ent );
	void DrawProjectile( Weapon* ent );
	void DrawItem( Weapon* item );
	void OffScreen( Player* player, int alpha );
	void DrawPlayer( Player* player );
	bool GetPlayerBoxRect( Player* player, Rect& box );
	void DrawHistorySkeleton( Player* player, int opacity );
	void DrawSkeleton( Player* player, int opacity );
	void RenderGlow( );
	void DrawHitboxMatrix( Player* player, matrix3x4_t* bones, Color col, float time );
	void DrawHitboxMatrix( LagComp::LagRecord_t* record, Color col, float time );
    void DrawBeams( );
	void DebugAimbotPoints( Player* player );
    void GetViewModel( );
    void ViewModel( );
    void ClientImpacts( );
	void DrawInfernoRadius( );
	void DrawSmokeRadius( );
};

extern Visuals g_visuals;
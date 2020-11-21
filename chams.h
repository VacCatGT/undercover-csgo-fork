#pragma once

class Chams {
public:
	enum model_type_t : uint32_t {
		invalid = 0,
		player,
		weapon,
		arms,
		view_weapon
	};

	enum color_type_t : uint32_t {
		normal,
		envmap_tint
	};

	struct Hitdata_t {
		BoneArray* m_last_pose;
		int time;
		Player* ent;
	};

public:
	void SetColor( Color col, color_type_t type );
	void SetAlpha( float alpha, IMaterial* mat = nullptr );
	void SetupMaterial( IMaterial* mat, Color col, bool z_flag );

	void init( );

	void DrawShotRecord( Player* ent, uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone );
	bool GenerateLerpedMatrix( int index, BoneArray* out );
	void RenderHistoryChams( int index );
	void ApplyChams( std::string material, std::string color, float alpha, int material_flag, bool flag_value, uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone,  bool chams_background = false );
	bool DrawModel( uintptr_t ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone );
	void SceneEnd( );
	bool IsInViewPlane( const vec3_t& world );
	bool SortPlayers( );

public:
	std::vector< Player* > m_players;
	bool m_running;
	IMaterial* debugambientcube;
	IMaterial* debugdrawflat;
	IMaterial* silhouette;
	std::vector< Hitdata_t > m_hits[ 65 ];
	BoneArray m_stored_matrices[ 65 ] [ 128 ];
};

extern Chams g_chams;
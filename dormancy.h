#pragma once

class Dormancy {
private:
	struct dormancy_data
	{
		Player* m_entity;
		bool	m_prevstate;
		bool	m_set;
		int		m_flags;
		vec3_t	m_oldpos = vec3_t( 0.f, 0.f, 0.f );
		struct {
			int m_time;
		} m_sound_data;
	};
public:
	void PositionCorrection( int stage );
	bool Set( const int index );

	bool IsViable( Player* entity );

	void SoundDormancy( );
	void AdjustPlayer( const int index );

	CUtlVector< IEngineSound::sound_info > soundbuffer;
	CUtlVector< IEngineSound::sound_info > sound_list;

	dormancy_data m_players[ 65 ];

}; extern Dormancy g_dormancy;
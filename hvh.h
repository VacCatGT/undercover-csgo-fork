#pragma once

class HVH {
public:
	bool   m_fake_duck;
	bool   m_double_tap;
	bool   m_hide_shots;
	bool   m_auto_peek;
	bool   m_jitter;
	bool   m_jitter_flip;
	Weapon* m_old_weapon;
	bool   m_invert;
	float  m_stored_view;
	int dword_3F32CD78;

	struct {
		bool m_autopeeking;
		bool m_should_retrack;
		vec3_t m_old_origin;
	} m_autopeek_data;

public:
	void AntiAim( );
	float Pitch( int a1 );
	float Real( float a1 );
	void AtTargets( );
	void Manual( );
	double Spin( float a1, bool a2 );
	float GetFakeAngle( float m_vecViewIn, bool m_bInvert, float MaxDesyncDelta );
	void Fake( );
	bool CanAntiAim();
	void Fakelag( );
	void FakeDuck( );
	void AutoPeek( );
	void SlowWalk( );
};

extern HVH g_hvh;
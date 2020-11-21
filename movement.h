#pragma once

struct MovementValues_t {
	float m_forwardmove;
	float m_sidemove;
};

class Movement {
public:
	float  m_speed;
	float  m_force;
	float  m_ideal;
	vec3_t m_mins;
	vec3_t m_maxs;
	vec3_t m_origin;
	vec3_t m_peek_origin;
	bool   m_started_peeking;
	bool   m_peek_ready = true;
	bool   m_shot;
	float  m_switch_value = 1.f;
	int    m_strafe_index;
	float  m_old_yaw;
	float  m_circle_yaw;
	bool   m_slow_motion;
	bool   m_in_transition;
	bool   m_edge_jump;
	bool   m_bhopping;
	MovementValues_t m_backup_moves;  
	std::vector< vec3_t > m_saved_positions;

public:
	void BunnyHop( );
	void FastStop( );
	void DirectionalStrafer( );
	void RotateMovement( float rotation );
	void FixMove( CUserCmd* cmd, ang_t old_angles );
	void EdgeJump( );
	void AutoStop( bool between = false );
	void FakeWalk( );
	void SlowWalk( );
	void SlideWalk( );
	void ClampMovementSpeed( float speed );
	void Stop( float speed );
};

extern Movement g_movement;
#pragma once

class Animations {
public:
	struct AnimationInfo_t {
		AnimationInfo_t( Player* pEntity, std::deque<LagComp::LagRecord_t> pRecords )
			: m_pEntity( pEntity ), m_pRecords( std::move( pRecords ) ), m_flLastSpawnTime( 0 ) {
		}

		void UpdateAnimations( LagComp::LagRecord_t* pRecord, LagComp::LagRecord_t* pPreviousRecord );

		Player* m_pEntity;
		std::deque<LagComp::LagRecord_t> m_pRecords;

		LagComp::LagRecord_t m_LatestRecord;
		LagComp::LagRecord_t m_PreviousRecord;

		float m_flLastSpawnTime{ };

		std::pair<int, std::pair<int, int>> m_iMissedDueToResolver{ };
		int m_iMissedDueToSpread{ };

		float m_flPreviousAngle{ };

		float m_flBrute{ };
		float m_flLatestDelta{ };
		float m_flBestSide{ };
		float m_flFinalResolveAngle{ };

		float m_flOldYaw{ };

		float m_flLastPinPulled{ };

		int m_iResolverIndex{ };

		bool m_bWalking{ };
		bool m_bUsingMaxDesync{ };

		ang_t m_vecLastReliableAngle;
	};

	struct AnimationData_t {
		float m_flPrimaryCycle;
		float m_flMovePlaybackRate;
		float m_flFeetWeight;
		float m_flVelocityLengthXY;
		int m_iMoveState;
	};

	std::unordered_map<ulong_t, AnimationInfo_t> m_ulAnimationInfo;

	void UpdatePlayer( Player* pEntity );
	void UpdateAnimations( Player* player, LagComp::LagRecord_t* record, int index, float angle = 58.f );
	bool GenerateMatrix( Player* pEntity, BoneArray* pBoneToWorldOut, int boneMask, float currentTime );
	void GetFakeMatrix( );
	void BuildLocalBones( Stage_t stage );
	void UpdateLocalAnimations( );
	void RebuiltLayer6( Player* player, LagComp::LagRecord_t::LayerData_t* record );

	AnimationInfo_t* GetAnimationInfo( Player* pEntity );

	LagComp::LagRecord_t m_RealData;
	LagComp::LagRecord_t m_RealShotData;

	LagComp::LagRecord_t m_FakeData;
	LagComp::LagRecord_t m_FakeShotData;

	AnimationData_t m_animation_data[65];

	bool     m_init_fake;
	float    m_fake_spawn_time;
	bool     m_allow_bones;
	bool     m_force_bones;
	bool     m_bEnablePVS;
	bool     m_local_matrix_ret;
	int      m_holding_firing_anim;
	bool     m_in_holding_cooldown;
	vec3_t   m_local_anim_origin[ 128 ];
	vec3_t   m_fake_anim_origin[ 128 ];
	bool     m_recreate_fake;
};

extern Animations g_anims;
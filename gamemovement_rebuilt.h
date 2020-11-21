//#pragma once
//#define MAX_CLIMB_SPEED	200
//
////-----------------------------------------------------------------------------
//// An entity identifier that works in both game + client dlls
////-----------------------------------------------------------------------------
//
//typedef CBaseHandle EntityHandle_t;
//
//
//#define INVALID_ENTITY_HANDLE INVALID_EHANDLE_INDEX
//#define PRINTF_FORMAT_STRING _Printf_format_string_
//enum PLAYER_ANIM;
//
//#define MAX_PLAYERS 64
//
//using soundlevel_t = int;
//using IntervalType_t = int;
//
//enum
//{
//	WL_NotInWater = 0,
//	WL_Feet,
//	WL_Waist,
//	WL_Eyes
//};
//
//class CMoveData
//{
//	//public:
//	//    char pad_0x00[183];
//public:
//	bool            m_bFirstRunOfFunctions : 1;
//	bool            m_bGameCodeMovedPlayer : 1;
//	bool            m_bNoAirControl : 1;
//
//	EntityHandle_t    m_nPlayerHandle;    // edict index on server, client entity handle on client
//
//	int                m_nImpulseCommand;    // Impulse command issued.
//	ang_t            m_vecViewAngles;    // Command view angles (local space)
//	ang_t            m_vecAbsViewAngles;    // Command view angles (world space)
//	int                m_nButtons;            // Attack buttons.
//	int                m_nOldButtons;        // From host_client->oldbuttons;
//	float            m_flForwardMove;
//	float            m_flSideMove;
//	float            m_flUpMove;
//
//	float            _m_flMaxSpeed;
//	float            m_flClientMaxSpeed;
//
//	// Variables from the player edict (sv_player) or entvars on the client.
//	// These are copied in here before calling and copied out after calling.
//	vec3_t            m_vecVelocity_;        // edict::velocity        // Current movement direction.
//	vec3_t            m_vecOldVelocity;
//	float            somefloat;
//	ang_t            m_vecAngles;        // edict::angles
//	ang_t            m_vecOldAngles;
//
//	// Output only
//	float            m_outStepHeight;    // how much you climbed this move
//	vec3_t            m_outWishVel;        // This is where you tried 
//	vec3_t            m_outJumpVel;        // This is your jump velocity
//
//										   // Movement constraints    (radius 0 means no constraint)
//	vec3_t            m_vecConstraintCenter;
//	float            m_flConstraintRadius;
//	float            m_flConstraintWidth;
//	float            m_flConstraintSpeedFactor;
//	bool            m_bConstraintPastRadius;        ///< If no, do no constraining past Radius.  If yes, cap them to SpeedFactor past radius
//
//	void            SetAbsOrigin( const vec3_t& vec );
//	const vec3_t& GetAbsOrigin( ) const;
//
//public:
//	vec3_t            _m_vecAbsOrigin;        // edict::origin
//
//};
//
////-----------------------------------------------------------------------------
//// Functions the engine provides to IGameMovement to assist in its movement.
////-----------------------------------------------------------------------------
//
//class IMoveHelper
//{
//public:
//	// Call this to set the singleton
//	static IMoveHelper* GetSingleton( ) { return sm_pSingleton; }
//
//	// Methods associated with a particular entity
//	virtual	char const* GetName( EntityHandle_t handle ) const = 0;
//
//	// sets the entity being moved
//	virtual void	SetHost( Entity* host ) = 0;
//
//	// Adds the trace result to touch list, if contact is not already in list.
//	virtual void	ResetTouchList( void ) = 0;
//	virtual bool	AddToTouched( const CGameTrace& tr, const vec3_t& impactvelocity ) = 0;
//	virtual void	ProcessImpacts( void ) = 0;
//
//	// Numbered line printf
//	virtual void	Con_NPrintf( int idx, char const* fmt, ... ) = 0;
//
//	// These have separate server vs client impementations
//	virtual void	StartSound( const vec3_t& origin, int channel, char const* sample, float volume, soundlevel_t soundlevel, int fFlags, int pitch ) = 0;
//	virtual void	StartSound( const vec3_t& origin, const char* soundname ) = 0;
//	virtual void	PlaybackEventFull( int flags, int clientindex, unsigned short eventindex, float delay, vec3_t& origin, vec3_t& angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 ) = 0;
//
//	// Apply falling damage to m_pHostPlayer based on m_pHostPlayer->m_flFallVelocity.
//	virtual bool	PlayerFallingDamage( void ) = 0;
//
//	// Apply falling damage to m_pHostPlayer based on m_pHostPlayer->m_flFallVelocity.
//	virtual void	PlayerSetAnimation( PLAYER_ANIM playerAnim ) = 0;
//
//	virtual IPhysicsSurfaceProps* GetSurfaceProps( void ) = 0;
//
//	virtual bool IsWorldEntity( const CBaseHandle& handle ) = 0;
//
//protected:
//	// Inherited classes can call this to set the singleton
//	static void SetSingleton( IMoveHelper* pMoveHelper ) { sm_pSingleton = pMoveHelper; }
//
//	// Clients shouldn't call delete directly
//	virtual			~IMoveHelper( ) {}
//
//	// The global instance
//	static IMoveHelper* sm_pSingleton;
//};
//
//class CMoveHelperClient : public IMoveHelper
//{
//public:
//	CMoveHelperClient( void );
//	virtual			~CMoveHelperClient( void );
//
//	char const* GetName( EntityHandle_t handle ) const;
//
//	// touch lists
//	virtual void	ResetTouchList( void );
//	virtual bool	AddToTouched( const CGameTrace& tr, const vec3_t& impactvelocity );
//	virtual void	ProcessImpacts( void );
//
//	// Numbered line printf
//	virtual void	Con_NPrintf( int idx, char const* fmt, ... );
//
//	virtual bool	PlayerFallingDamage( void );
//	virtual void	PlayerSetAnimation( PLAYER_ANIM eAnim );
//
//	// These have separate server vs client impementations
//	virtual void	StartSound( const vec3_t& origin, int channel, char const* sample, float volume, soundlevel_t soundlevel, int fFlags, int pitch );
//	virtual void	StartSound( const vec3_t& origin, const char* soundname );
//	virtual void	PlaybackEventFull( int flags, int clientindex, unsigned short eventindex, float delay, vec3_t& origin, vec3_t& angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 );
//	virtual IPhysicsSurfaceProps* GetSurfaceProps( void );
//
//	virtual bool IsWorldEntity( const CBaseHandle& handle );
//
//	void			SetHost( Entity* host );
//
//private:
//	// results, tallied on client and server, but only used by server to run SV_Impact.
//	// we store off our velocity in the CGameTrace structure so that we can determine results
//	// of shoving boxes etc. around.
//	struct touchlist_t
//	{
//		vec3_t	deltavelocity;
//		CGameTrace trace;
//
//		touchlist_t( ) {}
//
//	private:
//		touchlist_t( const touchlist_t& src );
//	};
//
//	CUtlVector<touchlist_t>			m_TouchList;
//
//	Entity* m_pHost;
//};
//
////-----------------------------------------------------------------------------
//// Purpose: The basic player movement interface
////-----------------------------------------------------------------------------
//
//class IGameMovement
//{
//public:
//	virtual			~IGameMovement( void ) {}
//
//	// Process the current movement command
//	virtual void	ProcessMovement( Player* pPlayer, CMoveData* pMove ) = 0;
//	virtual void	Reset( void ) = 0;
//	virtual void	StartTrackPredictionErrors( Player* pPlayer ) = 0;
//	virtual void	FinishTrackPredictionErrors( Player* pPlayer ) = 0;
//	virtual void	DiffPrint( char const* fmt, ... ) = 0;
//
//	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
//	virtual vec3_t const& GetPlayerMins( bool ducked ) const = 0;
//	virtual vec3_t const& GetPlayerMaxs( bool ducked ) const = 0;
//	virtual vec3_t const& GetPlayerViewOffset( bool ducked ) const = 0;
//
//	virtual bool		IsMovingPlayerStuck( void ) const = 0;
//	virtual Player* GetMovingPlayer( void ) const = 0;
//	virtual void		UnblockPusher( Player* pPlayer, Entity* pPusher ) = 0;
//
//	virtual void SetupMovementBounds( CMoveData* pMove ) = 0;
//};
//
//
//class CGameMovement : public IGameMovement
//{
//public:
//	//DECLARE_CLASS_NOBASE(CGameMovement);
//
//	//CGameMovement(void);
//	//virtual			~CGameMovement(void);
//
//	CGameMovement( void );
//	virtual	~CGameMovement( void );
//	virtual void ProcessMovement( Player* pPlayer, CMoveData* pMove ); // 1
//	virtual void Reset( );// 2
//	virtual void StartTrackPredictionErrors( Player* pPlayer ); // 3
//	virtual void FinishTrackPredictionErrors( Player* pPlayer ); // 4
//	virtual void DiffPrint( char const* fmt, ... ); // 5
//	virtual const vec3_t& GetPlayerMins( bool ducked ) const; // 6
//	virtual const vec3_t& GetPlayerMaxs( bool ducked ) const; // 7
//	virtual const vec3_t& GetPlayerViewOffset( bool ducked ) const; // 8
//	virtual bool IsMovingPlayerStuck( ) const; // 9
//	virtual Player* GetMovingPlayer( ) const; // 10
//	virtual void UnblockPusher( Player* pPlayer, Entity* pPusher ); // 11
//	virtual void SetupMovementBounds( CMoveData* moveData ); // 12
//	virtual vec3_t GetPlayerMins( ); // 13
//	virtual vec3_t GetPlayerMaxs( ); // 14
//	virtual void TracePlayerBBox( const vec3_t& rayStart, const vec3_t& rayEnd, int fMask, int collisionGroup, CGameTrace& tracePtr ); // 15
//	virtual unsigned int PlayerSolidMask( bool brushOnly = false, Player* testPlayer = nullptr ); // 16
//	virtual void PlayerMove( ); // 17
//	virtual float CalcRoll( const ang_t& angles, const vec3_t& velocity, float rollangle, float rollspeed ); // 18
//	virtual void DecayViewPunchAngle( ); // 19
//	virtual void CheckWaterJump( ); // 20
//	virtual void WaterMove( ); // 21
//	virtual void SlimeMove( ); //22
//	virtual void WaterJump( ); // 23
//	virtual void Friction( ); // 24
//	virtual void AirAccelerate( vec3_t& wishdir, float wishspeed, float accel ); // 25
//	virtual void AirMove( ); // 26
//	virtual bool CanAccelerate( ); // 27
//	virtual void Accelerate( vec3_t& wishdir, float wishspeed, float accel ); // 28
//	virtual void WalkMove( ); // 29
//	virtual void StayOnGround( ); // 30
//	virtual void FullWalkMove( ); // 31
//	virtual void OnJump( float stamina ); // 32
//	virtual void nullsub3( ); // 33
//	virtual void OnLand( float flFallVelocity ); // 34
//	bool		CheckInterval( IntervalType_t type );
//	virtual int GetCheckInterval( IntervalType_t type ); // 35
//	virtual void StartGravity( ); // 36
//	virtual void FinishGravity( ); // 37
//	virtual void AddGravity( ); // 38
//	virtual bool CheckJumpButton( ); // 39
//	virtual void FullTossMove( ); // 40
//	virtual void FullLadderMove( ); // 41
//	virtual int TryPlayerMove( vec3_t* pFirstDest = NULL, CGameTrace* pFirstTrace = NULL ); // 42
//	virtual bool LadderMove( ); // 43
//	virtual bool OnLadder( CGameTrace& pm ); // 44
//	virtual float LadderDistance( ); // 45
//	virtual unsigned int LadderMask( ); // 46
//	virtual float ClimbSpeed( ); // 47
//	virtual float LadderLateralMultiplier( ); // 48
//	void   CheckVelocity( void );
//	virtual int ClipVelocity( vec3_t& in, vec3_t& normal, vec3_t& out, float overbounce ); // 49
//	virtual bool CheckWater( ); // 50
//	virtual void GetWaterCheckPosition( int waterLevel, vec3_t* pos ); // 51
//	virtual void CategorizePosition( ); // 52
//	virtual void CheckParameters( ); // 53
//	virtual void ReduceTimers( ); // 54
//	virtual void CheckFalling( ); // 55
//	virtual void PlayerRoughLandingEffects( float fVol ); // 56
//	virtual void Duck( );// 57
//	virtual void HandleDuckingSpeedCrop( ); //58
//	virtual void FinishUnduck( ); //59
//	virtual void FinishDuck( ); // 60
//	virtual bool CanUnduck( ); // 61
//	virtual void UpdateDuckJumpEyeOffset( ); // 62
//	virtual bool CanUnduckJump( CGameTrace& tr ); // 63
//	virtual void StartUnduckJump( ); // 64
//	virtual void FinishUnduckJump( CGameTrace& tr ); // 65
//	virtual void SetDuckedEyeOffset( float duckFraction ); // 66
//	virtual void FixPlayerCrouchStuck( bool upward ); // 67
//	virtual void CategorizeGroundSurface( CGameTrace& tr ); // 68
//	virtual bool InWater( ); // 69
//	virtual CBaseHandle TestPlayerPosition( const vec3_t& pos, int collisionGroup, CGameTrace& pm ); // 70
//	virtual void SetGroundEntity( CGameTrace* pm ); // 71
//	virtual void StepMove( vec3_t& vecDestination, CGameTrace& trace ); // 72
//	virtual ITraceFilter* LockTraceFilter( int collisionGroup ); // 73
//	virtual void UnlockTraceFilter( ITraceFilter*& filter ); // 74
//	virtual bool GameHasLadders( ); // 75
//	void PerformFlyCollisionResolution( CGameTrace& pm, vec3_t& move );
//	void PushEntity( vec3_t& push, CGameTrace* pTrace );
//	void ResetGetWaterContentsForPointCache( );
//	int GetWaterContentsForPointCached( const vec3_t& point, int slot );
//	float ComputeConstraintSpeedFactor( void );
//	int CheckStuck( void );
//	void FullNoClipMove( float factor, float maxacceleration );
//	void FullObserverMove( void );
//
//	enum
//	{
//		// eyes, waist, feet points (since they are all deterministic
//		MAX_PC_CACHE_SLOTS = 3,
//	};
//
//	Player* player; //4
//	CMoveData* mv; //8
//	int m_nOldWaterLevel; //12
//	float m_flWaterEntryTime; //16
//	int m_nOnLadder; //20
//	vec3_t m_vecForward; //24
//	vec3_t m_vecRight; //36
//	vec3_t m_vecUp; //48
//	int m_CachedGetPointContents[ MAX_PLAYERS ][ MAX_PC_CACHE_SLOTS ];
//	vec3_t m_CachedGetPointContentsPoint[ MAX_PLAYERS ][ MAX_PC_CACHE_SLOTS ];
//	BOOL m_bSpeedCropped; //3132
//	bool m_bProcessingMovement; //3136
//	bool m_bInStuckTest; //3137
//	float			m_flStuckCheckTime[ MAX_PLAYERS + 1 ][ 2 ]; //3138
//	ITraceListData* m_pTraceListData; //3660
//	int m_nTraceCount; //3664
//};
//
//class CCSGameMovement : public CGameMovement
//{
//public:
//	CCSGameMovement( );
//
//	virtual void ProcessMovement( Player* basePlayer, CMoveData* moveData ); // 1
//	// CGameMovement::Reset  // 2
//	// CGameMovement::StartTrackPredictionErrors(); // 3
//	// CGameMovement::FinishTrackPredictionErrors(); // 4
//	virtual void DiffPrint( char const* fmt, ... ); // 5
//	// CGameMovement::GetPlayerMins(bool ducked) // 6
//	// CGameMovement::GetPlayerMaxs(bool ducked) // 7
//	// CGameMovement::GetPlayerViewOffset // 8
//	// CGameMovement::IsMovingPlayerStuck // 9
//	// CGameMovement::GetMovingPlayer // 10
//	virtual void UnblockPusher( Player* pPlayer, Entity* pPusher ); // 11
//	// CGameMovement::SetupMovementBounds // 12
//	// CGameMovement::GetPlayerMins(); // 13
//	// CGameMovement::GetPlayerMaxs(); // 14
//	// CGameMovement::TracePlayerBBox(); // 15
//	virtual unsigned int PlayerSolidMask( bool brushOnly = false, Player* testPlayer = nullptr ); // 16
//	virtual void PlayerMove( ); // 17
//	void AutoMountMove( );
//	// CGameMovement::CalcRoll // 18
//	// CGameMovement::DecayViewPunchAngle // 19
//	// CGameMovement::CheckWaterJump // 20
//	// CGameMovement::WaterMove(); // 21
//	// CGameMovement::SlimeMove(); //22
//	// CGameMovement::WaterJump // 23
//	// CGameMovement::Friction // 24
//	// CGameMovement::AirAccelerate // 25
//	virtual void AirMove( ); // 26 // 26
//	virtual bool CanAccelerate( ); // 27
//	virtual void Accelerate( vec3_t&, float, float ); // 28
//	virtual void WalkMove( ); // 29
//	// CGameMovement::StayOnGround // 30
//	// CGameMovement::FullWalkMove // 31
//	virtual void OnJump( float stamina ); // 32
//	virtual void nullsub3( ); // 33
//	virtual void OnLand( float flFallVelocity );	// 34
//	// CGameMovement::GetCheckInterval // 35
//	// CGameMovement::StartGravity // 36
//	// CGameMovement::FinishGravity // 37
//	// CGameMovement::AddGravity //38
//	virtual bool CheckJumpButton( ); // 39
//	// CGameMovement::FullTossMove // 40
//	// CGameMovement::FullLadderMove // 41
//	// CGameMovement::TryPlayerMove // 42
//	virtual bool LadderMove( ); // 43
//	virtual bool OnLadder( CGameTrace& trace ); // 44
//	virtual float LadderDistance( ); // 45
//	// CGameMovement::LadderMask // 46
//	virtual float ClimbSpeed( ) const; // 47
//	virtual float LadderLateralMultiplier( ) const; // 48
//	// CGameMovement::ClipVelocity // 49
//	// CGameMovement::CheckWater // 50
//	// CGameMovement::GetWaterCheckPosition // 51
//	// CGameMovement::CategorizePosition // 52
//	virtual void CheckParameters( ); // 53
//	virtual void ReduceTimers( ); // 54
//	// CGameMovement::CheckFalling // 55
//	// CGameMovement::PlayerRoughLandingEffects // 56
//	virtual void Duck( ); // 57
//	// CGameMovement::HandleDuckingSpeedCrop // 58
//	virtual void FinishUnduck( ); // 59
//	virtual void FinishDuck( ); // 60
//	virtual bool CanUnduck( ); // 61
//	// CGameMovement::UpdateDuckJumpEyeOffset // 62
//	// CGameMovement::CanUnduckJump // 63
//	// CGameMovement::StartUnduckJump // 64
//	// CGameMovement::FinishUnduckJump // 65
//	// CGameMovement::SetDuckedEyeOffset // 66
//	// CGameMovement::FixPlayerCrouchStuck // 67
//	// CGameMovement::CategorizeGroundSurface // 68
//	// CGameMovement::InWater // 69
//	// CGameMovement::TestPlayerPosition // 70
//	// CGameMovement::SetGroundEntity // 71
//	// CGameMovement::StepMove // 72
//	// CGameMovement::LockTraceFilter // 73
//	// CGameMovement::UnlockTraceFilter // 74
//	// CGameMovement::GameHasLadders // 75
//	virtual void PreventBunnyJumping( ); // 76
//	virtual void DecayAimPunchAngle( ); // 77
//	void ApplyDuckRatio( float flDuckAmount );
//	bool IsPlayerDucking( );
//	bool CanMove( Player* ent );
//
//	Player* m_pCSPlayer; //3668
//};
//
//extern IGameMovement* g_pGameMovement;
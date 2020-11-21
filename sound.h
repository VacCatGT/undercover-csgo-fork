#pragma once

class IRecipientFilter;

class IEngineSound {
public:
	// indexes for virtuals and hooks.
	enum indices : size_t {
		PRECACHESOUND = 0,
		ISSOUNDPRECACHED = 1,
		EMITSOUND = 5,
		EMITAMBIENTSOUND = 12,
		GETACTIVESOUNDS = 19
	};
public:
	struct sound_info
	{
		// Sound Guid
		int			guid;
		void* m_filenameHandle;		// filesystem filename handle - call IFilesystem to conver this to a string
		int			sound_source;
		int			m_nChannel;
		// If a sound is being played through a speaker entity (e.g., on a monitor,), this is the
		//  entity upon which to show the lips moving, if the sound has sentence data
		int			m_nSpeakerEntity;
		float		m_flVolume;
		float		m_flLastSpatializedVolume;
		// Radius of this sound effect (spatialization is different within the radius)
		float		m_flRadius;
		int			m_nPitch;
		vec3_t		*origin;
		vec3_t		*m_pDirection;

		// if true, assume sound source can move and update according to entity
		bool m_bUpdatePositions;
		// true if playing linked sentence
		bool m_bIsSentence;
		// if true, bypass all dsp processing for this sound (ie: music)	
		bool m_bDryMix;
		// true if sound is playing through in-game speaker entity.
		bool m_bSpeaker;
		// for snd_show, networked sounds get colored differently than local sounds
		bool from_server;
	};
	
	enum {
		PITCH_NORM = 100,	// non-pitch shifted
		PITCH_LOW = 95,		// other values are possible - 0-255, where 255 is very high
		PITCH_HIGH = 120
	};

	__forceinline void EmitAmbientSound( const char* sample, float volume, int pitch = PITCH_NORM, int flags = 0, float time = 0.f ) {
		return util::get_method< void( __thiscall* )( void*, const char*, float, int, int, float ) >( this, EMITAMBIENTSOUND )( this, sample, volume, pitch, flags, time );
	}

	__forceinline bool PrecacheSound( const char* sample, bool preload = false, bool is_ui_sound = false ) {
		return util::get_method< bool( __thiscall* )( void*, const char*, bool, bool ) >( this, PRECACHESOUND )( this, sample, preload, is_ui_sound );
	}	
	
	__forceinline bool IsSoundPrecached( const char* sample ) {
		return util::get_method< bool( __thiscall* )( void*, const char* ) >( this, ISSOUNDPRECACHED )( this, sample );
	}

	__forceinline void GetActiveSounds( CUtlVector< sound_info > &sndlist ) {
		return util::get_method< void( __thiscall* )( void*, CUtlVector< sound_info >& ) >( this, GETACTIVESOUNDS )( this, sndlist );
	}
};
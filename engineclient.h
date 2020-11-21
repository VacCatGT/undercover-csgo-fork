#pragma once

#define NET_FRAMES_MASK 63

class bf_read {
public:
	const char* m_pDebugName;
	bool m_bOverflow;
	int m_nDataBits;
	unsigned int m_nDataBytes;
	unsigned int m_nInBufWord;
	int m_nBitsAvail;
	const unsigned int* m_pDataIn;
	const unsigned int* m_pBufferEnd;
	const unsigned int* m_pData;

	bf_read() = default;

	bf_read(const void* pData, int nBytes, int nBits = -1) {
		StartReading(pData, nBytes, 0, nBits);
	}

	void StartReading(const void* pData, int nBytes, int iStartBit, int nBits) {
		// Make sure it's dword aligned and padded.
		m_pData = (uint32_t*)pData;
		m_pDataIn = m_pData;
		m_nDataBytes = nBytes;

		if (nBits == -1) {
			m_nDataBits = nBytes << 3;
		}
		else {
			m_nDataBits = nBits;
		}
		m_bOverflow = false;
		m_pBufferEnd = reinterpret_cast<uint32_t const*>(reinterpret_cast<uint8_t const*>(m_pData) + nBytes);
		if (m_pData)
			Seek(iStartBit);
	}

	bool Seek(int nPosition) {
		bool bSucc = true;
		if (nPosition < 0 || nPosition > m_nDataBits) {
			m_bOverflow = true;
			bSucc = false;
			nPosition = m_nDataBits;
		}
		int nHead = m_nDataBytes & 3; // non-multiple-of-4 bytes at head of buffer. We put the "round off"
									  // at the head to make reading and detecting the end efficient.

		int nByteOfs = nPosition / 8;
		if ((m_nDataBytes < 4) || (nHead && (nByteOfs < nHead))) {
			// partial first dword
			uint8_t const* pPartial = (uint8_t const*)m_pData;
			if (m_pData) {
				m_nInBufWord = *(pPartial++);
				if (nHead > 1)
					m_nInBufWord |= (*pPartial++) << 8;
				if (nHead > 2)
					m_nInBufWord |= (*pPartial++) << 16;
			}
			m_pDataIn = (uint32_t const*)pPartial;
			m_nInBufWord >>= (nPosition & 31);
			m_nBitsAvail = (nHead << 3) - (nPosition & 31);
		}
		else {
			int nAdjPosition = nPosition - (nHead << 3);
			m_pDataIn = reinterpret_cast<uint32_t const*>(
				reinterpret_cast<uint8_t const*>(m_pData) + ((nAdjPosition / 32) << 2) + nHead);
			if (m_pData) {
				m_nBitsAvail = 32;
				GrabNextDWord();
			}
			else {
				m_nInBufWord = 0;
				m_nBitsAvail = 1;
			}
			m_nInBufWord >>= (nAdjPosition & 31);
			m_nBitsAvail = std::min(m_nBitsAvail, 32 - (nAdjPosition & 31)); // in case grabnextdword overflowed
		}
		return bSucc;
	}

	FORCEINLINE void GrabNextDWord(bool bOverFlowImmediately = false) {
		if (m_pDataIn == m_pBufferEnd) {
			m_nBitsAvail = 1; // so that next read will run out of words
			m_nInBufWord = 0;
			m_pDataIn++; // so seek count increments like old
			if (bOverFlowImmediately)
				m_bOverflow = true;
		}
		else if (m_pDataIn > m_pBufferEnd) {
			m_bOverflow = true;
			m_nInBufWord = 0;
		}
		else {
			m_nInBufWord = DWORD(*(m_pDataIn++));
		}
	}
};

class bf_write {
public:
	unsigned char* m_pData;
	int m_nDataBytes;
	int m_nDataBits;
	int m_iCurBit;
	bool m_bOverflow;
	bool m_bAssertOnOverflow;
	const char* m_pDebugName;

	void StartWriting(void* pData, int nBytes, int iStartBit = 0, int nBits = -1) {
		// Make sure it's dword aligned and padded.
		// The writing code will overrun the end of the buffer if it isn't dword aligned, so truncate to force alignment
		nBytes &= ~3;

		m_pData = (unsigned char*)pData;
		m_nDataBytes = nBytes;

		if (nBits == -1) {
			m_nDataBits = nBytes << 3;
		}
		else {
			m_nDataBits = nBits;
		}

		m_iCurBit = iStartBit;
		m_bOverflow = false;
	}

	bf_write() {
		m_pData = NULL;
		m_nDataBytes = 0;
		m_nDataBits = -1; // set to -1 so we generate overflow on any operation
		m_iCurBit = 0;
		m_bOverflow = false;
		m_bAssertOnOverflow = true;
		m_pDebugName = NULL;
	}

	// nMaxBits can be used as the number of bits in the buffer.
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	bf_write(void* pData, int nBytes, int nBits = -1) {
		m_bAssertOnOverflow = true;
		m_pDebugName = NULL;
		StartWriting(pData, nBytes, 0, nBits);
	}

	bf_write(const char* pDebugName, void* pData, int nBytes, int nBits = -1) {
		m_bAssertOnOverflow = true;
		m_pDebugName = pDebugName;
		StartWriting(pData, nBytes, 0, nBits);
	}
};

class CLC_Move {
private:
	char __PAD0[0x8]; // 0x0 two vtables
public:
	int backup_commands;
	int new_commands;
	std::string* data;
	bf_write* data_out;
	bf_read* data_in;
};                       // size: 0x50

template < typename T >
class CNetMessagePB : public INetMessage, public T {};
using CCLCMsg_Move_t = CNetMessagePB< CLC_Move >;

class INetMessage {
public:
	virtual	~INetMessage() = default;

	virtual void SetNetChannel(void* netchan) = 0;
	virtual void SetReliable(bool state) = 0;
	virtual bool Process() = 0;
	virtual	bool ReadFromBuffer(void* buffer) = 0;
	virtual	bool WriteToBuffer(void* buffer) = 0;
	virtual bool IsReliable() const = 0;
	virtual int	GetType() const = 0;
	virtual int	GetGroup() const = 0;
	virtual const char* GetName() const = 0;
	virtual void* GetNetChannel() const = 0;
	virtual const char* ToString() const = 0;
};

class INetChannel {
public:
	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	enum {
		FLOW_OUTGOING = 0,
		FLOW_INCOMING = 1,
		MAX_FLOWS     = 2
	};

	enum indices : size_t {
		GETLATENCY    = 9,
		GETAVGLATENCY = 10,
		PROCESSPACKET = 39,
		SENDNETMSG    = 40,
		SENDDATAGRAM  = 46,
	};

public:

	__forceinline const char* GetAddress( ) {
		return util::get_method< const char* ( __thiscall* )( decltype( this ) ) >( this, 1 )( this );
	}

	__forceinline float GetLatency( int flow ) {
		return util::get_method< float( __thiscall* )( decltype( this ), int ) >( this, GETLATENCY )( this, flow );
	}

	__forceinline float GetAvgLatency( int flow ) {
		return util::get_method< float( __thiscall* )( decltype( this ), int ) >( this, GETAVGLATENCY )( this, flow );
	}

	__forceinline bool SendDatagram(void* data) {
		using fn = bool(__thiscall*)(void*, void*);
		return (*(fn**)this)[46](this, data);
	}
	__forceinline bool SendNetMsg( INetMessage* msg, bool rel = false, bool voice = false ) {
		return util::get_method< bool( __thiscall* )( decltype( this ), void*, bool, bool ) >( this, SENDNETMSG )( this, msg, rel, voice );
	}

private:
	PAD( 0x14 );

public:
	bool m_processing_messages;		// 0x0014
	bool m_should_delete;			// 0x0015

private:
	PAD( 0x2 );

public:
	int m_out_seq;					// 0x0018 last send outgoing sequence number
	int m_in_seq;					// 0x001C last received incoming sequnec number
	int m_out_seq_ack;				// 0x0020 last received acknowledge outgoing sequnce number
	int m_out_rel_state;			// 0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int m_in_rel_state;				// 0x0028 state of incoming reliable data
	int m_choked_packets;			// 0x002C number of choked packets

private:
	PAD( 0x414 );					// 0x0030
};

struct player_info_t {
	uint64_t      m_data_map;
	union {
		int64_t   m_xuid;
		struct {
			int   m_xuid_low;
			int   m_xuid_high;
		};
	};
	char          m_name[ 128 ];
	int           m_user_id;
	char          m_guid[ 33 ];
	uint32_t      m_friends_id;
	char          m_friends_name[ 128 ];
	bool          m_fake_player;
	bool          m_is_hltv;
	uint32_t      m_custom_files[ 4 ];
	uint8_t       m_files_downloaded;
};

class CEventInfo {
public:
	enum {
		EVENT_INDEX_BITS    = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA      = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};

	// 0 implies not in use

	short				m_class_id;		// 0x00 // 0 implies not in use
	float				m_fire_delay;	// 0x02 // if non-zero, the delay time when the event should be fired ( fixed up on the client )
	const void*         m_send_table ;		// 0x06
	const ClientClass*  m_client_class;	// 0x0A // clienclass pointer
	void*				m_data;			// 0x0E // raw event data
	std::intptr_t		m_bits;	// 0x12
	int					m_flags;			// 0x16
	std::byte			pad0[ 0x16 ];		// 0x1A
	CEventInfo* m_next;
};

class CClientState {
public:
	enum indices : size_t {
		TEMPENTITIES = 36,
	};

	class CClockDriftManager {
	public:
		float clock_offsets[17];
		uint32_t cur_clock_offset;
		uint32_t m_server_tick;
		uint32_t m_client_tick;
	};

public:
	std::byte		pad0[ 0x9C ];				// 0x0000
	INetChannel*    m_net_channel;			// 0x009C
	int				iChallengeNr;			// 0x00A0
	std::byte		pad1[ 0x64 ];				// 0x00A4
	int				iSignonState;			// 0x0108
	std::byte		pad2[ 0x8 ];				// 0x010C
	float			m_flNextCmdTime;			// 0x0114
	int				nServerCount;			// 0x0118
	int				iCurrentSequence;		// 0x011C
	std::byte		pad3[ 0x54 ];				// 0x0120
	int				m_delta_tick;				// 0x0174
	bool			bPaused;				// 0x0178
	std::byte		pad4[ 0x7 ];				// 0x0179
	int				iViewEntity;			// 0x0180
	int				iPlayerSlot;			// 0x0184
	char			szLevelName[ MAX_PATH ];	// 0x0188
	char			szLevelNameShort[ 80 ];	// 0x028C
	char			szMapGroupName[ 80 ];		// 0x02DC
	char			szLastLevelNameShort[ 80 ]; // 0x032C
	std::byte		pad5[ 0xC ];				// 0x037C
	int				nMaxClients;			// 0x0388 
	std::byte		pad6[ 0x4984 ];			// 0x038C
	float			flLastServerTickTime;	// 0x4D10
	bool			bInSimulation;			// 0x4D14
	std::byte		pad7[ 0xB ];				// 0x4D15
	int				iOldTickcount;			// 0x4D20
	float			flTickRemainder;		// 0x4D24
	float			flFrameTime;			// 0x4D28
	int				m_last_outgoing_command;	// 0x4D2C
	int				m_choked_commands;		// 0x4D30
	int				m_last_command_ack;		// 0x4D34
	int				iCommandAck;			// 0x4D38
	int				iSoundSequence;			// 0x4D3C
	std::byte		pad8[ 0x50 ];				// 0x4D40
	vec3_t			angViewPoint;			// 0x4D90
	std::byte		pad9[ 0xD0 ];				// 0x4D9C
	CEventInfo* m_events;				// 0x4E6C // should be CUtlFixedLinkedList<CEventInfo> instead with 0x2C size
};

class CGlobalState {
public:
	uintptr_t*    m_manager;
	CClientState* m_client_state;
};

class ISpatialQuery;

class IVEngineClient {
public:
	enum indices : size_t {
		GETSCREENSIZE            = 5,
		GETPLAYERINFO            = 8,
		GETPLAYERFORUSERID       = 9,
		GETLOCALPLAYER           = 12,
		GETLASTTIMESTAMP         = 14,
		GETVIEWANGLES            = 18,
		SETVIEWANGLES            = 19,
		//GETMAXCLIENTS          = 20,
		ISINGAME                 = 26,
		ISCONNECTED              = 27,
		ISBOXVISIBLE             = 32,
		WORLDTOSCREENMATRIX      = 37,
		FIREEVENTS               = 59,
		GETNETCHANNELINFO        = 78,
		ISPAUSED                 = 90,
		//ISTAKINGSCREENSHOT     = 92,
		ISHLTV                   = 93,
		//SETOCCLUSIONPARAMETERS = 96,
		EXECUTECLIENTCMD         = 114,
		ISVOICERECORDING         = 224,
	};

public:
	__forceinline void GetScreenSize( int& width, int& height ) {
		return util::get_method< void( __thiscall* )( decltype( this ), int&, int& ) >( this, GETSCREENSIZE )( this, width, height );
	}

	__forceinline bool GetPlayerInfo( int index, player_info_t* info ) {
		return util::get_method< bool( __thiscall* )( decltype( this ), int, player_info_t* ) >( this, GETPLAYERINFO )( this, index, info );
	}

	__forceinline int GetPlayerForUserID( int uid ) {
		return util::get_method< int( __thiscall* )( decltype( this ), int ) >( this, GETPLAYERFORUSERID )( this, uid );
	}

	__forceinline int GetLocalPlayer( ) {
		return util::get_method< int( __thiscall* )( decltype( this ) ) >( this, GETLOCALPLAYER )( this );
	}

	__forceinline float GetLastTimestamp( ) {
		return util::get_method< float( __thiscall* )( decltype( this ) ) >( this, GETLASTTIMESTAMP )( this );
	}

	__forceinline void GetViewAngles( ang_t& angle ) {
		return util::get_method< void( __thiscall* )( decltype( this ), ang_t& ) >( this, GETVIEWANGLES )( this, angle );
	}

	__forceinline void SetViewAngles( ang_t& angle ) {
		return util::get_method< void( __thiscall* )( decltype( this ), ang_t& ) >( this, SETVIEWANGLES )( this, angle );
	}

	__forceinline bool IsInGame( ) {
		return util::get_method< bool( __thiscall* )( decltype( this ) ) >( this, ISINGAME )( this );
	}	
	
	__forceinline bool IsVoiceRecording( ) {
		return util::get_method< bool( __thiscall* )( decltype( this ) ) >( this, ISVOICERECORDING )( this );
	}

	__forceinline const VMatrix& WorldToScreenMatrix( ) {
		return util::get_method< const VMatrix&( __thiscall* )( decltype( this ) ) >( this, WORLDTOSCREENMATRIX )( this );
	}

	__forceinline void FireEvents( ) {
		return util::get_method< void( __thiscall* )( decltype( this ) ) >( this, FIREEVENTS )( this );
	}

	__forceinline INetChannel* GetNetChannelInfo( ) {
		return util::get_method< INetChannel*( __thiscall* )( decltype( this ) ) >( this, GETNETCHANNELINFO )( this );
	}

	__forceinline bool IsPaused( ) {
		return util::get_method< bool( __thiscall* )( decltype( this ) ) >( this, ISPAUSED )( this );
	}

	__forceinline void ExecuteClientCmd( const char* cmd ) {
		return util::get_method< void( __thiscall* )( decltype( this ), const char* )>( this, EXECUTECLIENTCMD )( this, cmd );
	}


	_forceinline ISpatialQuery* GetBSPTreeQuery( ) { 
		return util::get_method< ISpatialQuery*( __thiscall* )( decltype( this ) ) >( this, 43 )( this );
	}

	//__forceinline void SetOcclusionParameters( float min, float max ) {
	//	float arr[] = { min, max };
	//	return util::get_method< void( __thiscall* )( decltype( this ), float* ) >( this, SETOCCLUSIONPARAMETERS )( this, arr );
	//}
};
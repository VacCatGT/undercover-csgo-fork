#pragma once

class TickbaseSystem {
public:
	// Stores information about current tickbase shift
	struct ShiftData {
		int m_next_shift_amount;
		int m_ticks_to_shift;
		int m_current_shift;
		int m_stored_tickbase;
		int m_ticks_shifted_last;
		bool m_did_shift_before;
		bool m_should_attempt_shift;
		int m_needs_recharge;
		bool m_prepare_recharge;
		bool m_should_be_ready;
		int m_switched_weapon_passive;
		bool m_can_shift_tickbase;
		bool m_should_disable;
		int m_shift_time;
		bool m_round_start;
		bool m_in_recharge;
		bool m_wait_for_charge;
	} m_shift_data;

	// Used to fix prediction when shifting tickbase
	struct Prediction {
		int m_shifted_command;
		int m_shifted_ticks;
		int m_original_tickbase;
	} m_prediction;

	struct CLMove {
		bool m_ready_to_process;
		int m_extra_processing_ticks;
		int m_backup_out_seq;
	} m_client_move;

	// Prototypes - main functions necessary for tickbase shifting
	void WriteUserCmd( bf_write* buf, CUserCmd* incmd, CUserCmd* outcmd );
	void Teleport( float AccumulatedExtraSamples, bool FinalTick );

	void PreMovement( );
	void PostMovement( );
	void PassiveRecharge( );
};

extern TickbaseSystem g_tickbase;
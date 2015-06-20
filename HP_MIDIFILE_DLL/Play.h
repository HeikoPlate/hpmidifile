// Play.h: Schnittstelle für die Klasse Play.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAY_H__8E96987B_1C8F_4D06_A56F_A41D38E17818__INCLUDED_)
#define AFX_PLAY_H__8E96987B_1C8F_4D06_A56F_A41D38E17818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#include <basetsd.h>
#include <streams.h>
#include <Dshow.h>
#include "Play.h"
#include "midifile.h"
#include "HP_Array.h"

#define NO_TAKT   -2
#define NO_BEAT   -2
#define END_TAKT  -1
#define END_BEAT  -1

class PlayClass  
{
public:
	PlayClass(MIDIFile* midifile);
	PlayClass::PlayClass();
	virtual ~PlayClass();

	UINT play(int sel, bool snd_before);
	UINT set_play_stop_wait(int modus);
	UINT set_play_time(int new_time);
	long get_play_time();
	UINT play_tempo(int percent);
	UINT play_transpose(int steps, const bool transp_list[16]);
	UINT play_single_event(unsigned char* event_data, int length);
	UINT play_mute(const bool mute_list[16]);
	long get_tick_number();

	HP_PLAY_MONITOR * pm_instance;

	bool is_playing;

private:

	UINT add_before_selected(int *first_tme_selected);

	HANDLE hThread;
	DWORD threadID;

	static CBaseReferenceClock *irc;

	static const int delta_ticks_sysex; // ticks after an sysex-event
	static const int delta_ticks_else;  // ticks after an controller and program change

	DWORD	m_dwAdvise;
	CAMEvent m_EventAdvise;
	int delay_ms;
	REFERENCE_TIME corr_time;
	DWORD dwExitCode;

	UINT create_basic_playlist();

	// functions running in thread:
	static void thread_player(PlayClass *pl);
//	bool thread_prepare_playlist();
	bool thread_create_reduced_playlist(int new_time, int call_time);
	DWORD thread_wait();
	void thread_send_next_event();
	bool thread_midi_single_event(unsigned char* event_data, int length);
	bool thread_midi_out_list_event(Play_Array *list, int list_index);
	void thread_stop_play();

	MIDIFile * mf; // actual instance of midifile

	// the playlists
	// basic_playlist is the pointer to a list of all playevents
	// reduced_playlist is computed from basic-playlist, if selected or
	// new playtime is wanted
	// playlist gets the pointer of basic_playlist or reduced_playlist

	Play_Array *basic_playlist; // pointer to a list of all play-events
	Play_Array *reduced_playlist; // pointer to 
	Play_Array *playlist;

	long tick_number;

	int play_stop_wait_marker; // HP_GO_PLAY, HP_WAIT_PLAY or HP_STOP_PLAY
	int new_play_start_time;
	int last_new_play_start_time;

	static HMIDIOUT midi_handle;
	MIDIHDR	midiHdr;
	bool midiHdr_unprepared;
	long play_time;

	static int playlist_index;
	static int remainded_sleep_time;
	Play_Array *sound_off_list;
	int sound_off_list_index;
	static const int max_sleep_time;
	int play_tempo_factor;
	int transpose_steps;
	bool chan_transpose_list[16];
	unsigned char single_event_data[512];
	int single_event_length;
	bool chan_mute_list[16];

	int selected;
	bool send_before;

};

#endif // !defined(AFX_PLAY_H__8E96987B_1C8F_4D06_A56F_A41D38E17818__INCLUDED_)

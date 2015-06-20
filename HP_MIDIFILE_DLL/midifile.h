/************************************************************************/
/************************************************************************/
/*                                                                      */
/* midifile.h: Headerfile for class MIDIFile                            */
/*                                                                      */
/************************************************************************/
/************************************************************************/

#if !defined(AFX_MIDIFILE_H__ECEFB402_C6DA_11D5_9348_0080AD7896CF__INCLUDED_)
#define AFX_MIDIFILE_H__ECEFB402_C6DA_11D5_9348_0080AD7896CF__INCLUDED_

#include "HP_Array.h"
#include "HP_midifile.h"
#include "Play.h"
#include "InsFile.h"
#include <stdio.h>

// #define HP_MESSAGE			// activate to get messages from functions

// #define HP_SINGLE_CTRL_99	// activate if single CC#99 are allowed

// Supplement event types, used only in DLL

	#define HP_TYPE_NONE				0xfefefe

	#define HP_NOTE_OFF					0x000100
	#define HP_NOTE_ON					HP_NOTE

	#define HP_DATA_ENTRY_MSB			0x220400
	#define HP_DATA_ENTRY_LSB			0x230400
	#define HP_NRPN_LSB					0x240400
	#define HP_NRPN_MSB					HP_NRPN
	#define HP_RPN_LSB					0x250400
	#define HP_RPN_MSB					HP_RPN
	#define HP_DATA_INCREMENT			0x260400
	#define HP_DATA_DECREMENT			0x270400

	// MIDI-Events

	#define HP_MIDI_EVENT				0x00	// type
	#define HP_META_EVENT				0x01	// sub1type
	#define HP_CONTROL_CHANGE			0x04	// sub1type
	#define HP_XF_META					0xff	// sub1type


// Switches for Functions

	#define HP_IDCREMENT		true
	#define HP_NO_IDCREMENT		false


class MIDIFile
{

	// public methods
public:

	static void message(const char* fmt,...);

	MIDIFile();
	virtual ~MIDIFile();
	// load/save
	UINT load(const char *name);
	UINT save(const char *name, const int format);
	UINT is_more_chunks();

	// set switches
	void set_first(bool first = HP_FIRST);

	// insert events
	UINT insert_note(int time, int length, int chan, int note, int velocity);
	UINT insert_aftertouch(int time, int chan, int note, int pressure);
	UINT insert_controller(int time, int chan, int type, int value, int controller_number = HP_NO_CONTROLLER_NUMBER);
	UINT insert_rpn_nrpn(int time, int chan, int type, int number, int value, bool absolute);
	UINT insert_programchange(int time, int chan, int program_number);
	UINT insert_channelpressure(int time, int chan, int pressure);
	UINT insert_pitchwheel(int time, int chan, int value);
	UINT insert_sysex(int time, unsigned char* data);

	// metaevents
	UINT insert_sequencenumber(int number);

	// text, copyright, track-name, instrument, lyric, marker, cue point:
	UINT insert_texttype(int time, int event_type, char* text, int chan = HP_ALL_CHAN);
	UINT insert_midiport(int time, int track, int port);
	UINT insert_tempo(int time, int bpm);
	UINT insert_tempo_double(int time, double bpm);
	UINT insert_smpte_offset(int hr, int mn, int se, int fr, int ff);
	UINT insert_timesignature(int time, int num, int denum, int metronome_click, int n32);
	UINT insert_keysignature(int time, char *key);

	UINT insert_chord(int time, unsigned char cr, unsigned char ct, unsigned char bn, unsigned char bt);
	UINT insert_ymeta_event(int time, UINT ytype, unsigned char *event_data, int length);


	UINT insert_rawevent(int time, unsigned char *data, int length, int chan = HP_ALL_CHAN);

    // change events
	UINT change_ppqn(int newppqn);
	void change_channel(int chan);

	UINT change_note(int id, int chan, int note, bool absolute);
	UINT change_velocity(int id, int chan, int note, int value,bool percent);
	UINT change_velocity_selected(int value, bool percent);
	UINT change_note_length(int id, int chan, int note, int value, bool percent);
	UINT transpose_selected(int steps);
	UINT quantisize_selected(int resolution, bool start, bool length);
	UINT shift_notes_selected(int ticks);

	UINT change_aftertouch(int id, int note, int value, bool percent);
	UINT change_controller(int id, UINT type, int value, bool percent);
	UINT change_controller_selected(UINT type,int value, bool percent);
	UINT change_rpn_nrpn(UINT type, bool absolute, int id, int value, bool percent);
	UINT change_program(int id ,int number);
	UINT change_channel_pressure(int id, int pressure);
	UINT change_pitch_wheel(int id, int value);
	UINT change_sysex(int id, unsigned char* data);
	UINT change_sequence_number(int id, int new_number);
	UINT change_texttype(int id, UINT type, char* new_text);
	UINT change_midiport(int id, int new_track, int new_port);
	UINT change_tempo(int id , int value, bool percent);
	UINT change_tempo_double(int id , double value, bool percent);
	UINT change_smpte_offset(int id , int hr, int mn, int se, int fr, int ff);
	UINT change_time_signature(int id, int new_num, int new_denum, int new_m_click, int new_n32);
	UINT change_key_signature(int id , char* key);
	UINT change_chord(int id, unsigned char cr, unsigned char ct, unsigned char bn, unsigned char bt);

	UINT shift_event(int id, int ticks);
	UINT shift_selected(int ticks);

	// utilities
	UINT get_takt_beat_tick(int resulting_time, int *takt, int *beat, int *tick);
	UINT MIDIFile::get_takt_beat_tick(int resulting_time,
									  int time_of_taktchange,
									  int takt_of_taktchange,
									  int num,
									  int denum,
									  int *takt, int *beat, int *tick);
	UINT get_time_from_takt_beat_tick(int takt, int beat, int tick, int *time);
	UINT get_last_time(int *time);

	// read
	UINT read_ppqn(int *act_ppqn);
	UINT read_note(int id , int *time, int *chan, int *note, int *velocity, int *length);
	UINT read_aftertouch(int id, int *time, int *chan, int *note, int *pressure);
	UINT read_controller(int id, int *time, int *chan, int *number, int *value);
	UINT read_rpn_nrpn(UINT type, int id, int *time, int *chan, bool *absolute, int *number, int *value);
	UINT read_program_change(int id, int *time, int *chan, int *number);
	UINT read_channel_pressure(int id, int *time, int *chan, int *pressure);
	UINT read_pitch_wheel(int id, int *time, int *chan, int *value);
	UINT read_sysex(int id, int *time, unsigned char** data, int *length);
	UINT read_sequence_number(int id, int *time, int *number);
	UINT read_text_from_meta(int id, int *time, UINT type, int *chan, char** text);
	UINT read_midiport(int id, int *time, int *chan, int *port);
	UINT read_event_data(int id, int *time, unsigned char **data, int *data_length);
	UINT read_tempo(int id , int *time, int *bpm, int *cps);
	UINT read_tempo_double(int id , int *time, double *bpm, int *cps);
	UINT read_smpte_offset(int id, int *hr, int *mn, int *se, int *fr, int *ff);
	UINT read_time_signature(int id, int *time, int *num, int *denum, int *metronome_click, int *n32);
	UINT read_key_signature(int id, int *time, char **key);
	UINT read_chord(int id, int *time, unsigned char* cr, unsigned char* ct, unsigned char* bn, unsigned char* bt,char** chord);
	UINT read_ymeta_event(int id, int *time, UINT ytype, unsigned char **event_data, int *length);

	// stream functions
	void rewind();
	UINT seek(int id);
	UINT read_event(int *id, int *chan, int *time, int *etype);
	UINT delete_short_notes_selected(int limit_length);
	UINT settle_overlapping_notes(bool method);

	UINT delete_event(int index);

	// buffer functions
	UINT select(int index);
	void select_by_chan_time(int chan, int time1=0, int time2=HP_NO_TIME_LIMIT);
	UINT deselect(int id);
	void deselect_channel_events(int chan);
	void copy_selected_to_buf(Mev_Array *dest_mevs);
	void copy_selected_from_buf(Mev_Array *copybuf, int *copy_ppqn,int time, bool delete_in_dest, int del_events_chan);

	// Number of MIDI-Device
	static int MIDI_device; // initial value: Default

	// play functions
	UINT play(int selected, bool send_before);
	long duration(int modus);
	long get_play_time();
	long get_play_event_time();
	bool is_playing();
	UINT set_play_stop_wait(int modus);
	UINT set_play_time(int new_time);
	UINT play_tempo(int percent);
	UINT play_transpose(int steps, const bool transp_list[16]);
	UINT play_single_event(unsigned char* event_data, int length);
	UINT play_mute(const bool mute_list[16]);

	UINT init_play_monitor(HP_PLAY_MONITOR * pmonitor_instance);

	HP_PLAY_MONITOR * pm_instance;

	void *play_object; // public list for class PlayClass

	Mev_Array *eventlist;
	Tempo_Array *tempolist;
	void reorganize_tempolist(Mev_Array *mevlist, int ppqn);

	// internal public
	UINT get_no_events();
	MIDIEvent* get_event(int index);
	static int eval_var_len(int len, unsigned char *var_len_bytes);
	static int convert_var_len_to_int(unsigned char *var_len_bytes);
	static UINT eval_sysex_data(unsigned char **sysex, int *sysex_length, const unsigned char * data);
	static UINT convert_key_to_intern(const char *key, unsigned char * sf, unsigned char * mi);

	InsFile *insfile;

private:

	bool read_some_bytes(unsigned char data[], int length);
	void put_byte(unsigned char byte);
	void put_bytes(unsigned char *bytes, int length);
	unsigned char get_byte(bool back = false);
	void get_bytes(unsigned char *data, int length);
	int insert_var_length(int delta_time);
	void change_ppqn_basic(Mev_Array *mevlist, int newppqn, int *ppqn);
	bool is_xg_event(unsigned char* eventdata, int length);
	void compute_rpn_nrpn(ControlChangeEvent *cev, int new_time);

	Mev_Array* prep_for_save(Mev_Array *track);

	bool scan_sysex();
	void scan_note_off();
	void scan_note_on();
	void scan_aftertouch();
	void scan_control_change();
	void scan_patch_change();
	void scan_channel_pressure();
	void scan_pitch_wheel();
	void scan_0xfn();

	FILE *source_mf;
	FILE *dest_mf;

	UINT data_behind_tracks;
	int no_tracks;
	int fpos;				// Offset in source-file
	int track_pos;			// Offset in track
	int smf;
	int ppqn;
	int cps;
	int no_of_sequencenumbers;

	bool insert_first;		// switch for insert events in a
							// sequence of events with same time

	unsigned char *mtrk_data;

	unsigned char *event_data;	// temporary event buffer (load, scan...)
	int event_data_max;			// length of whole event_data
	int event_data_length;		// number of relevant bytes in event_data

	Mev_Array *open_note;

	// global data for compute_rpn_nrpn only
	struct {ControlChangeEvent *msb;ControlChangeEvent *lsb;} rpn_opc[16], nrpn_opc[16];
	ControlChangeEvent *last_rpn_missed_data_lsb[16];
	ControlChangeEvent *last_nrpn_missed_data_lsb[16];
	unsigned char old_rpn_msb[16];
	unsigned char old_nrpn_msb[16];

	Uch_Array dest_bytelist;

	// stream
	int pos;				// position (index) in eventlist
};

#endif // !defined(AFX_MIDIFILE_H__ECEFB402_C6DA_11D5_9348_0080AD7896CF__INCLUDED_)

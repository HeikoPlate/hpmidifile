/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  midifile_insert.cpp: Insert Events                                             */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/

#include "stdAfx.h"

#include "midifile.h"
#include "HP_midifile.h"

#include "MIDIEvent.h"
#include "MetaEvent.h"
#include "SysExEvent.h"

//**************************************************************************
//*
//*		insert note
//*
//**************************************************************************

UINT MIDIFile::insert_note(int time, int length, int chan, int note, int velocity)
{
	unsigned char event_data[3];

	if (ppqn < 0) return HP_ERR_NO_PPQN;

	NoteOnEvent * noteon = new NoteOnEvent(time, event_data, 3);
	noteon->event_data[0] = 0x90 | (chan & 0x0f);
	noteon->event_data[1] = note & 0x7f;
	noteon->event_data[2] = velocity & 0x7f;
	noteon->chan = chan;

	int index = eventlist->insert(noteon, insert_first);
	if (index <= pos) pos++;

	NoteOffEvent * noteoff = new NoteOffEvent(time+length,event_data,3);
	noteoff->event_data[0] = 0x80 | (chan & 0x0f);
	noteoff->event_data[1] = note & 0x7f;
	noteoff->event_data[2] = velocity & 0x7f;
	noteoff->chan = chan;

	index = eventlist->insert(noteoff, insert_first);
	if (index <= pos) pos++;

	noteon->assoc_noteoff = noteoff;
	noteoff->assoc_noteon = noteon;


	eventlist->check();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert aftertouch
//*
//**************************************************************************

UINT MIDIFile::insert_aftertouch(int time, int chan, int note, int pressure)
{
	unsigned char event_data[3];
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	AftertouchEvent * a_event = new AftertouchEvent(time, event_data, 3);
	a_event->event_data[0] = 0xa0 | (chan & 0x0f);
	a_event->event_data[1] = note & 0x7f;
	a_event->event_data[2] = pressure & 0x7f;
	a_event->chan = chan;

	int index = eventlist->insert(a_event, insert_first);
	if (index <= pos) pos++;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert controller
//*
//**************************************************************************

UINT MIDIFile::insert_controller(int time, int chan, int type, int value, int controller_number)
{
	unsigned char event_data[3];
	int i;

	if (ppqn < 0) return HP_ERR_NO_PPQN;

	const struct{int type; unsigned char cnumber;} controller_list[31] =
	{
		{HP_BANK_SELECT_MSB			,  0},
		{HP_MODULATION				,  1},
		{HP_BREATH					,  2},
		{HP_PORTAMENTO_TIME			,  5},
		{HP_MAIN_VOLUME				,  7},
		{HP_PANPOT					, 10},
		{HP_EXPRESSION				, 11},
		{HP_BANK_SELECT_LSB			, 32},
		{HP_SUSTAIN					, 64},
		{HP_PORTAMENTO				, 65},
		{HP_SOSTENUTO				, 66},
		{HP_SOFT_PEDAL				, 67},
		{HP_HARMONIC_CONTENT		, 71},
		{HP_RELEASE_TIME			, 72},
		{HP_ATTACK_TIME				, 73},
		{HP_BRIGHTNESS				, 74},
		{HP_DECAY_TIME				, 75},
		{HP_VIBRATO_RATE			, 76},
		{HP_VIBRATO_DEPTH			, 77},
		{HP_VIBRATO_DELAY			, 78},
		{HP_PORTAMENTO_CONTROL		, 84},
		{HP_REVERB_SEND_LEVEL		, 91},
		{HP_CHORUS_SEND_LEVEL		, 93},
		{HP_VARIATION_SEND_LEVEL	, 94},
		{HP_ALL_SOUND_OFF			, 120},
		{HP_RESET_ALL_CONTROLLERS	, 121},
		{HP_ALL_NOTES_OFF			, 123},
		{HP_OMNI_OFF				, 124},
		{HP_OMNI_ON					, 125},
		{HP_MONO					, 126},
		{HP_POLY					, 127}
	};

	bool found = false;

	unsigned char cnumber = controller_number;
	int ctype = type;

	if ((controller_number) == HP_NO_CONTROLLER_NUMBER)
	{ // insert_controller with type, find number
		for (i=0; i<31; i++)
		{
			if (controller_list[i].type==ctype)
			{
				cnumber = controller_list[i].cnumber;
				found = true;
				break;
			}
		}
		if (!found)	return HP_ERR_BAD_CONTROLLER;
	}
	else
	if ((controller_number>=0)&&(controller_number<=127))
	{ // insert_controller with number, find type
		for (i=0; i<31; i++)
		{
			if (cnumber==controller_list[i].cnumber)
			{
				ctype = controller_list[i].type;
				found = true;
				break;
			}
		}
		if (!found) ctype = HP_CONTROLLER_TYPE;
	}
	else 
	{
		return HP_ERR_BAD_CONTROLLER;
	}

	ControlChangeEvent * c_event = new ControlChangeEvent(time, event_data, 3);
	c_event->event_data[0] = 0xb0 | (chan & 0x0f);
	c_event->event_data[1] = cnumber & 0x7f;
	c_event->event_data[2] = value & 0x7f;
	c_event->type = ctype;
	c_event->chan = chan;

	int index = eventlist->insert(c_event, insert_first);
	if (index <= pos) pos++;
	eventlist->check();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert rpn and nrpn
//*
//**************************************************************************

UINT MIDIFile::insert_rpn_nrpn(int time, int chan, int type, int number, int value, bool absolute)
{
	unsigned char event_data[3];
	if ((type != HP_RPN) && (type != HP_NRPN)) return HP_ERR_BAD_CONTROLLER;
	
	event_data[0] = 0xb0 + (chan&0x0f);		// Control Change + channel
	
	// RPN/NRPN MSB
	event_data[1] = (type==HP_RPN)? 101: 99;
	event_data[2] = (number>>7) & 0x7f;
	ControlChangeEvent* msb = new ControlChangeEvent(time,event_data,3);
	msb->chan = chan;

	// RPN/NRPN LSB
	event_data[1] = (type==HP_RPN)? 100: 98;
	event_data[2] = number & 0x7f;
	ControlChangeEvent* lsb = new ControlChangeEvent(time,event_data,3);
	lsb->chan = chan;
	msb->assoc_fine = lsb;

	if (absolute)
	{
		// RPN/NRPN DATA MSB
		event_data[1] = 6;
		event_data[2] = (value >>7) & 0x7f;
		ControlChangeEvent* data_msb = new ControlChangeEvent(time,event_data,3);
		data_msb->chan = chan;
		msb->assoc_data_coarse = data_msb;

		// RPN/NRPN DATA LSB
		event_data[1] = 38;
		event_data[2] = value & 0x7f;
		ControlChangeEvent* data_lsb = new ControlChangeEvent(time,event_data,3);
		data_lsb->chan = chan;
		msb->assoc_data_fine = data_lsb;
	}
	else
	{ // data entry in- or decrement
		if (value > 0)
		{
			event_data[1] = 96; // data entry increment
			event_data[2] = value & 0x7f;
		}
		else
		{
			event_data[1] = 97; // data entry decrement
			event_data[2] = (-value) & 0x7f;
		}
		ControlChangeEvent* data_idc = new ControlChangeEvent(time,event_data,3);
		data_idc->chan = chan;
		msb->assoc_data_idcrement = data_idc;
	}
	// insert events

	int index =	eventlist->insert(msb,false);
	if (index <= pos) pos++;

	if (msb->assoc_fine != NULL)
	{
		index = eventlist->insert(msb->assoc_fine, false);
		if (index <= pos) pos++;
	}
	if (msb->assoc_data_coarse != NULL)		
	{
		index = eventlist->insert(msb->assoc_data_coarse, false);
		if (index <= pos) pos++;
	}
	if (msb->assoc_data_fine != NULL)		
	{
		index = eventlist->insert(msb->assoc_data_fine, false);
		if (index <= pos) pos++;
	}

	if (msb->assoc_data_idcrement != NULL)	
	{
		index = eventlist->insert(msb->assoc_data_idcrement, false);
		if (index <= pos) pos++;
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert program change
//*
//**************************************************************************


UINT MIDIFile::insert_programchange(int time, int chan, int program_number)
{
	unsigned char event_data[2];
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	ProgramChangeEvent * p_event = new ProgramChangeEvent(time, event_data, 2);
	p_event->event_data[0] = 0xc0 | (chan & 0x0f);
	p_event->event_data[1] = program_number & 0x7f;
	p_event->chan = chan;

	int index = eventlist->insert(p_event, insert_first);
	if (index <= pos) pos++;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert channel pressure
//*
//**************************************************************************

UINT MIDIFile::insert_channelpressure(int time, int chan, int pressure)
{
	unsigned char event_data[2];
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	ChannelPressureEvent * cp_event = new ChannelPressureEvent(time, event_data, 2);
	cp_event->event_data[0] = 0xd0 | (chan & 0x0f);
	cp_event->event_data[1] = pressure & 0x7f;
	cp_event->chan = chan;

	int index = eventlist->insert(cp_event, insert_first);
	if (index <= pos) pos++;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert pitch wheel
//*
//**************************************************************************

UINT MIDIFile::insert_pitchwheel(int time, int chan, int value)
{
	unsigned char event_data[3];
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	PitchWheelEvent * pw_event = new PitchWheelEvent(time, event_data, 3);
	pw_event->event_data[0] = 0xe0 | (chan & 0x0f);
	pw_event->event_data[1] = value & 0x7f;
	pw_event->event_data[2] = (value>>7) & 0x7f;
	pw_event->chan = chan;

	int index = eventlist->insert(pw_event, insert_first);
	if (index <= pos) pos++;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert sysex
//*
//**************************************************************************

UINT MIDIFile::insert_sysex(int time, unsigned char* data)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	unsigned char* sysex = NULL;
	int sysex_length;
	int result = eval_sysex_data(&sysex, &sysex_length,data);

	if (result != HP_ERR_NONE) return result;

	SysExEvent *s_event = new SysExEvent(time, sysex,sysex_length);
	delete[] sysex;

	int index = eventlist->insert(s_event, insert_first);
	if (index <= pos) pos++;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert sequence number
//*
//**************************************************************************

UINT MIDIFile::insert_sequencenumber(int number)
{
	if (no_of_sequencenumbers>0) return HP_ERR_SEQUENCENUMBER;
	if (ppqn < 0) return HP_ERR_NO_PPQN;
	unsigned char event_data[5];
	event_data[0] = 0xff;
	event_data[1] = 0x00;
	event_data[2] = 0x02;
	event_data[3] = (number>>8) & 0xff;
	event_data[4] = number & 0xff; // 8-bit data bytes allowed?

	SequenceNumberEvent * sq_event = new SequenceNumberEvent(0, event_data, 5);
	int index = eventlist->insert(sq_event, true); // insert as first event in mtrk
	if (index <= pos) pos++;

	no_of_sequencenumbers++;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert texttype
//*		used with Text, Copyright, Sequence/Track, Lyric, Marker, Cuepoint
//*
//**************************************************************************

UINT MIDIFile::insert_texttype(int time, int event_type, char* text, int chan)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;
	int i;
	int len = strlen(text);

	unsigned char var_len_bytes[16];
	int var_len_length = eval_var_len(len, var_len_bytes);
	unsigned char *event_data = new unsigned char[2+var_len_length+len];
	MIDIEvent *t_event;

	switch (event_type)
	{
	case HP_TEXT:
		t_event = new TextEvent(time, event_data, 2+var_len_length+len);
		t_event->event_data[1] = 0x01;
		break;
	case HP_COPYRIGHT:
		t_event = new CopyrightEvent(time, event_data, 2+var_len_length+len);
		t_event->event_data[1] = 0x02;
		break;
	case HP_SONG_TRACK_NAME:
		t_event = new SongSequTrackNameEvent(time, event_data, 2+var_len_length+len);
		t_event->event_data[1] = 0x03;
		((MetaEvent*)t_event)->track = chan;
		break;
	case HP_INSTRUMENT:
		t_event = new InstrumentNameEvent(time, event_data, 2+var_len_length+len);
		t_event->event_data[1] = 0x04;
		((MetaEvent*)t_event)->track = chan;
		break;
	case HP_LYRIC:
		t_event = new LyricEvent(time, event_data, 2+var_len_length+len);
		t_event->event_data[1] = 0x05;
		break;
	case HP_MARKER:
		t_event = new MarkerEvent(time, event_data, 2+var_len_length+len);
		t_event->event_data[1] = 0x06;
		break;
	case HP_CUE_POINT:
		t_event = new CuePointEvent(time, event_data, 2+var_len_length+len);
		t_event->event_data[1] = 0x07;
		break;
	default:
		return HP_ERR_EVENTTYPE;
	}
	t_event->event_data[0] = 0xff;
	for (i=0; i<var_len_length; i++)
	{
		t_event->event_data[2+i] = var_len_bytes[i];
	}
	for (i=0; i<len; i++)
	{
		t_event->event_data[2+var_len_length+i] = text[i];
	}

	int index = eventlist->insert(t_event, insert_first);
	if (index <= pos) pos++;

	delete[] event_data;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert midi port
//*
//**************************************************************************

UINT MIDIFile::insert_midiport(int time, int track, int port)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;
	unsigned char event_data[4];
	event_data[0] = 0xff;
	event_data[1] = 0x21;
	event_data[2] = 0x01;
	event_data[3] = port & 0x7f;


	MidiPortEvent * mp_event = new MidiPortEvent(time, event_data, 4);
	((MetaEvent*)mp_event)->track = track;

	int index = eventlist->insert(mp_event, insert_first);
	if (index <= pos) pos++;
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert tempo
//*
//**************************************************************************

UINT MIDIFile::insert_tempo(int time, int bpm)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	long tempo = 60000000/bpm;
	unsigned char event_data[6];
	event_data[0] = 0xff;
	event_data[1] = 0x51;
	event_data[2] = 0x03;
	event_data[5] = tempo & 0xff; // 8-bit data bytes allowed
	tempo >>= 8;
	event_data[4] = tempo & 0xff;
	tempo >>= 8;
	event_data[3] = tempo & 0xff;

	TempoEvent * tp_event = new TempoEvent(time, event_data, 6);
	int msec_tick = (tp_event->msq)/ppqn;
	tempolist->insert(time,msec_tick);

	int index = eventlist->insert(tp_event, insert_first);
	if (index <= pos) pos++;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert tempo
//*
//**************************************************************************

UINT MIDIFile::insert_tempo_double(int time, double bpm)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	long tempo = (long)(60000000.0/bpm);
	unsigned char event_data[6];
	event_data[0] = 0xff;
	event_data[1] = 0x51;
	event_data[2] = 0x03;
	event_data[5] = tempo & 0xff; // 8-bit data bytes allowed
	tempo >>= 8;
	event_data[4] = tempo & 0xff;
	tempo >>= 8;
	event_data[3] = tempo & 0xff;

	TempoEvent * tp_event = new TempoEvent(time, event_data, 6);
	int msec_tick = (tp_event->msq)/ppqn;
	tempolist->insert(time,msec_tick);

	int index = eventlist->insert(tp_event, insert_first);
	if (index <= pos) pos++;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert SMPTE offset
//*
//**************************************************************************

UINT MIDIFile::insert_smpte_offset(int hr, int mn, int se, int fr, int ff)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	if ((hr < 0) ||
		((mn < 0) && (mn >= 60)) ||
		((se < 0) && (se >= 60)) ||
		((fr < 0) && (fr >= 30)) ||
		((ff < 0) && (ff >= 100))  )
	{
		return HP_ERR_BAD_SMPTE_OFFSET;
	}
	
	unsigned char event_data[7];

	event_data[0] = 0xff;
	event_data[1] = 0x54;
	event_data[2] = 0x05;
	event_data[3] = hr;
	event_data[4] = mn;
	event_data[5] = se;
	event_data[6] = fr;
	event_data[7] = ff;

	SmpteOffsetEvent * so_event = new SmpteOffsetEvent(0, event_data, 8);
	int index = eventlist->insert(so_event, insert_first);
	if (index <= pos) pos++;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert time signature
//*
//**************************************************************************

UINT MIDIFile::insert_timesignature(int time, int num, int denum, int metronome_click, int n32)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	unsigned char event_data[7];
	event_data[0] = 0xff;
	event_data[1] = 0x58;
	event_data[2] = 0x04;
	event_data[3] = num & 0x7f;

	// convert denum from parameter
	int intern_denum = 0; // value OK for denum = 1
	if (denum < 1) return HP_ERR_INVALID_DENUM;
	if (denum > 1)
	{
		int scr_denum = denum;
		do
		{
			if ((scr_denum&0x01)!= 0) return HP_ERR_INVALID_DENUM;
			scr_denum >>= 1;
			intern_denum++;
		} while(scr_denum != 1);
	}
	
	event_data[4] = intern_denum & 0x7f;
	event_data[5] = metronome_click & 0x7f;
	event_data[6] = n32 & 0x7f;

	TimeSignatureEvent * ts_event = new TimeSignatureEvent(time, event_data, 7);
	int index = eventlist->insert(ts_event, insert_first);
	if (index <= pos) pos++;
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert key signature
//*
//**************************************************************************

UINT MIDIFile::insert_keysignature(int time, char *key)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;
	
	int result;
	unsigned char sf, mi;
	result = convert_key_to_intern(key,&sf,&mi);
	if (result != HP_ERR_NONE) return result;

	unsigned char event_data[5];
	event_data[0] = 0xff;
	event_data[1] = 0x59;
	event_data[2] = 0x02;
	event_data[3] = sf;
	event_data[4] = mi;

	KeySignatureEvent * ks_event = new KeySignatureEvent(time, event_data, 5);
	ks_event->set_key(key);

	int index = eventlist->insert(ks_event, insert_first);
	if (index <= pos) pos++;
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert yamaha chord event
//*
//**************************************************************************

UINT MIDIFile::insert_chord(int time, unsigned char cr, unsigned char ct, unsigned char bn, unsigned char bt)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;
	
	int result;

	unsigned char event_data[10];
	event_data[0] = 0xff;
	event_data[1] = 0x7f;
	event_data[2] = 0x07;
	event_data[3] = 0x43;
	event_data[4] = 0x7b;
	event_data[5] = 0x01;
	event_data[6] = cr & 0x7f;
	event_data[7] = ct & 0x7f;
	event_data[8] = bn & 0x7f;
	event_data[9] = bt & 0x7f;


	ChordNameEvent * ch_event = new ChordNameEvent(time, event_data, 10);
	result = ch_event->convert_chord_to_text(cr,ct,bn,bt,&(ch_event->chord));
	if (result != HP_ERR_NONE) return result;
	int index = eventlist->insert(ch_event, insert_first);
	if (index <= pos) pos++;
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert yamaha meta events
//*
//**************************************************************************


UINT MIDIFile::insert_ymeta_event(int time, UINT ytype, unsigned char *event_data, int length)
{
	unsigned char ymeta_type = event_data[5];
	unsigned char ymeta_additional_type = event_data[7];
	if (!is_xg_event(event_data,length)) return HP_ERR_EVENTDATA;

	MIDIEvent *mev;
	
	switch (ytype)
	{
	case HP_QUICK_START:
		if ((event_data[4] != 0x73) || (ymeta_type != 0x0a) || (ymeta_additional_type != 4)) return HP_ERR_EVENTDATA;
		mev = new QuickStartEvent(time, event_data,length);
		break;
	case HP_SCORE_LAYOUT:
		if ((event_data[4] != 0x73) || (ymeta_type != 0x0a) || (ymeta_additional_type != 5)) return HP_ERR_EVENTDATA;
		mev = new ScoreLayoutEvent(time, event_data,length);
		break;
	case HP_SCORE_START_BAR:
		if ((event_data[4] != 0x73) || (ymeta_type != 0x0a) || (ymeta_additional_type != 7)) return HP_ERR_EVENTDATA;
		mev = new ScoreStartBarEvent(time, event_data,length);
		break;
	case HP_KEYBOARD_VOICE:
		if ((event_data[5] != 0x73) || (event_data[6] != 0x0d)) return HP_ERR_EVENTDATA;
		mev = new KeyboardVoiceEvent(time, event_data,length);
		break;
	case HP_XF_VERSION_ID:
		if ((event_data[4] != 0x7b) || (ymeta_type != 0x00)) return HP_ERR_EVENTDATA;
		mev = new XFVersionIdEvent(time, event_data,length);
		break;
	case HP_REHEARSAL_MARK:
		if ((event_data[4] != 0x7b) || (ymeta_type != 0x02)) return HP_ERR_EVENTDATA;
		mev = new RehearsalMarkEvent(time, event_data,length);
		break;
	case HP_PHRASE_MARK:
		if ((event_data[4] != 0x7b) || (ymeta_type != 0x03)) return HP_ERR_EVENTDATA;
		mev = new PhraseMarkEvent(time, event_data,length);
		break;
	case HP_MAX_PHRASE_MARK:
		if ((event_data[4] != 0x7b) || (ymeta_type != 0x04)) return HP_ERR_EVENTDATA;
		mev = new MaxPhraseMarkEvent(time, event_data,length);
		break;
	case HP_FINGERING_NUMBER:
		if ((event_data[4] != 0x7b) || (ymeta_type != 0x05)) return HP_ERR_EVENTDATA;
		mev = new FingeringNumberEvent(time, event_data,length);
		break;
	case HP_GUIDE_TRACK_FLAG:
		if ((event_data[4] != 0x7b) || (ymeta_type != 0x0c)) return HP_ERR_EVENTDATA;
		mev = new GuideTrackFlagEvent(time, event_data,length);
		break;
	case HP_GUITAR_INFORMATION_FLAG:
		if ((event_data[4] != 0x7b) || (ymeta_type != 0x10)) return HP_ERR_EVENTDATA;
		mev = new GuitarInformationFlagEvent(time, event_data,length);
		break;
	case HP_CHORD_VOICING_FOR_GUITAR:
		if ((event_data[4] != 0x7b) || (ymeta_type != 0x12)) return HP_ERR_EVENTDATA;
		mev = new ChordVoicingForGuitarEvent(time, event_data,length);
		break;
	case HP_LYRICS_BITMAP:
		if ((event_data[4] != 0x7b) || (ymeta_type != 0x21)) return HP_ERR_EVENTDATA;
		mev = new LyricsBitmapEvent(time, event_data,length);
		break;
	case HP_OTHER_YAMAHA_META:
		mev = new YMetaEvent(time, event_data,length);
		break;
	default:
		return HP_ERR_EVENTDATA;
	}

	int index = eventlist->insert(mev, insert_first);
	if (index <= pos) pos++;
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		insert raw event
//*
//**************************************************************************

UINT MIDIFile::insert_rawevent(int time, unsigned char *data, int length, int track_no)
{
	MIDIEvent * rawev = new MIDIEvent(time, data, length);
	rawev->chan = track_no;
	rawev->type = HP_RAW_EVENT;
	int index = eventlist->insert(rawev, insert_first);
	if (index <= pos) pos++;
	return HP_ERR_NONE;
}



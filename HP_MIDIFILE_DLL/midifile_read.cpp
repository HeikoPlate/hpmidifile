/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  midifile_read.cpp : Read Functions                                             */
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
//*		read  event from stream
//*
//**************************************************************************

UINT MIDIFile::read_event(int *id, int *chan, int *time, int *etype)
{
	if (pos >= eventlist->get_length())
	{
		return HP_ERR_RANGE_EVENTLIST;
	}

	MIDIEvent *mev = eventlist->get(pos);
	*id	   = pos;
	*chan  = mev->chan;
	*time  = mev->resulting_time;
	*etype = mev->type;
	pos++;
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read event data
//*
//**************************************************************************

UINT MIDIFile::read_event_data(int id, int *time, unsigned char **data, int *data_length)
{
	MIDIEvent *mev;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	*time = mev->resulting_time;
	*data_length = mev->event_data_length;

	*data = new unsigned char[mev->event_data_length];
	int i;
	for (i=0; i<mev->event_data_length; i++)
	{
		(*data)[i] = mev->event_data[i];
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read ppqn
//*
//**************************************************************************

UINT MIDIFile::read_ppqn(int *act_ppqn)
{
	if (ppqn < 0)
	{
		return HP_ERR_NO_PPQN;
	}

	*act_ppqn = ppqn;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read note
//*
//**************************************************************************

UINT MIDIFile::read_note(int id , int *time, int *chan, int *note, int *velocity, int *length)
{
	MIDIEvent *mev;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_NOTE_ON) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	*chan = mev->chan;
	*note = ((NoteOnEvent*)mev)->note();
	*velocity = ((NoteOnEvent*)mev)->vel();
	*length = (((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time - mev->resulting_time;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read aftertouch
//*
//**************************************************************************

UINT MIDIFile::read_aftertouch(int id, int *time, int *chan, int *note, int *pressure)
{

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	MIDIEvent *mev = eventlist->get(id);

	if (mev->type != HP_AFTERTOUCH) return HP_ERR_BAD_ID;

	*time = ((AftertouchEvent*)mev)->resulting_time;
	*chan = ((AftertouchEvent*)mev)->chan;
	*note = ((AftertouchEvent*)mev)->event_data[1];
	*pressure = ((AftertouchEvent*)mev)->event_data[2];
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read controller
//*
//**************************************************************************

UINT MIDIFile::read_controller(int id, int *time, int *chan, int *number, int *value)
{

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	MIDIEvent *mev = eventlist->get(id);

	if ((mev->type & HP_TYPE_MASK) != HP_CONTROLLER_TYPE)	 return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	*chan = mev->chan;
	*number = ((ControlChangeEvent*)mev)->number();
	*value = ((ControlChangeEvent*)mev)->value();
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read RPN and NRPN
//*
//**************************************************************************

UINT MIDIFile::read_rpn_nrpn(UINT type, int id, int *time, int *chan, bool *absolute, int *number, int *value)
{
	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	MIDIEvent *mev = eventlist->get(id);

	if (mev->type != type)	 return HP_ERR_BAD_ID;
	
	*time = mev->resulting_time;
	*chan = mev->chan;

	bool scr_absolute =((((ControlChangeEvent*)mev)->assoc_data_idcrement) == NULL);
	int scr_number = (((ControlChangeEvent*)mev)->value()) << 7;
	int scr_value = 0;

	if (((ControlChangeEvent*)mev)->assoc_fine != NULL)
	{
		scr_number += (((ControlChangeEvent*)mev)->assoc_fine)->value();
	}

	if (scr_absolute)
	{

		if (((ControlChangeEvent*)mev)->assoc_data_coarse != NULL)
		{
			scr_value = ((((ControlChangeEvent*)mev)->assoc_data_coarse)->value()) << 7;
		}

		if (((ControlChangeEvent*)mev)->assoc_data_fine != NULL)
		{
			scr_value += (((ControlChangeEvent*)mev)->assoc_data_fine)->value();
		}
	}
	else
	{
		scr_value = (((ControlChangeEvent*)mev)->assoc_data_idcrement)->value();
		if ((((ControlChangeEvent*)mev)->assoc_data_idcrement)->type == HP_DATA_DECREMENT)
		{
			scr_value = -scr_value;
		}
	}

	*number = scr_number;
	*value = scr_value;
	*absolute = scr_absolute;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read program change
//*
//**************************************************************************

UINT MIDIFile::read_program_change(int id, int *time, int *chan, int *number)
{

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	MIDIEvent *mev = eventlist->get(id);

	if (mev->type != HP_PROGRAM_CHANGE)	 return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	*chan = mev->chan;
	*number = ((ProgramChangeEvent*)mev)->number();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read channel pressure
//*
//**************************************************************************

UINT MIDIFile::read_channel_pressure(int id, int *time, int *chan, int *pressure)
{

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	MIDIEvent *mev = eventlist->get(id);

	if (mev->type != HP_CHANNEL_PRESSURE) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	*chan = mev->chan;
	*pressure = ((ChannelPressureEvent*)mev)->pressure();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read pitch wheel
//*
//**************************************************************************

UINT MIDIFile::read_pitch_wheel(int id, int *time, int *chan, int *value)
{

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	MIDIEvent *mev = eventlist->get(id);

	if (mev->type != HP_PITCH_WHEEL) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	*chan = mev->chan;
	*value = ((PitchWheelEvent*)mev)->wheel();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read sysex
//*
//**************************************************************************

UINT MIDIFile::read_sysex(int id, int *time, unsigned char** data, int *length)
{
	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	MIDIEvent *mev = eventlist->get(id);

	if (mev->type != HP_SYSEX) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	((SysExEvent*)mev)->sysex(data,length);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read sequence number
//*
//**************************************************************************

UINT MIDIFile::read_sequence_number(int id, int *time, int *number)
{
	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	MIDIEvent *mev = eventlist->get(id);

	if (mev->type != HP_SEQUENCE_NUMBER) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	*number = ((SequenceNumberEvent*)mev)->number();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read text from meta, used with Text, Copyright, Sequence/Track-Name
//*							 Instrument, Lyric, Marker, CuePoint
//*
//**************************************************************************

UINT MIDIFile::read_text_from_meta(int id, int *time, UINT type, int *chan, char** text)
{
	int i, k;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	MIDIEvent *mev = eventlist->get(id);
	if (mev->type != type) return HP_ERR_BAD_ID;

	*chan = -1;

	switch (mev->type)
	{
	case HP_TEXT:
		break;
	case HP_COPYRIGHT:
		break;
	case HP_SONG_TRACK_NAME:
		*chan = ((MetaEvent*)mev)->track; 
		break;
	case HP_INSTRUMENT:
		*chan = ((MetaEvent*)mev)->track; 
		break;
	case HP_LYRIC:
		break;
	case HP_MARKER:
		break;
	case HP_CUE_POINT:
		break;
	default:
		return HP_ERR_EVENTTYPE;
	}
	
	*time = mev->resulting_time;

	unsigned char var_len_bytes[64];

	k = -1;
	do
	{
		k++;
		var_len_bytes[k] = mev->event_data[k+2];
	} while (((var_len_bytes[k]&0x80) != 0) && (k < 64));

	int mev_text_length = convert_var_len_to_int(var_len_bytes);
	
	*text = new char[mev_text_length+1];

	k+=3; // posit on first char
	
	for (i=0; i< mev_text_length; i++)
	{
		(*text)[i] = mev->event_data[k+i];
	}
	(*text)[mev_text_length] = 0;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read MIDI port
//*
//**************************************************************************

UINT MIDIFile::read_midiport(int id, int *time, int *track, int *port)
{
	MIDIEvent *mev;
	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_MIDI_PORT) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	*track = ((MetaEvent*)mev)->track;
	*port = ((MidiPortEvent*)mev)->port();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read tempo
//*
//**************************************************************************

UINT MIDIFile::read_tempo(int id, int *time, int *bpm, int * cps)
{
	MIDIEvent *mev;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_TEMPO) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	*bpm = ((TempoEvent*)mev)->bpm;
	*cps = 	(1000000*ppqn)/((TempoEvent*)mev)->msq;


	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read tempo_double
//*
//**************************************************************************

UINT MIDIFile::read_tempo_double(int id, int *time, double *bpm, int * cps)
{
	MIDIEvent *mev;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_TEMPO) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	*bpm = ((TempoEvent*)mev)->bpm_double;
	*cps = 	(1000000*ppqn)/((TempoEvent*)mev)->msq;


	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read smpte offset 
//*
//**************************************************************************

UINT MIDIFile::read_smpte_offset(int id, int *hr, int *mn, int *se, int *fr, int *ff)
{
	MIDIEvent *mev;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_SMPTE_OFFSET) return HP_ERR_BAD_ID;

	*hr = ((SmpteOffsetEvent*)mev)->hr();
	*mn = ((SmpteOffsetEvent*)mev)->mn();
	*se = ((SmpteOffsetEvent*)mev)->se();
	*fr = ((SmpteOffsetEvent*)mev)->fr();
	*ff = ((SmpteOffsetEvent*)mev)->ff();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read time signature
//*
//**************************************************************************

UINT MIDIFile::read_time_signature(int id, int *time, int *num, int *denum, int *metronome_click, int *n32)
{
	MIDIEvent *mev;
	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_TIME_SIGNATURE) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	*num = ((TimeSignatureEvent*)mev)->num;
	*denum = ((TimeSignatureEvent*)mev)->denum;
	*metronome_click = ((TimeSignatureEvent*)mev)->metronome_click();
	*n32 = ((TimeSignatureEvent*)mev)->n32();
	
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read key signature
//*
//**************************************************************************

UINT MIDIFile::read_key_signature(int id, int *time, char **key)
{
	MIDIEvent *mev;
	

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_KEY_SIGNATURE) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;

	(*key) = new char[strlen(((KeySignatureEvent*)mev)->key)+1];
	strcpy(*key,((KeySignatureEvent*)mev)->key);
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read yamaha chord
//*
//**************************************************************************

UINT MIDIFile::read_chord(int id, int *time, unsigned char* cr, unsigned char* ct, unsigned char* bn, unsigned char* bt,char** chord)
{
	MIDIEvent *mev;
	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_CHORD_NAME) return HP_ERR_BAD_ID;

	*time = mev->resulting_time;
	
	((ChordNameEvent*)mev)->cr_ct_bn_bt(cr,ct,bn,bt);

	(*chord) = new char[strlen(((ChordNameEvent*)mev)->chord)+1];
	strcpy(*chord ,((ChordNameEvent*)mev)->chord);
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		read yamaha meta event
//*
//**************************************************************************

UINT MIDIFile::read_ymeta_event(int id, int *time, UINT ytype, unsigned char **event_data, int *length)
{
	int i;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
	
	switch (ytype)
	{
	case HP_SCORE_START_BAR:
	case HP_QUICK_START:
	case HP_SCORE_LAYOUT:
	case HP_KEYBOARD_VOICE:
	case HP_XF_VERSION_ID:
	case HP_REHEARSAL_MARK:
	case HP_PHRASE_MARK:
	case HP_MAX_PHRASE_MARK:
	case HP_FINGERING_NUMBER:
	case HP_GUIDE_TRACK_FLAG:
	case HP_GUITAR_INFORMATION_FLAG:
	case HP_CHORD_VOICING_FOR_GUITAR:
	case HP_LYRICS_BITMAP:
	case HP_OTHER_YAMAHA_META:
		break;
	default:
		return HP_ERR_EVENTDATA;
	}
	
	MIDIEvent *mev = eventlist->get(id);

	if (ytype != mev->type) return HP_ERR_EVENTTYPE;

	(*length) = mev->event_data_length;

	(*event_data) = new unsigned char[(*length)];
	
	for (i=0; i<(*length); i++)
	{
		(*event_data)[i] = (mev->event_data)[i];
	}

	return HP_ERR_NONE;
}

/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  midifile_change.h: Change Functions                                                               */
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
//*		change channel for all selected channel-events
//*
//**************************************************************************

void MIDIFile::change_channel(int chan)
{
	int i;

	for (i=0; i < eventlist->get_length(); i++)
	{
		MIDIEvent *mev = eventlist->get(i);
		if ((mev->selected > 0) && (mev->chan != -1))
		{
			mev->change_chan(chan);
		}
	}
}

//**************************************************************************
//*
//*		change or set ppqn
//*
//**************************************************************************

UINT MIDIFile::change_ppqn(int newppqn)
{
	const int allowed_ppqn[]=
	{
	  HP_PPQN48,  HP_PPQN72,  HP_PPQN96,  HP_PPQN120,
	  HP_PPQN144, HP_PPQN168, HP_PPQN192, HP_PPQN216,
	  HP_PPQN240, HP_PPQN360, HP_PPQN384, HP_PPQN480,
	  HP_PPQN960, HP_PPQN1920
	};

	int i;
	int ppqnOK = false;

	for (i=0; i<14; i++)
	{
		if (newppqn==allowed_ppqn[i])
		{
			ppqnOK = true;
			break;
		}
	}
	if (!ppqnOK) return HP_ERR_INVALID_PPQN;

	if (ppqn < 0)
	{
		ppqn = newppqn;
		tempolist->ppqn = ppqn;
		return HP_ERR_NONE;
	}

	change_ppqn_basic(eventlist,newppqn,&ppqn);
	reorganize_tempolist(eventlist,newppqn);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change note
//*
//**************************************************************************

UINT MIDIFile::change_note(int id , int chan, int note, bool absolute)
{
	//	id >= 0:					change for one event only
	//	id = HP_ALL_EVENTS			change for all events
	//  chan >= 0					change for chan
	//  note >=0					new note value
	//	absolute = HP_ABSOLUTE:		change old note by increment (note>0) or decrement (note < 0)
	//	percent = HP_RELATIVE:		note gets note

	int note_value;
	int new_note_value;

	MIDIEvent *mev;

	if (id != HP_ALL_EVENTS)
	{	// ignore  note
		if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
		mev = eventlist->get(id);
		if (mev->type != HP_NOTE_ON)
		{
			return HP_ERR_BAD_ID;
		}

		note_value = ((NoteOnEvent*)mev)->note();
		new_note_value = (absolute==HP_ABSOLUTE)?(note):note_value+note;
		if ((new_note_value < 0)||(new_note_value > 127))
		{
			return HP_ERR_BAD_NEW_NOTE;
		}
		((NoteOnEvent*)mev)->set_note(new_note_value);
		((NoteOnEvent*)mev)->set_chan(chan);

		mev = ((NoteOnEvent*)mev)->assoc_noteoff;
		((NoteOffEvent*)mev)->set_note(new_note_value);
		((NoteOffEvent*)mev)->set_chan(chan);
	}
	else
	{	// look at all note_on

		int i;

		for (i=0; i < eventlist->get_length(); i++)
		{
			mev = eventlist->get(i);
			if (mev->type != HP_NOTE_ON)
			{
				continue;
			}

			if (mev->chan != chan)
			{
				continue;
			}
			note_value = ((NoteOnEvent*)mev)->note();
			new_note_value = (absolute==HP_ABSOLUTE)?(note_value):note_value+note;
			if ((new_note_value < 0)||(new_note_value > 127))
			{
				return HP_ERR_BAD_NEW_NOTE;
			}
			((NoteOnEvent*)mev)->set_note(new_note_value);
			((NoteOnEvent*)mev)->set_chan(chan);
			mev = ((NoteOnEvent*)mev)->assoc_noteoff;
			((NoteOffEvent*)mev)->set_note(new_note_value);
			((NoteOffEvent*)mev)->set_chan(chan);
		}
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change velocity
//*
//**************************************************************************

UINT MIDIFile::change_velocity(int id , int chan, int note, int value, bool percent)
{
	//	id >= 0:					change for one event only
	//	id = HP_ALL_EVENTS			change for all events
	//  chan >= 0					change for chan
	//  chan = HP_ALL_CHAN			change for all chan
	//  note >=0					change for note
	//  note = HP_ALL_NOTES			change for all notes
	//	percent = HP_PERCENT:		change old velocity with percent-value, 150: +50%, 60: 60%
	//	percent = HP_NO_PERCENT:	new velocity gets value

	int old_vel, new_vel;
	int note_chan, note_value;

	MIDIEvent *mev;

	if (id != HP_ALL_EVENTS)
	{	// ignore chan and note
		if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
		mev = eventlist->get(id);
		if (mev->type != HP_NOTE_ON)
		{
			return HP_ERR_BAD_ID;
		}

		note_chan = mev->chan;
		if ((chan != HP_ALL_CHAN)&&(note_chan != chan))
		{
			return HP_ERR_BAD_ID;
		}

		note_value = ((NoteOnEvent*)mev)->note();
		if ((note != HP_ALL_NOTES)&&(note_value != note))
		{
			return HP_ERR_BAD_ID;
		}

		old_vel = ((NoteOnEvent*)mev)->vel();
		new_vel = (percent==HP_PERCENT)?(old_vel*value/100):value;
		if (new_vel <= 0) 
		{
			delete_event(id); // delete note
			eventlist->sort();
		}
		else
		{
			if (new_vel > 127) new_vel = 127;
			((NoteOnEvent*)mev)->set_vel(new_vel);
		}
	}
	else
	{	// look at all note_on
		int i;

		for (i=0; i < eventlist->get_length(); i++)
		{
			mev = eventlist->get(i);
			if (mev->type != HP_NOTE_ON)
			{
				continue;
			}

			note_chan = mev->chan;
			if ((chan != HP_ALL_CHAN)&&(note_chan != chan))
			{
				continue;
			}

			note_value = ((NoteOnEvent*)mev)->note();
			if ((note != HP_ALL_NOTES)&&(note_value != note))
			{
				continue;
			}

			old_vel = ((NoteOnEvent*)mev)->vel();
			new_vel = (percent==HP_PERCENT)?(old_vel*value/100):value;
			if (new_vel <= 0) 
			{
				delete_event(i); // delete note
				i--;
				eventlist->sort();
			}
			else
			{
				if (new_vel > 127) new_vel = 127;
				((NoteOnEvent*)mev)->set_vel(new_vel);
			}
		}
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change velocity for selected notes
//*
//**************************************************************************

UINT MIDIFile::change_velocity_selected(int value, bool percent)
{
	//	percent = HP_PERCENT:		change old velocity with percent-value, 150: +50%, 60: 60%
	//	percent = HP_NO_PERCENT:	new velocity gets value
	
	int old_vel, new_vel;
	MIDIEvent *mev;
	
	
	// look at all note_on
	int i;
	
	for (i=0; i < eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if ((mev->type != HP_NOTE_ON)||(mev->selected == 0))
		{
			continue;
		}
		
		old_vel = ((NoteOnEvent*)mev)->vel();
		new_vel = (percent==HP_PERCENT)?(old_vel*value/100):value;
		if (new_vel <= 0)
		{
			delete_event(i); // delete note
			i--;
			eventlist->sort();
		}
		else
		{
			if (new_vel > 127) new_vel = 127;
			((NoteOnEvent*)mev)->set_vel(new_vel);
		}
	}

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change note_length
//*
//**************************************************************************

UINT MIDIFile::change_note_length(int id , int chan, int note, int value, bool percent)
{
	//	id >= 0:					change for one event only
	//	id = HP_ALL_EVENTS			change for all events
	//  chan >= 0					change for chan
	//  chan = HP_ALL_CHAN			change for all chan
	//  note >=0					change for note
	//  note = HP_ALL_NOTES			change for all notes
	//	percent = HP_PERCENT:		change old velocity with percent-value, 150: +50%, 60: 60%
	//	percent = HP_NO_PERCENT:	new velocity gets value

	int old_length, new_length, new_off_time;
	int note_chan, note_value;

	MIDIEvent *mev;

	if (id != HP_ALL_EVENTS)
	{	// ignore chan and note
		if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
		mev = eventlist->get(id);
		if (mev->type != HP_NOTE_ON)
		{
			return HP_ERR_BAD_ID;
		}

		note_chan = mev->chan;
		if ((chan != HP_ALL_CHAN)&&(note_chan != chan))
		{
			return HP_ERR_BAD_ID;
		}

		note_value = ((NoteOnEvent*)mev)->note();
		if ((note != HP_ALL_NOTES)&&(note_value != note))
		{
			return HP_ERR_BAD_ID;
		}

		old_length = (((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time - mev->resulting_time;
		new_length = (percent==HP_PERCENT)?(old_length*value/100):value;
		new_off_time = mev->resulting_time + new_length;

		(((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time = new_off_time;
	}
	else
	{	// look at all note_on
		int i;

		for (i=0; i < eventlist->get_length(); i++)
		{
			mev = eventlist->get(i);
			if (mev->type != HP_NOTE_ON)
			{
				continue;
			}

			note_chan = mev->chan;
			if ((chan != HP_ALL_CHAN)&&(note_chan != chan))
			{
				continue;
			}

			note_value = ((NoteOnEvent*)mev)->note();
			if ((note != HP_ALL_NOTES)&&(note_value != note))
			{
				continue;
			}

			old_length = (((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time - mev->resulting_time;
			new_length = (percent==HP_PERCENT)?(old_length*value/100):value;
			new_off_time = mev->resulting_time + new_length;

			(((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time = new_off_time;
		}
	}
	eventlist->sort();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		transpose selected
//*
//**************************************************************************

UINT MIDIFile::transpose_selected(int steps)
{
	int note_value;
	int new_note_value;
	int i;
	MIDIEvent *mev;

	// first: look in all events for good new note-values
	for (i=0; i < eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if ((mev->type != HP_NOTE_ON)||(mev->selected == 0))
		{
			continue;
		}

		note_value = ((NoteOnEvent*)mev)->note();
		new_note_value = note_value+steps;
		if ((new_note_value<0)||(new_note_value>127)) return HP_ERR_BAD_TRANSPOSE;
	}

	// now: perform transpose
	for (i=0; i < eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if ((mev->type != HP_NOTE_ON)||(mev->selected == 0))
		{
			continue;
		}

		note_value = ((NoteOnEvent*)mev)->note();
		new_note_value = note_value+steps;
		((NoteOnEvent*)mev)->set_note(new_note_value);
		mev = ((NoteOnEvent*)mev)->assoc_noteoff;
		((NoteOffEvent*)mev)->set_note(new_note_value);
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		quantisize selected
//*
//**************************************************************************

UINT MIDIFile::quantisize_selected(int resolution, bool start, bool length)
{

	if (!start && !length) return HP_ERR_QUANTISIZE;

	int tslot;

	switch (resolution)
	{
	case 1: 
		tslot = 4*ppqn;
		break;
	case 2:
		tslot = 2*ppqn;
		break;
	case 4:
		tslot = ppqn;
		break;
	case 8:
		tslot = ppqn/2;
		break;
	case 16:
		tslot = ppqn/4;
		break;
	case 32:
		tslot = ppqn/8;
		break;
	case 64:
		tslot = ppqn/16;
		break;
	case 13:
		tslot = 8*ppqn/3; // whole in triole
		break;
	case 23:
		tslot = 4*ppqn/3; // half in triole
		break;
	case 43:
		tslot = 2*ppqn/3; // quarter in triole
		break;
	case 83:
		tslot = ppqn/3;   // 8th in triole
		break;
	case 163:
		tslot = ppqn/6;   // 16th in triole
		break;
	case 323:
		tslot = ppqn/12;  // 32th in triole
		break;
	default:
		return HP_ERR_QUANTISIZE;
	}


	int note_on_time;
	int note_off_time;
	int note_length;
	int remainder;
	int delta;

	int i;
	MIDIEvent *mev;

	for (i=0; i < eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if ((mev->type != HP_NOTE_ON)||(mev->selected == 0))
		{
			continue;
		}

		if (start)
		{
			note_on_time = mev->resulting_time;
			note_off_time = (((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time;
			note_length = note_off_time-note_on_time;
			// quantisize start time
			remainder = note_on_time%tslot;
			if (remainder < tslot/2)
			{
				delta = -remainder;
			}
			else
			{
				delta = tslot-remainder;
			}
			mev->resulting_time = note_on_time + delta;
			(((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time = note_off_time + delta;
		}

		if (length)
		{
			note_on_time = mev->resulting_time;
			note_off_time = (((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time;
			note_length = note_off_time-note_on_time;
			// quantisize note length
			remainder = note_length%tslot;
			if (remainder < tslot/2)
			{
				delta = -remainder;
			}
			else
			{
				delta = tslot-remainder;
			}

			if (note_length + delta <=0)
			{
//				delete_event(i);
//				i--;
				(((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time = note_on_time +tslot;
			}
			else
			{
				(((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time = note_off_time + delta;

			}			
		}
	}
	eventlist->sort();
	return HP_ERR_NONE;
}


//**************************************************************************
//*
//*		shift notes selected
//*
//**************************************************************************

UINT MIDIFile::shift_notes_selected(int ticks)
{
	int note_time;
	int new_note_time;
	int i;
	MIDIEvent *mev;

	// first: look in all events for good new times
	for (i=0; i < eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if ((mev->type != HP_NOTE_ON)||(mev->selected == 0))
		{
			continue;
		}

		note_time = mev->resulting_time;
		new_note_time = note_time+ticks;
		if (new_note_time<0) return HP_ERR_BAD_SHIFT;
	}

	// now: perform shift
	for (i=0; i < eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if ((mev->type != HP_NOTE_ON)||(mev->selected == 0))
		{
			continue;
		}

		note_time = mev->resulting_time;
		new_note_time = note_time+ticks;
		mev->resulting_time = new_note_time;
		mev = ((NoteOnEvent*)mev)->assoc_noteoff;
		note_time = mev->resulting_time;
		new_note_time = note_time+ticks;
		mev->resulting_time = new_note_time;
	}

	eventlist->sort();
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change aftertouch
//*
//**************************************************************************

UINT MIDIFile::change_aftertouch(int id , int note, int value, bool percent)
{
	//	id >= 0:					change for one event only
	//  0 <= note <= 127			
	//	id = HP_ALL_EVENTS:			change for all events
	//	percent = HP_PERCENT:		change old value with percent-value, 150: +50%, 60: 60%
	//	percent = HP_NO_PERCENT:	new value gets value

	int old_value, new_value;
	MIDIEvent *mev;

	if (id != HP_ALL_EVENTS)
	{
		if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
		mev = eventlist->get(id);
		if (mev->type != HP_AFTERTOUCH)
		{
			return HP_ERR_BAD_ID;
		}

		old_value = ((AftertouchEvent*)mev)->value();
		new_value = (percent==HP_PERCENT)?(old_value*value/100):value;
		((AftertouchEvent*)mev)->set_value(new_value&0x7f);
		if (note != HP_NO_NOTE)
		{
			((AftertouchEvent*)mev)->set_note(note&0x7f);
		}
	}
	else 
	{
		int i;
		for (i=0; i<eventlist->get_length(); i++)
		{
			mev = eventlist->get(i);
			if (mev->type != HP_AFTERTOUCH)
			{
				continue;
			}
			old_value = ((AftertouchEvent*)mev)->value();
			new_value = (percent==HP_PERCENT)?(old_value*value/100):value;
			((AftertouchEvent*)mev)->set_value(new_value&0x7f);
			if (note != HP_NO_NOTE)
			{
				((AftertouchEvent*)mev)->set_note(note&0x7f);
			}
		}
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change controller
//*
//**************************************************************************

UINT MIDIFile::change_controller(int id , UINT type, int value, bool percent)
{
	//	id >= 0:					change for one event only
	//  type						type of controller
	//	percent = HP_PERCENT:		change old value with percent-value, 150: +50%, 60: 60%
	//	percent = HP_NO_PERCENT:	new value gets value

	int old_value, new_value;
	int new_number = -1;
	MIDIEvent *mev;

	if ((id < 0) || (id > eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
	
	mev = eventlist->get(id);

	old_value = ((ControlChangeEvent*)mev)->value();
	new_value = (percent==HP_PERCENT)?(old_value*value/100):value;
	if (new_value<0) new_value = 0;
	if (new_value>127) new_value = 127;

	if ((mev->type & HP_TYPE_MASK) != HP_CONTROLLER_TYPE) return HP_ERR_BAD_ID;
	if (type != ((ControlChangeEvent*)mev)->type)
	{
		switch (type)
		{
		case HP_BANK_SELECT_MSB:
			new_number = 0;
			break;
		case HP_MODULATION:
			new_number = 1;
			break;
		case HP_BREATH:
			new_number = 2;
			break;
		case HP_PORTAMENTO_TIME:
			new_number = 5;
			break;
		case HP_DATA_ENTRY_MSB:
			new_number = 6;
			break;
		case HP_MAIN_VOLUME:
			new_number = 7;
			break;
		case HP_PANPOT:
			new_number = 10;
			break;
		case HP_EXPRESSION:
			new_number = 11;
			break;
		case HP_BANK_SELECT_LSB:
			new_number = 32;
			break;
		case HP_DATA_ENTRY_LSB:
			new_number = 38;
			break;
		case HP_SUSTAIN:
			new_number = 64;
			break;
		case HP_PORTAMENTO:
			new_number = 65;
			break;
		case HP_SOSTENUTO:
			new_number = 66;
			break;
		case HP_SOFT_PEDAL:
			new_number = 67;
			break;
		case HP_HARMONIC_CONTENT:
			new_number = 71;
			break;
		case HP_RELEASE_TIME:
			new_number = 72;
			break;
		case HP_ATTACK_TIME:
			new_number = 73;
			break;
		case HP_BRIGHTNESS:
			new_number = 74;
			break;
		case HP_DECAY_TIME:
			new_number = 75;
			break;
		case HP_VIBRATO_RATE:
			type = HP_VIBRATO_RATE;
			new_number = 76;
			break;
		case HP_VIBRATO_DEPTH:
			new_number = 77;
			break;
		case HP_VIBRATO_DELAY:
			new_number = 78;
			break;
		case HP_PORTAMENTO_CONTROL:
			new_number = 84;
			break;
		case HP_REVERB_SEND_LEVEL:
			new_number = 91;
			break;
		case HP_CHORUS_SEND_LEVEL:
			new_number = 93;
			break;
		case HP_VARIATION_SEND_LEVEL:
			new_number = 94;
			break;
		case HP_DATA_INCREMENT:
			new_number = 96;
			break;
		case HP_DATA_DECREMENT:
			new_number = 97;
			break;
		case HP_NRPN_LSB:
			new_number = 98;
			break;
		case HP_NRPN_MSB:
			new_number = 99;
			break;
		case HP_RPN_LSB:
			new_number = 100;
			break;
		case HP_RPN_MSB:
			new_number = 101;
			break;
		case HP_ALL_SOUND_OFF:
			new_number = 120;
			break;
		case HP_RESET_ALL_CONTROLLERS:
			new_number = 121;
			break;
		case HP_ALL_NOTES_OFF:
			new_number = 123;
			break;
		case HP_OMNI_OFF:
			new_number = 124;
			break;
		case HP_OMNI_ON:
			new_number = 125;
			break;
		case HP_MONO:
			new_number = 126;
			break;
		case HP_POLY:
			new_number = 127;
			break;
		default:
			new_number = -1;
		}
		if (new_number == -1) return HP_ERR_BAD_ID;
	}

	((ControlChangeEvent*)mev)->set_value(new_value);

	if (new_number != -1)
	{
		((ControlChangeEvent*)mev)->set_number(new_number,type);
	}

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change controller selected
//*
//**************************************************************************

UINT MIDIFile::change_controller_selected(UINT type,int value, bool percent)
{
	//	percent = HP_PERCENT:		change old value with percent-value, 150: +50%, 60: 60%
	//	percent = HP_NO_PERCENT:	new value gets value

	MIDIEvent *mev;
	int id;
	for (id = 0; id < eventlist->get_length(); id++)	
	{
		mev = eventlist->get(id);
		if ((mev->type != type) || (mev->selected == 0)) continue;
		change_controller(id,type,value,percent);
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change RPN/NRPN
//*
//**************************************************************************

UINT MIDIFile::change_rpn_nrpn(UINT type, bool absolute, int id, int value, bool percent)
{
	//	id >= 0:					change for one event only
	//  value						new value
	//	percent = HP_PERCENT:		change old value with percent-value, 150: +50%, 60: 60%
	//	percent = HP_NO_PERCENT:	new value gets value

	int read_time, read_chan, read_number, read_value, new_value;
	bool read_absolute;
	int result;
	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
	result = read_rpn_nrpn(type,id,&read_time,&read_chan,&read_absolute,&read_number,&read_value);

	if (result != HP_ERR_NONE) return result;

	delete_event(id);

	if (absolute)
	{
		new_value = (percent==HP_PERCENT)?(read_value*value/100):value;
	}
	else
	{
		new_value = value;
	}

	return insert_rpn_nrpn(read_time,read_chan,type, read_number,new_value, read_absolute);
 }

//**************************************************************************
//*
//*		change program
//*
//**************************************************************************

UINT MIDIFile::change_program(int id ,int number)
{
	MIDIEvent *mev;

	if ((id < 0) || (id > eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
	
	mev = eventlist->get(id);
	if (mev->type != HP_PROGRAM_CHANGE) return HP_ERR_BAD_ID;

	((ProgramChangeEvent*)mev)->set_number(number);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change channel pressure
//*
//**************************************************************************

UINT MIDIFile::change_channel_pressure(int id ,int pressure)
{
	MIDIEvent *mev;

	if ((id < 0) || (id > eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
	
	mev = eventlist->get(id);
	if (mev->type != HP_CHANNEL_PRESSURE) return HP_ERR_BAD_ID;

	((ChannelPressureEvent*)mev)->set_pressure(pressure);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change pitch wheel
//*
//**************************************************************************

UINT MIDIFile::change_pitch_wheel(int id ,int value)
{
	MIDIEvent *mev;

	if ((id < 0) || (id > eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
	
	mev = eventlist->get(id);
	if (mev->type != HP_PITCH_WHEEL) return HP_ERR_BAD_ID;

	((PitchWheelEvent*)mev)->set_wheel(value);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change sysex
//*
//**************************************************************************

UINT MIDIFile::change_sysex(int id, unsigned char *data)
{
	MIDIEvent *mev;

	if ((id < 0) || (id > eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
	
	mev = eventlist->get(id);
	if (mev->type != HP_SYSEX) return HP_ERR_BAD_ID;

	return ((SysExEvent*)mev)->set_sysex(data);
}

//**************************************************************************
//*
//*		change sequence number
//*
//**************************************************************************

UINT MIDIFile::change_sequence_number(int id, int new_number)
{
	MIDIEvent *mev;

	if ((id < 0) || (id > eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
	
	mev = eventlist->get(id);
	if (mev->type != HP_SEQUENCE_NUMBER) return HP_ERR_BAD_ID;

	((SequenceNumberEvent*)mev)->set_number(new_number);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change texttype, used with text, copyright, track name, instrument, 
//*                                lyric, marker, cuepoint
//*
//**************************************************************************

UINT MIDIFile::change_texttype(int id, UINT type,char* new_text)
{
	int read_time, read_chan;
	int result;

	char *read_text;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	result = read_text_from_meta(id,&read_time,type,&read_chan,&read_text);
	delete[] read_text;

	if (result != HP_ERR_NONE) return result;

	delete_event(id);

	return insert_texttype(read_time,type,new_text, read_chan);
 }

//**************************************************************************
//*
//*		change MIDI port
//*
//**************************************************************************

UINT MIDIFile::change_midiport(int id, int new_track, int new_port)
{
	MIDIEvent *mev;

	if ((id < 0) || (id > eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
	
	mev = eventlist->get(id);
	if (mev->type != HP_MIDI_PORT) return HP_ERR_BAD_ID;
	if (new_track != HP_NO_TRACK)
	{
		((MetaEvent*)mev)->track = new_track;
	}

	((MidiPortEvent*)mev)->set_port(new_port);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change tempo
//*
//**************************************************************************

UINT MIDIFile::change_tempo(int id , int value, bool percent)
{
	//	id >= 0:					change for one event only
	//	id = HP_ALL_EVENTS:			change for all events
	//	percent = HP_PERCENT:		change old bpm with percent-value, 150: +50%, 60: 60%
	//	percent = HP_NO_PERCENT:	new bpm gets value

	int old_bpm, new_bpm, new_msec_tick;
	MIDIEvent *mev;

	if (id != HP_ALL_EVENTS)
	{
		if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
		mev = eventlist->get(id);
		if (mev->type != HP_TEMPO)
		{
			return HP_ERR_BAD_ID;
		}

		old_bpm = ((TempoEvent*)mev)->get_bpm();
		new_bpm = (percent==HP_PERCENT)?(old_bpm*value/100):value;

		((TempoEvent*)mev)->set_tempo(new_bpm);

		tempolist->remove(mev->resulting_time);
		new_msec_tick = (((TempoEvent*)mev)->msq)/ppqn;
		tempolist->insert(mev->resulting_time,new_msec_tick);
	}
	else 
	{
		int i;
		for (i=0; i<eventlist->get_length(); i++)
		{
			mev = eventlist->get(i);
			if (mev->type != HP_TEMPO)
			{
				continue;
			}
			old_bpm = ((TempoEvent*)mev)->get_bpm();
			new_bpm = (percent==HP_PERCENT)?(old_bpm*value/100):value;
			tempolist->remove(mev->resulting_time);

			((TempoEvent*)mev)->set_tempo(new_bpm);

			new_msec_tick = (((TempoEvent*)mev)->msq)/ppqn;
			tempolist->insert(mev->resulting_time,new_msec_tick);
		}
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change tempo double
//*
//**************************************************************************

UINT MIDIFile::change_tempo_double(int id , double value, bool percent)
{
	//	id >= 0:					change for one event only
	//	id = HP_ALL_EVENTS:			change for all events
	//	percent = HP_PERCENT:		change old bpm with percent-value, 150: +50%, 60: 60%
	//	percent = HP_NO_PERCENT:	new bpm gets value

	double old_bpm, new_bpm;
	int new_msec_tick;
	MIDIEvent *mev;

	if (id != HP_ALL_EVENTS)
	{
		if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;
		mev = eventlist->get(id);
		if (mev->type != HP_TEMPO)
		{
			return HP_ERR_BAD_ID;
		}

		old_bpm = ((TempoEvent*)mev)->get_bpm_double();
		new_bpm = (percent==HP_PERCENT)?(old_bpm*value/100.0):value;

		((TempoEvent*)mev)->set_tempo_double(new_bpm);

		tempolist->remove(mev->resulting_time);
		new_msec_tick = (int)(((double)((TempoEvent*)mev)->msq)/(double)ppqn);
		tempolist->insert(mev->resulting_time,new_msec_tick);
	}
	else 
	{
		int i;
		for (i=0; i<eventlist->get_length(); i++)
		{
			mev = eventlist->get(i);
			if (mev->type != HP_TEMPO)
			{
				continue;
			}
			old_bpm = ((TempoEvent*)mev)->get_bpm_double();
			new_bpm = (percent==HP_PERCENT)?(old_bpm*value/100.0):value;
			tempolist->remove(mev->resulting_time);

			((TempoEvent*)mev)->set_tempo_double(new_bpm);

			new_msec_tick = (int)(((double)((TempoEvent*)mev)->msq)/(double)ppqn);
			tempolist->insert(mev->resulting_time,new_msec_tick);
		}
	}
	return HP_ERR_NONE;
}
//**************************************************************************
//*
//*		change smpte offset
//*
//**************************************************************************

UINT MIDIFile::change_smpte_offset(int id , int hr, int mn, int se, int fr, int ff)
{
	int result;
	int read_hr, read_mn, read_se, read_fr, read_ff;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	result = read_smpte_offset(id,&read_hr,&read_mn,&read_se,&read_fr,&read_ff);

	if (result != HP_ERR_NONE) return result;

	delete_event(id);

	return insert_smpte_offset(hr,mn,se,fr,ff);
}

//**************************************************************************
//*
//*		change time signature
//*
//**************************************************************************

UINT MIDIFile::change_time_signature(int id, int new_num, int new_denum, int new_m_click, int new_n32)
{
	MIDIEvent *mev;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_TIME_SIGNATURE) return HP_ERR_BAD_ID;

	((TimeSignatureEvent*)mev)->set_num(new_num);
	((TimeSignatureEvent*)mev)->set_denum(new_denum);
	((TimeSignatureEvent*)mev)->set_metronome_click(new_m_click);
	((TimeSignatureEvent*)mev)->set_n32(new_n32);
	
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change key signature
//*
//**************************************************************************

UINT MIDIFile::change_key_signature(int id , char* key)
{
	MIDIEvent *mev;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_KEY_SIGNATURE) return HP_ERR_BAD_ID;
	
	int result;
	result = ((KeySignatureEvent*)mev)->set_key(key);
	if (result != HP_ERR_NONE) return result;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		change yamaha chord event
//*
//**************************************************************************

UINT MIDIFile::change_chord(int id, unsigned char cr, unsigned char ct, unsigned char bn, unsigned char bt)
{
	MIDIEvent *mev;

	if ((id < 0) || (id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	mev = eventlist->get(id);

	if (mev->type != HP_CHORD_NAME) return HP_ERR_BAD_ID;

	ChordNameEvent* cev = (ChordNameEvent*)mev;
	cev->set_cr_ct_bn_bt(cr,ct,bn,bt);

	int result;
	result = cev->convert_chord_to_text(cr,ct,bn,bt,&(cev->chord));
	if (result != HP_ERR_NONE) return result;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		shift event
//*
//**************************************************************************

UINT MIDIFile::shift_event(int id, int ticks)
{
	//	ticks >= 0:					shift right
	//	ticks < 0:					shift left

	if ((id < 0)||(id >= eventlist->get_length())) return HP_ERR_RANGE_EVENTLIST;

	if (ticks == 0) return HP_ERR_NONE;

	int time;
	int old_time;

	MIDIEvent * mev = eventlist->get(id);

	old_time = mev->resulting_time;
	time = old_time + ticks;

	if (time >= 0)
	{
		mev->resulting_time = time;
	}
	else
	{
		return HP_ERR_BAD_SHIFT;
	}

	if (mev->type==HP_TEMPO)
	{
		int msec_tick = ((TempoEvent*)mev)->msq/ppqn;
		tempolist->remove(old_time);
		tempolist->insert(time,msec_tick);
	}

	if (mev->type == HP_NOTE_ON)
	{
		time = (((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time + ticks;
		if (time >= 0)
		{
			(((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time = time;
		}
	}

	if ((mev->type == HP_RPN_MSB) || (mev->type == HP_NRPN_MSB))
	{
		if (((ControlChangeEvent*)mev)->assoc_fine != NULL)
		{
			time = (((ControlChangeEvent*)mev)->assoc_fine)->resulting_time + ticks;
			if (time >= 0)
			{
				(((ControlChangeEvent*)mev)->assoc_fine)->resulting_time = time;
			}
		}
		if (((ControlChangeEvent*)mev)->assoc_data_coarse != NULL)
		{
			time = (((ControlChangeEvent*)mev)->assoc_data_coarse)->resulting_time + ticks;
			if (time >= 0)
			{
				(((ControlChangeEvent*)mev)->assoc_data_coarse)->resulting_time = time;
			}
		}
		if (((ControlChangeEvent*)mev)->assoc_data_fine != NULL)
		{
			time = (((ControlChangeEvent*)mev)->assoc_data_fine)->resulting_time + ticks;
			if (time >= 0)
			{
				(((ControlChangeEvent*)mev)->assoc_data_fine)->resulting_time = time;
			}
		}
		if (((ControlChangeEvent*)mev)->assoc_data_idcrement != NULL)
		{
			time = (((ControlChangeEvent*)mev)->assoc_data_idcrement)->resulting_time + ticks;
			if (time >= 0)
			{
				(((ControlChangeEvent*)mev)->assoc_data_idcrement)->resulting_time = time;
			}
		}
	}

	eventlist->sort();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		shift selected events, overwriting not considered
//*
//**************************************************************************

UINT MIDIFile::shift_selected(int ticks)
{
	//	ticks >= 0:					shift right
	//	ticks < 0:					shift left

	int i;
	MIDIEvent * mev;

	if (ticks == 0) return HP_ERR_NONE;

	for (i=0; i<eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if (mev->selected == 0)
		{
			continue;
		}
		if ((mev->resulting_time+ticks)<0)
		{
			return HP_ERR_BAD_SHIFT;
		}
	}

	for (i=0; i<eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if (mev->selected == 0)
		{
			continue;
		}
		if ((mev->resulting_time+ticks)>=0)
		{
			mev->resulting_time += ticks;
		}
	}

	eventlist->sort();
	reorganize_tempolist(eventlist,ppqn);

	return HP_ERR_NONE;
}


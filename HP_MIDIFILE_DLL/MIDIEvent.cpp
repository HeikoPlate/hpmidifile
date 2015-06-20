/************************************************************************/
/************************************************************************/
/*                                                                      */
/* MIDIEvent.cpp: Implementation of all MIDIEvent classes               */
/*                                                                      */
/************************************************************************/
/************************************************************************/

#include "stdafx.h"
#include "HP_midifile.h"
#include "midifile.h"
#include "MIDIEvent.h"

#include "MetaEvent.h"
#include "SysExEvent.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Default Constructor
//////////////////////////////////////////////////////////////////////

MIDIEvent::MIDIEvent()
{
	type = HP_TYPE_NONE;

	resulting_time = 0;
	event_data = NULL;
	event_data_length = -1;
	chan = HP_NO_CHAN; // initial: no chan. will be filled by derived classes
	selected = 0;
}

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////

MIDIEvent::MIDIEvent(int me_resulting_time,unsigned char* me_event_data,int me_event_data_length)
{
	int i;
	resulting_time = me_resulting_time;
	event_data_length = me_event_data_length;
	event_data = new unsigned char[event_data_length];
	for (i=0; i<event_data_length; i++)
	{
		event_data[i] = me_event_data[i];
	}
	chan = HP_NO_CHAN; // initial: no chan. will be filled by derived classes
	selected = 0;
}

//////////////////////////////////////////////////////////////////////
// Operator =
//////////////////////////////////////////////////////////////////////

const MIDIEvent& MIDIEvent::operator= (const MIDIEvent& mev)
{
	int i;

	if (this == &mev)
	{
		return *this;
	}

	type = mev.type;

	resulting_time = mev.resulting_time;
	event_data_length = mev.event_data_length;
	event_data = new unsigned char[event_data_length];
	for (i=0; i<event_data_length; i++)
	{
		event_data[i] = mev.event_data[i];
	}
	
	chan = mev.chan;
	selected = mev.selected;
	
	return *this;
}	

//////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////

MIDIEvent::~MIDIEvent()
{
	delete[] event_data;
}

void MIDIEvent::change_chan(int new_chan)
{
	if (chan == HP_NO_CHAN) return;
	chan = new_chan;
	event_data[0] &= 0xf0;
	event_data[0] |= (chan & 0x0f);
}

UINT MIDIEvent::sub1type()
{
	return(type & 0xff);
}

UINT MIDIEvent::sub2type()
{
	return((type>>8) & 0xff);
}

UINT MIDIEvent::sub3type()
{
	return((type>>16) & 0xff);
}

//////////////////////////////////////////////////////////////////////
// NoteOffEvent class status 0x8n
//////////////////////////////////////////////////////////////////////

NoteOffEvent::NoteOffEvent():MIDIEvent()
{
	type = HP_NOTE_OFF;
	chan = HP_NO_CHAN;
}

NoteOffEvent::NoteOffEvent(int resulting_time,unsigned char* event_data, int event_data_length):MIDIEvent(resulting_time,event_data,event_data_length)
{
	type = HP_NOTE_OFF;
	chan = event_data[0] & 0x0f;

	// convert to note on with velocity 0
	event_data[0] = 0x90 | chan;
	event_data[2] = 0x00;
}

const NoteOffEvent& NoteOffEvent::operator= (const NoteOffEvent& noev)
{
	if (this == &noev)
	{
		return *this;
	}

	MIDIEvent::operator=(noev);

	assoc_noteon = NULL; // filled later

	return *this;
}

NoteOffEvent::~NoteOffEvent()
{

}

int NoteOffEvent::note()
{
	return event_data[1] &0x7f;
}


void NoteOffEvent::set_note(int new_note)
{
	event_data[1] = new_note &0x7f;
}

void NoteOffEvent::set_chan(int new_chan)
{
	event_data[0] = (event_data[0]&0xf0)|(new_chan&0x0f);
}


//////////////////////////////////////////////////////////////////////
// NoteOnEvent class	status = 0x9n
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

NoteOnEvent::NoteOnEvent():MIDIEvent()
{
	type = HP_NOTE_ON;
	chan = HP_NO_CHAN;
}

NoteOnEvent::NoteOnEvent(int resulting_time,unsigned char* event_data, int event_data_length):MIDIEvent(resulting_time,event_data,event_data_length)
{
	type = HP_NOTE_ON;
	chan = event_data[0] & 0x0f;
}

const NoteOnEvent& NoteOnEvent::operator= (const NoteOnEvent& noev)
{
	if (this == &noev)
	{
		return *this;
	}
	MIDIEvent::operator=(noev);
	assoc_noteoff = NULL; // filled later

	return *this;
}

NoteOnEvent::~NoteOnEvent()
{

}

int NoteOnEvent::note()
{
	return event_data[1] &0x7f;
}

void NoteOnEvent::set_note(int new_note)
{
	event_data[1] = new_note &0x7f;
}

void NoteOnEvent::set_chan(int new_chan)
{
	event_data[0] = (event_data[0]&0xf0)|(new_chan&0x0f);
}

int NoteOnEvent::vel()
{
	return event_data[2];
}

void NoteOnEvent::set_vel(int vel)
{
	event_data[2] = vel&0x7f;
}

//////////////////////////////////////////////////////////////////////
// AftertouchEvent class	status = 0xan
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

AftertouchEvent::AftertouchEvent():MIDIEvent()
{
	type = HP_AFTERTOUCH;
	chan = HP_NO_CHAN;
}

AftertouchEvent::AftertouchEvent(int resulting_time,unsigned char* event_data, int event_data_length):MIDIEvent(resulting_time,event_data,event_data_length)
{
	type = HP_AFTERTOUCH;
	chan = event_data[0] & 0x0f;
}

const AftertouchEvent& AftertouchEvent::operator= (const AftertouchEvent& noev)
{
	if (this == &noev)
	{
		return *this;
	}
	MIDIEvent::operator=(noev);

	return *this;
}

AftertouchEvent::~AftertouchEvent()
{

}

int AftertouchEvent::value()
{
	return event_data[2];
}

void AftertouchEvent::set_value(int value)
{
	event_data[2] = value & 0x7f;
}

void AftertouchEvent::set_note(int note)
{
	event_data[1] = note & 0x7f;
}

//////////////////////////////////////////////////////////////////////
// ControlChangeEvent class	status = 0xbn
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ControlChangeEvent::ControlChangeEvent():MIDIEvent()
{
	type = HP_CONTROLLER_TYPE;
	chan = HP_NO_CHAN;
	assoc_fine			 = NULL;
	assoc_data_coarse	 = NULL;
	assoc_data_fine		 = NULL;
	assoc_data_idcrement = NULL;
}

ControlChangeEvent::ControlChangeEvent(int resulting_time,unsigned char* event_data, int event_data_length):MIDIEvent(resulting_time,event_data,event_data_length)
{
	switch (event_data[1])
	{
	case 0:
		type = HP_BANK_SELECT_MSB;
		break;
	case 1:
		type = HP_MODULATION;
		break;
	case 2:
		type = HP_BREATH;
		break;
	case 5:
		type = HP_PORTAMENTO_TIME;
		break;
	case 6:
		type = HP_DATA_ENTRY_MSB;
		break;
	case 7:
		type = HP_MAIN_VOLUME;
		break;
	case 10:
		type = HP_PANPOT;
		break;
	case 11:
		type = HP_EXPRESSION;
		break;
	case 32:
		type = HP_BANK_SELECT_LSB;
		break;
	case 38:
		type = HP_DATA_ENTRY_LSB;
		break;
	case 64:
		type = HP_SUSTAIN;
		break;
	case 65:
		type = HP_PORTAMENTO;
		break;
	case 66:
		type = HP_SOSTENUTO;
		break;
	case 67:
		type = HP_SOFT_PEDAL;
		break;
	case 71:
		type = HP_HARMONIC_CONTENT;
		break;
	case 72:
		type = HP_RELEASE_TIME;
		break;
	case 73:
		type = HP_ATTACK_TIME;
		break;
	case 74:
		type = HP_BRIGHTNESS;
		break;
	case 75:
		type = HP_DECAY_TIME;
		break;
	case 76:
		type = HP_VIBRATO_RATE;
		break;
	case 77:
		type = HP_VIBRATO_DEPTH;
		break;
	case 78:
		type = HP_VIBRATO_DELAY;
		break;
	case 84:
		type = HP_PORTAMENTO_CONTROL;
		break;
	case 91:
		type = HP_REVERB_SEND_LEVEL;
		break;
	case 93:
		type = HP_CHORUS_SEND_LEVEL;
		break;
	case 94:
		type = HP_VARIATION_SEND_LEVEL;
		break;
	case 96:
		type = HP_DATA_INCREMENT;
		break;
	case 97:
		type = HP_DATA_DECREMENT;
		break;
	case 98:
		type = HP_NRPN_LSB;
		break;
	case 99:
		type = HP_NRPN_MSB;
		break;
	case 100:
		type = HP_RPN_LSB;
		break;
	case 101:
		type = HP_RPN_MSB;
		break;
	case 120:
		type = HP_ALL_SOUND_OFF;
		break;
	case 121:
		type = HP_RESET_ALL_CONTROLLERS;
		break;
	case 123:
		type = HP_ALL_NOTES_OFF;
		break;
	case 124:
		type = HP_OMNI_OFF;
		break;
	case 125:
		type = HP_OMNI_ON;
		break;
	case 126:
		type = HP_MONO;
		break;
	case 127:
		type = HP_POLY;
		break;
	default:
		type = HP_CONTROLLER_TYPE;
	}
	chan = event_data[0] & 0x0f;
	assoc_fine			 = NULL;
	assoc_data_coarse	 = NULL;
	assoc_data_fine		 = NULL;
	assoc_data_idcrement = NULL;
}

const ControlChangeEvent& ControlChangeEvent::operator= (const ControlChangeEvent& noev)
{
	if (this == &noev)
	{
		return *this;
	}
	MIDIEvent::operator=(noev);

	assoc_fine = NULL;				// filled later
	assoc_data_coarse = NULL;
	assoc_data_fine = NULL;
	assoc_data_idcrement = NULL;

	return *this;
}

ControlChangeEvent::~ControlChangeEvent()
{

}

int ControlChangeEvent::number()
{
	return event_data[1];
}

int ControlChangeEvent::value()
{
	return event_data[2];
}

void ControlChangeEvent::set_number(int new_number, int new_type)
{
	event_data[1] = new_number;
	type = new_type;
}

void ControlChangeEvent::set_value(int new_value)
{
	event_data[2] = new_value;
}

int ControlChangeEvent::rpn_nrpn_value(bool absolute)
{
	int rpn_nrpn_value = 0;
	if (absolute)
	{
		if (assoc_data_coarse != NULL) 
		{
			rpn_nrpn_value = (((ControlChangeEvent *)assoc_data_coarse)->value())*127;
		}
		if (assoc_data_fine != NULL) 
		{
			rpn_nrpn_value += ((ControlChangeEvent *)assoc_data_fine)->value();
		}
	}
	else
	{
		if (assoc_data_idcrement != NULL) 
		{
			rpn_nrpn_value = ((ControlChangeEvent *)assoc_data_idcrement)->value();
		}
	}
	return rpn_nrpn_value;
}

//////////////////////////////////////////////////////////////////////
// ProgramChangeEvent class	status = 0xcn
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ProgramChangeEvent::ProgramChangeEvent():MIDIEvent()
{
	type = HP_PROGRAM_CHANGE;
	chan = HP_NO_CHAN;
}

ProgramChangeEvent::ProgramChangeEvent(int resulting_time,unsigned char* event_data, int event_data_length):MIDIEvent(resulting_time,event_data,event_data_length)
{
	type = HP_PROGRAM_CHANGE;
	chan = event_data[0] & 0x0f;
}

const ProgramChangeEvent& ProgramChangeEvent::operator= (const ProgramChangeEvent& noev)
{
	if (this == &noev)
	{
		return *this;
	}
	MIDIEvent::operator=(noev);

	return *this;
}

ProgramChangeEvent::~ProgramChangeEvent()
{

}

int ProgramChangeEvent::number()
{
	return event_data[1];
}

void ProgramChangeEvent::set_number(int new_number)
{
	event_data[1] = new_number & 0x7f;
}

//////////////////////////////////////////////////////////////////////
// ChannelPressureEvent class	status = 0xdn
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ChannelPressureEvent::ChannelPressureEvent():MIDIEvent()
{
	type = HP_CHANNEL_PRESSURE;
	chan = HP_NO_CHAN;
}

ChannelPressureEvent::ChannelPressureEvent(int resulting_time,unsigned char* event_data, int event_data_length):MIDIEvent(resulting_time,event_data,event_data_length)
{
	type = HP_CHANNEL_PRESSURE;
	chan = event_data[0] & 0x0f;
}

const ChannelPressureEvent& ChannelPressureEvent::operator= (const ChannelPressureEvent& noev)
{
	if (this == &noev)
	{
		return *this;
	}
	MIDIEvent::operator=(noev);

	return *this;
}

ChannelPressureEvent::~ChannelPressureEvent()
{

}

int ChannelPressureEvent::pressure()
{
	return event_data[1];
}

void ChannelPressureEvent::set_pressure(int new_pressure)
{
	event_data[1] = new_pressure & 0x7f;
}

//////////////////////////////////////////////////////////////////////
// PitchWheelEvent class	status = 0xen
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

PitchWheelEvent::PitchWheelEvent():MIDIEvent()
{
	type = HP_PITCH_WHEEL;
	chan = HP_NO_CHAN;
}


PitchWheelEvent::PitchWheelEvent(int resulting_time,unsigned char* event_data, int event_data_length):MIDIEvent(resulting_time,event_data,event_data_length)
{
	type = HP_PITCH_WHEEL;
	chan = event_data[0] & 0x0f;
}


const PitchWheelEvent& PitchWheelEvent::operator= (const PitchWheelEvent& noev)
{
	if (this == &noev)
	{
		return *this;
	}
	MIDIEvent::operator=(noev);

	return *this;
}

PitchWheelEvent::~PitchWheelEvent()
{

}

int PitchWheelEvent::wheel()
{
	return (event_data[1]|(event_data[2]<<7));
}

void PitchWheelEvent::set_wheel(int new_value)
{
	event_data[1] = new_value &0x7f;
	event_data[2] = (new_value>>7) & 0x7f;
}






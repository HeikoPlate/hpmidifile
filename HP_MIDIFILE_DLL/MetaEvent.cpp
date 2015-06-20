/************************************************************************/
/************************************************************************/
/*                                                                      */
/* MetaEvent.cpp: Implementation of all MetaEvent classes               */
/*                                                                      */
/************************************************************************/
/************************************************************************/

#include "stdafx.h"
#include "HP_midifile.h"
#include "midifile.h"
#include "MetaEvent.h"

MetaEvent::MetaEvent():MIDIEvent()
{
	type = HP_OTHER_META;
	track = -1; // default
}

MetaEvent::MetaEvent(int resulting_time,unsigned char* event_data,int event_data_length):MIDIEvent(resulting_time,event_data,event_data_length)
{
	type = HP_OTHER_META;
	track = -1; // default
}

MetaEvent::~MetaEvent()
{

}

const MetaEvent& MetaEvent::operator= (const MetaEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MIDIEvent::operator=(ev);
	track = ev.track;

	return *this;
}


//////////////////////////////////////////////////////////////////////
// SequenceNumberEvent class	FF 00
//////////////////////////////////////////////////////////////////////

SequenceNumberEvent::SequenceNumberEvent():MetaEvent()
{
	type = HP_SEQUENCE_NUMBER;
}

SequenceNumberEvent::SequenceNumberEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_SEQUENCE_NUMBER;
}

SequenceNumberEvent::~SequenceNumberEvent()
{

}

const SequenceNumberEvent& SequenceNumberEvent::operator= (const SequenceNumberEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

int SequenceNumberEvent::number()
{
	int value = event_data[3]<<8;
	value |= event_data[4];
	return value;
}

void SequenceNumberEvent::set_number(int new_number)
{
	event_data[3] = (new_number>>8) & 0x7f;
	event_data[4] = new_number & 0x7f;
}

//////////////////////////////////////////////////////////////////////
// TextEvent class	FF 01
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

TextEvent::TextEvent():MetaEvent()
{
	type = HP_TEXT;
}

TextEvent::TextEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_TEXT;
}

TextEvent::~TextEvent()
{

}

const TextEvent& TextEvent::operator= (const TextEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// CopyrightEvent class	FF 02
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CopyrightEvent::CopyrightEvent():MetaEvent()
{
	type = HP_COPYRIGHT;
}

CopyrightEvent::CopyrightEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_COPYRIGHT;
}

CopyrightEvent::~CopyrightEvent()
{

}

const CopyrightEvent& CopyrightEvent::operator= (const CopyrightEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// SongSequTrackEvent class	FF 03
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SongSequTrackNameEvent::SongSequTrackNameEvent():MetaEvent()
{
	type = HP_SONG_TRACK_NAME;
}

SongSequTrackNameEvent::SongSequTrackNameEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_SONG_TRACK_NAME;
}

SongSequTrackNameEvent::~SongSequTrackNameEvent()
{

}

const SongSequTrackNameEvent& SongSequTrackNameEvent::operator= (const SongSequTrackNameEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// InstrumentNameEvent class	FF 04
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

InstrumentNameEvent::InstrumentNameEvent():MetaEvent()
{
	type = HP_INSTRUMENT;
}

InstrumentNameEvent::InstrumentNameEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_INSTRUMENT;
}

InstrumentNameEvent::~InstrumentNameEvent()
{

}

const InstrumentNameEvent& InstrumentNameEvent::operator= (const InstrumentNameEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// LyricEvent class	FF 05
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

LyricEvent::LyricEvent():MetaEvent()
{
	type = HP_LYRIC;
}

LyricEvent::LyricEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_LYRIC;
}

LyricEvent::~LyricEvent()
{

}

const LyricEvent& LyricEvent::operator= (const LyricEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// MarkerEvent class	FF 06
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

MarkerEvent::MarkerEvent():MetaEvent()
{
	type = HP_MARKER;
}

MarkerEvent::MarkerEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_MARKER;
}

MarkerEvent::~MarkerEvent()
{

}

const MarkerEvent& MarkerEvent::operator= (const MarkerEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// CuePointEvent class	FF 07
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CuePointEvent::CuePointEvent():MetaEvent()
{
	type = HP_CUE_POINT;
}

CuePointEvent::CuePointEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_CUE_POINT;
}

CuePointEvent::~CuePointEvent()
{

}

const CuePointEvent& CuePointEvent::operator= (const CuePointEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// ChannelPrefixEvent class	FF 20 
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ChannelPrefixEvent::ChannelPrefixEvent():MetaEvent()
{
	type = HP_CHANNEL_PREFIX;
}

ChannelPrefixEvent::ChannelPrefixEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_CHANNEL_PREFIX;
	chan = event_data[3] &0x7f;
}

ChannelPrefixEvent::~ChannelPrefixEvent()
{

}

const ChannelPrefixEvent& ChannelPrefixEvent::operator= (const ChannelPrefixEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// MidiPortEvent class	FF 21
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

MidiPortEvent::MidiPortEvent():MetaEvent()
{
	type = HP_MIDI_PORT;
}

MidiPortEvent::MidiPortEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_MIDI_PORT;
}

MidiPortEvent::~MidiPortEvent()
{

}

const MidiPortEvent& MidiPortEvent::operator= (const MidiPortEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

int MidiPortEvent::port()
{
	return event_data[3];
}

void MidiPortEvent::set_port(int new_port)
{
	event_data[3] = new_port & 0x7f;
}

//////////////////////////////////////////////////////////////////////
// EndMtrkEvent class	FF 2F
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

// The class EndMtrkEvent is not used in program 


EndMtrkEvent::EndMtrkEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_END_OF_TRACK;
}

EndMtrkEvent::~EndMtrkEvent()
{

}

const EndMtrkEvent& EndMtrkEvent::operator= (const EndMtrkEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// TempoEvent class	FF 51
//////////
////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

TempoEvent::TempoEvent():MetaEvent()
{
	type = HP_TEMPO;
	msq = -1;
	bpm = -1;
}

TempoEvent::TempoEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_TEMPO;
	msq = (event_data[3]*256+event_data[4])*256+event_data[5];
	bpm = 60000000/msq;
	bpm_double = 60000000.0/(double)msq;
}

TempoEvent::~TempoEvent()
{

}

const TempoEvent& TempoEvent::operator= (const TempoEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	msq = ev.msq;
	bpm = ev.bpm;
	bpm_double = ev.bpm_double;

	return *this;
}


int TempoEvent::get_bpm()
{
	return bpm;
}

double TempoEvent::get_bpm_double()
{
	return bpm_double;
}

void TempoEvent::set_tempo(int new_bpm)
{
	bpm = new_bpm;
	msq = 60000000/bpm;

	long scr_msq = msq;
	event_data[5] = scr_msq%256;
	scr_msq >>= 8;
	event_data[4] = scr_msq%256;
	scr_msq >>= 8;
	event_data[3] = scr_msq &0xff;
}

void TempoEvent::set_tempo_double(double new_bpm_double)
{
	bpm_double = new_bpm_double;
	msq = (long)(60000000.0/bpm_double);

	long scr_msq = msq;
	event_data[5] = scr_msq%256;
	scr_msq >>= 8;
	event_data[4] = scr_msq%256;
	scr_msq >>= 8;
	event_data[3] = scr_msq &0xff;
}


//////////////////////////////////////////////////////////////////////
// SmpteOffsetEvent class	FF 54
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SmpteOffsetEvent::SmpteOffsetEvent():MetaEvent()
{
	type = HP_SMPTE_OFFSET;
}

SmpteOffsetEvent::SmpteOffsetEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_SMPTE_OFFSET;
}

SmpteOffsetEvent::~SmpteOffsetEvent()
{

}

const SmpteOffsetEvent& SmpteOffsetEvent::operator= (const SmpteOffsetEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

int SmpteOffsetEvent::hr()
{
	return event_data[3];
}

int SmpteOffsetEvent::mn()
{
	return event_data[4];
}

int SmpteOffsetEvent::se()
{
	return event_data[5];
}

int SmpteOffsetEvent::fr()
{
	return event_data[6];
}

int SmpteOffsetEvent::ff()
{
	return event_data[7];
}

//////////////////////////////////////////////////////////////////////
// TimeSignatureEvent class	FF 58
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
TimeSignatureEvent::TimeSignatureEvent():MetaEvent()
{
	type = HP_TIME_SIGNATURE;
	num   = -1;
	denum = -1;
}

TimeSignatureEvent::TimeSignatureEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_TIME_SIGNATURE;

	num = event_data[3];
	if (event_data[4]==0)
	{
		denum = 1;
	}
	else
	{
		denum = 2;
		for (int i=1; i<event_data[4]; i++) // da Potenz ^ nicht funktioniert
		{
			denum *= 2;
		}
	}
}

TimeSignatureEvent::~TimeSignatureEvent()
{

}

const TimeSignatureEvent& TimeSignatureEvent::operator= (const TimeSignatureEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);
	num = ev.num;
	denum = ev.denum;

	return *this;
}

int TimeSignatureEvent::metronome_click()
{
	return event_data[5];
}

int TimeSignatureEvent::n32()
{
	return event_data[6];
}
	
void TimeSignatureEvent::set_num(int new_num)
{
	num = new_num & 0x7f;
	event_data[3] = num;
}

UINT TimeSignatureEvent::set_denum(int new_denum)
{
	// convert denum from parameter
	int intern_denum = 0; // value OK for denum = 1
	if (new_denum < 1) return HP_ERR_INVALID_DENUM;
	if (new_denum > 1)
	{
		int scr_denum = new_denum;
		do
		{
			if ((scr_denum&0x01)!= 0) return HP_ERR_INVALID_DENUM;
			scr_denum >>= 1;
			intern_denum++;
		} while(scr_denum != 1);
	}
	
	denum = new_denum;
	event_data[4] = intern_denum & 0x7f;

	return HP_ERR_NONE;
}

void TimeSignatureEvent::set_metronome_click(int new_m_click)
{
	event_data[5] = new_m_click & 0x7f;
}

void TimeSignatureEvent::set_n32(int new_n32)
{
	event_data[6] = new_n32 & 0x7f;
}

//////////////////////////////////////////////////////////////////////
// KeySignatureEvent class	FF 59
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

KeySignatureEvent::KeySignatureEvent():MetaEvent()
{
	type = HP_KEY_SIGNATURE;
	key = NULL;
}

KeySignatureEvent::KeySignatureEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	
	type = HP_KEY_SIGNATURE;
	int i;
	char scr_key[8];
	for (i=0; i<8; i++)
	{
		scr_key[i] = 0;
	}
	
	char* maj_list[30] =
	{
		"G",	// -11
		"D",
		"A",
		"E",
		"B",
		"Gb",
		"Db",
		"Ab",
		"Eb",
		"Bb",
		"F",
		"C",	// 0
		"G",
		"D",
		"A",
		"E",
		"B",
		"F#",
		"C#",
		"Ab",
		"Eb",
		"Bb",
		"F"		// 11
	};
	
	char* min_list[30] =
	{
		"E",	// -11
		"B",
		"F#",
		"Db",
		"Ab",
		"Eb",
		"Bb",
		"F",
		"C",
		"G",
		"D",
		"A",	// 0
		"E",
		"B",
		"F#",
		"C#",
		"Ab",
		"Eb",
		"Bb",
		"F",
		"C",
		"G",
		"D"		// 11
	};
	
	bool maj = (event_data[4]==0);
	int ix = (char)event_data[3];
	ix += 11;

	if ((ix<0)||(ix>22))
	{
		bool correct_syntax = false;
		throw correct_syntax;
	}

	strcpy(scr_key,maj?maj_list[ix]:min_list[ix]);
	strcat(scr_key,maj?"":"m");
	key = new char[strlen(scr_key)+1];
	strcpy(key,scr_key);
}

KeySignatureEvent::~KeySignatureEvent()
{
	if (key != NULL) delete[] key;
	key = NULL;
}

const KeySignatureEvent& KeySignatureEvent::operator= (const KeySignatureEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	if (key != NULL) delete[] key;
	key = new char[strlen(ev.key)+1];
	strcpy(key,ev.key);
	return *this;
}

unsigned char KeySignatureEvent::sf()
{
	return event_data[3];
}

unsigned char KeySignatureEvent::mi()
{
	return event_data[4];
}

UINT KeySignatureEvent::set_key(char *new_key)
{
	unsigned char sf, mi;
	UINT result;

	result = MIDIFile::convert_key_to_intern(new_key,&sf,&mi);
	if (result != HP_ERR_NONE) return result;

	event_data[3] = sf;
	event_data[4] = mi;

	if (key != NULL) delete[] key;
	key = new char[strlen(new_key)+1];

	strcpy(key,new_key);
	return HP_ERR_NONE;
}

//////////////////////////////////////////////////////////////////////
// YMetaEvent class	FF 7F
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

YMetaEvent::YMetaEvent():MetaEvent()
{
	type = HP_OTHER_YAMAHA_META;
}

YMetaEvent::YMetaEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_OTHER_YAMAHA_META;
}

YMetaEvent::~YMetaEvent()
{

}

const YMetaEvent& YMetaEvent::operator= (const YMetaEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// ScoreStartBarEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ScoreStartBarEvent::ScoreStartBarEvent()
{
	type = HP_SCORE_START_BAR;
}

ScoreStartBarEvent::ScoreStartBarEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_SCORE_START_BAR;
}

ScoreStartBarEvent::~ScoreStartBarEvent()
{

}

const ScoreStartBarEvent& ScoreStartBarEvent::operator= (const ScoreStartBarEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// QuickStartEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

QuickStartEvent::QuickStartEvent()
{
	type = HP_QUICK_START;
}

QuickStartEvent::QuickStartEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_QUICK_START;
}

QuickStartEvent::~QuickStartEvent()
{

}

const QuickStartEvent& QuickStartEvent::operator= (const QuickStartEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// ScoreLayoutEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ScoreLayoutEvent::ScoreLayoutEvent()
{
	type = HP_SCORE_LAYOUT;
}

ScoreLayoutEvent::ScoreLayoutEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_SCORE_LAYOUT;
}

ScoreLayoutEvent::~ScoreLayoutEvent()
{

}

const ScoreLayoutEvent& ScoreLayoutEvent::operator= (const ScoreLayoutEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// LyricsBitmapEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


LyricsBitmapEvent::LyricsBitmapEvent()
{
	type = HP_LYRICS_BITMAP;

}

LyricsBitmapEvent::LyricsBitmapEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type = HP_LYRICS_BITMAP;

}

LyricsBitmapEvent::~LyricsBitmapEvent()
{

}

const LyricsBitmapEvent& LyricsBitmapEvent::operator= (const LyricsBitmapEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// KeyboardVoiceEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

KeyboardVoiceEvent::KeyboardVoiceEvent()
{
	type =  HP_KEYBOARD_VOICE;

}

KeyboardVoiceEvent::KeyboardVoiceEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type =  HP_KEYBOARD_VOICE;

}

KeyboardVoiceEvent::~KeyboardVoiceEvent()
{

}

const KeyboardVoiceEvent& KeyboardVoiceEvent::operator= (const KeyboardVoiceEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// XFVersionIdEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

XFVersionIdEvent::XFVersionIdEvent()
{
	type =  HP_XF_VERSION_ID;

}

XFVersionIdEvent::XFVersionIdEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type =  HP_XF_VERSION_ID;

}

XFVersionIdEvent::~XFVersionIdEvent()
{

}

const XFVersionIdEvent& XFVersionIdEvent::operator= (const XFVersionIdEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// ChordNameEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ChordNameEvent::ChordNameEvent()
{
	type =  HP_CHORD_NAME;
	chord = NULL;

}

ChordNameEvent::ChordNameEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type =  HP_CHORD_NAME;
	unsigned char cr = event_data[6];
	unsigned char ct = event_data[7];
	unsigned char bn = event_data[8];
	unsigned char bt = event_data[9];
	convert_chord_to_text(cr,ct,bn,bt,&chord);
}

ChordNameEvent::~ChordNameEvent()
{
	if (chord != NULL) delete[] chord;
	chord = NULL;
}

const ChordNameEvent& ChordNameEvent::operator= (const ChordNameEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}

	if (chord!=NULL)
	{
		delete[] chord;
		chord = NULL;
	}
	if (ev.chord != NULL)
	{
		chord = new char[(int)strlen(ev.chord)+1];
		strcpy(chord,ev.chord);
	}

	MetaEvent::operator=(ev);

	return *this;
}

void ChordNameEvent::cr_ct_bn_bt(unsigned char* cr,unsigned char* ct,unsigned char* bn,unsigned char* bt)
{
	*cr = event_data[6];
	*ct = event_data[7];
	*bn = event_data[8];
	*bt = event_data[9];
}

void ChordNameEvent::set_cr_ct_bn_bt(unsigned char cr,unsigned char ct,unsigned char bn,unsigned char bt)
{
	event_data[6] = cr & 0x7f;
	event_data[7] = ct & 0x7f;
	event_data[8] = bn & 0x7f;
	event_data[9] = bt & 0x7f;
}

UINT ChordNameEvent::convert_chord_to_text(unsigned char cr,unsigned char ct,unsigned char bn,unsigned char bt,char** text)
{
	char chord_text[64];

	// check correct syntax
	bool correct_syntax = true;
	int flat_sharps;
	int note;
	int type;

	// cr, ct
	flat_sharps = cr>>4;
	if ((flat_sharps<0)||(flat_sharps>6))
	{
		correct_syntax = false;
	}
	note = cr&0x0f;
	if ((note<1)||(note>7))
	{
		correct_syntax = false;
	}
	type = ct;
	if ((type<0)||(type>37))
	{
		correct_syntax = false;
	}

	// bn, bt
	if (bn!=0x7f)
	{
		flat_sharps = bn>>4;
		if ((flat_sharps<0)||(flat_sharps>6))
		{
			correct_syntax = false;
		}
		note = bn&0x0f;
		if ((note<1)||(note>7))
		{
			correct_syntax = false;
		}
	}
	if (bt!=0x7f)
	{
		type = bt;
		if ((type<0)||(type>37))
		{
			correct_syntax = false;
		}
	}

	if (!correct_syntax)
	{

/*
		(*text) = new char[2];
		strcpy(*text,"?");
		return HP_ERR_NONE;
*/
		throw correct_syntax;
		
  }

	// end check correct syntax

	// chord_text Base Notes

	const int cbn_len = 16;

	const char* chord_base_note[cbn_len] =
	{
		"?",			// 30
		"C",
		"D",
		"E",
		"F",
		"G",
		"A",
		"B",			// 37
		"?",
		"?",
		"?",
		"?",
		"?",
		"?",
		"?",
		"?",
	};

	const int ct_len = 49;
	
	char* chord_type[ct_len] =
	{
		"",			// 0x00
		"6",
		"M7",
		"M7(#11)",
		"9",
		"M7(9)",
		"6(9)",
		"aug",
		"m",		// 0x08
		"m6",
		"m7",
		"m7b5",
		"m(9)",
		"m7(9)",
		"m7_11",
		"mM7",
		"mM7_9",	// 0x10
		"dim",
		"dim7",
		"7",
		"7sus4",
		"7b5",
		"7(9)",
		"7(#11)",
		"7(13)",	// 0x18
		"7(b9)",
		"7(b13)",
		"7(#9)",
		"M7aug",
		"7aug",
		"1+8",
		"1+5",
		"sus4",		// 0x20
		"sus2",
		"cc",       // or cc
		"M7b5",
		"(b5)",
		"mM7b5",
		"??",
		"??",
		"??",		// 0x28
		"??",           
		"??",
		"??",
		"??",
		"??",
		"??",
		"??",
		"??"		// 0x30
	};
	
	int base = cr-0x30;
	char baseadds[8] = "";
	
	while (base < 0)
	{
		base += 0x10;
		strcat(baseadds,"b");
	}
	while (base >= 0x10)
	{
		base -= 0x10;
		strcat(baseadds,"#");
	}
	
	if (strcmp(chord_type[ct],"cc")==0)
	{
		strcpy(chord_text, chord_type[ct]);
	}
	else
	{
		strcpy(chord_text, chord_base_note[base]);
		strcat(chord_text,baseadds);
		strcat(chord_text, chord_type[ct]);
	}

	if (bn != 0x7f)
	{
		base = bn-0x30;
	    strcpy(baseadds,"");
		while (base < 0)
		{
			base += 0x10;
			strcat(baseadds,"b");
		}
		while (base >= 0x10)
		{
			base -= 0x10;
			strcat(baseadds,"#");
		}

		strcat(chord_text,"/");
		strcat(chord_text, chord_base_note[base]);
		strcat(chord_text,baseadds);
		if (bt != 0x7f)
		{
			strcat(chord_text, chord_type[bt]);
		}
	}

	(*text) = new char[strlen(chord_text)+1];
	strcpy(*text,chord_text);

	return HP_ERR_NONE;
}

//////////////////////////////////////////////////////////////////////
// RehearsalMarkEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

RehearsalMarkEvent::RehearsalMarkEvent()
{
	type =  HP_REHEARSAL_MARK;

}

RehearsalMarkEvent::RehearsalMarkEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type =  HP_REHEARSAL_MARK;

}

RehearsalMarkEvent::~RehearsalMarkEvent()
{

}

const RehearsalMarkEvent& RehearsalMarkEvent::operator= (const RehearsalMarkEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// PhraseMarkEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

PhraseMarkEvent::PhraseMarkEvent()
{
	type =  HP_PHRASE_MARK;

}

PhraseMarkEvent::PhraseMarkEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type =  HP_PHRASE_MARK;

}

PhraseMarkEvent::~PhraseMarkEvent()
{

}

const PhraseMarkEvent& PhraseMarkEvent::operator= (const PhraseMarkEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// MaxPhraseMarkEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

MaxPhraseMarkEvent::MaxPhraseMarkEvent()
{
	type =  HP_MAX_PHRASE_MARK;

}

MaxPhraseMarkEvent::MaxPhraseMarkEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type =  HP_MAX_PHRASE_MARK;

}

MaxPhraseMarkEvent::~MaxPhraseMarkEvent()
{

}

const MaxPhraseMarkEvent& MaxPhraseMarkEvent::operator= (const MaxPhraseMarkEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// FingeringNumberEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

FingeringNumberEvent::FingeringNumberEvent()
{
	type =  HP_FINGERING_NUMBER;

}

FingeringNumberEvent::FingeringNumberEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type =  HP_FINGERING_NUMBER;

}

FingeringNumberEvent::~FingeringNumberEvent()
{

}

const FingeringNumberEvent& FingeringNumberEvent::operator= (const FingeringNumberEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// GuideTrackFlagEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

GuideTrackFlagEvent::GuideTrackFlagEvent()
{
	type =  HP_GUIDE_TRACK_FLAG;

}

GuideTrackFlagEvent::GuideTrackFlagEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type =  HP_GUIDE_TRACK_FLAG;

}

GuideTrackFlagEvent::~GuideTrackFlagEvent()
{

}

const GuideTrackFlagEvent& GuideTrackFlagEvent::operator= (const GuideTrackFlagEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// GuitarInformationFlagEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

GuitarInformationFlagEvent::GuitarInformationFlagEvent()
{
	type =  HP_GUITAR_INFORMATION_FLAG;

}

GuitarInformationFlagEvent::GuitarInformationFlagEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type =  HP_GUITAR_INFORMATION_FLAG;

}

GuitarInformationFlagEvent::~GuitarInformationFlagEvent()
{

}

const GuitarInformationFlagEvent& GuitarInformationFlagEvent::operator= (const GuitarInformationFlagEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

//////////////////////////////////////////////////////////////////////
// ChordVoicingForGuitarEvent class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

ChordVoicingForGuitarEvent::ChordVoicingForGuitarEvent()
{
	type =  HP_CHORD_VOICING_FOR_GUITAR;

}

ChordVoicingForGuitarEvent::ChordVoicingForGuitarEvent(int resulting_time,unsigned char* event_data,int event_data_length):MetaEvent(resulting_time,event_data,event_data_length)
{
	type =  HP_CHORD_VOICING_FOR_GUITAR;

}

ChordVoicingForGuitarEvent::~ChordVoicingForGuitarEvent()
{

}

const ChordVoicingForGuitarEvent& ChordVoicingForGuitarEvent::operator= (const ChordVoicingForGuitarEvent& ev)
{
	if (this == &ev)
	{
		return *this;
	}
	MetaEvent::operator=(ev);

	return *this;
}

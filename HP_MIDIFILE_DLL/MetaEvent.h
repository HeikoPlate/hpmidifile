/************************************************************************/
/************************************************************************/
/*                                                                      */
/* MetaEvent.h: Headerfile for all MetaEvent classes                    */
/*                                                                      */
/************************************************************************/
/************************************************************************/

#if !defined(AFX_METAEVENT_H__A6FEECA4_FB27_4329_B3C2_D1F2E0F52443__INCLUDED_)
#define AFX_METAEVENT_H__A6FEECA4_FB27_4329_B3C2_D1F2E0F52443__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MIDIEvent.h"

class MetaEvent : public MIDIEvent  
{
public:
	MetaEvent();
	MetaEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~MetaEvent();
	const MetaEvent& operator= (const MetaEvent& ev);
	int track;

};

class SequenceNumberEvent : public MetaEvent  
{
public:
	SequenceNumberEvent();
	SequenceNumberEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const SequenceNumberEvent& operator= (const SequenceNumberEvent& ev);
	virtual ~SequenceNumberEvent();
	int number();
	void set_number(int new_number);

};

class TextEvent : public MetaEvent  
{
public:
	TextEvent();
	TextEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const TextEvent& operator= (const TextEvent& ev);
	virtual ~TextEvent();
};

class CopyrightEvent : public MetaEvent  
{
public:
	CopyrightEvent();
	CopyrightEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const CopyrightEvent& operator= (const CopyrightEvent& ev);
	virtual ~CopyrightEvent();
};

class SongSequTrackNameEvent : public MetaEvent  
{
public:
	SongSequTrackNameEvent();
	SongSequTrackNameEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const SongSequTrackNameEvent& operator= (const SongSequTrackNameEvent& ev);
	virtual ~SongSequTrackNameEvent();
};

class InstrumentNameEvent : public MetaEvent  
{
public:
	InstrumentNameEvent();
	InstrumentNameEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const InstrumentNameEvent& operator= (const InstrumentNameEvent& ev);
	virtual ~InstrumentNameEvent();
};

class LyricEvent : public MetaEvent  
{
public:
	LyricEvent();
	LyricEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const LyricEvent& operator= (const LyricEvent& ev);
	virtual ~LyricEvent();
};

class MarkerEvent : public MetaEvent  
{
public:
	MarkerEvent();
	MarkerEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const MarkerEvent& operator= (const MarkerEvent& ev);
	virtual ~MarkerEvent();
};

class CuePointEvent : public MetaEvent  
{
public:
	CuePointEvent();
	CuePointEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const CuePointEvent& operator= (const CuePointEvent& ev);
	virtual ~CuePointEvent();
};

class ChannelPrefixEvent : public MetaEvent  
{
public:
	ChannelPrefixEvent();
	ChannelPrefixEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const ChannelPrefixEvent& operator= (const ChannelPrefixEvent& ev);
	virtual ~ChannelPrefixEvent();
};

class MidiPortEvent : public MetaEvent  
{
public:
	MidiPortEvent();
	MidiPortEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const MidiPortEvent& operator= (const MidiPortEvent& ev);
	virtual ~MidiPortEvent();
	int port();
	void set_port(int new_port);
};

class EndMtrkEvent : public MetaEvent  // not needed
{
public:
	EndMtrkEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~EndMtrkEvent();
	const EndMtrkEvent& operator= (const EndMtrkEvent& ev);
};

class TempoEvent : public MetaEvent  
{
public:
	TempoEvent();
	TempoEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~TempoEvent();
	const TempoEvent& operator= (const TempoEvent& ev);
	void set_tempo(int new_bpm);
	void set_tempo_double(double new_bpm_double);
	int get_bpm();
	double get_bpm_double();
	long msq; // milliseconds per quarter
	int bpm;  // beats per minute
	double bpm_double; // beats per minute (double)
};

class SmpteOffsetEvent : public MetaEvent  
{
public:
	SmpteOffsetEvent();
	SmpteOffsetEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~SmpteOffsetEvent();
	const SmpteOffsetEvent& operator= (const SmpteOffsetEvent& ev);

	int hr();
	int mn();
	int se();
	int fr();
	int ff();
};

class TimeSignatureEvent : public MetaEvent  
{
public:
	TimeSignatureEvent();
	TimeSignatureEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~TimeSignatureEvent();
	const TimeSignatureEvent& operator= (const TimeSignatureEvent& ev);

	int num;
	int denum;

	int metronome_click();
	int n32();
	
	void set_num(int new_num);
	UINT set_denum(int new_denum);
	void set_metronome_click(int new_m_click);
	void set_n32(int new_n32);
};

class KeySignatureEvent : public MetaEvent  
{
public:
	KeySignatureEvent();
	KeySignatureEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~KeySignatureEvent();
	const KeySignatureEvent& operator= (const KeySignatureEvent& ev);
	char *key;
	unsigned char sf();
	unsigned char mi();

	UINT set_key(char *new_key);
};

class YMetaEvent : public MetaEvent  
{
public:
	YMetaEvent();
	YMetaEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~YMetaEvent();
	const YMetaEvent& operator= (const YMetaEvent& ev);
};

class ScoreStartBarEvent : public MetaEvent  
{
public:
	ScoreStartBarEvent();
	ScoreStartBarEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~ScoreStartBarEvent();
	const ScoreStartBarEvent& operator= (const ScoreStartBarEvent& ev);
};

class QuickStartEvent : public MetaEvent  
{
public:
	QuickStartEvent();
	QuickStartEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~QuickStartEvent();
	const QuickStartEvent& operator= (const QuickStartEvent& ev);
};

class ScoreLayoutEvent : public MetaEvent  
{
public:
	ScoreLayoutEvent();
	ScoreLayoutEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~ScoreLayoutEvent();
	const ScoreLayoutEvent& operator= (const ScoreLayoutEvent& ev);
};

class LyricsBitmapEvent : public MetaEvent  
{
public:
	LyricsBitmapEvent();
	LyricsBitmapEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~LyricsBitmapEvent();
	const LyricsBitmapEvent& operator= (const LyricsBitmapEvent& ev);
};

class KeyboardVoiceEvent : public MetaEvent  
{
public:
	KeyboardVoiceEvent();
	KeyboardVoiceEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~KeyboardVoiceEvent();
	const KeyboardVoiceEvent& operator= (const KeyboardVoiceEvent& ev);
};

class XFVersionIdEvent : public MetaEvent   
{
public:
	XFVersionIdEvent();
	XFVersionIdEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~XFVersionIdEvent();
	const XFVersionIdEvent& operator= (const XFVersionIdEvent& ev);
};

class ChordNameEvent : public MetaEvent   
{
public:
	ChordNameEvent();
	ChordNameEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~ChordNameEvent();
	const ChordNameEvent& operator= (const ChordNameEvent& ev);
	char *chord;
	void cr_ct_bn_bt(unsigned char* cr,unsigned char* ct,unsigned char* bn,unsigned char* bt);
	void set_cr_ct_bn_bt(unsigned char cr,unsigned char ct,unsigned char bn,unsigned char bt);
	static UINT convert_chord_to_text(unsigned char cr,unsigned char ct,unsigned char bn,unsigned char bt,char** text);
};

class RehearsalMarkEvent : public MetaEvent  
{
public:
	RehearsalMarkEvent();
	RehearsalMarkEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~RehearsalMarkEvent();
	const RehearsalMarkEvent& operator= (const RehearsalMarkEvent& ev);
};

class PhraseMarkEvent : public MetaEvent   
{
public:
	PhraseMarkEvent();
	PhraseMarkEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~PhraseMarkEvent();
	const PhraseMarkEvent& operator= (const PhraseMarkEvent& ev);
};

class MaxPhraseMarkEvent : public MetaEvent  
{
public:
	MaxPhraseMarkEvent();
	MaxPhraseMarkEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~MaxPhraseMarkEvent();
	const MaxPhraseMarkEvent& operator= (const MaxPhraseMarkEvent& ev);
};

class FingeringNumberEvent : public MetaEvent  
{
public:
	FingeringNumberEvent();
	FingeringNumberEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~FingeringNumberEvent();
	const FingeringNumberEvent& operator= (const FingeringNumberEvent& ev);
};

class GuideTrackFlagEvent : public MetaEvent  
{
public:
	GuideTrackFlagEvent();
	GuideTrackFlagEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~GuideTrackFlagEvent();
	const GuideTrackFlagEvent& operator= (const GuideTrackFlagEvent& ev);
};

class GuitarInformationFlagEvent : public MetaEvent  
{
public:
	GuitarInformationFlagEvent();
	GuitarInformationFlagEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~GuitarInformationFlagEvent();
	const GuitarInformationFlagEvent& operator= (const GuitarInformationFlagEvent& ev);
};

class ChordVoicingForGuitarEvent : public MetaEvent  
{
public:
	ChordVoicingForGuitarEvent();
	ChordVoicingForGuitarEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	virtual ~ChordVoicingForGuitarEvent();
	const ChordVoicingForGuitarEvent& operator= (const ChordVoicingForGuitarEvent& ev);
};

#endif // !defined(AFX_METAEVENT_H__A6FEECA4_FB27_4329_B3C2_D1F2E0F52443__INCLUDED_)

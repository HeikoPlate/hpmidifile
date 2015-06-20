/************************************************************************/
/************************************************************************/
/*                                                                      */
/* MIDIEvent.h: Headerfile for all Standard MIDIEvent classes           */
/*                                                                      */
/************************************************************************/
/************************************************************************/

#if !defined(AFX_MIDIEVENT_H__0A9ADF85_F9A1_4E37_8CB5_A1BFB889805A__INCLUDED_)
#define AFX_MIDIEVENT_H__0A9ADF85_F9A1_4E37_8CB5_A1BFB889805A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MIDIEvent  
{
public:
	MIDIEvent();
	MIDIEvent(int me_resulting_time,unsigned char* me_event_data,int me_event_data_length);
	const MIDIEvent& operator= (const MIDIEvent& mev);
	virtual ~MIDIEvent();

	void change_chan(int chan);

	UINT sub1type();
	UINT sub2type();
	UINT sub3type();

	UINT type;
	int tempo;

	int resulting_time;
	unsigned char* event_data;
	int event_data_length;
	int chan;
	int selected;
};


class NoteOnEvent : public MIDIEvent  
{
	// attributes
public:
	NoteOnEvent();	// Default Constructor
	NoteOnEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const NoteOnEvent& operator= (const NoteOnEvent& noev); // Assignment Operator
	virtual ~NoteOnEvent();
	int note();	// note value
	void set_note(int new_note);
	void set_chan(int new_chan);
	int vel();
	void set_vel(int vel);

	MIDIEvent* assoc_noteoff;
};

class NoteOffEvent : public MIDIEvent  
{
	// attributes
public:

	NoteOffEvent();	// Default Constructor
	NoteOffEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const NoteOffEvent& operator= (const NoteOffEvent& noev); // Assignment Operator
	virtual ~NoteOffEvent();
	int note();	// note value
	void set_note(int new_note);
	void set_chan(int new_chan);

	MIDIEvent* assoc_noteon;
};

class AftertouchEvent : public MIDIEvent  
{
public:
	AftertouchEvent();	// Default Constructor
	AftertouchEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const AftertouchEvent& operator= (const AftertouchEvent& noev); // Assignment Operator
	virtual ~AftertouchEvent();
	int value();
	void set_value(int value);
	void set_note(int note);
};

class ControlChangeEvent : public MIDIEvent  
{
public:
	ControlChangeEvent();	// Default Constructor
	ControlChangeEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const ControlChangeEvent& operator= (const ControlChangeEvent& noev); // Assignment Operator
	virtual ~ControlChangeEvent();
	void set_number(int new_number, int new_type);
	void set_value(int new_value);
	int rpn_nrpn_value(bool absolute);

	int number();
	int value();
	
	// only used with types HP_RPN_MSB and HP_NRPN_MSB:
	ControlChangeEvent * assoc_fine;
	ControlChangeEvent * assoc_data_coarse;
	ControlChangeEvent * assoc_data_fine;
	ControlChangeEvent * assoc_data_idcrement;


};

class ProgramChangeEvent : public MIDIEvent  
{
public:
	ProgramChangeEvent();	// Default Constructor
	ProgramChangeEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const ProgramChangeEvent& operator= (const ProgramChangeEvent& noev); // Assignment Operator
	virtual ~ProgramChangeEvent();
	int number();
	void set_number(int new_number);
};

class ChannelPressureEvent : public MIDIEvent  
{
public:
	ChannelPressureEvent();	// Default Constructor
	ChannelPressureEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const ChannelPressureEvent& operator= (const ChannelPressureEvent& noev); // Assignment Operator
	virtual ~ChannelPressureEvent();
	int pressure();
	void set_pressure(int new_pressure);
};

class PitchWheelEvent : public MIDIEvent  
{
public:
	PitchWheelEvent();	// Default Constructor
	PitchWheelEvent(int resulting_time,unsigned char* event_data,int event_data_length);
	const PitchWheelEvent& operator= (const PitchWheelEvent& noev); // Assignment Operator
	virtual ~PitchWheelEvent();
	int wheel();
	void set_wheel(int new_value);
};

#endif // !defined(AFX_MIDIEVENT_H__0A9ADF85_F9A1_4E37_8CB5_A1BFB889805A__INCLUDED_)

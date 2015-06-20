/************************************************************************/
/************************************************************************/
/*                                                                      */
/* HP_Array.h: Headerfile for Class HP_Array                            */
/*                                                                      */
/************************************************************************/
/************************************************************************/


#if !defined(AFX_HP_ARRAY_H__CEC4F535_3B8D_43D3_897A_77C5589FF431__INCLUDED_)
#define AFX_HP_ARRAY_H__CEC4F535_3B8D_43D3_897A_77C5589FF431__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HP_midifile.h"
#include "MIDIEvent.h"
#include "mmsystem.h"

// Types of PlayEvents

#define MIDIEVENT 0
#define TAKTBEAT  1
#define CHORD     2
#define LYRIC     3

class Mev_Array  
{
private:

	MIDIEvent* *elems;		// dynamic array of elements of type MIDIEvent*
	int size;		// actual length
	int numElems;	// actual number of elements

public:
	Mev_Array();					// default constructor
	virtual ~Mev_Array();			// destructor
	Mev_Array(const Mev_Array & t);	// copy constructor
	clear();						// get empty 
	MIDIEvent* get(int index);
	MIDIEvent* get_last();			// get last element
	void add(MIDIEvent* elem);				// add at the end
	void insert(MIDIEvent* elem,int index);	// insert at index
	int insert(MIDIEvent* elem, bool first = false);	// insert by resulting_time
	void delete_from_end();					// delete from end, also MIDIEvent
	
	void remove(int index);			// delete at index
	int no_bytes();					// compute no of bytes (var length delta-times plus data_length
	int get_length();				// returns numElems;
	void sort();					// sort MIDIEvents by resulting_time
	void check();
};

class Uch_Array  
{
private:

	unsigned char *elems;		// dynamic array of elements of type MIDIEvent*
	int size;		// actual length
	int numElems;	// actual number of elements

public:
	Uch_Array();					// default constructor
	virtual ~Uch_Array();			// destructor
	int get_length();				// no of bytes
	void clear();					// delete al elements
	Uch_Array(const Uch_Array & t);	// copy constructor
	unsigned char operator[] (int index);		// operator []
	void add(unsigned char elem);				// add at the end
	void insert(unsigned char elem,int index);	// insert at index
	unsigned char pop();						// get and delete from end
	void remove(int index);			// delete at index
};

class PlayEvent
{
public:
	int type;
	int takt;
	int beat;
	int event_time;         // time in ticks
	int millisec_delta_time;// millisec, actual, to be played 
	unsigned char* data;	// event_data to be played
	int length;				// no of bytes of event-data
};

class Play_Array  
{
private:

	PlayEvent *elems;	// dynamic array of play time (msec) and event_data
	int size;		// actual length
	int numElems;	// actual number of elements

public:
	Play_Array();					// default constructor
	virtual ~Play_Array();			// destructor
	Play_Array(const Play_Array & t);	// copy constructor
	int get_length();				// get num_elements 

	PlayEvent* get(int index);
	int Play_Array::event_time(int index); // get event_time (ticks)
	int delta_time_millisec(int index);	// get delta_time (millisec)
	int type(int index); // get type of element
	int takt(int index); // get takt number
	int beat(int index); // get beat number

	unsigned char* data(int index,
		                int *length);	// read event-data
	void add(int type, int takt, int beat, int event_time, int millisec_delta_time,
	  unsigned char* data, int length);	// add at the end
	void add(PlayEvent* pev);
	//	void test_print();
};

class TempoElement
{
public:
	int time;				// time-stamp of tempo-event 
	int msec_tick;			// mikroseconds per tick
};

class Tempo_Array  
{
private:

	TempoElement *elems;	// dynamic array of play time (msec) and event_data
	int size;				// actual length
	int numElems;			// actual number of elements
	int time(int index);	// get time
	int msec_tick(int index); // get msec_tick

public:
	Tempo_Array();			// default constructor
	virtual ~Tempo_Array();	// destructor
	Tempo_Array(const Tempo_Array & t);	// copy constructor
	int get_length();		// get num_elements 
	int millisec_delta_time(int time1, int time2);
	void sort();			// sort by time
	void add(int time, int msec_tick);		// add Tempo-Element at the end
	void insert(int time, int msec_tick);	// insert Tempo-Element at time-prosition
	void remove(int time);	// remove all Tempo-Elements of given time
	void check();			// check time order

	int ppqn;

//	void test_print();
};

class CWBankVoice_Array  
{
private:

	HP_CWBANKVOICE* *elems;	// dynamic array of elements of type HP_CWBANKVOICE
	int size;				// actual length
	int numElems;			// actual number of elements

public:
	CWBankVoice_Array();					// default constructor
	virtual ~CWBankVoice_Array();			// destructor
	int get_length();						// no of elements
	HP_CWBANKVOICE *operator[] (int index);	// operator []
	void add(HP_CWBANKVOICE *elem);			// add at the end
	void sort();							// sort elements by voice-name
};

class CWDrum_Array  
{
private:

	HP_CWDRUM* *elems;	// dynamic array of elements of type HP_CWBANKVOICE
	int size;				// actual length
	int numElems;			// actual number of elements

public:
	CWDrum_Array();					// default constructor
	virtual ~CWDrum_Array();			// destructor
	int get_length();						// no of elements
	HP_CWDRUM *operator[] (int index);	// operator []
	void add(HP_CWDRUM *elem);			// add at the end
};

#endif // !defined(AFX_HP_ARRAY_H__CEC4F535_3B8D_43D3_897A_77C5589FF431__INCLUDED_)

/************************************************************************/
/************************************************************************/
/*                                                                      */
/* HP_Array.cpp: Implementation of the Classes Mev_Array, Uch_Array,    */
/*               Play_Array and Tempo_Array                        ,    */
/*                                                                      */
/************************************************************************/
/************************************************************************/

#include "stdafx.h"
#include "HP_Array.h"
#include "HP_midifile.h"
#include "midifile.h"

/************************************************************************/
/*                                                                      */
/*  Mev_Array                                                           */
/*                                                                      */
/************************************************************************/

Mev_Array::Mev_Array()
{
	numElems = 0;	// empty
	size = 200;
	elems = new MIDIEvent* [size];
}

Mev_Array::~Mev_Array()
{
	delete [] elems;	// free storage
}

Mev_Array::clear()
{
	numElems = 0;	// empty
}

Mev_Array::Mev_Array(const Mev_Array& t)	// copy constructor
{
	int i;
	numElems = t.numElems;
	size =  numElems;
	elems = new MIDIEvent* [size];

	// copy elems
	for (i = 0; i < numElems; i++)
	{
		elems[i] = t.elems[i];
	}
}

MIDIEvent* Mev_Array::get(int index)
{
	if (index >= numElems)
	{
		MIDIFile::message("Mev_Array: Index out of range");
	}
	return (elems[index]);
}

MIDIEvent* Mev_Array::get_last()
{
	if (numElems==0)
	{
		MIDIFile::message("Mev_Array: array empty");
	}
	return (elems[numElems-1]);
}

void Mev_Array::add(MIDIEvent* elem)				// add at the end
{
	int i;
	if (size <= numElems)
	{
		MIDIEvent** tmp = elems; // hold old elems
		size *= 2;		// double size
		elems = new MIDIEvent* [size];
		for (i=0; i<numElems; i++)
		{
			elems[i] = tmp[i];
		}
		delete [] tmp;
	}
	elems[numElems++] = elem;
}

void Mev_Array::insert(MIDIEvent* elem,int index)	// insert at index
{
	int i;
	if (index >= numElems)
	{
		MIDIFile::message("Mev_Array: Index out of range");
	}
	
	MIDIEvent** tmp = elems;
	size++;
	numElems++;

	elems = new MIDIEvent* [size];
	for (i=0; i<index; i++)
	{
		elems[i] = tmp[i];
	}
	elems[index] = elem;
	for (i=index; i<numElems; i++)
	{
		elems[i+1] = tmp[i];
	}
	delete [] tmp;
}

int Mev_Array::insert(MIDIEvent* elem, bool first)	// insert by resulting_time
{
	int i;

	int elem_time = elem->resulting_time;

	int last_time = numElems==0?0:elems[numElems-1]->resulting_time;
	
	if (last_time <= elem_time)
	{
		add(elem);
		return numElems-1;
	}

	int act_time;

	for (i=numElems-2; i>=0; i--)
	{
		act_time = elems[i]->resulting_time; 

		if (first)
		{
			if ((elem_time <= last_time)&&(elem_time > act_time))
			{
				insert(elem,i+1);
				return (i+1);
			}
		}
		else
		{
			if ((elem_time <= last_time)&&(elem_time >= act_time))
			{
				insert(elem,i+1);
				return (i+1);
			}
		}

		if (i==0)
		{
			insert(elem,0);
			return 0;
		}
		last_time = act_time;
	}
	return numElems;
}

void Mev_Array::delete_from_end()				// delete from end, also Events
{
	if (numElems == 0)
	{
		MIDIFile::message("Mev_Array: Pop from empty array");
	}

	numElems--;
	MIDIEvent *mev = elems[numElems];
	delete mev;
}

void Mev_Array::remove(int index)	// delete at index, 
{
	int i;
	if (index >= numElems)
	{
		MIDIFile::message("Index out of range");
	}
	
	MIDIEvent** tmp = elems;
	
	elems = new MIDIEvent* [size];
	for (i=0; i<index; i++)
	{
		elems[i] = tmp[i];
	}

	for (i=index+1; i<numElems; i++)
	{
		elems[i-1] = tmp[i];
	}

	delete [] tmp;
	numElems--;
}

int Mev_Array::get_length()
{
	return numElems;
}

int Mev_Array::no_bytes()	// compute no of bytes (var length delta-times plus data_length
{
	int i;
	int length = 0;
	int no_bytes_var_length;
	int delta_time;
	int last_resulting_time = 0;

	for (i=0; i<numElems; i++)
	{
		delta_time = elems[i]->resulting_time - last_resulting_time;

		no_bytes_var_length = 1;
		while ( (delta_time >>=7))
		{
			no_bytes_var_length ++;
		}
		last_resulting_time = elems[i]->resulting_time;
		length += no_bytes_var_length + elems[i]->event_data_length;
	}
	return length;
};

void Mev_Array::sort()
{
	int i,j;
	for (i=1; i<numElems; i++)
	{
		MIDIEvent* m = elems[i];
		j = i-1;
		while ((j>=0) && (elems[j]->resulting_time > m->resulting_time))
		{
			elems[j+1] = elems[j];
			j--;
		}
		elems[j+1] = m;
	}
}

void Mev_Array::check()
{
	int i;
	int last_time = 0;
	for (i=0; i<numElems; i++)
	{
		if (elems[i]->resulting_time < last_time)
		{
			MIDIFile::message("Mev_Array: Array not sorted");
			return;
		}
		last_time = elems[i]->resulting_time;
	}
}

/************************************************************************/
/*                                                                      */
/*  Uch_Array                                                           */
/*                                                                      */
/************************************************************************/

Uch_Array::Uch_Array()
{
	numElems = 0;	// empty
	size = 200;
	elems = new unsigned char [size];
}

Uch_Array::~Uch_Array()
{
	delete [] elems;	// free storage
}

int Uch_Array::get_length()
{
	return numElems;
}

void Uch_Array::clear()
{
	delete [] elems;	// free storage
	numElems = 0;	// empty
	size = 200;
	elems = new unsigned char [size];
}

Uch_Array::Uch_Array(const Uch_Array& t)	// copy constructor
{
	int i;
	numElems = t.numElems;
	size =  numElems;
	elems = new unsigned char [size];

	// copy elems
	for (i = 0; i < numElems; i++)
	{
		elems[i] = t.elems[i];
	}
}

unsigned char Uch_Array::operator[] (int index)		// operator []
{
	if (index >= numElems)
	{
		MIDIFile::message(NULL,"Uch_Array: Index out of range");
		return 0x00;
	}
	return (elems[index]);
}

void Uch_Array::add(unsigned char elem)				// add at the end
{
	int i;
	if (size <= numElems)
	{
		unsigned char* tmp = elems; // hold old elems
		size *= 2;		// double size
		elems = new unsigned char [size];
		for (i=0; i<numElems; i++)
		{
			elems[i] = tmp[i];
		}
		delete [] tmp;
	}
	elems[numElems++] = elem;
}

void Uch_Array::insert(unsigned char elem,int index)	// insert at index
{
	int i;
	if (index >= numElems)
	{
		MIDIFile::message("Uch_Array: Index out of range");
		return;
	}
	
	unsigned char* tmp = elems;
	size++;
	numElems++;

	elems = new unsigned char [size];
	for (i=0; i<index; i++)
	{
		elems[i] = tmp[i];
	}
	elems[index] = elem;
	for (i=index; i<size; i++)
	{
		elems[i+1] = tmp[i];
	}
	delete [] tmp;
}

unsigned char Uch_Array::pop()				// get and delete from end
{
	if (numElems == 0)
	{
		MIDIFile::message("Uch_Array: Pop from empty array");
		return 0x00;
	}
	return elems[--numElems];
}

void Uch_Array::remove(int index)	// delete at index
{
	int i;
	if (index >= numElems)
	{
		MIDIFile::message("Uch_Array: Index out of range");
		return;
	}
	
	unsigned char* tmp = elems;
	
	elems = new unsigned char [size];
	for (i=0; i<index; i++)
	{
		elems[i] = tmp[i];
	}

	for (i=index+1; i<numElems; i++)
	{
		elems[i-1] = tmp[i];
	}
	delete [] tmp;
	numElems--;
}

/************************************************************************/
/*                                                                      */
/*  Play_Array                                                          */
/*                                                                      */
/************************************************************************/

Play_Array::Play_Array()
{
	numElems = 0;	// empty
	size = 200;
	elems = new PlayEvent[size];
}

Play_Array::~Play_Array()
{
	int i;
	for (i=0; i < numElems; i++)
	{
		delete[] elems[i].data;
	}
	delete [] elems;	// free storage
}

int Play_Array::get_length()
{
	return numElems;
}

Play_Array::Play_Array(const Play_Array& t)	// copy constructor
{
	int i,k;
	numElems = t.numElems;
	size =  numElems;
	elems = new PlayEvent[size];

	// copy elems
	for (i = 0; i < numElems; i++)
	{
		elems[i].event_time = t.elems[i].event_time;
		elems[i].millisec_delta_time = t.elems[i].millisec_delta_time;

		elems[i].data = new unsigned char [t.elems[i].length]; 
		for (k=0; k<elems[i].length; i++)
		{
			elems[i].data[k] = t.elems[i].data[k];
		}

		elems[i].length = t.elems[i].length;
	}
}

void Play_Array::add(int type, int takt, int beat, int event_time, int millisec_delta_time, unsigned char* data, int length) // add at the end
{
	int i;
	if (size <= numElems)
	{
		PlayEvent* tmp = elems; // hold old elems
		size *= 2;		// double size
		elems = new PlayEvent [size];
		for (i=0; i<numElems; i++)
		{
			elems[i] = tmp[i];
		}
		delete [] tmp;
	}
	elems[numElems].type = type;
	elems[numElems].takt = takt;
	elems[numElems].beat = beat;
	elems[numElems].event_time = event_time;
	elems[numElems].millisec_delta_time = millisec_delta_time;
	elems[numElems].data = new unsigned char[length];
	for (i=0; i < length; i++)
	{
		elems[numElems].data[i] = data[i];
	}
	elems[numElems].length = length;

	numElems++;
}

PlayEvent* Play_Array::get(int index)
{
	return &elems[index];
}

void Play_Array::add(PlayEvent* pev)
{
	int i;
	if (size <= numElems)
	{
		PlayEvent* tmp = elems; // hold old elems
		size *= 2;		// double size
		elems = new PlayEvent [size];
		for (i=0; i<numElems; i++)
		{
			elems[i] = tmp[i];
		}
		delete [] tmp;
	}
	elems[numElems].type = pev->type;
	elems[numElems].takt = pev->takt;
	elems[numElems].beat = pev->beat;
	elems[numElems].event_time = pev->event_time;
	elems[numElems].millisec_delta_time = pev->millisec_delta_time;
	elems[numElems].data = new unsigned char[pev->length];
	for (i=0; i < pev->length; i++)
	{
		elems[numElems].data[i] = pev->data[i];
	}
	elems[numElems].length = pev->length;

	numElems++;
}


int Play_Array::delta_time_millisec(int index) // get delta_time (millisec)
{
	if ((index < 0) || (index >= numElems)) return -1;
	return elems[index].millisec_delta_time;
}

int Play_Array::event_time(int index) // get event_time (ticks)
{
	if ((index < 0) || (index >= numElems)) return -1;
	return elems[index].event_time;
}

int Play_Array::type(int index) // get takt number
{
	if ((index < 0) || (index >= numElems)) return -1;
	return elems[index].type;
}

int Play_Array::takt(int index) // get takt number
{
	if ((index < 0) || (index >= numElems)) return -1;
	return elems[index].takt;
}

int Play_Array::beat(int index) // get beat number
{
	if ((index < 0) || (index >= numElems)) return -1;
	return elems[index].beat;
}

unsigned char* Play_Array::data(int index,
		                        int *length)	// read event-data
{
	if ((index < 0) || (index >= numElems)) return NULL;
	*length = elems[index].length;
	return elems[index].data;
}

/*
void Play_Array::test_print()
{
	int i;

	char r[80];
	for (i=0; i<numElems; i++)
	{
		sprintf(r,"Play_Array[%i]: event_time %i, millisec_delta_time %i",
			         i, elems[i].event_time, elems[i].millisec_delta_time);
		MessageBox(NULL,r,NULL,MB_OK);
	}
}
*/


/************************************************************************/
/*                                                                      */
/*  Tempo_Array                                                         */
/*                                                                      */
/************************************************************************/


Tempo_Array::Tempo_Array()
{
	numElems = 0;	// empty
	size = 200;
	elems = new TempoElement[size];
}

Tempo_Array::~Tempo_Array()
{
	delete [] elems;	// free storage
}

Tempo_Array::Tempo_Array(const Tempo_Array& t)	// copy constructor
{
	int i;
	numElems = t.numElems;
	size =  numElems;
	elems = new TempoElement[size];

	// copy elems
	for (i = 0; i < numElems; i++)
	{
		elems[i].time = t.elems[i].time;
		elems[i].msec_tick = t.elems[i].msec_tick;
	}
}

int Tempo_Array::get_length()
{
	return numElems;
}

int Tempo_Array::time(int index) // get time (ticks)
{
	if (index == -1) return 0;
	return elems[index].time;
}

int Tempo_Array::msec_tick(int index) // get msec_ticks
{
	if (index == -1) return 600000/ppqn; // Default BPM 100
	return elems[index].msec_tick;
}

int Tempo_Array::millisec_delta_time(int time1, int time2)
{
	int i;
	if (time2 <= time1) return 0;  // maybe an error if <
	
	int index1 = -1; // index in tempolist with the tempo at time1
	int index2 = -1; // index in tempolist with the tempo at time2

	long msecs = 0;

	for (i=numElems-1; i>=0; i--)
	{
		if (time(i)<= time1)
		{
			index1 = i;
			break;
		}
	}

	for (i=numElems-1; i>=0; i--)
	{
		if (time(i)<= time2)
		{
			index2 = i;
			break;
		}
	}

	double accumulated_msecs = 0;
	int scr_time = time1;
	
	for (i = index1; i < index2; i++)
	{
		accumulated_msecs += (double)(msec_tick(i))*(double)(time(i+1)-scr_time);
		scr_time = time(i+1);
	}

	accumulated_msecs += (double)(msec_tick(index2))*(double)(time2 - scr_time);

	return (int)(accumulated_msecs/1000.+0.5);
}


void Tempo_Array::sort()	// sort array by time
{
	int i,j;

	for (i=1; i<numElems; i++)
	{
		int m_time = elems[i].time;
		int m_msec_tick = elems[i].msec_tick;
		j = i-1;
		while ((j>=0) && (elems[j].time > m_time))
		{
			elems[j+1].time = elems[j].time;
			elems[j+1].msec_tick = elems[j].msec_tick;
			j--;
		}
		elems[j+1].time = m_time;
		elems[j+1].msec_tick = m_msec_tick;
	}
}

void Tempo_Array::add(int time, int msec_tick)	// add Tempo-Element at the end
{
	int i;
	if (size <= numElems)
	{
		TempoElement* tmp = elems; // hold old elems
		size *= 2;		// double size
		elems = new TempoElement [size];
		for (i=0; i<numElems; i++)
		{
			elems[i] = tmp[i];
		}
		delete [] tmp;
	}
	elems[numElems].time = time;
	elems[numElems].msec_tick = msec_tick;

	numElems++;
}

void Tempo_Array::insert(int time, int msec_tick)	// insert Tempo-Element at time-prosition
{
	add(time,msec_tick);
	sort();
}

void Tempo_Array::remove(int time)   // remove all Tempo-Elements of given time
{
	int i, k;
	for (i=0; i < numElems; i++)
	{
		if (elems[i].time==time)
		{ // this element to be deleted
			for (k = i+1; k < numElems; k++)
			{
				elems[k-1].time = elems[k].time;
				elems[k-1].msec_tick = elems[k].msec_tick;
			}
			numElems--;
		}
	}
}

void Tempo_Array::check()
{
	int i;
	int last_time = 0;
	for (i=0; i<numElems; i++)
	{
		if (elems[i].time < last_time)
		{
			MIDIFile::message("Tempo_Array: Array not sorted");
			return;
		}
		last_time = elems[i].time;
	}
}

/*
void Tempo_Array::test_print()
{
	int i;

	char r[80];
	for (i=0; i<numElems; i++)
	{
		sprintf(r,"Tempo_Array[%i]: time %i, msec_tick %i",
			         i, elems[i].time, elems[i].msec_tick);
		MessageBox(NULL,r,NULL,MB_OK);
	}
}
*/

/************************************************************************/
/*                                                                      */
/*  CWBankVoice_Array                                                   */
/*                                                                      */
/************************************************************************/

/*
class CWBankVoice_Array  
{
private:

	HP_CWBANKVOICE *elems;		// dynamic array of elements of type HP_CWBANKVOICE
	int size;		// actual length
	int numElems;	// actual number of elements

public:
	void clear();					// delete all elements
	unsigned char operator[] (int index);		// operator []
	void add(unsigned char elem);				// add at the end
	void sort();					// sort elements by voice-name
};
*/
CWBankVoice_Array::CWBankVoice_Array()
{
	numElems = 0;	// empty
	size = 1500;
	elems = new HP_CWBANKVOICE* [size];
}

CWBankVoice_Array::~CWBankVoice_Array()
{

	int i;
	for (i=0; i<numElems; i++)
	{
		delete elems[i];
		elems[i] = NULL;
	}
	delete[] elems;	// free storage
}

int CWBankVoice_Array::get_length()
{
	return numElems;
}

void CWBankVoice_Array::add(HP_CWBANKVOICE* elem)	// add at the end
{
	int i;
	if (size <= numElems)
	{
		HP_CWBANKVOICE** tmp = elems;	// hold old elems
		size += 500;					// enlarge
		elems = new HP_CWBANKVOICE* [size];
		for (i=0; i<numElems; i++)
		{
			elems[i] = tmp[i];
		}
		delete [] tmp;
	}
	elems[numElems++] = elem;
}

void CWBankVoice_Array::sort()
{
	HP_CWBANKVOICE* m;
	int i,j;
	for (i=1; i<numElems; i++)
	{
		j = i-1;
		m = elems[i];
		while ((j>=0) && (strcoll((elems[j])->voice_name,m->voice_name)>0))
		{
			elems[j+1] = elems[j];
			j--;
		}
		elems[j+1] = m;
	}
//	for (i=0; i<30;i++)
//	{
//		MIDIFile::message("%s", elems[i]->voice_name);
//	}
}

HP_CWBANKVOICE* CWBankVoice_Array::operator[] (int index)		// operator []
{
	if (index >= numElems)
	{
		MIDIFile::message(NULL,"Uch_Array: Index out of range");
		return 0x00;
	}
	return (elems[index]);
}

/************************************************************************/
/*                                                                      */
/*  CWDrum_Array                                                        */
/*                                                                      */
/************************************************************************/

/* 
class CWDrum_Array  
{
private:

	HP_CWDRUM* *elems;	// dynamic array of elements of type HP_CWBANKVOICE
	int size;			// actual length
	int numElems;		// actual number of elements

public:
	CWDrum_Array();					    // default constructor
	virtual ~CWDrum_Array();			// destructor
	int get_length();						// no of elements
	HP_CWDRUM *operator[] (int index);	// operator []
	void add(HP_CWDRUM *elem);			// add at the end
};
*/


CWDrum_Array::CWDrum_Array()
{
	numElems = 0;	// empty
	size = 50;
	elems = new HP_CWDRUM* [size];
}

CWDrum_Array::~CWDrum_Array()
{

	int i;
	for (i=0; i<numElems; i++)
	{
		delete elems[i];
		elems[i] = NULL;
	}
	delete[] elems;	// free storage
}

int CWDrum_Array::get_length()
{
	return numElems;
}

void CWDrum_Array::add(HP_CWDRUM* elem)	// add at the end
{
	int i;
	if (size <= numElems)
	{
		HP_CWDRUM** tmp = elems;	// hold old elems
		size += 100;					// enlarge
		elems = new HP_CWDRUM* [size];
		for (i=0; i<numElems; i++)
		{
			elems[i] = tmp[i];
		}
		delete [] tmp;
	}
	elems[numElems++] = elem;

/*
    // test
	for (i=numElems-1; i<numElems;i++)
	{
		MIDIFile::message("add: bank_no=%i, voice_no=%i, is_drum=%i",
			elems[i]->bank_no, elems[i]->voice_no, elems[i]->is_drum);
	}
*/
}

HP_CWDRUM* CWDrum_Array::operator[] (int index)		// operator []
{
	if (index >= numElems)
	{
		MIDIFile::message(NULL,"Uch_Array: Index out of range");
		return 0x00;
	}
	return (elems[index]);
}










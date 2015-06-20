/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  SysExEvent.cpp : Implementation of class SysExEvent                            */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/

#include "stdafx.h"
#include "HP_midifile.h"
#include "midifile.h"

#include "SysExEvent.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SysExEvent::SysExEvent():MIDIEvent()
{
	type = HP_SYSEX;
}

SysExEvent::SysExEvent(int resulting_time,unsigned char* event_data, int event_data_length):MIDIEvent(resulting_time,event_data,event_data_length)
{
	type = HP_SYSEX;
}

SysExEvent::~SysExEvent()
{

}

const SysExEvent& SysExEvent::operator= (const SysExEvent& noev)
{
	if (this == &noev)
	{
		return *this;
	}
	MIDIEvent::operator=(noev);

	return *this;
}

void SysExEvent::sysex(unsigned char** data, int *length)
{
	int i;
	// overread var len

	int k = 1;
	unsigned char byte = event_data[1];
	if ((byte & 0x80) != 0)
	{
		do
		{ 
			k++;
			byte = event_data[k];
		} while ((byte & 0x80) != 0);
	}

	*length = event_data_length-k;
	*data = new unsigned char[*length];
	(*data)[0] = 0xf0;

	for (i=1+k; i<event_data_length; i++)
	{
		(*data)[i-k] = event_data[i];
	}
}

UINT SysExEvent::set_sysex(unsigned char* data)
{
	unsigned char* sysex = NULL;
	int sysex_length;

	int result = MIDIFile::eval_sysex_data(&sysex,&sysex_length,data);
	if (result != HP_ERR_NONE) return result;

	delete[] event_data; 

	event_data_length = sysex_length;
	event_data = sysex;

	return HP_ERR_NONE;
}

	
	



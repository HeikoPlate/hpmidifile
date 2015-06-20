/************************************************************************/
/************************************************************************/
/*                                                                      */
/* SysExEvent.h: Headerfile for class SysExEvent                        */
/*                                                                      */
/************************************************************************/
/************************************************************************/

#if !defined(AFX_SYSEXEVENT_H__FB398607_1529_41B0_8506_CAF492938577__INCLUDED_)
#define AFX_SYSEXEVENT_H__FB398607_1529_41B0_8506_CAF492938577__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MIDIEvent.h"

class SysExEvent : public MIDIEvent  
{
public:
	SysExEvent();
	SysExEvent(int resulting_time,unsigned char* event_data, int event_data_length);
	virtual ~SysExEvent();
	const SysExEvent& operator= (const SysExEvent& noev); // Assignment Operator
	void sysex(unsigned char** data, int *length); 
	UINT set_sysex(unsigned char* data);
};

#endif // !defined(AFX_SYSEXEVENT_H__FB398607_1529_41B0_8506_CAF492938577__INCLUDED_)

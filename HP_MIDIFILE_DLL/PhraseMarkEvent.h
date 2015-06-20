// PhraseMarkEvent.h: Schnittstelle für die Klasse PhraseMarkEvent.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHRASEMARKEVENT_H__7E1F7BAD_17E9_4313_9EEE_E5AD9EB398D4__INCLUDED_)
#define AFX_PHRASEMARKEVENT_H__7E1F7BAD_17E9_4313_9EEE_E5AD9EB398D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MetaEvent.h"

class PhraseMarkEvent : public MetaEvent  
{
public:
	PhraseMarkEvent();
	virtual ~PhraseMarkEvent();

};

#endif // !defined(AFX_PHRASEMARKEVENT_H__7E1F7BAD_17E9_4313_9EEE_E5AD9EB398D4__INCLUDED_)

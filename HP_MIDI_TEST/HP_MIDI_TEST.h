// HP_MIDI_TEST.h : Haupt-Header-Datei f�r die Anwendung HP_MIDI_TEST
//

#if !defined(AFX_HP_MIDI_TEST_H__575C552D_376F_47A0_950F_0D6249978E35__INCLUDED_)
#define AFX_HP_MIDI_TEST_H__575C552D_376F_47A0_950F_0D6249978E35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CHP_MIDI_TESTApp:
// Siehe HP_MIDI_TEST.cpp f�r die Implementierung dieser Klasse
//

class CHP_MIDI_TESTApp : public CWinApp
{
public:
	CHP_MIDI_TESTApp();
	HMODULE hl;


// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CHP_MIDI_TESTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CHP_MIDI_TESTApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_HP_MIDI_TEST_H__575C552D_376F_47A0_950F_0D6249978E35__INCLUDED_)

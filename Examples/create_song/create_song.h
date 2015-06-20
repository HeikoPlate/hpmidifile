// create_song.h : Haupt-Header-Datei für die Anwendung CREATE_SONG
//

#if !defined(AFX_CREATE_SONG_H__8C9B9D0E_2AC9_4650_9106_8E2B8A897281__INCLUDED_)
#define AFX_CREATE_SONG_H__8C9B9D0E_2AC9_4650_9106_8E2B8A897281__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "HP_midifile.h"
#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CCreate_songApp:
// Siehe create_song.cpp für die Implementierung dieser Klasse
//

class CCreate_songApp : public CWinApp
{
public:
	CCreate_songApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CCreate_songApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CCreate_songApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_CREATE_SONG_H__8C9B9D0E_2AC9_4650_9106_8E2B8A897281__INCLUDED_)

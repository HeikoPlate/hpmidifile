// midifile_player.h : Haupt-Header-Datei f�r die Anwendung MIDIFILE_PLAYER
//

#if !defined(AFX_MIDIFILE_PLAYER_H__0BF74739_0D94_49A0_9301_D2B41E763A26__INCLUDED_)
#define AFX_MIDIFILE_PLAYER_H__0BF74739_0D94_49A0_9301_D2B41E763A26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CMidifile_playerApp:
// Siehe midifile_player.cpp f�r die Implementierung dieser Klasse
//

class CMidifile_playerApp : public CWinApp
{
public:
	CMidifile_playerApp();

// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMidifile_playerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CMidifile_playerApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class MY_PLAY_MONITOR : public HP_PLAY_MONITOR
{
public:
   void HP_PlayTBMonitor(int takt, int beat);
   void HP_PlayChMonitor(char* chord_text);
   void HP_PlayLyMonitor(char* lyric_text);

   HWND whandle; // needed to display of takt, beat, chord and lyric
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_MIDIFILE_PLAYER_H__0BF74739_0D94_49A0_9301_D2B41E763A26__INCLUDED_)

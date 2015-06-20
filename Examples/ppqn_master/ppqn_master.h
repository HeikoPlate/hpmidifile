// ppqn_master.h : Haupt-Header-Datei für die Anwendung PPQN_MASTER
//

#if !defined(AFX_PPQN_MASTER_H__90A469D6_2FC0_49B6_B3C0_F36CF90F760C__INCLUDED_)
#define AFX_PPQN_MASTER_H__90A469D6_2FC0_49B6_B3C0_F36CF90F760C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CPpqn_masterApp:
// Siehe ppqn_master.cpp für die Implementierung dieser Klasse
//

class CPpqn_masterApp : public CWinApp
{
public:
	CPpqn_masterApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPpqn_masterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CPpqn_masterApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PPQN_MASTER_H__90A469D6_2FC0_49B6_B3C0_F36CF90F760C__INCLUDED_)

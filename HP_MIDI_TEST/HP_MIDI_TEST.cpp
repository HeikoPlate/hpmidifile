// HP_MIDI_TEST.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "HP_MIDI_TEST.h"
#include "HP_MIDI_TESTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHP_MIDI_TESTApp

BEGIN_MESSAGE_MAP(CHP_MIDI_TESTApp, CWinApp)
	//{{AFX_MSG_MAP(CHP_MIDI_TESTApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHP_MIDI_TESTApp Konstruktion

CHP_MIDI_TESTApp::CHP_MIDI_TESTApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CHP_MIDI_TESTApp-Objekt

CHP_MIDI_TESTApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHP_MIDI_TESTApp Initialisierung

BOOL CHP_MIDI_TESTApp::InitInstance()
{
	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

	CHP_MIDI_TESTDlg dlg;
	m_pMainWnd = &dlg;

	//	LoadLibrary only needed to set a breakpoint in the DLL, not needed later by the user
    hl = LoadLibrary("d:\\Programmierung\\HP_MIDIFILE_DLL\\HP_MIDIFILE.DLL\\debug\\hp_midifile.lib");
	dlg.hl = hl;

	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// ZU ERLEDIGEN: Fügen Sie hier Code ein, um ein Schließen des
		//  Dialogfelds über OK zu steuern
	}
	else if (nResponse == IDCANCEL)
	{
		// ZU ERLEDIGEN: Fügen Sie hier Code ein, um ein Schließen des
		//  Dialogfelds über "Abbrechen" zu steuern
	}

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.

    return FALSE;
}

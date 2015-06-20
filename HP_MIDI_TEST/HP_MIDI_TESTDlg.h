// HP_MIDI_TESTDlg.h : Header-Datei
//

#if !defined(AFX_HP_MIDI_TESTDLG_H__9E5EF622_0B2B_471D_A9F5_B738D2C084D3__INCLUDED_)
#define AFX_HP_MIDI_TESTDLG_H__9E5EF622_0B2B_471D_A9F5_B738D2C084D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\HP_MIDIFILE_DLL\HP_midifile.h"


/////////////////////////////////////////////////////////////////////////////
// CHP_MIDI_TESTDlg Dialogfeld

class CHP_MIDI_TESTDlg : public CDialog
{
// Konstruktion
public:
	CHP_MIDI_TESTDlg(CWnd* pParent = NULL);	// Standard-Konstruktor


	HP_MIDIFILE_MODULE* mf;

	bool is_stop;
	bool is_pause;
	bool is_playing;
	bool is_master_volume;
	bool is_ctrl7;
	HMODULE hl;


	void *pm;

// Dialogfelddaten
	//{{AFX_DATA(CHP_MIDI_TESTDlg)
	enum { IDD = IDD_HP_MIDI_TEST_DIALOG };
	CButton	m_master_volume;
	CButton	m_ctrl7;
	CEdit	m_start_time;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CHP_MIDI_TESTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

	private:
		char sourcefile[256];

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CHP_MIDI_TESTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRun();
	afx_msg void OnSelectsource();
	afx_msg void OnPlay();
	afx_msg void OnPause();
	afx_msg void OnStop();
	afx_msg void OnExit();
	afx_msg void OnOriginalTempo();
	afx_msg void OnHalfTempo();
	afx_msg void OnDoubleTempo();
	afx_msg void OnSave();
	afx_msg void OnSetTime();
	afx_msg void OnSendSingleEvent();
	afx_msg void OnMasterVolumeCheck();
	afx_msg void OnCtrl7Check();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class MY_PLAY_MONITOR : public HP_PLAY_MONITOR
{
public:
	void HP_PlayTBMonitor(int takt, int beat);
	void HP_PlayChMonitor(char* chord_text);
	void HP_PlayLyMonitor(char* lyric_text);
	CHP_MIDI_TESTDlg* dlg;

	HWND whandle; // you need it, if you want to display takt and beat
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_HP_MIDI_TESTDLG_H__9E5EF622_0B2B_471D_A9F5_B738D2C084D3__INCLUDED_)

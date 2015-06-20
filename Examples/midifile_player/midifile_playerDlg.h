// midifile_playerDlg.h : Header-Datei
//

#if !defined(AFX_MIDIFILE_PLAYERDLG_H__F410A7F9_3890_4EBA_8AAF_27261E8316A6__INCLUDED_)
#define AFX_MIDIFILE_PLAYERDLG_H__F410A7F9_3890_4EBA_8AAF_27261E8316A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMidifile_playerDlg Dialogfeld

class CMidifile_playerDlg : public CDialog
{
// Konstruktion
public:
	CMidifile_playerDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CMidifile_playerDlg)
	enum { IDD = IDD_MIDIFILE_PLAYER_DIALOG };
	CButton	m_exit;
	CButton	m_load;
	CButton	m_stop;
	CButton	m_play;
	CButton	m_go_on;
	CButton	m_pause;
	CProgressCtrl	m_progress;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMidifile_playerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung

private:
	char sourcefile[MAX_PATH];
	HP_MIDIFILE_MODULE* mf;
	UINT result;
	UINT_PTR m_nTimer;
	int duration; // time of complete song [sec]

protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CMidifile_playerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLoad();
	afx_msg void OnPlay();
	afx_msg void OnPause();
	afx_msg void OnGoOn();
	afx_msg void OnStop();
	virtual void OnOK();
	afx_msg void OnInfo();
	afx_msg void OnMidiDevice();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MIDIFILE_PLAYERDLG_H__F410A7F9_3890_4EBA_8AAF_27261E8316A6__INCLUDED_)

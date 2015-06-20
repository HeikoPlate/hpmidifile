// ppqn_masterDlg.h : Header-Datei
//

#if !defined(AFX_PPQN_MASTERDLG_H__34BB3E7D_0987_4A5C_A097_C99ED562264D__INCLUDED_)
#define AFX_PPQN_MASTERDLG_H__34BB3E7D_0987_4A5C_A097_C99ED562264D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPpqn_masterDlg Dialogfeld

class CPpqn_masterDlg : public CDialog
{
// Konstruktion
public:
	CPpqn_masterDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CPpqn_masterDlg)
	enum { IDD = IDD_PPQN_MASTER_DIALOG };
		// HINWEIS: der Klassenassistent fügt an dieser Stelle Datenelemente (Members) ein
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPpqn_masterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CPpqn_masterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnPpqn();
	afx_msg void OnExit();
	afx_msg void OnInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	char sourcefile[MAX_PATH];
	CString destination;

	HP_MIDIFILE_MODULE* mf;
	UINT result;
public:
	static const int cancel_returned;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.


#endif // !defined(AFX_PPQN_MASTERDLG_H__34BB3E7D_0987_4A5C_A097_C99ED562264D__INCLUDED_)

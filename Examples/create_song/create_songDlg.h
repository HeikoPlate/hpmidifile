// create_songDlg.h : Header-Datei
//

#if !defined(AFX_CREATE_SONGDLG_H__1E698EB3_F90A_4405_ADC7_DA34F64C7B4C__INCLUDED_)
#define AFX_CREATE_SONGDLG_H__1E698EB3_F90A_4405_ADC7_DA34F64C7B4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCreate_songDlg Dialogfeld

class CCreate_songDlg : public CDialog
{
// Konstruktion
public:
	CCreate_songDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CCreate_songDlg)
	enum { IDD = IDD_CREATE_SONG_DIALOG };
		// HINWEIS: der Klassenassistent fügt an dieser Stelle Datenelemente (Members) ein
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CCreate_songDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CCreate_songDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void look_for_error(const int result);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_CREATE_SONGDLG_H__1E698EB3_F90A_4405_ADC7_DA34F64C7B4C__INCLUDED_)

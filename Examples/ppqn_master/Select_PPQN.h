#if !defined(AFX_SELECT_PPQN_H__34414BDF_AB81_46D0_9919_1CE5640E051E__INCLUDED_)
#define AFX_SELECT_PPQN_H__34414BDF_AB81_46D0_9919_1CE5640E051E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Select_PPQN.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Select_PPQN 

class Select_PPQN : public CDialog
{
// Konstruktion
public:
	Select_PPQN(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(Select_PPQN)
	enum { IDD = IDD_SELECT_PPQN };
	CListBox	m_ppqn_list;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(Select_PPQN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
public:
	void set_ppqn(int ppqn); 

private:
	typedef struct pnl {int no; char* str;} PNL; 
	static pnl ppqn_list[];
	static const int ppqn_list_length;
	int old_ppqn;
	int new_ppqn;

protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(Select_PPQN)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SELECT_PPQN_H__34414BDF_AB81_46D0_9919_1CE5640E051E__INCLUDED_

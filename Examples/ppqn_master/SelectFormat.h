#if !defined(AFX_SELECTFORMAT_H__5D42E8CC_5ED3_45CD_8ECC_79C3D6EEDF0D__INCLUDED_)
#define AFX_SELECTFORMAT_H__5D42E8CC_5ED3_45CD_8ECC_79C3D6EEDF0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectFormat.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SelectFormat 

class SelectFormat : public CDialog
{
// Konstruktion
public:
	SelectFormat(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(SelectFormat)
	enum { IDD = IDD_SELECT_MIDIFORMAT };
	CButton	m_select_smf0;
	CButton	m_select_smf1;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SelectFormat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SelectFormat)
	afx_msg void OnSmf0();
	afx_msg void OnSmf1();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int format;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SELECTFORMAT_H__5D42E8CC_5ED3_45CD_8ECC_79C3D6EEDF0D__INCLUDED_

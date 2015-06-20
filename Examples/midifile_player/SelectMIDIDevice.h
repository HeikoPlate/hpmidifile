#if !defined(AFX_SELECTMIDIDEVICE_H__4AEA4D6B_2BD8_4846_AB8E_7DF6F1CEEE43__INCLUDED_)
#define AFX_SELECTMIDIDEVICE_H__4AEA4D6B_2BD8_4846_AB8E_7DF6F1CEEE43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectMIDIDevice.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SelectMIDIDevice 

class SelectMIDIDevice : public CDialog
{
// Konstruktion
public:
	SelectMIDIDevice(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(SelectMIDIDevice)
	enum { IDD = IDD_SELECT_MIDI_DEVICE };
	CListBox	m_device_list;
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(SelectMIDIDevice)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung

private:
	int no_devices;
	int id_list[32];
	int id;

protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SelectMIDIDevice)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_SELECTMIDIDEVICE_H__4AEA4D6B_2BD8_4846_AB8E_7DF6F1CEEE43__INCLUDED_

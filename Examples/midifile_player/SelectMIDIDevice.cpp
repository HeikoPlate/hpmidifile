// SelectMIDIDevice.cpp: Implementation
//

#include "stdafx.h"
#include "midifile_player.h"
#include "SelectMIDIDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialog SelectMIDIDevice 


SelectMIDIDevice::SelectMIDIDevice(CWnd* pParent /*=NULL*/)
	: CDialog(SelectMIDIDevice::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelectMIDIDevice)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void SelectMIDIDevice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectMIDIDevice)
	DDX_Control(pDX, IDC_DEVICE_LIST, m_device_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelectMIDIDevice, CDialog)
	//{{AFX_MSG_MAP(SelectMIDIDevice)
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//***********************************************************************
//
//	fill data in dlg
//
//***********************************************************************

BOOL SelectMIDIDevice::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HP_DEVICE *devices;

	int k;

	UINT result;

	// get list available of MIDI-devices
	result = HP_GetMIDIDevices(&devices,&no_devices);
    if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return TRUE;
	}

	// save id's local and  device-names in dlg-list
	for (k=0; k<no_devices; k++)
	{
		int id = devices[k].device_id;
		char* name = devices[k].device_name;
		id_list[k] = id;
		m_device_list.AddString(name);
	}

	// must be done!
	HP_Delete(devices);

	m_device_list.SetCurSel(0);
	id = -1;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

//***********************************************************************
//
//	OK, send selected id
//
//***********************************************************************

void SelectMIDIDevice::OnOK() 
{
	int gcs = m_device_list.GetCurSel();

	if (gcs != LB_ERR)
	{
		id = id_list[gcs];
	}
	
	EndDialog(id);
}

//***********************************************************************
//
//	Cancel
//
//***********************************************************************

void SelectMIDIDevice::OnCancel() 
{
	EndDialog(-1); // return the dlg of the MIDI-Mapper
}

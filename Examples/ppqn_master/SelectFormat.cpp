// SelectFormat.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ppqn_master.h"
#include "SelectFormat.h"
#include "ppqn_masterDLG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SelectFormat 


SelectFormat::SelectFormat(CWnd* pParent /*=NULL*/)
	: CDialog(SelectFormat::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelectFormat)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void SelectFormat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectFormat)
	DDX_Control(pDX, IDC_SMF0, m_select_smf0);
	DDX_Control(pDX, IDC_SMF1, m_select_smf1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelectFormat, CDialog)
	//{{AFX_MSG_MAP(SelectFormat)
	ON_BN_CLICKED(IDC_SMF0, OnSmf0)
	ON_BN_CLICKED(IDC_SMF1, OnSmf1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SelectFormat 

void SelectFormat::OnSmf0() 
{
	if (m_select_smf0.GetCheck()==1)
	{
		m_select_smf1.SetCheck(0);
		format = HP_SMF0;
	}
	else
	{
		m_select_smf1.SetCheck(1);
		format = HP_SMF1;
	}	
}

void SelectFormat::OnSmf1() 
{
	if (m_select_smf1.GetCheck()==1)
	{
		m_select_smf0.SetCheck(0);
		format = HP_SMF1;
	}
	else
	{
		m_select_smf0.SetCheck(1);
		format = HP_SMF0;
	}	
}

BOOL SelectFormat::OnInitDialog() 
{
	CDialog::OnInitDialog();

	format = HP_SMF0;
	m_select_smf0.SetCheck(1);
	m_select_smf1.SetCheck(0);

	return TRUE;  // return TRUE unless you set the focus to a control
              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SelectFormat::OnOK() 
{
	EndDialog(format);	
}

void SelectFormat::OnCancel() 
{
	EndDialog(CPpqn_masterDlg::cancel_returned);	
}

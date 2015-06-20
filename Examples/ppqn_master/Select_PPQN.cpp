// Select_PPQN.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ppqn_master.h"
#include "Select_PPQN.h"
#include "ppqn_masterDLG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Select_PPQN

const int Select_PPQN::ppqn_list_length = 14;

Select_PPQN::pnl Select_PPQN::ppqn_list[ppqn_list_length] =
	{
		{HP_PPQN48, 	"48"},
		{HP_PPQN72, 	"72"},
		{HP_PPQN96, 	"96"},
		{HP_PPQN120, 	"120"},
		{HP_PPQN144,	"144"},
		{HP_PPQN168,	"168"},
		{HP_PPQN192,	"192"},
		{HP_PPQN216,	"216"},
		{HP_PPQN240,	"240"},
		{HP_PPQN360,	"360"},
		{HP_PPQN384,	"384"},
		{HP_PPQN480,	"480"},
		{HP_PPQN960,	"960"},
		{HP_PPQN1920,	"1920"}
	};


Select_PPQN::Select_PPQN(CWnd* pParent /*=NULL*/)
	: CDialog(Select_PPQN::IDD, pParent)
{
	//{{AFX_DATA_INIT(Select_PPQN)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void Select_PPQN::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Select_PPQN)
	DDX_Control(pDX, IDC_PPQN_LIST, m_ppqn_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Select_PPQN, CDialog)
	//{{AFX_MSG_MAP(Select_PPQN)
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten Select_PPQN

void Select_PPQN::OnOK()
{
	// TODO: Zusätzliche Prüfung hier einfügen
	int gcs = m_ppqn_list.GetCurSel( );
	if (gcs == LB_ERR) return;

	new_ppqn = ppqn_list[gcs].no;

//	CString str;
//	str.Format("New PPQN %i",new_ppqn);
//	MessageBox(str,NULL,MB_OK);
	EndDialog(new_ppqn);
}

BOOL Select_PPQN::OnInitDialog()
{
	CDialog::OnInitDialog();
	int i;
	for (i=0; i < ppqn_list_length; i++)
	{
		m_ppqn_list.AddString(ppqn_list[i].str);
		if(old_ppqn == ppqn_list[i].no)
		{
			m_ppqn_list.SetCurSel(i);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void Select_PPQN::set_ppqn(int ppqn)
{
	old_ppqn = ppqn;
}

void Select_PPQN::OnCancel() 
{
	// TODO: Zusätzlichen Bereinigungscode hier einfügen
	EndDialog(CPpqn_masterDlg::cancel_returned);	
}

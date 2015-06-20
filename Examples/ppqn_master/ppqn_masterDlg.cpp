// ppqn_masterDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "ppqn_master.h"
#include "ppqn_masterDlg.h"
#include "SelectFormat.h"
#include "Select_PPQN.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPpqn_masterDlg Dialogfeld

const int CPpqn_masterDlg::cancel_returned = -1;

CPpqn_masterDlg::CPpqn_masterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPpqn_masterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPpqn_masterDlg)
		// HINWEIS: Der Klassenassistent fügt hier Member-Initialisierung ein
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	LoadLibrary("c:\\Programmierung\\HP_MIDIFILE_DLL\\HP_MIDIFILE.DLL\\debug\\hp_midifile.dll");

}

void CPpqn_masterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPpqn_masterDlg)
		// HINWEIS: Der Klassenassistent fügt an dieser Stelle DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPpqn_masterDlg, CDialog)
	//{{AFX_MSG_MAP(CPpqn_masterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_PPQN, OnPpqn)
	ON_BN_CLICKED(IDOK, OnExit)
	ON_BN_CLICKED(IDC_INFO, OnInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPpqn_masterDlg Nachrichten-Handler

BOOL CPpqn_masterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// ZU ERLEDIGEN: Hier zusätzliche Initialisierung einfügen

	mf = NULL;

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CPpqn_masterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CPpqn_masterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CPpqn_masterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPpqn_masterDlg::OnLoad()
{

	static char BASED_CODE szFilter[] =
		"MIDI Files (*.mid)|*.mid|All Files (*.*)|*.*||";


	CFileDialog openfile_dlg(TRUE,
						  "mid",
						  NULL,
						  OFN_FILEMUSTEXIST ||
						  OFN_PATHMUSTEXIST,
						  szFilter,
						  NULL);

	int return_value = openfile_dlg.DoModal();
	if (return_value == IDOK)
	{
		strcpy(sourcefile,openfile_dlg.m_ofn.lpstrFile);
	}
	else if (return_value == IDCANCEL)
	{
		return;
	}

	mf = HP_Init();
	if (mf == NULL)
	{
		MessageBox("Error Initialize",NULL,MB_OK);
		return;
	}

	result = HP_Load(mf, sourcefile);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		HP_Free(mf);
		mf = 0;
		return;
	}

}

void CPpqn_masterDlg::OnSave()
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (mf == 0)
	{
		MessageBox("No source loaded",NULL,MB_OK);
		return;
	}
	
	bool source_allowed = false;

	static char BASED_CODE szFilter[] =
		"MIDI File (*.mid)|*.mid|All Files (*.*)|*.*||";

	CFileDialog openfile_dlg(FALSE,
						  "mid",
						  NULL,
						  OFN_PATHMUSTEXIST,
						  szFilter,
						  NULL);

	int return_value = openfile_dlg.DoModal();
	if (return_value == IDOK)
	{
		destination = openfile_dlg.m_ofn.lpstrFile;
	}
	else
	{
		if (return_value == IDCANCEL)
		{
			return;
		}
	}

	// Format?
	SelectFormat dlg;

	int format = dlg.DoModal();
	if (format == cancel_returned)
	{
		return;
	}

	if (destination==(CString)sourcefile)
	{
			if (IDYES == AfxMessageBox("Overwrite source?",MB_YESNO,0))
			{
				source_allowed = true;
			}
			else
			{
				return;
			}
	}


	result = HP_Save(mf,destination,format);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	result = HP_Free(mf);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	mf = NULL;
}

void CPpqn_masterDlg::OnPpqn()
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen

	if (mf == 0)
	{
		MessageBox("No source loaded",NULL,MB_OK);
		return;
	}
	
	int old_ppqn;
	int new_ppqn;

	// PPQN ?
	Select_PPQN dlg;

	// old PPQN?
	result = HP_ReadPPQN(mf,&old_ppqn);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
	
	// old PPQN will be selected in PPQN list
	dlg.set_ppqn(old_ppqn);

	new_ppqn = dlg.DoModal();

	if (new_ppqn == cancel_returned)
	{
		return;
	}

	if (new_ppqn != old_ppqn)
	{
		result = HP_ChangePPQN(mf,new_ppqn);
		if (result!=HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
	}
}

void CPpqn_masterDlg::OnExit() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if (mf != 0) 
	{
		HP_Free(mf);
		mf = NULL;
	}
	CDialog::OnOK();
}

void CPpqn_masterDlg::OnInfo() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	CAboutDlg dlg;
	dlg.DoModal();
}

// midifile_playerDlg.cpp : Implementation 
//

#include "stdafx.h"
#include "midifile_player.h"
#include "midifile_playerDlg.h"
#include "SelectMIDIDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialog for "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	//}}AFX_VIRTUAL

// Implementation
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
// CMidifile_playerDlg Dialogfeld

CMidifile_playerDlg::CMidifile_playerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMidifile_playerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMidifile_playerDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CMidifile_playerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMidifile_playerDlg)
	DDX_Control(pDX, IDOK, m_exit);
	DDX_Control(pDX, IDC_LOAD, m_load);
	DDX_Control(pDX, IDC_STOP, m_stop);
	DDX_Control(pDX, IDC_PLAY, m_play);
	DDX_Control(pDX, IDC_GO_ON, m_go_on);
	DDX_Control(pDX, IDC_PAUSE, m_pause);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMidifile_playerDlg, CDialog)
	//{{AFX_MSG_MAP(CMidifile_playerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
	ON_BN_CLICKED(IDC_GO_ON, OnGoOn)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_INFO, OnInfo)
	ON_BN_CLICKED(IDC_MIDI_DEVICE, OnMidiDevice)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidifile_playerDlg Message-Handler

BOOL CMidifile_playerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	SetIcon(m_hIcon, TRUE);			// Large Icon
	SetIcon(m_hIcon, FALSE);		// Small Icon
	
	// for additive Initialisation

	mf = NULL;			// handle of HP MIDIFILE 
	m_nTimer = NULL;

	m_play.ModifyStyle(0,WS_DISABLED);
	m_stop.ModifyStyle(WS_VISIBLE,0);
	m_pause.ModifyStyle(0,WS_DISABLED);
	m_go_on.ModifyStyle(WS_VISIBLE,0);

	return TRUE;  
}

void CMidifile_playerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMidifile_playerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CMidifile_playerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//***********************************************************************
//
//	Initialize HP MIDIFILE and Load MIDI-file
//
//***********************************************************************

void CMidifile_playerDlg::OnLoad() 
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

	Invalidate();   
    UpdateWindow();
	
	if (return_value == IDOK)
	{
		strcpy(sourcefile,openfile_dlg.m_ofn.lpstrFile);
	}
	else if (return_value == IDCANCEL)
	{
		return;
	}

	// not the first Load?
	if (mf != NULL)
	{
		HP_Free(mf);
		mf = NULL;
	}

	// init HP MIDIFILE
	mf = HP_Init();
	if (mf == NULL)
	{
		MessageBox("Error Initialize",NULL,MB_OK);
		return;
	}

	// load MIDI-file
	result = HP_Load(mf, sourcefile);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		HP_Free(mf);
		mf = 0;
		return;
	}

	// compute duration of the total song and set to dlg-window
	duration = HP_Duration(mf)/1000; // secs
	CString t;
	t.Format("%i sec",duration);
	SetDlgItemText(IDC_SONG_TIME,t);

	// compute number of last takt and set to dlg-window
	{
		int time;
		HP_GetLastTime(mf,&time);
	
		int last_takt, beat, tick;
		result = HP_GetTaktBeatTick(mf,time,&last_takt, &beat, &tick);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
		}
		t.Format("%i",last_takt);
		SetDlgItemText(IDC_LAST_TAKT,t);
	}
	
	// show Play Button
	m_play.ModifyStyle(WS_DISABLED,WS_VISIBLE);
	Invalidate();   
    UpdateWindow();
}

//***********************************************************************
//
//	Start playing
//
//***********************************************************************

void CMidifile_playerDlg::OnPlay() 
{
	if (mf == NULL)
	{
		MessageBox("Load source first",NULL,MB_OK);
		return;
	}

	// Monitoring

	// create instance of MY_PLAY_MONITOR
	MY_PLAY_MONITOR *pm= new MY_PLAY_MONITOR;
	// send the handle of the actual Window to the instance of MY_PLAY_MONITOR
	pm->whandle = this->m_hWnd;
	// initialize the monitor
	HP_InitPlayMonitor(mf,pm);


	// initialize timer and set progress 0
	if (m_nTimer != NULL) 
	{
		KillTimer(m_nTimer);
		m_nTimer = NULL;
		m_progress.SetPos(0);

	}

	// set range of progress and set duration
	duration = HP_Duration(mf)/1000; // secs
	m_progress.SetRange32(0,duration);
	CString t;
	t.Format("%i sec",duration);
	SetDlgItemText(IDC_SONG_TIME,t);

	// start play of total song
	result = HP_Play(mf, HP_ALL);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	// aktivate dlg buttons, which can be pushed now
	m_go_on.ModifyStyle(WS_VISIBLE,0);
	m_pause.ModifyStyle(WS_DISABLED,WS_VISIBLE);
	m_play.ModifyStyle(WS_VISIBLE,WS_DISABLED);
	m_stop.ModifyStyle(0,WS_VISIBLE);
	m_load.ModifyStyle(0,WS_DISABLED);
	m_exit.ModifyStyle(0,WS_DISABLED);
 	Invalidate();   
    UpdateWindow();

	// activate and display first progress
	
	m_nTimer = SetTimer(1, 1000, 0);

	if (HP_IsPlaying(mf))
	{
		int actsecs = HP_PlayTime(mf)/1000;
		m_progress.SetPos(actsecs);
	}
}

//***********************************************************************
//
//	timer function
//
//***********************************************************************

void CMidifile_playerDlg::OnTimer(UINT nIDEvent) 
{
	if (!HP_IsPlaying(mf))
	{	// play done
		KillTimer(m_nTimer);
		m_nTimer = NULL;
		m_progress.SetPos(0);
		m_go_on.ModifyStyle(WS_VISIBLE,0);
		m_pause.ModifyStyle(0,WS_VISIBLE|WS_DISABLED);
		m_play.ModifyStyle(WS_DISABLED,WS_VISIBLE);
		m_stop.ModifyStyle(WS_VISIBLE,0);
 		SetDlgItemText(IDC_PLAY_TIME,"");
		Invalidate();   
		UpdateWindow();
	}
	else
	{	// playing, update dlg
		int secs = HP_PlayTime(mf)/1000; // secs
		m_progress.SetPos(secs);
		CString t;
		t.Format("%i sec",secs);
		SetDlgItemText(IDC_PLAY_TIME,t);
	}
	// activate next
	CDialog::OnTimer(nIDEvent);
}

//***********************************************************************
//
//	Pause
//
//***********************************************************************

void CMidifile_playerDlg::OnPause() 
{

	if (mf == NULL) return;
	if (!HP_IsPlaying(mf)) return;

	m_pause.ModifyStyle(WS_VISIBLE,0);
	m_go_on.ModifyStyle(0,WS_VISIBLE);
	Invalidate();   
    UpdateWindow();

	result = HP_SetPlayStopWait(mf, HP_WAIT_PLAY);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
}

//***********************************************************************
//
//	Go on playing after pause
//
//***********************************************************************

void CMidifile_playerDlg::OnGoOn() 
{
	if (mf == NULL) return;

	m_go_on.ModifyStyle(WS_VISIBLE,0);
	m_pause.ModifyStyle(WS_DISABLED,WS_VISIBLE);
	Invalidate();   
    UpdateWindow();

	result = HP_SetPlayStopWait(mf, HP_GO_PLAY);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
}

//***********************************************************************
//
//	Stop playing
//
//***********************************************************************

void CMidifile_playerDlg::OnStop() 
{
	if (mf == NULL) return;

	result = HP_SetPlayStopWait(mf, HP_STOP_PLAY);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}

	KillTimer(m_nTimer);
	m_nTimer = NULL;
	m_progress.SetPos(0);
	SetDlgItemText(IDC_PLAY_TIME,"");

	m_go_on.ModifyStyle(WS_VISIBLE,0);
	m_pause.ModifyStyle(0,WS_VISIBLE|WS_DISABLED);
	m_play.ModifyStyle(WS_DISABLED,WS_VISIBLE);
	m_stop.ModifyStyle(WS_VISIBLE,0);
	m_load.ModifyStyle(WS_DISABLED,0);
	m_exit.ModifyStyle(WS_DISABLED,0);
	Invalidate();   
    UpdateWindow();
}

//***********************************************************************
//
//	Exit
//
//***********************************************************************

void CMidifile_playerDlg::OnOK() 
{
	if (mf != NULL)
	{
		if (HP_IsPlaying(mf))
		{
			result = HP_SetPlayStopWait(mf, HP_STOP_PLAY);
			if (result!=HP_ERR_NONE)
			{
				MessageBox(HP_ErrText(result),NULL,MB_OK);
			}
		}
		m_progress.SetPos(0);
		HP_Free(mf);
		mf = NULL;
	}
	CDialog::OnOK();
}

//***********************************************************************
//
//	Info
//
//***********************************************************************

void CMidifile_playerDlg::OnInfo() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}

//***********************************************************************
//
//  Selection of MIDI-device	
//
//***********************************************************************

void CMidifile_playerDlg::OnMidiDevice() 
{
	if (mf != NULL)
	{
		if (HP_IsPlaying(mf)) return;
	}

	int id;

	// open dlg for selection and get back the device-id
	// look at class SelectMIDIDevice

	SelectMIDIDevice dlg;
	id = dlg.DoModal();

	// select device
	result = HP_SelectMIDIDevice(id); 
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
}


//***********************************************************************
//
//	Functions of Play Monitoring
//
//***********************************************************************

void MY_PLAY_MONITOR::HP_PlayTBMonitor(int takt, int beat)
{
   if (takt==-1)
   {
      SetDlgItemText(whandle,IDC_TAKT_BEAT,"");
      return;
   }

   CString r;
   r.Format("%4.4i:%2.2i",takt+1,beat+1);
   SetDlgItemText(whandle,IDC_TAKT_BEAT,r);
}

void MY_PLAY_MONITOR::HP_PlayChMonitor(char* chord_text)
{
   SetDlgItemText(whandle,IDC_CHORD,chord_text);
   HP_Delete(chord_text); // always needed !
}

void MY_PLAY_MONITOR::HP_PlayLyMonitor(char* lyric_text)
{
   SetDlgItemText(whandle,IDC_LYRIC,lyric_text);
   HP_Delete(lyric_text); // always needed !
}




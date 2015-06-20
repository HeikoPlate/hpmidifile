#include "stdafx.h"
#include "create_song.h"
#include "create_songDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreate_songDlg Dialogfeld

CCreate_songDlg::CCreate_songDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreate_songDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreate_songDlg)
		// HINWEIS: Der Klassenassistent fügt hier Member-Initialisierung ein
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCreate_songDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreate_songDlg)
		// HINWEIS: Der Klassenassistent fügt an dieser Stelle DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCreate_songDlg, CDialog)
	//{{AFX_MSG_MAP(CCreate_songDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreate_songDlg Nachrichten-Handler

BOOL CCreate_songDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// ZU ERLEDIGEN: Hier zusätzliche Initialisierung einfügen

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CCreate_songDlg::OnPaint()
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
HCURSOR CCreate_songDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCreate_songDlg::look_for_error(const int result)
{

	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		CDialog::OnOK();
	}
}

void CCreate_songDlg::OnOK()
{
	// The new MIDI-File will be created in this function

	const int PPQN = 120;

	int i;
	int time;	// no. of ticks from start 

	// note-length
	const int half = PPQN*2;	// ticks of a half-note
	const int quarter = PPQN;	// ticks of a quarter note
	const int quaver = PPQN/2;	// ticks of a quaver note

	HP_MIDIFILE_MODULE* mf = HP_Init();	// starts a MIDI-file session

	if (mf == NULL)
	{
		MessageBox("Error HP_Init",NULL,MB_OK);
		CDialog::OnOK();
	}

	look_for_error(HP_ChangePPQN(mf,PPQN));	// set PPQN
	look_for_error(HP_InsertSongTrackName(mf,"Billy's Bill"));	// set title

	HP_SetFirst(HP_LAST);	// add events of same time at the end
	look_for_error(HP_InsertCopyright(mf,0,"Copyright 2003"));
	look_for_error(HP_InsertTimeSignature(mf,0,3,4));		// 3/4		
	look_for_error(HP_InsertKeySignature(mf,0,"D"));		// D maj
	look_for_error(HP_InsertTempo(mf,0,60));				// bpm = 60
	look_for_error(HP_InsertProgramChange(mf,0,3,13));		// Xylophone
	look_for_error(HP_InsertProgramChange(mf,0,1,24));		// Nylon Guitar
	look_for_error(HP_InsertProgramChange(mf,0,2,32));		// Aco Bass

	// prepare notes
	 
	struct	{int takt;
			int beat;
			int tick;
			int length;
			int note;
			int chan;
			int vel;	} note[] =
	{
		{0,2,0,		quaver,	0x4e,3,100},
		{0,2,0,		quaver,	0x4a,3,100},
		{0,2,quaver,quaver,	0x4f,3,100},
		{0,2,quaver,quaver,	0x4c,3,100},
		{1,0,0,		quarter,0x4e,3,100},
		{1,0,0,		quarter,0x51,3,100},
		{1,0,0,		quarter,0x39,1,80 },
		{1,0,0,		quarter,0x3e,1,80 },
		{1,0,0,		quarter,0x42,1,80 },
		{1,0,0,		quarter,0x26,2,100},
		{1,1,0,		quarter,0x4a,3,100},
		{1,1,0,		quarter,0x4e,3,100},
		{1,1,0,		quarter,0x39,1,80 },
		{1,1,0,		quarter,0x3e,1,80 },
		{1,1,0,		quarter,0x42,1,80 },
		{1,1,0,		quarter,0x26,2,100},
		{1,2,0,		quarter,0x53,3,100},
		{1,2,0,		quarter,0x4f,3,100},
		{1,2,0,		quarter,0x3b,1,80 },
		{1,2,0,		quarter,0x3e,1,80 },
		{1,2,0,		quarter,0x43,1,80 },
		{1,2,0,		quaver,	0x2b,2,100},
		{1,2,quaver,quaver,	0x2f,2,100},
		{2,0,0,		half,	0x4e,3,100},
		{2,0,0,		half,	0x51,3,100},
		{2,0,0,		half,	0x39,1,80 },
		{2,0,0,		half,	0x3e,1,80 },
		{2,0,0,		half,	0x42,1,80 },
		{2,0,0,		half,	0x26,2,100},
		{-1,0,0,    0,   	0,	 0,0  }
	};

	// insert notes

	for (i=0; true; i++)
	{
		if (note[i].takt == -1) break;

	 	look_for_error(HP_GetTimeFromTaktBeatTick(mf,note[i].takt,note[i].beat,note[i].tick,&time));
		look_for_error(HP_InsertNote(mf,time,note[i].length,note[i].note,note[i].chan,note[i].vel));
	}

	// prepare lyrics

	struct { int takt;
			 int beat;
			 int tick;
			 char* text;  } lyric[] =
	{
		{0,2,0,		"Could "},
		{0,2,quaver,"you "},
		{1,0,0,		"pay "},
		{1,1,0,		"just "},
		{1,2,0,		"my "},
		{2,0,0,		"bill? "},
		{-1,0,0,	""}
	};

	// insert lyrics

	for (i=0; true; i++)
	{
		if (lyric[i].takt == -1) break;

		look_for_error(HP_GetTimeFromTaktBeatTick(mf,lyric[i].takt,lyric[i].beat,lyric[i].tick,&time));;
		look_for_error(HP_InsertLyric(mf,time,lyric[i].text));
	}

	// prepare and insert SysEx "Turn general MIDI system on"

	HP_SetFirst(mf);	// the first event before all
	unsigned char gm_on[] = {0xf0,0x7e,0x7f,0x09,0x01,0xf7};
	look_for_error(HP_InsertSysEx(mf,0,gm_on));	

	// play the song
	HP_Play(mf,HP_ALL);
	Sleep(8000);

	// save the MIDI-file in c:\temp

	look_for_error(HP_Save(mf,"c:\\temp\\billy.mid"));

	HP_Free(mf);	// finish MIDI-file session

	CDialog::OnOK(); // end of program
}

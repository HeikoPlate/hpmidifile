// HP_MIDI_TESTDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "HP_MIDI_TEST.h"
#include "HP_MIDI_TESTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHP_MIDI_TESTDlg Dialogfeld

CHP_MIDI_TESTDlg::CHP_MIDI_TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHP_MIDI_TESTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHP_MIDI_TESTDlg)
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	sourcefile[0] = 0x00;
}

void CHP_MIDI_TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHP_MIDI_TESTDlg)
	DDX_Control(pDX, IDC_MASTER_VOLUME_CHECK, m_master_volume);
	DDX_Control(pDX, IDC_CTRL7_CHECK, m_ctrl7);
	DDX_Control(pDX, IDC_START_TIME, m_start_time);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHP_MIDI_TESTDlg, CDialog)
	//{{AFX_MSG_MAP(CHP_MIDI_TESTDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RUN, OnRun)
	ON_BN_CLICKED(IDC_SELECTSOURCE, OnSelectsource)
	ON_BN_CLICKED(IDC_Play, OnPlay)
	ON_BN_CLICKED(IDC_Pause, OnPause)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDOK, OnExit)
	ON_BN_CLICKED(IDC_ORIGINAL_TEMPO, OnOriginalTempo)
	ON_BN_CLICKED(IDC_HALF_TEMPO, OnHalfTempo)
	ON_BN_CLICKED(IDC_DOUBLE_TEMPO, OnDoubleTempo)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_SET_TIME, OnSetTime)
	ON_BN_CLICKED(IDC_SINGLE_EVENT_SEND, OnSendSingleEvent)
	ON_BN_CLICKED(IDC_MASTER_VOLUME_CHECK, OnMasterVolumeCheck)
	ON_BN_CLICKED(IDC_CTRL7_CHECK, OnCtrl7Check)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHP_MIDI_TESTDlg Nachrichten-Handler

BOOL CHP_MIDI_TESTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	mf = NULL;
	pm = NULL;
	is_stop = true;
	is_playing = false;
	is_pause = false;

	SetDlgItemInt(IDC_START_TIME,0);

	m_master_volume.SetCheck(true);
	m_ctrl7.SetCheck(false);
	is_master_volume = true;
	is_ctrl7 = false;

	SetDlgItemInt(IDC_PARAMETER,127);


	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CHP_MIDI_TESTDlg::OnPaint() 
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
HCURSOR CHP_MIDI_TESTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/***************************************************************/
//
//	Play Monitoring
//
/***************************************************************/

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
	HP_Delete(chord_text);
}

void MY_PLAY_MONITOR::HP_PlayLyMonitor(char* lyric_text)
{
	SetDlgItemText(whandle,IDC_LYRIC,lyric_text);
	HP_Delete(lyric_text);
}

/***************************************************************/
//
//	Test Calls
//
/***************************************************************/

void CHP_MIDI_TESTDlg::OnRun() 
{
	MessageBox(HP_GetVersion());
	if (hl==0)
	{
		hl = LoadLibrary("d:\\Programmierung\\HP_MIDIFILE_DLL\\HP_MIDIFILE.DLL\\debug\\hp_midifile.lib");
	}

	UINT result;

//
	if (sourcefile[0]==0)
	{
		MessageBox("Load Source",NULL,MB_OK);
		return;
	}
//
	mf = HP_Init();
//
	result = HP_Load(mf, sourcefile);

	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
//
/*
	if (HP_MoreChunks(mf)==0)
	{
		MessageBox("No data behind tracks",NULL,MB_OK);
	}
	else
	{
		MessageBox("Any data behind tracks",NULL,MB_OK);
	}
*/


/*
// Functions:

	// Get last time from loaded MIDI-File
	CString r;
	int last_time;
	
	result = HP_GetLastTime(mf,&last_time);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("Last Time %i",last_time);
		MessageBox(r,NULL,MB_OK);
	}
*/
/*
		// create instance of MY_PLAY_MONITOR
		pm= new MY_PLAY_MONITOR;
		((MY_PLAY_MONITOR*)pm)->dlg = this;

		// send the handle of the actual Window to the instance of My_PTBM
		((MY_PLAY_MONITOR*)pm)->whandle = this->m_hWnd;
		// initialize the takt/beat-monitor 
		HP_InitPlayMonitor(mf,(MY_PLAY_MONITOR*)pm);	
*/
/*	
		// Play
		result = HP_Play(mf,HP_ALL);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}



		while (true)
		while (HP_IsPlaying(mf))
		{
			CString r;
			int time = HP_PlayEventTime(mf);
			if (time == -1)
			{
				MessageBox("PlayEventTime: -1", NULL, MB_OK);
				break;
			}
			else
			{
				if (time < 10000)
				{
					r.Format("PlayEventTime: %i ticks", time);
					MessageBox(r,NULL,MB_OK);
				}
			}
//
			// Test HP_PlaySingleEvent
//
			bool event_sent = false;
			if ((time>10000)&&!event_sent)
			{

				int length = 3;
				unsigned char event_data[3] = 
					{0xb0, 0x07, 0x00}; // ctrl. 7, chan 0, value 0

//				int length = 9;
//				unsigned char event_data[9] =
//					{0xf0,0x43,0x10,0x4c,0x00,0x00,0x04,0x40,0xf7}; // Master Volume 64


				result = HP_PlaySingleEvent(mf,event_data,length);

//
				if (result != HP_ERR_NONE)
				{
					MessageBox(HP_ErrText(result),NULL,MB_OK);
					return;
				}

//

				event_sent = true;
			}
//

			Sleep(1000);
		}
		MessageBox("Play finished", NULL, MB_OK);
//				

//		delete ptbm;				

//****************************************************************	
// Cakewalk Instrument Definitions
//****************************************************************	

*/
/*
	CString r;

// ========================================================
	result = HP_CWInsLoad(mf,"c:\\PSR8000\\insFiles\\tyros_kbds_1.07.ins");

 // result = HP_CWInsLoad(mf,"c:\\Users\\Heiko\\Desktop\\BK_Test.ins");

    if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

// ========================================================

	HP_CWDEF* def_list;
	int no_defs;


	result = HP_CWInsGetDefs(mf, &def_list, &no_defs);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	int i;
	for (i=0; i< no_defs; i++)
	{
		MessageBox(def_list[i].def_name,NULL,MB_OK);
	}

//	HP_Delete(def_list);

// ========================================================

	result = HP_CWInsSetDef(mf,no_defs-1); 
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
//
// ========================================================
	int is_drum;
	long bank_no; 
	int voice_no;

	
	bank_no = -1; voice_no = -1; // is_drum should be 1
	result = HP_CWInsIsDrumVoice(mf, bank_no,voice_no,&is_drum); 
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
	r.Format("%i, %i, %i",bank_no, voice_no, is_drum);
	MessageBox(r);

	bank_no = 15360; voice_no = 56; // is_drum should be 0
	result = HP_CWInsIsDrumVoice(mf, bank_no,voice_no,&is_drum); 
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
	r.Format("%i, %i, %i",bank_no, voice_no, is_drum);
	MessageBox(r);

	bank_no = 16128; voice_no = 35; // is_drum should be 1
	result = HP_CWInsIsDrumVoice(mf, bank_no,voice_no,&is_drum); 
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
	r.Format("%i, %i, %i",bank_no, voice_no, is_drum);
	MessageBox(r);

	bank_no = 16128; voice_no = 36; // is_drum should be 0
	result = HP_CWInsIsDrumVoice(mf, bank_no,voice_no,&is_drum); 
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
	r.Format("%i, %i, %i",bank_no, voice_no, is_drum);
	MessageBox(r);

// ========================================================
*/

/*
	HP_CWBANKVOICE *bank_voice_list = NULL;
	long bank_voice_list_len = 0;
	char search_string[20] = "";

	result = HP_CWInsGetBankVoiceList(mf, search_string, 
						&bank_voice_list,&bank_voice_list_len);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	int i;
	for (i=0; i<bank_voice_list_len; i++)
	{
		r.Format("1. Bank %i: %s, Voice %i: %s",
			(bank_voice_list[i]).bank_number,
			(bank_voice_list[i]).bank_name,
			(bank_voice_list[i]).voice_number,
			(bank_voice_list[i]).voice_name);
		MessageBox(r, NULL, MB_OK);
	}


	MessageBox("=================================",NULL,MB_OK);

	// refine search
	strcpy(search_string,"Live!Studio");
	result = HP_CWInsGetBankVoiceList(mf, search_string, 
						&bank_voice_list,&bank_voice_list_len);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	for (i=0; i<bank_voice_list_len; i++)
	{
		r.Format("2. Bank %i: %s, Voice %i: %s",
			(bank_voice_list[i]).bank_number,
			(bank_voice_list[i]).bank_name,
			(bank_voice_list[i]).voice_number,
			(bank_voice_list[i]).voice_name);
		MessageBox(r,NULL,MB_OK);
	}


	MessageBox("=================================",NULL,MB_OK);


	// start a new search	
	strcpy(search_string,"Grand Piano");
	HP_Delete(bank_voice_list);
	bank_voice_list_len = 0;
	result = HP_CWInsGetBankVoiceList(mf, search_string, 
						&bank_voice_list,&bank_voice_list_len);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	for (i=0; i<bank_voice_list_len; i++)
	{
		r.Format("3. Bank %i: %s, Voice %i: %s",
			(bank_voice_list[i]).bank_number,
			(bank_voice_list[i]).bank_name,
			(bank_voice_list[i]).voice_number,
			(bank_voice_list[i]).voice_name);
		MessageBox(r,NULL,MB_OK);
	}

	HP_Delete(bank_voice_list);
*/

// ========================================================
/*
	char * bank_name;
	HP_CWVOICE * voices;
//	int i;
	result = HP_CWInsGetBankVoices(mf,117,&bank_name,&voices);

	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
	
	MessageBox(bank_name, NULL, MB_OK);

	for (i=0; i<32; i++)
	{
		MessageBox(voices[i].voice_name, NULL, MB_OK);
	}
	
	HP_Delete(voices);

//
// läuft
// ========================================================
	HP_CWBANK* bank_list;
	int no_banks;
//	int i;

	result = HP_CWInsGetBanks(mf, &bank_list, &no_banks);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	for (i=0; i< no_banks; i++)
	{
		r.Format("%i: %s",bank_list[i].bank_number,bank_list[i].bank_name);
		MessageBox(r);
	}

	HP_Delete(bank_list);
//

// läuft
// ========================================================
//	char *bank_name;
	char *voice_name;
	char *note_name;

	result = HP_CWInsGetBankVoiceNoteName(mf,16256, 25, 27, &bank_name,&voice_name, &note_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("Bank %s, Patch %s, Note %s",bank_name, voice_name,note_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(bank_name);
		HP_Delete(voice_name);
		HP_Delete(note_name);
	}

	result = HP_CWInsGetBankVoiceNoteName(mf,16256, 25, 28, &bank_name,&voice_name, &note_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("Bank %s, Patch %s, Note %s",bank_name, voice_name,note_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(bank_name);
		HP_Delete(voice_name);
		HP_Delete(note_name);
	}

	result = HP_CWInsGetBankVoiceNoteName(mf,0, 0, 0, &bank_name,&voice_name, &note_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("Bank %s, Patch %s, Note %s",bank_name, voice_name,note_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(bank_name);
		HP_Delete(voice_name);
		HP_Delete(note_name);
	}

	result = HP_CWInsGetBankVoiceNoteName(mf,114, 15, 48, &bank_name,&voice_name, &note_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("Bank %s, Patch %s, Note %s",bank_name, voice_name,note_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(bank_name);
		HP_Delete(voice_name);
		HP_Delete(note_name);
	}

	result = HP_CWInsGetBankVoiceNoteName(mf,16128, 40, 22, &bank_name,&voice_name, &note_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("Bank %s, Patch %s, Note %s",bank_name, voice_name,note_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(bank_name);
		HP_Delete(voice_name);
		HP_Delete(note_name);
	}

	result = HP_CWInsGetBankVoiceNoteName(mf,HP_GM_VOICE_BANK, 32, 29, &bank_name,&voice_name, &note_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("Bank %s, Patch %s, Note %s",bank_name, voice_name,note_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(bank_name);
		HP_Delete(voice_name);
		HP_Delete(note_name);
	}
	
	result = HP_CWInsGetBankVoiceNoteName(mf,HP_GM_PERC_BANK, 0, 71, &bank_name,&voice_name, &note_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("Bank %s, Patch %s, Note %s",bank_name, voice_name,note_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(bank_name);
		HP_Delete(voice_name);
		HP_Delete(note_name);
	}

	result = HP_CWInsGetBankVoiceNoteName(mf,HP_GM_VOICE_BANK, 0, 48, &bank_name,&voice_name, &note_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("Bank %s, Patch %s, Note %s",bank_name, voice_name,note_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(bank_name);
		HP_Delete(voice_name);
		HP_Delete(note_name);
	}

//

// läuft

// ========================================================
	char* controller_name;

	result = HP_CWInsGetControllerName(mf,94,&controller_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("Controller Name %s",controller_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(controller_name);
	}
//
// läuft
// ========================================================
	char* rpn_name;

	result = HP_CWInsGetRPNName(mf,16383,&rpn_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("RPN Name %s",rpn_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(rpn_name);
	}

// ========================================================
	char* nrpn_name;
	result = HP_CWInsGetNRPNName(mf,180,&nrpn_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("NRPN Name %s",nrpn_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(nrpn_name);
	}

	result = HP_CWInsGetNRPNName(mf,4072,&nrpn_name);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
	else
	{
		r.Format("NRPN Name %s",nrpn_name);
		MessageBox(r,NULL,MB_OK);
		HP_Delete(nrpn_name);
	}
//

// läuft
//  HP_CWInsGetControllerNames =============================================

	HP_CWCONTROLLER* controller_names;
//	int i;

	result = HP_CWInsGetControllerNames(mf, &controller_names);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	for (i=0; i<128; i++)
	{
		r.Format("Controller %i: %s",i,controller_names[i].controller_name);
		MessageBox(r);
	}

	HP_Delete(controller_names);
//

// läuft
//  HP_CWInsGetNoteNames =============================================

	HP_CWNOTE* note_names;
//	char* bank_name;
//	char* voice_name;
//	int i;

	
	result = HP_CWInsGetNoteNames(mf,16256,82,&bank_name,&voice_name,&note_names);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	for (i=0; i<128; i++)
	{
		r.Format("Note(%s,%s,%i): %s",bank_name,voice_name,i,note_names[i].note_name);
		MessageBox(r);
	}

	HP_Delete(note_names);
	HP_Delete(bank_name);
	HP_Delete(voice_name);

	result = HP_CWInsGetNoteNames(mf,HP_GM_VOICE_BANK,1,&bank_name,&voice_name,&note_names);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	for (i=0; i<128; i++)
	{
		r.Format("Note(%s,%s,%i): %s",bank_name,voice_name,i,note_names[i].note_name);
		MessageBox(r);
	}

	HP_Delete(note_names);
	HP_Delete(bank_name);
	HP_Delete(voice_name);

	result = HP_CWInsGetNoteNames(mf,HP_GM_PERC_BANK,1,&bank_name,&voice_name,&note_names);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	for (i=0; i<128; i++)
	{
		r.Format("Note(%s,%s,%i): %s",bank_name,voice_name,i,note_names[i].note_name);
		MessageBox(r);
	}

	HP_Delete(note_names);
	HP_Delete(bank_name);
	HP_Delete(voice_name);
//


// läuft
//  HP_CWInsGetRPNNames =============================================

	HP_CWRPN* rpn_list;
	long no_rpn;
//	int i;

	result = HP_CWInsGetRPNNames(mf, &rpn_list, &no_rpn);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	for (i=0; (i<32)&&(i<no_rpn); i++)
	{
		r.Format("RPN %i: %s",rpn_list[i].rpn_number,rpn_list[i].rpn_name);
		MessageBox(r);
	}

	HP_Delete(rpn_list);

//  HP_CWInsGetNRPNNames =============================================

	HP_CWNRPN* nrpn_list;
	long no_nrpn;
//	int i;

	result = HP_CWInsGetNRPNNames(mf, &nrpn_list, &no_nrpn);
	if (result!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	for (i=0; (i<32)&&(i<no_nrpn); i++)
	{
		r.Format("NRPN %i: %s",nrpn_list[i].nrpn_number,nrpn_list[i].nrpn_name);
		MessageBox(r);
	}

	HP_Delete(nrpn_list);
//
*/
//****************************************************************	
// Ende Instrumentendefinition Test===============================
//****************************************************************	
//

/*

	// MIDIFile erzeugen, Test
	int PPQN;
	HP_ReadPPQN(mf,&PPQN);
	CString str;
	str.Format("PPQN= %i",PPQN);
	MessageBox(str, NULL,MB_OK);


	int quaver = PPQN/2;
	int quarter = PPQN;

	struct {int chan; int note; int vel; int takt; int beat; int tick; int length;} note[] =
	{
		{0,0x4e,100,1,2,0,quaver},
		{0,0x4a,100,1,2,0,quaver},
		{0,0x4f,100,1,2,quaver,quaver},
		{0,0x4c,100,1,2,quaver,quaver},
		{0,0x4e,100,2,0,0,quarter},
		{0,0x51,100,2,0,0,quarter}
	};


	for (int i=0; i<6; i++)
	{
		int time;
	 	HP_GetTimeFromTaktBeatTick(mf,note[i].takt,note[i].beat,note[i].tick,&time);
		CString r;
		r.Format("Time = %x  oder %i",time,time);
//		MessageBox(r,NULL,MB_OK);
		HP_InsertNote(mf,time,note[i].length,note[i].note,note[i].chan,note[i].vel);
	}


*/
/*
// 2 Takte 6/8
// int no_takts = 2;
// int no_beats = 6;

// 2 Takte 3/1
 int no_takts = 2;
 int no_beats = 3;

// 2 Takte 4/4
// int no_takts = 2;
// int no_beats = 4;


int takt, beat, tick;
CString r;
tick = 0;
for (takt=0; takt<no_takts;takt++)
{
	for (beat=0; beat<no_beats; beat++)
	{
		int time;
	 	HP_GetTimeFromTaktBeatTick(mf,takt,beat,tick,&time);
		r.Format("takt %i, beat %i, tick %i, time %i", takt,beat,tick,time);

		MessageBox(r);
	}
}
*/

/*
int takt = 0;
int beat = 2;
int tick = 3;
CString r;
int time;

HP_GetTimeFromTaktBeatTick(mf,takt,beat,tick,&time);
r.Format("takt %i, beat %i, tick %i, time %i", takt,beat,tick,time);
MessageBox(r);
*/

/*

	// Delete first two note-events

	int id, chan,time, type, no = 2;
	
	HP_Rewind(mf);

	while ( HP_ReadEvent(mf,&id,&chan,&time,&type) == HP_ERR_NONE)
	{
		if (type == HP_NOTE)
		{
			HP_DeleteEvent(mf,id);
			no--;
			if (no==0)	break;
		}
	}
*/
/*
	// Delete first rpn-event

	int id, chan, time, type;
	
	HP_Rewind(mf);

	while ( HP_ReadEvent(mf,&id,&chan,&time,&type) == HP_ERR_NONE)
	{
		if (type == HP_RPN)
		{
			HP_DeleteEvent(mf,id);
		}
	}
*/
/*
	int takt, beat, tick;
	result = HP_GetTaktBeatTick(mf,0x12c,&takt, &beat, &tick);
*/
/*
	int takt, beat, tick;
	result = HP_GetTaktBeatTick(mf,0x4b0,&takt, &beat, &tick);
	
	int time;
	result = HP_GetTimeFromTaktBeatTick(mf,3,1,0,&time);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
	
	result = HP_InsertNote(mf,0x78,0x78,0x01,0x40,0x64); // time/length/chan/note/vel
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/
/*
	int time;
	result = HP_GetTimeFromTaktBeatTick(mf,3,0,0,&time);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/

/*
	// Get takt, beat and tick while scanning

	int id, chan, time, type;
	int time_of_taktchange;
	int takt_of_taktchange;
    int num = 4;
	int denum = 4;
	int metronome_click,n32;
	int takt, beat, tick;

	int no_messages = 0;
	int max_messages = 10;
	
	HP_Rewind(mf);

	while ( HP_ReadEvent(mf,&id,&chan,&time,&type) == HP_ERR_NONE)
	{
		if (type == HP_TIME_SIGNATURE)
		{
			result = HP_ReadTimeSignature(mf,id,&time_of_taktchange,&num,&denum,&metronome_click,&n32);
			if (result != HP_ERR_NONE)
			{
				MessageBox(HP_ErrText(result),NULL,MB_OK);
				return;
			}
			result = HP_GetTaktBeatTick(mf,time,&takt_of_taktchange,&beat,&tick);
			if (result != HP_ERR_NONE)
			{
				MessageBox(HP_ErrText(result),NULL,MB_OK);
				return;
			}
			continue;
		}
		
		if (type == HP_NOTE)
		{
			if (no_messages >= 10) break;
			result = HP_GetTaktBeatTick(mf,time,time_of_taktchange,takt_of_taktchange,num,denum,&takt,&beat,&tick);
			if (result != HP_ERR_NONE)
			{
				MessageBox(HP_ErrText(result),NULL,MB_OK);
				return;
			}
			CString r;
			r.Format("Note on time %i, takt %i, beat %i, tick %i",time, takt+1, beat+1, tick);
			MessageBox(r);
			no_messages++;
		}
 	}
	HP_Rewind(mf);

	// Get takt, beat and tick whithout scanning

	no_messages = 0;

	while ( HP_ReadEvent(mf,&id,&chan,&time,&type) == HP_ERR_NONE)
	{
		if (type == HP_NOTE)
		{
			if (no_messages >= 10) break;
			result = HP_GetTaktBeatTick(mf,time,&takt,&beat,&tick);
			if (result != HP_ERR_NONE)
			{
				MessageBox(HP_ErrText(result),NULL,MB_OK);
				return;
			}
			CString r;
			r.Format("Note on time %i, takt %i, beat %i, tick %i",time, takt+1, beat+1, tick);
			MessageBox(r);
			no_messages++;
		}
 	}
*/	
/*
	// Find Error in HP_GetTaktBeatTick of HP MIDIFILE Version 1.2
	HP_Rewind(mf);

	int chan, time, type, takt, beat, tick, id, l_takt, l_beat;
	l_takt = 0;
	l_beat = 0;

	while ( HP_ReadEvent(mf,&id,&chan,&time,&type) == HP_ERR_NONE)
	{
		result = HP_GetTaktBeatTick(mf,time,&takt, &beat, &tick);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
		if ((l_takt != takt) || (l_beat != beat))
		{
			CString r;
			r.Format("takt %i, beat %i, tick %i",takt, beat, tick);
			MessageBox(r);
			l_takt = takt;
			l_beat = beat;
			if (takt >= 5) break;
		}
	}
*/
/*
	// Find Error in HP_ReadPitchWheel
	HP_Rewind(mf);

	int id, chan, time, type;
    int value;
	result = HP_InsertPitchWheel(mf,1920,0,8335); 
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}

	while ( HP_ReadEvent(mf,&id,&chan,&time,&type) == HP_ERR_NONE)
	{
       if (type==HP_PITCH_WHEEL)
	   {
			result = HP_ReadPitchWheel(mf,id,&time,&chan,&value);
			if (result != HP_ERR_NONE)
			{
				MessageBox(HP_ErrText(result),NULL,MB_OK);
			}
			CString r;
			r.Format("ReadPitchWheel: time %i, chan %i, value %i",time,chan,value);
			MessageBox(r,NULL,MB_OK);
			result = HP_ChangePitchWheel(mf,id,20);

			if (result != HP_ERR_NONE)
			{
				MessageBox(HP_ErrText(result),NULL,MB_OK);
				return;
			}
			HP_ReadPitchWheel(mf,id,&time,&chan,&value);
			r.Format("ChangePitchWheel: time %i, chan %i, value %i",time,chan,value);
			MessageBox(r,NULL,MB_OK);

			break;
		}
	}
*/

  
/*
	HP_SetFirst(mf);
	result = HP_InsertNote(mf,1920,240,0x40,0,0x68); 
		//  = time/length/note/chan/vel
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/
/*
	result = HP_InsertController(mf,0x1f0,9,HP_BANK_SELECT_LSB,0x41); 
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/
/*
	result = HP_InsertNRPNAbsolute(mf,0x1f2,4,700,2000);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/
/*
	result = HP_InsertRPNRelative(mf,0x1f2,4,700,-5);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/

/*
	// Meta-Events

// Read Lyrics

int id, type, chan, time;
HP_Rewind(mf);
while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
{
   if (type==HP_LYRIC)
   {
      char* text;
      result = HP_ReadLyric(mf,id,&time,&text);
      if (result != HP_ERR_NONE)
      {
         MessageBox(HP_ErrText(result),NULL,MB_OK);
         return;
      }
      CString r;
      r.Format("Lyric: time %i, %s",time,text);
      MessageBox(r,NULL,MB_OK);
      HP_Delete(text);
   }
}
*/
/*
	
	{
		// Text-Meta CuePoint
		char* text = "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
		result = HP_InsertCuePoint(mf,0,text);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
		
		// Read CuePoint
		int id, type, chan, time;
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_CUE_POINT)
			{
				char *text;
				HP_ReadCuePoint(mf,id,&time,&text);
				CString r;
				r.Format("Cuepoint: time %i, %s",time,text);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(text);
				break;
			}
		}
		
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_CUE_POINT)
			{
				char* new_text = "Hallo";
				HP_ChangeCuePoint(mf,id,new_text);
				break;
			}
		}
		
		// Read new CuePoint
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_CUE_POINT)
			{
				char *text;
				HP_ReadCuePoint(mf,id,&time,&text);
				CString r;
				r.Format("Cuepoint: time %i, %s",time,text);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(text);
				break;
			}
		}
	}

*/
/*
// Read and Write CopyRight

int id, type, chan, time;
HP_Rewind(mf);
while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
{
//	if (type != HP_COPYRIGHT) continue;
	char text[1024] = "";
	HP_DeleteEvent(mf,id);
	strcat(text,"123456789012345678901234567890123456789012345678901234567890\n");
	strcat(text,"123456789012345678901234567890123456789012345678901234567890\n");
	strcat(text,"123456789012345678901234567890123456789012345678901234567890\n");
	strcat(text,"123456789012345678901234567890123456789012345678901234567890\n");
	strcat(text,"123456789012345678901234567890123456789012345678901234567890\n");

	result = HP_InsertCopyright(mf,time,text);
	
	CString r;
	r.Format("Len of Copyright %i",(int)strlen(text));
	MessageBox(r);

	// 02
//	char* copyright = "Copyright Heiko Plate";
//	result = HP_InsertCopyright(mf,0,copyright);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
	break;
}
*/
/*
	// 03
	char* trackname = "Chan1";
	result = HP_InsertSongTrackName(mf,trackname);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

    // 4
	char* instrument = "Piano";
	result = HP_InsertInstrument(mf,3,instrument);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/
/*
	// 5
	char* lyric = "Ü";
	result = HP_InsertLyric(mf,480,lyric);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/
/*
	// 6
	char* marker = "Marker";
	result = HP_InsertMarker(mf,3,marker);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	// 7
	char* cuepoint = "Cuepoint";
	result = HP_InsertCuePoint(mf,3,cuepoint);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/

	// MIDI Port
/*		
		result = HP_InsertMIDIPort(mf,0,1,5); // time 0, track 1, port 5
		if (result != HP_ERR_NONE)
		{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
*/

/*
		int ppqn;
		HP_ReadPPQN(mf,&ppqn);

		for (i=1; i<16; i++)
		{
			result = HP_InsertMIDIPort(mf,i*ppqn,i,2*i); // time 0, track 1, port 5
			if (result != HP_ERR_NONE)
			{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
				return;
			}
		}

*/
/*
		int id, type, chan, time, track;
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_MIDI_PORT)
			{
				int port;
				HP_ReadMidiPort(mf,id,&time,&track,&port);
				CString r;
				r.Format("MidiPort: time %i, track %i, port %i",time,track,port);
				MessageBox(r,NULL,MB_OK);
				break;
			}
		}

		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_MIDI_PORT)
			{
				HP_ChangeMidiPort(mf,id,HP_NO_TRACK,2); // change port to 2 only
				break;
			}
		}		

		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_MIDI_PORT)
			{
				int port;
				HP_ReadMidiPort(mf,id,&time,&chan,&port);
				CString r;
				r.Format("MidiPort: time %i, chan %i, port %i",time,chan,port);
				MessageBox(r,NULL,MB_OK);
				break;
			}
		}
	}
*/
/*
	{	// SMPTE Offset
		UINT result;
		
		result = HP_InsertSmpteOffset(mf,4,5,6,7,8); 
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}

		int id, type, chan, time;
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_SMPTE_OFFSET)
			{
				int hr, mn, se, fr, ff;
				HP_ReadSmpteOffset(mf,id,&hr,&mn,&se,&fr,&ff);
				CString r;
				r.Format("SmpteOffset: hr %i, mn %i, se %i, fr %i, ff %i",hr,mn,se,fr,ff);
				MessageBox(r,NULL,MB_OK);
				break;
			}
		}

		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_SMPTE_OFFSET)
			{
				HP_ChangeSmpteOffset(mf,id,0,1,2,3,4);
				break;
			}
		}		

		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_SMPTE_OFFSET)
			{
				int hr, mn, se, fr, ff;
				HP_ReadSmpteOffset(mf,id,&hr,&mn,&se,&fr,&ff);
				CString r;
				r.Format("SmpteOffset: hr %i, mn %i, se %i, fr %i, ff %i",hr,mn,se,fr,ff);
				MessageBox(r,NULL,MB_OK);
				break;
			}
		}
	}
*/
/*
	unsigned char trackname[] = "Trackname";
	result = HP_InsertSongTrackName(mf,trackname,9,5);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	result = HP_InsertKeySignature(mf,0x3c0,"A"); // C#m
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/
/*
	{	// Time Signature
		UINT result;
		HP_SetFirst(mf);
		result = HP_InsertTimeSignature(mf,1925,3,4); // 3/4 
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
	}
*/

/*
{	// Time Signature and Shift Event
	int id, type, chan, time;
	HP_Rewind(mf);
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_TIME_SIGNATURE)
		{
			int num, denum, metronome_click,n32;
			HP_ReadTimeSignature(mf,id,&time,&num,&denum,&metronome_click,&n32);
			CString r;
			r.Format("TimeSignature_orig: time %i, num %i, denum %i, m_click %i, n32 %i",
				                     time,num,denum,metronome_click,n32);
			MessageBox(r,NULL,MB_OK);

			HP_ShiftEvent(mf,id,3845);
			break;
		}
	}
	HP_Rewind(mf);
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_TIME_SIGNATURE)
		{
			int num, denum, metronome_click,n32;
			HP_ReadTimeSignature(mf,id,&time,&num,&denum,&metronome_click,&n32);
			CString r;
			r.Format("TimeSignature_shift: time %i, num %i, denum %i, m_click %i, n32 %i",
				                     time,num,denum,metronome_click,n32);
			MessageBox(r,NULL,MB_OK);

			HP_ShiftEvent(mf,id,3845);
			break;
		}
	}
}
*/
/*
	{	// Time Signature
		UINT result;
		result = HP_InsertTimeSignature(mf,0x3c0,3,4); // 3/4 
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}

		int id, type, chan, time;
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_TIME_SIGNATURE)
			{
				int num, denum, metronome_click,n32;
				HP_ReadTimeSignature(mf,id,&time,&num,&denum,&metronome_click,&n32);
				CString r;
				r.Format("TimeSignature: time %i, num %i, denum %i, m_click %i, n32 %i",
					                     time,num,denum,metronome_click,n32);
				MessageBox(r,NULL,MB_OK);
				break;
			}
		}

		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_TIME_SIGNATURE)
			{
				HP_ChangeTimeSignature(mf,id,4,4,0x1a,0x0a); // Change to 4/4 
				break;
			}
		}		

		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_TIME_SIGNATURE)
			{
				int num, denum, metronome_click,n32;
				HP_ReadTimeSignature(mf,id,&time,&num,&denum,&metronome_click,&n32);
				CString r;
				r.Format("TimeSignature: time %i, num %i, denum %i, m_click %i, n32 %i",
					                     time,num,denum,metronome_click,n32);
				MessageBox(r,NULL,MB_OK);
				break;
			}
		}
	}
*/

	{	// Key Signature, Type Name
/*
		UINT result;
		result = HP_InsertKeySignature(mf,1440,"Fm");
		MessageBox("Key Signature Fm inserted at 1440");
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
*/
/*
		int id, type, chan, time;
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{

			if (type==HP_KEY_SIGNATURE)
			{
				char* key;
				HP_ReadKeySignature(mf,id,&time,&key);
				CString r;
				r.Format("KeySignature: time %i, key %s",time,key);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(key);


				HP_ChangeKeySignature(mf,id,"Fm");

				HP_ReadKeySignature(mf,id,&time,&key);
				r.Format("KeySignature changed: time %i, key %s",time,key);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(key);

			}
		}
*/
/*
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_KEY_SIGNATURE)
			{
				HP_ChangeKeySignature(mf,id,"B");  
				break;
			}
		}		

		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_KEY_SIGNATURE)
			{
				char* key;
				HP_ReadKeySignature(mf,id,&time,&key);
				CString r;
				r.Format("KeySignature: time %i, key %s",time,key);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(key);
			}
		}
*/
	}
/*

	{ // YAMAHA Meta Events, Chord separate
		int id, time, length;
		
		unsigned char data[] = {0xff,0x7f,0x08,0x43,0x7b,0x01,2,3,4,5,6};
		unsigned char* event_data;

		data[4] = 0x73;
		data[5] = 0x0a;
		HP_InsertScoreStartBar(mf, 0,data,11);
		data[5] = 0x0d;
		HP_InsertKeyboardVoice(mf, 0,data,11);
		data[4] = 0x7b;
		data[5] = 0x00;
		HP_InsertXFVersionID(mf, 0,data,11);
		data[5] = 0x02;
		HP_InsertRehearsalMark(mf, 0,data,11);
		data[5] = 0x03;
		HP_InsertPhraseMark(mf, 0,data,11);
		data[5] = 0x04;
		HP_InsertMaxPhraseMark(mf, 0,data,11);
		data[5] = 0x05;
		HP_InsertFingeringNumber(mf, 0,data,11);
		data[5] = 0x05;
		HP_InsertGuideTrackFlag(mf, 0,data,11);
		data[5] = 0x10;
		HP_InsertGuitarInformationFlag(mf, 0,data,11);
		data[5] = 0x12;
		HP_InsertChordVoicingForGuitar(mf, 0,data,11);
		data[5] = 0x21;
		HP_InsertLyricsBitmap(mf, 0,data,11);
        data[5] = 0x0a;

		HP_InsertOtherYamahaMeta(mf, 0,data,11);
*/
/*
		int type, chan;
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{

			if (type==HP_SCORE_START_BAR)
			{
				HP_ReadScoreStartBar(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_SCORE_START_BAR: time %x, %x, %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5],
					event_data[6],event_data[7]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_QUICK_START)
			{
				HP_ReadQuickStart(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_QUICK_START: time %x, %x, %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5],
					event_data[6],event_data[7]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_SCORE_LAYOUT)
			{
				HP_ReadScoreLayout(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_SCORE_LAYOUT: time %x, %x, %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5],
					event_data[6],event_data[7]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_KEYBOARD_VOICE)
			{
				HP_ReadKeyboardVoice(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_KEYBOARD_VOICE: time %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_XF_VERSION_ID)
			{
				HP_ReadXFVersionID(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_XF_VERSION_ID: time %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_REHEARSAL_MARK)
			{
				HP_ReadRehearsalMark(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_REHEARSAL_MARK: time %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_PHRASE_MARK)
			{
				HP_ReadPhraseMark(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_PHRASE_MARK: time %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_MAX_PHRASE_MARK)
			{
				HP_ReadMaxPhraseMark(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_MAX_PHRASE_MARK: time %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_FINGERING_NUMBER)
			{
				HP_ReadFingeringNumber(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_FINGERING_NUMBER: time %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_GUIDE_TRACK_FLAG)
			{
				HP_ReadGuideTrackFlag(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_GUIDE_TRACK_FLAG: time %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_GUITAR_INFORMATION_FLAG)
			{
				HP_ReadGuitarInformationFlag(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_GUITAR_INFORMATION_FLAG: time %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_CHORD_VOICING_FOR_GUITAR)
			{
				HP_ReadChordVoicingForGuitar(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_CHORD_VOICING_FOR_GUITAR: time %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
			if (type==HP_LYRICS_BITMAP)
			{
				HP_ReadLyricsBitmap(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_LYRICS_BITMAP: time %x, %x, %x, %x, %x, %x, %x ",
					time,event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}

			if (type==HP_OTHER_YAMAHA_META)
			{
				HP_ReadOtherYamahaMeta(mf,id,&time,&event_data,&length);
				CString r;
				r.Format("HP_OTHER_YAMAHA_META: time %i,length %i, %x, %x, %x, %x, %x, %x, %x ,%x, %x, %x, %x",
					time, length, event_data[0],event_data[1],event_data[2],event_data[3],event_data[4],event_data[5],
					event_data[6],event_data[7],event_data[8],event_data[9],event_data[10]);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(event_data);
			}
		}
/*

	}

	{	// Chord
		UINT result;

		result = HP_InsertChord(mf,1000,0x12,0x0f,0x31,0x7f); // DbbminMaj7/C  
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}

		int id, type, chan, time;
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_CHORD_NAME)
			{
				unsigned char cr, ct,bn, bt;
				char *chord;
				HP_ReadChord(mf,id,&time,&cr,&ct,&bn,&bt,&chord);
				CString r;
				r.Format("Chord: time %x, cr %x, ct %x, bn %x, bt %x, chord %s",
														time,cr,ct,bn,bt,chord);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(chord);
				break;
			}
		}

		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_CHORD_NAME)
			{
				HP_ChangeChord(mf,id,0x41,0x09,0x7f,0x7f); // C#min6  
				break;
			}
		}		

		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_CHORD_NAME)
			{
				unsigned char cr, ct,bn, bt;
				char *chord;
				HP_ReadChord(mf,id,&time,&cr,&ct,&bn,&bt,&chord);
				CString r;
				r.Format("Chord: time %x, cr %x, ct %x, bn %x, bt %x, chord %s",
														time,cr,ct,bn,bt,chord);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(chord);
				break;
			}
		}

	}
*/


/*
	{	// Raw Event
		UINT result;
		// Insert Aftertouch by Raw Data
		unsigned char at_data[] = {0xa4,0x63,0x65};
		result = HP_InsertRawEvent(mf,0x3c5,at_data,3); 
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}

		int id, type, chan, time;
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_RAW_EVENT)
			{
				
				int length;
				unsigned char* event = NULL;

				HP_ReadEventData(mf,id,&time,&event,&length);
				CString r;
				r.Format("Raw Event: time %x",time);
				MessageBox(r,NULL,MB_OK);
				int i;
				for (i=0; i<length; i++)
				{
					r.Format("%x",event[i]);
					MessageBox(r,NULL,MB_OK);
				}
				HP_Delete(event);
				break;
			}
		}
	}
*/
/*
	{	
		int id, chan, time, type;
		CString r;

		HP_Rewind(mf);

		while (HP_ReadEvent(mf,&id,&chan,&time,&type) == HP_ERR_NONE)
		{
			r.Format("id = %i",id);
			MessageBox(r);
			HP_DeleteEvent(mf,id);
		}
	}
*/

/*
	result = HP_ChangePPQN(mf,HP_PPQN120);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/
/*
	result = HP_InsertNote(mf,2,0x02,0,0x1e0,0x0x,0x64,0x3c);
		//  = takt/beat/tick/length/chan/note/vel

	  if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/

/*
// *************************************************************
	// Buffer Test
	// Load midi0 and select all and copy all (notes) in buffer. Change channel to 0. Load test and
	// paste buffer, time = 0, delete events of chan 4.. Save in test1

	HP_Rewind(mf);
	HP_Select(mf,HP_ALL_EVENTS);


	HP_Copy(mf);

//	result = HP_Free(mf); // close mf
//	if (result != HP_ERR_NONE)
//	{
//		MessageBox(HP_ErrText(result),NULL,MB_OK);
//		return;
//	}

//	HP_ChangeChannelInBuffer(0);

	// Copy buffer in mfdest

//	HP_MIDIFILE_MODULE* mfdest = HP_Init();

//	if (HP_Load(mfdest,"c:\\tmp\\test.mid" )!=HP_ERR_NONE)
//	{
//		MessageBox(HP_ErrText(result),NULL,MB_OK);
//		return;
//	}

//	HP_Paste(mfdest,0,HP_DELETE,HP_ALL_EVENTS);
//	HP_ChangePPQN(mfdest,480);



//	result = HP_Save(mfdest,"c:\\tmp\\test1.mid",HP_SMF0);
//	if (result != HP_ERR_NONE)
//	{
//		MessageBox(HP_ErrText(result),NULL,MB_OK);
//		return;
//	}

//	result = HP_Free(mfdest);
//	if (result != HP_ERR_NONE)
//	{
//		MessageBox(HP_ErrText(result),NULL,MB_OK);
//		return;
//	}
*/

//****************************************************************


// *************************************************************
/*
	// Buffer Test with NRPN. mf must have one or more NRPN
	CString r;
	HP_Rewind(mf);
	int id, chan, type, time, number, value;
	bool absolute;

	// select one NRPN
	while (HP_ReadEvent(mf,&id,&chan,&time,&type) == HP_ERR_NONE)
	{
		if (type == HP_RPN)
		{
			HP_Select(mf,id);
			HP_ReadRPN(mf,id,&time,&chan,&absolute,&number,&value);
			r.Format("RPN mf: chan %i, time %x , number %x, value %x",chan,time, number, value);
			MessageBox(r);
//			break;
		}
		if (type == HP_NRPN)
		{
			HP_Select(mf,id);
			HP_ReadNRPN(mf,id,&time,&chan,&absolute,&number,&value);
			r.Format("NRPN mf: chan %i, time %x , number %x, value %x",chan,time, number, value);
			MessageBox(r);
//			break;
		}
	}

	HP_Copy(mf); // copy the only NRPN

	// Copy buffer in mfdest

	HP_MIDIFILE_MODULE* mfdest = HP_Init();

	HP_Paste(mfdest, 0x200,HP_NO_DELETE,HP_NO_CHAN);

	HP_Rewind(mfdest);
	while (HP_ReadEvent(mfdest,&id,&chan,&time,&type) == HP_ERR_NONE)
	{
		MessageBox (HP_TypeName(type));
		if (type == HP_RPN)
		{
			HP_ReadRPN(mfdest,id,&time,&chan,&absolute,&number,&value);
			r.Format("RPN mfdest: chan %i, time %x , number %x, value %x",chan,time, number, value);
			MessageBox(r);
		}
		if (type == HP_NRPN)
		{
			HP_ReadNRPN(mfdest,id,&time,&chan,&absolute,&number,&value);
			r.Format("NRPN mfdest: chan %i, time %x , number %x, value %x",chan,time, number, value);
			MessageBox(r);
		}
	}

	HP_Free(mfdest);
*/
// *************************************************************


/*
	// Change Tempo Int
	result = HP_ChangeTempo(mf,HP_ALL_EVENTS,66,HP_PERCENT); 
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/

/*
	// Change Tempo Double
	result = HP_ChangeTempo(mf,HP_ALL_EVENTS,85.2,HP_NO_PERCENT); 
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	// Insert Tempo Double
	result = HP_InsertTempo(mf,8000,90.1); 
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
	
*/
/*	
	// Read Tempo double Test
	HP_Rewind(mf);
	int id, chan, time, type, cps;
	double bpm_double;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_TEMPO)
		{
			HP_ReadTempo(mf,id,&time,&bpm_double,&cps);
			r.Format("Tempo Double: time = %i, BPM = %f ",time, bpm_double);
			MessageBox(r,NULL,MB_OK);
		}
	}
*/

/*
	// Change Velocity
	result = HP_ChangeVelocity(mf,HP_ALL_EVENTS,4,0x4a,110,HP_PERCENT); 
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/
/*
	// Shift
	HP_Select(mf,HP_ALL_EVENTS);
	result = HP_ShiftSelected(mf,-384); 
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
*/
/*
	// ShiftEvent (Shift first Note found)
	HP_Rewind(mf);
	int id, chan, time, type;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_NOTE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			break;
		}
	}
*/
/*
	// Change note, velocity and length (first note found)
	HP_Rewind(mf);
	int id, chan, time, type, note, vel, length;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_NOTE)
		{
			HP_ReadNote(mf,id,&time, &chan,&note,&vel,&length);
			HP_ChangeNote(mf,id,4,note,HP_ABSOLUTE); // change only channel
//			HP_ChangeVelocity(mf,id,4,0x60,80,HP_PERCENT);
//			HP_ChangeNoteLength(mf,id,4,0x60,200,HP_PERCENT);
			break;
		}
	}
*/
/*
	// select and transpose notes
	int last_time;
	HP_GetLastTime(mf,&last_time);
	int end_time = (last_time-4)/2;
	HP_SelectChanTime(mf,4,end_time);
	if (HP_TransposeSelected(mf,2)!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
*/
/*
// Bedesem 
	UINT rtn;
	CString source = "C:\\source.mid";
	rtn = HP_Free(mf);
	mf = NULL;
	mf = HP_Init();
	rtn = HP_Load(mf,source);
	rtn = HP_ChangePPQN(mf,480);
	
    int meas1 = 0;
	int meas2 = 4*480;
	int meas3 = 2*meas2;
	int meas4 = 3*meas2;
	int meas5 = 4*meas2;
	int meas6 = 5*meas2;

//	rtn= HP_SelectChanTime(mf,0,meas3,HP_NO_TIME_LIMIT);
	rtn= HP_SelectChanTime(mf,0,meas3,meas4);

    
	rtn=  HP_Copy(mf);
	rtn = HP_Free(mf);

	HP_MIDIFILE_MODULE* mf1;
	mf1=HP_Init();
	CString fileToLoad= "c:\\dest.mid";
	rtn= HP_Load(mf1, fileToLoad);
	rtn= HP_ChangePPQN(mf1,480);

	int insertTime = meas6;
	rtn = HP_Paste(mf1,insertTime,HP_NO_DELETE,0);

	int id, chan, time, type, length, note, velocity;
	HP_Rewind(mf1);

	while (HP_ReadEvent(mf1,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (time >= meas2) break;
		if (type == HP_NOTE)
		{
			HP_ReadNote(mf1,id,&time, &chan, &note, &velocity, &length);
			CString r;
			r.Format("time = %i, chan = %i, note = %i, velocity = %i, length = %i",
				time, chan+1, note, velocity, length);
			MessageBox(r);
		}
	}


  
	CString tt=  "c:\\dest1.mid";
	rtn= HP_Save(mf1,tt,HP_SMF0);
*/

/*
	UINT rtn;
	rtn = HP_ChangePPQN(mf,1920);
    
	// Statement # 1    rtn= HpSelectChanTime(HPid2,15,122880,HP_NO_TIME_LIMIT) 
 
    rtn= HP_SelectChanTime(mf,15,122880,153600);
 
  
    rtn=  HP_Copy(mf);
	rtn = HP_Free(mf);
	HP_MIDIFILE_MODULE* mf3;
  
	mf3=HP_Init();
	CString fileToLoad= "c:\\PadTemplate.mid";
	rtn= HP_Load(mf3, fileToLoad);
	rtn= HP_ChangePPQN(mf3,1920);
  
 
	int insertTime=0;
	rtn = HP_Paste(mf3, insertTime,HP_NO_DELETE, 15);
	rtn= HP_ChangePPQN(mf3,1920);
  
	CString tt=  "c:\\Test19All.mid";
	rtn= HP_Save(mf3,tt,HP_SMF1);
*/
/*
	// select and quantisize notes
	HP_SelectChanTime(mf,0);
	if (HP_QuantisizeSelected(mf,43,true,true)!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
*/


//	HP_SettleOverlappingNotes(mf,HP_NO_ASSEMBLE);

/*
	// select and shift notes left two quarters
	int ppqn;
	HP_ReadPPQN(mf, &ppqn);
	HP_SelectChanTime(mf,4);
	if (HP_ShiftNotesSelected(mf,-2*ppqn)!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
*/
/*
	HP_SelectChanTime(mf,4);
	if (HP_ChangeVelocitySelected(mf,70,HP_PERCENT)!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
*/
/*
	HP_SelectChanTime(mf,4);
	if (HP_ChangeControllerSelected(mf,HP_MAIN_VOLUME,120,HP_PERCENT)!=HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
*/
/*
	// HP_ChangeController
	int id, chan, time, type, number, value;
	HP_Rewind(mf);
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if ((type==HP_MAIN_VOLUME)||(type==HP_EXPRESSION))
		{
			HP_ReadController(mf,id,&time,&chan,&number,&value);
			r.Format("Controller: chan %i, number %i, value %i",chan,number,value);
			MessageBox(r,NULL,MB_OK);
			HP_ChangeController(mf,id,HP_MAIN_VOLUME,85,HP_NO_PERCENT);
			HP_ReadController(mf,id,&time,&chan,&number,&value);
			r.Format("Controller: chan %i, number %i, value %i",chan,number,value);
			MessageBox(r,NULL,MB_OK);
			break;
		}
	}
*/
/*
	// Read note
	HP_Rewind(mf);
	int id, chan, time, type, note, vel, length;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_NOTE)
		{
			HP_ReadNote(mf,id,&time, &chan,&note,&vel,&length);
			break;
		}
	}
	CString r;
	r.Format("Note: chan %i, note %i, velocity %i, length %i",chan,note,vel,length);
	MessageBox(r,NULL,MB_OK);
*/
/*
	// Read Aftertouch
	HP_Rewind(mf);
	int id, chan, time, type, note, pressure;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_AFTERTOUCH)
		{
			HP_ReadAftertouch(mf,id,&time,&chan,&note,&pressure);
			CString r;
			r.Format("Aftertouch: time %i, chan %i, note %i, pressure %i",time,chan,note,pressure);
			MessageBox(r,NULL,MB_OK);  
			break;
		}
	}
*/
/*
	// Read Controller
	HP_Rewind(mf);
	int id, chan, type, time, number, value;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if ((type&HP_TYPE_MASK)==HP_CONTROLLER_TYPE)
		{
			HP_ReadController(mf,id,&time,&chan,&number,&value);
			CString r;
			r.Format("Controller: time %i, chan %i, number %i, value %i",time,chan,number,value);
			MessageBox(r,NULL,MB_OK);  
			break;
		}
	}
*/
/*
	// Change RPN
	HP_Rewind(mf);
	int id, type, chan, time, number, value;
	bool absolute;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_RPN)
		{
			HP_ChangeRPNAbsolute(mf,id,128,HP_NO_PERCENT);
			break;
		}
	}
*/
/*
	// Read RPN and NRPN
	HP_Rewind(mf);
	int id, type, chan, time, number, value;
	bool absolute;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_RPN)
		{
			HP_ReadRPN(mf,id,&time,&chan,&absolute,&number,&value);
			CString r;
			CString s = absolute?"absolute":"relative";
			r.Format("RPN: time %i, chan %i,msb %i, lsb %i, dmsb %i, dlsb %i, %s",
				time,chan,(number>>7)&0x7f,number&0x7f,(value>>7)&0x7f,value&0x7f,s);
			MessageBox(r,NULL,MB_OK);  
			continue;
		}
		if (type==HP_NRPN)
		{
			HP_ReadNRPN(mf,id,&time,&chan,&absolute,&number,&value);
			CString r;
			CString s = absolute?"absolute":"relative";
			r.Format("NRPN: time %i, chan %i,msb %i, lsb %i, dmsb %i, dlsb %i, %s",
				time,chan,(number>>7)&0x7f,number&0x7f,(value>>7)&0x7f,value&0x7f,s);
			MessageBox(r,NULL,MB_OK);  
			continue;
		}
	}
*/
/*
	{	// Sysex 
		unsigned char data[] =	{0xf0,  0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
										0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0x01,0x02,0x03,
								0xf7};

		result = HP_InsertSysEx(mf,768,data);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;

		}
		
		unsigned char xg_on[] = {0xf0,0x7e, 0x7f, 0x09, 0x01, 0xf7};
		HP_SetFirst(mf);
		result = HP_InsertSysEx(mf, 384, xg_on);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}

		int id, type, chan, time;
		unsigned char *data1;
		int length;

		// Read SysEx
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_SYSEX)
			{
				int i;
				HP_ReadSysex(mf,id,&time,&data1,&length);
				CString r;
				r.Format("SysEx: time %i",time);

				for (i=0; i< length; i++)
				{
					r.Format("%x",data1[i]);
					MessageBox(r,NULL,MB_OK);
				}
				HP_Delete(data1);
				break;
			}
		}

		// Change SysEx
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_SYSEX)
			{
				unsigned char data[] = {0xf0,0x43,0x10,0x4c,0x00,0x00,0x7e,0x00,0xf7};
				HP_ChangeSysex(mf,id,data);
				break;
			}
		}


		// Read SysEx
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_SYSEX)
			{
				int i;
				HP_ReadSysex(mf,id,&time,&data1,&length);
				CString r;
				r.Format("SysEx: time %i",time);

				for (i=0; i< length; i++)
				{
					r.Format("%x",data1[i]);
					MessageBox(r,NULL,MB_OK);
				}
				HP_Delete(data1);
				break;
			}
		}

	}
*/
/*

	{	// SequenceNumber
		int id, type, chan, time;
		int number;

		HP_InsertSequenceNumber(mf,0x9876);

		// Read Sequence Number
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_SEQUENCE_NUMBER)
			{
				HP_ReadSequenceNumber(mf,id,&time,&number);
				CString r;
				r.Format("SequenceNumber: time %i, number %x",time, number);
				MessageBox(r,NULL,MB_OK);
				break;
			}
		}

		// Change Sequence Number
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_SEQUENCE_NUMBER)
			{
				number = 0x1234;
				HP_ChangeSequenceNumber(mf,id,number);
				break;
			}
		}

		// Read Sequence Number
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_SEQUENCE_NUMBER)
			{
				HP_ReadSequenceNumber(mf,id,&time,&number);
				CString r;
				r.Format("SequenceNumber: time %i, number %x",time, number);
				MessageBox(r,NULL,MB_OK);
				break;
			}
		}

	}


	{
		// Read Copyright
		int id, type, chan, time;
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_COPYRIGHT)
			{
				char *text;
				HP_ReadCopyright(mf,id,&time,&text);
				CString r;
				r.Format("Copyright: time %i, %s",time,text);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(text);
				break;
			}
		}
	}

	{
		// Read Text
		int id, type, chan, time;
		HP_Rewind(mf);
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (type==HP_TEXT)
			{
				char* text;
				HP_ReadText(mf,id,&time,&text);
				CString r;
				r.Format("Text: time %i, %s",time,text);
				MessageBox(r,NULL,MB_OK);
				HP_Delete(text);
				break;
			}
		}
	}
	
*/
/*
	// Read Tempo int
	HP_Rewind(mf);
	int id, chan, time, type, bpm, cps;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_TEMPO)
		{
			HP_ReadTempo(mf,id,&time,&bpm,&cps);
			break;
		}
	}
//	CString r;
	r.Format("Tempo Int: %i BPM",bpm);
	MessageBox(r,NULL,MB_OK);



	// Read Tempo double
	HP_Rewind(mf);
//	int id, chan, time, type, cps;
	double bpm_double;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_TEMPO)
		{
			HP_ReadTempo(mf,id,&time,&bpm_double,&cps);
			break;
		}
	}
//	CString r;
	r.Format("Tempo Double: %f BPM",bpm_double);
	MessageBox(r,NULL,MB_OK);
*/
/*
	//  Other Meta
	HP_Rewind(mf);
	int id, chan, time, type;
	while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
	{
		if (type==HP_OTHER_META)
		{
			HP_DeleteEvent(mf,id);
		}
	}
*/

// Play Funktions

	{

		// HP_GetMIDIDevices, HP_SelectMIDIDevice
	
//		HP_DEVICE *devices;
//		int no_devices;

//		UINT result;
//	
//		HP_GetMIDIDevices(&devices,&no_devices);
/*
		int k;
		CString r;
		r.Format("No of MIDI Devices: %i",no_devices);
		MessageBox(r,NULL,MB_OK);

		for (k=0; k<no_devices; k++)
		{
			int id = devices[k].device_id;
			char* name = devices[k].device_name;
			r.Format("ID: %i, Device-Name: %s",id,name);
			MessageBox(r,NULL,MB_OK);
		}
*/


//		result = HP_SelectMIDIDevice(65);
//		result = HP_SelectMIDIDevice(devices[0].device_id);
//		if (result != HP_ERR_NONE)
//		{
//			MessageBox(HP_ErrText(result),NULL,MB_OK);
//			return;
//		}
//		HP_Delete(devices);

/*		
		{
			long duration = HP_Duration(mf);
			CString r;
			r.Format("Duration %i millisec",duration);
			MessageBox(r,NULL,MB_OK);
		}


		// Play entire song

		result = HP_Play(mf,HP_ALL);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
*/

/*
		Sleep(8000);
		result = HP_SetPlayStopWait(mf,HP_WAIT_PLAY);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
		Sleep(5000);
		result = HP_SetPlayStopWait(mf,HP_GO_PLAY);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
		Sleep(4000);
		result = HP_SetPlayStopWait(mf,HP_STOP_PLAY);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}

/*
		{
			Sleep(2000);
			long play_time = HP_PlayTime(mf);
			CString r;
			r.Format("Play-Time after 2 sec: %i millisec",play_time);
			MessageBox(r,NULL,MB_OK);
			Sleep(2000);
			play_time = HP_PlayTime(mf);
			r.Format("Play-Time after 4 sec: %i millisec",play_time);
			MessageBox(r,NULL,MB_OK);
		}
*/
/*
		// Play Selected with events before
		HP_Rewind(mf);
		int id, chan, time, type;
		while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
		{
			if (time < 0x5dc0) continue; // for a long song 0x5dc0, for a short 720
			HP_Select(mf,id);

		}
		
		// HP_SEND_EFFECT_EVENTS_BEFORE
		// HP_NO_SEND_BEFORE		
		result = HP_Play(mf,HP_SELECTED,HP_SEND_EFFECT_EVENTS_BEFORE);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
*/

/*
		result = HP_SetPlayStopWait(mf,HP_WAIT_PLAY);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}

		result = HP_SetPlayStopWait(mf,HP_GO_PLAY);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}

		result = HP_SetPlayStopWait(mf,HP_STOP_PLAY);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}

		result = HP_Play(mf,HP_NO_SEND_BEFORE);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
		}

		result = HP_Play(mf,HP_NO_SEND_BEFORE);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
		}
*/
	}

/*
	while (HP_IsPlaying(mf))
	{
		Sleep(1000);
	}
	MessageBox("Play finished");
*/

	MessageBox("Run Done, next Save, then Close or Close only");

}

void CHP_MIDI_TESTDlg::OnSelectsource() 
{

	static char BASED_CODE szFilter[] = 
		"MIDI Files (*.mid)|*.mid|Style Files (*.sty)|*.sty|Cakewalk SysEx Bank Files (*.syx)|*.syx|All Files (*.*)|*.*||";
//

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
//

/*
	strcpy(sourcefile,"C:\\Users\\Heiko\\Desktop\\test.mid");
*/
	MessageBox("Next : Run");
}

void CHP_MIDI_TESTDlg::OnPlay() 
{
	if (mf==NULL)
	{
		MessageBox("Forgotten: Run");
		return;
	}
	
	if (is_playing &&!HP_IsPlaying(mf))
	{
		is_stop = true;
	}

// Test Copy Paste with Play
/*
	HP_Rewind(mf);
	HP_Select(mf,HP_ALL_EVENTS);
	HP_Copy(mf);
	HP_Free(mf); // close mf
	mf = HP_Init();
	HP_Paste(mf,0,HP_DELETE,HP_ALL_EVENTS);
*/
	UINT result;

	bool with_selected = false;

	if (is_stop)
	{
		if (with_selected)
		{
			MessageBox("Selected");
			int id, type, chan, time;
			HP_Rewind(mf);
			while (HP_ReadEvent(mf,&id,&chan,&time,&type)==HP_ERR_NONE)
			{
				if (time < 8*480) continue;
			
				HP_Select(mf,id);
			}

			// Play selected
			result = HP_Play(mf,HP_SELECTED,HP_SEND_EFFECT_EVENTS_BEFORE);
//			result = HP_Play(mf,HP_SELECTED,HP_NO_SEND_BEFORE);
			if (result != HP_ERR_NONE)
			{
				MessageBox(HP_ErrText(result),NULL,MB_OK);
				return;
			}
		}
		else
		{
			MessageBox("Play all");

			result = HP_Play(mf,HP_ALL);
			if (result != HP_ERR_NONE)
			{
				MessageBox(HP_ErrText(result),NULL,MB_OK);
				return;
			}
		}

		is_stop = false;
		is_pause = false;
		is_playing = true;
	}

	if (is_pause)
	{
		UINT result = HP_SetPlayStopWait(mf,HP_GO_PLAY);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}
		is_playing = true;
		is_pause = false;
		is_stop = false;
	}

	// Test HP_PlayTime, Push Play sometimes
/*
	long pt = HP_PlayTime(mf);
	CString r; r.Format("Playtime %i",pt);
	MessageBox(r);
*/
}

void CHP_MIDI_TESTDlg::OnPause() 
{
	if (mf==NULL)
	{
		MessageBox("Forgotten: Run");
		return;
	}

	UINT result = HP_SetPlayStopWait(mf,HP_WAIT_PLAY);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
	is_stop = false;
	is_pause = true;
	is_playing = false;
}

void CHP_MIDI_TESTDlg::OnStop() 
{
	if (mf==NULL)
	{
		MessageBox("Forgotten: Run");
		return;
	}

	if (is_playing)
	{
		UINT result = HP_SetPlayStopWait(mf,HP_STOP_PLAY);
		if (result != HP_ERR_NONE)
		{
			MessageBox(HP_ErrText(result),NULL,MB_OK);
			return;
		}

		is_playing = false;
		is_pause = false;
		is_stop = true;
	}
}

void CHP_MIDI_TESTDlg::OnExit() 
{
//	UINT result;

	if (mf!=NULL)
	{
		if (is_pause)
		{
			MessageBox("Push Play");
			return;
		}

		if (is_playing && HP_IsPlaying(mf))
		{
			MessageBox("Push Stop");
			return;
		}
	}

	if (pm != NULL) delete pm;

	MessageBox("HP_MIDI_TEST finished");

	CDialog::OnOK();
	if (hl != NULL)
	{
		FreeLibrary(hl);
		hl = NULL;
	}
}

void CHP_MIDI_TESTDlg::OnOriginalTempo() 
{
	UINT result;
	if (mf==NULL)
	{
		MessageBox("Forgotten: Run");
		return;
	}
	result = HP_PlayTempo(mf);
	if(result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
}

void CHP_MIDI_TESTDlg::OnHalfTempo() 
{
	UINT result;
	if (mf==NULL)
	{
		MessageBox("Forgotten: Run");
		return;
	}
	result = HP_PlayTempo(mf,50);
	if(result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
}

void CHP_MIDI_TESTDlg::OnDoubleTempo() 
{
	UINT result;
	if (mf==NULL)
	{
		MessageBox("Forgotten: Run");
		return;
	}
	result = HP_PlayTempo(mf,200);
	if(result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}
}

void CHP_MIDI_TESTDlg::OnSave() 
{
	UINT result;
	CString m_destination;

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
		m_destination = openfile_dlg.m_ofn.lpstrFile;
	}
	else 
	{
		if (return_value == IDCANCEL)
		{
			m_destination = "";
			return;
		}
	}

	CString file = m_destination;	

	CString destfname = m_destination; // convert to CString

	if (m_destination == "") return;
	
	result = HP_Save(mf,m_destination,HP_SMF0);
//	result = HP_Save(mf,m_destination,HP_SMF1);

	if(result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}

	MessageBox("Save OK, next Close");
	
}


void CHP_MIDI_TESTDlg::OnSetTime() 
{
	int new_time = GetDlgItemInt(IDC_START_TIME);

	UINT result = HP_SetPlayTime(mf,new_time);
	if(result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
}

void CHP_MIDI_TESTDlg::OnSendSingleEvent() 
{
	int param;
	int length;
	int result;

//	MessageBox("Send");

	param = GetDlgItemInt(IDC_PARAMETER);

	if (mf==NULL)
	{
		MessageBox("Forgotten: Run");
		return;
	}
	if (!is_playing)
	{
		MessageBox("Forgotten: Play");
		return;
	}

	if (is_master_volume)
	{
		length = 9;
		unsigned char event_data[9] =
			{0xf0,0x43,0x10,0x4c,0x00,0x00,0x04,0x40,0xf7}; // Master Volume 64
		event_data[7] = param;
		result = HP_PlaySingleEvent(mf, event_data, length);
	}

	if (is_ctrl7)
	{
		length = 3;
		unsigned char event_data[3] = 
			{0xb0, 0x07, 0x40}; // ctrl. 7, chan 0, value 64
		event_data[2] = param;
		result = HP_PlaySingleEvent(mf, event_data, length);
	}
	
	if(result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
		return;
	}
}

void CHP_MIDI_TESTDlg::OnMasterVolumeCheck() 
{
	if (m_master_volume.GetCheck())
	{
		m_ctrl7.SetCheck(false);
		is_master_volume = true;
		is_ctrl7 = false;
		SetDlgItemInt(IDC_PARAMETER,127);
	}
	else
	{
		m_ctrl7.SetCheck(true);
		is_master_volume = false;
		is_ctrl7 = true;
		SetDlgItemInt(IDC_PARAMETER,64);
	}	
}

void CHP_MIDI_TESTDlg::OnCtrl7Check() 
{
	if (m_ctrl7.GetCheck())
	{
		m_master_volume.SetCheck(false);
		is_master_volume = false;
		is_ctrl7 = true;
		SetDlgItemInt(IDC_PARAMETER,64);
	}
	else
	{
		m_master_volume.SetCheck(true);
		is_master_volume = true;
		is_ctrl7 = false;
		SetDlgItemInt(IDC_PARAMETER,127);
	}	
}

void CHP_MIDI_TESTDlg::OnClose() 
{
	int result;
	result = HP_Free(mf);
	if (result != HP_ERR_NONE)
	{
		MessageBox(HP_ErrText(result),NULL,MB_OK);
	}

	mf = NULL;

	if (hl != 0)
	{
		FreeLibrary(hl);
		hl = NULL;
	}

	MessageBox("Close done, next Load or Exit",NULL,MB_OK);

}

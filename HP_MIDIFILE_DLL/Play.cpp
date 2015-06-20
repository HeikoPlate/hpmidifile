// Play.cpp: Implementierung der Klasse Play.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Play.h"
#include "MetaEvent.h"
#include "midifile.h"

HMIDIOUT PlayClass::midi_handle = NULL;

int PlayClass::playlist_index = -1;
int PlayClass::remainded_sleep_time = 0;
CBaseReferenceClock *PlayClass::irc = NULL;

const int PlayClass::max_sleep_time		= 200;
const int PlayClass::delta_ticks_sysex	= 5;   // ticks after an sysex-event
const int PlayClass::delta_ticks_else	= 2;   // ticks after an controller and program change

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

PlayClass::PlayClass(MIDIFile* midifile)
{
	mf = midifile; // save actual instance of MIDIFile
	playlist = NULL;
	reduced_playlist = NULL;
	basic_playlist = NULL;

	// initialize sound_off_list;
	sound_off_list = new Play_Array;
	unsigned char sound_off[3] = {0xb0, 120, 0x00};
	int i;
	for (i=0; i<16; i++)
	{
		sound_off[0] = 0xb0 + i;
		sound_off_list->add(MIDIEVENT,NO_TAKT,NO_BEAT,0,5,sound_off,3); // time 0 not correct, but not needed
		chan_transpose_list[i] = false;
		chan_mute_list[i] = false;
	}
	sound_off_list_index = -1; // default: not used
	new_play_start_time = -1;
	last_new_play_start_time = -1;
	tick_number = 0;
	play_tempo_factor = 10000;
	transpose_steps = 0;
	single_event_data[0] = 0;
	single_event_length = 0;

	pm_instance = NULL;
	corr_time = 0;

	m_dwAdvise = 0;
	hThread = NULL;
	is_playing = false;

	HRESULT hr;

	if (irc == NULL)
	{	// only one instance of CBaseReferenceClock!!
		irc = new CBaseReferenceClock("HP_MIDIFILE_Ireferenceclock",NULL,&hr,NULL);
	}

	midiHdr_unprepared = true;
}

PlayClass::PlayClass()
{
}

PlayClass::~PlayClass()
{
	while (is_playing)
	{
		Sleep(100);
	}

	if (basic_playlist != NULL) delete basic_playlist;
	if (reduced_playlist != NULL) delete reduced_playlist;
	if (sound_off_list != NULL) delete sound_off_list;

	if (hThread != NULL)
	{
		CloseHandle(hThread);
		hThread = NULL;
	}

	if (!midiHdr_unprepared)
	{
		midiOutUnprepareHeader(midi_handle, &midiHdr, sizeof(MIDIHDR));
		midiHdr_unprepared = true;
	}
}

//**************************************************************************
//*
//*	to set stop or wait play
//*
//**************************************************************************

UINT PlayClass::set_play_stop_wait(int modus)
{
//	mf->message("set_stop_wait, modus %i",modus);
	if (!is_playing) return HP_ERR_PLAY;
	play_stop_wait_marker = modus;

	if ((modus == HP_WAIT_PLAY) || (modus == HP_STOP_PLAY))
	{
		sound_off_list_index = 0; // play sound_off_list initiated
	}

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*	to set new play time
//*
//**************************************************************************

UINT PlayClass::set_play_time(int new_time)
{
//	mf->message("set_play_time, new_time %i",new_time);
	if (!is_playing) return HP_ERR_PLAY; 

	new_play_start_time = new_time;
	sound_off_list_index = 0; // play sound_off_list initiated

	return HP_ERR_NONE;
}


//**************************************************************************
//*
//*	to get play_time
//*
//**************************************************************************

long PlayClass::get_play_time()
{
	return play_time;
}

//**************************************************************************
//*
//*	to get the actual tick number of play
//*
//**************************************************************************

long PlayClass::get_tick_number()
{
	return tick_number;
}

//**************************************************************************
//*
//*	to change the play tempo
//*
//**************************************************************************

UINT PlayClass::play_tempo(int percent)
{
	if ((percent < 10) || (percent > 400))
	{
		return HP_ERR_PLAY;
	}
	
	play_tempo_factor = (int)((10000./(double)percent)*100.+0.5);
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*	transpose notes while playing
//*
//**************************************************************************

UINT PlayClass::play_transpose(int steps, const bool transp_list[16])
{
	int i;

	if ((steps < -12) || (steps > 12))
	{
		return HP_ERR_PLAY;
	}

	for (i=0; i<16; i++)
	{
		chan_transpose_list[i] = transp_list[i];
	}

	transpose_steps = steps;
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*	send single midievent while playing
//*
//**************************************************************************

UINT PlayClass::play_single_event(unsigned char* event_data, int length)
{
	int i;

	if ((length<=0)||(length>=512)||!is_playing)
	{
		return HP_ERR_PLAY;
	}

	for (i=0; i<length;i++)
	{
		single_event_data[i] = event_data[i];
	}
	single_event_length = length;

	return HP_ERR_NONE;
}


//**************************************************************************
//*
//*	to mute notes while playing
//*
//**************************************************************************

UINT PlayClass::play_mute(const bool mute_list[16])
{
	int i;

	for (i=0; i<16; i++)
	{
		chan_mute_list[i] = mute_list[i];
	}

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*	add all controller and sysex to basic_playlist 
//* before the first selected event. Called from create_basic_list
//*
//**************************************************************************

UINT PlayClass::add_before_selected(int *first_tme_selected)
{
	int delta_ticks;
	int event_time = 0;
	int ms_delta_time = 0;
	
	int i, k;
	MIDIEvent *mev;

	// find out time of first selected event
	int first_time_selected = (mf->eventlist->get_last())->resulting_time; // default
	for (i=0; i<mf->eventlist->get_length(); i++)
	{
		mev = mf->eventlist->get(i);
		if (mev->selected > 0)
		{
			first_time_selected = mev->resulting_time;
			break;
		}
	}
	*first_tme_selected = first_time_selected;

	// create basic_playlist
	if (basic_playlist == NULL) return HP_ERR_PLAY;	

	for (i=0; i < mf->eventlist->get_length(); i++)
	{
		mev = mf->eventlist->get(i);
		if (mev->resulting_time >= first_time_selected) break;

		// send controller and sysex only		
		unsigned char status = mev->event_data[0]; 
		if ( (status != 0xf0) &&			// sysex
			 ((status&0xf0) != 0xb0) &&		// controller
			 ((status&0xf0) != 0xc0)    )	// program change
		{
			continue;
		}

		unsigned char *data = new unsigned char[mev->event_data_length];
		bool sysex = (mev->event_data[0] == 0xf0);
		
		// skip length bytes in sysex 
		int index = 0;
		int length = 0;
		for (k=0; k<mev->event_data_length; k++)
		{
			if (sysex && (k==1))
			{ 
				int j = 0;
				while ((mev->event_data[k+j]&0x80) != 0)
				{
					j++;
				}
				k += j+1;
			}
			data[index]= mev->event_data[k];
			index++;
			length++;
		}

		event_time = mev->resulting_time;
		// add play-event in basic_playlist. basic_playlist is always sorted
		basic_playlist->add(MIDIEVENT,NO_TAKT,NO_BEAT,event_time, ms_delta_time, data, length);
		// free data, each time new one in loop
		delete[] data;

		delta_ticks = sysex?delta_ticks_sysex:delta_ticks_else;
		ms_delta_time = (mf->tempolist)->millisec_delta_time(0,delta_ticks);
	}
	return HP_ERR_NONE;
}


//**************************************************************************
//*
//*	create the basic playlist, consider selected 
//*
//**************************************************************************

UINT PlayClass::create_basic_playlist()
{
	int i, k;
	MIDIEvent *mev;
	int last_time = -1;

	bool add_to_playlist;

	const bool with_pm = (pm_instance!=NULL);

	int ppqn;
	mf->read_ppqn(&ppqn);
	int beats_per_takt = 4;		// init: num = 4
	int ticks_per_beat = ppqn;	// init: denum = 4
	int act_takt = 0;
	int act_beat = 0;
	int next_time_of_beat = 0;
	int first_time_selected = 0;

	int millisec_delta_time;

	// create basic_playlist
	if (basic_playlist != NULL) delete basic_playlist;	
	basic_playlist = new Play_Array();

	if ((selected==HP_SELECTED) && (send_before==HP_SEND_EFFECT_EVENTS_BEFORE))
	{
		add_before_selected(&first_time_selected);
	}


	for (i=0; i < (mf->eventlist)->get_length(); i++)
	{
		add_to_playlist = true; // default

		mev = mf->eventlist->get(i);

		int mev_time = mev->resulting_time;

		if (mev_time < first_time_selected) continue;
		if ((selected==HP_SELECTED) && (mev->selected==0)) continue;

		unsigned char status = mev->event_data[0];
		if (last_time == -1) last_time = mev_time;

		if (with_pm)
		{
			while (next_time_of_beat <= mev_time)
			{
				int next_time = 
					next_time_of_beat<mev_time?next_time_of_beat:mev_time;
				millisec_delta_time = 
					(mf->tempolist)->millisec_delta_time(last_time,next_time);
				last_time = next_time_of_beat;
				basic_playlist->add(TAKTBEAT,act_takt, act_beat, next_time_of_beat, millisec_delta_time,NULL,0);
				next_time_of_beat += ticks_per_beat;
				act_beat++;
				if (act_beat >= beats_per_takt)
				{
					act_takt++;
					act_beat = 0;
				}
			}
		}

		if ( (status&0xf0) == 0xf0)
		{
			if (status!=0xf0)	
			{   // don't send system-common- and meta-events
				// analyse time-signure if needed
				add_to_playlist = false;
			}
		}

		if ((mev->type==HP_TIME_SIGNATURE) && with_pm)	// only called if needed
		{
			TimeSignatureEvent *tse = (TimeSignatureEvent*)mev;	

			beats_per_takt = tse->num;
			int denum = tse->denum;
			
			next_time_of_beat -= ticks_per_beat;
			if (next_time_of_beat < 0) next_time_of_beat = 0;


			ticks_per_beat =	denum==4?ppqn:
								denum==2?2*ppqn:
								denum==1?4*ppqn:
								denum==8?ppqn/2:
								denum==16?ppqn/4:
								denum==32?ppqn/8:
								ppqn;

			next_time_of_beat += ticks_per_beat;

			add_to_playlist = false;
		}

		if ((mev->type==HP_CHORD_NAME) && with_pm)	// only called if needed
		{
			ChordNameEvent* cne = (ChordNameEvent*) mev;
			millisec_delta_time = 
					(mf->tempolist)->millisec_delta_time(last_time,mev_time);
			basic_playlist->add(CHORD,NO_TAKT,NO_BEAT, mev_time, 
				          millisec_delta_time, (unsigned char*)(cne->chord), strlen(cne->chord)+1);
			
			last_time = mev_time;

			continue;
		}

		if ((mev->type==HP_SYSEX) && with_pm)  // look for chord-sysex
		{
			unsigned char* data = mev->event_data;
			
			if (mev->event_data_length == 10)
			{
				if ((data[2]==0x43)&&(data[3]==0x7e)&&(data[4]==0x02))
				{	// yamaha chord sysex 
					int cr = data[5];
					int ct = data[6];
					int bn = data[7];
					int bt = data[8];
					char * chord_text;
					ChordNameEvent::convert_chord_to_text(cr,ct,bn,bt,&chord_text);
					millisec_delta_time = 
						(mf->tempolist)->millisec_delta_time(last_time,mev_time);
					basic_playlist->add(CHORD,NO_TAKT,NO_BEAT, mev_time, 
				          millisec_delta_time, (unsigned char *)chord_text, strlen(chord_text)+1);
					
					delete chord_text;
					last_time = mev_time;
				}
			}
		}
			
		if ((mev->type==HP_LYRIC) && with_pm)	// only called if needed
		{
			LyricEvent* le = (LyricEvent*) mev;
			millisec_delta_time = 
					(mf->tempolist)->millisec_delta_time(last_time,mev_time);

			int time, chan;
			char * lyric_text;
			mf->read_text_from_meta(i,&time,HP_LYRIC,&chan,&lyric_text);
			
			basic_playlist->add(LYRIC,NO_TAKT,NO_BEAT, mev_time, 
				          millisec_delta_time, (unsigned char *)lyric_text, strlen(lyric_text)+1);
			
			last_time = mev_time;

			continue;
		}

		if (add_to_playlist)
		{
			unsigned char *data = new unsigned char[mev->event_data_length];
			bool sysex = (mev->event_data[0] == 0xf0);

			// skip length bytes in sysex 
			int index = 0;
			int length = 0;
			for (k=0; k<mev->event_data_length; k++)
			{
				if (sysex && (k==1))
				{ 
					int j = 0;
					while ((mev->event_data[k+j]&0x80) != 0)
					{
						j++;
					}
					k += j+1;
				}
				data[index]= mev->event_data[k];
				index++;
				length++;
			}
			
			// no of millisecs since preceeding event
			// convert difference of ticks to millisecs
			millisec_delta_time = 
				(mf->tempolist)->millisec_delta_time(last_time,mev_time);
			
			// add play-event in basic_playlist. basic_playlist is always sorted
			basic_playlist->add(MIDIEVENT,NO_TAKT,NO_BEAT, mev_time, millisec_delta_time, data, length);
			
			// free data, each time new one in loop
			delete[] data;
			last_time = mev_time;
		}
	}

	// add note off for all channels at the end
	unsigned char note_off[3] = {0xb0, 123, 0x00};
	last_time += 240;
	for (i=0; i<16; i++)
	{
		note_off[0] = 0xb0 + i;
		basic_playlist->add(MIDIEVENT,NO_TAKT,NO_BEAT,last_time,5,note_off,3);
		last_time++;
	}

	play_time = 0;
	tick_number = 0;

	// initialise start timer
	playlist_index = -1;
	remainded_sleep_time = 0;
	delay_ms = 0;

	return HP_ERR_NONE;
}


//**************************************************************************
//*
//*	play selected or all events, creates a thread
//*
//**************************************************************************


UINT PlayClass::play(int sel, bool snd_before)
{
	selected = sel;
	send_before = snd_before;

	UINT result;

	if (is_playing) return HP_ERR_ALLOCATED_DEVICE;

	// Open selected MIDI Out device, it will be closed in the thread 
	result =  midiOutOpen(&midi_handle, mf->MIDI_device, 0, 0, CALLBACK_NULL);
	if (result != MMSYSERR_NOERROR)
	{
		play_stop_wait_marker = HP_GO_PLAY;

		switch (result)
		{
		case MIDIERR_NODEVICE:		return HP_ERR_PLAY;
		case MMSYSERR_ERROR:		return HP_ERR_ALLOCATED_DEVICE;
		case MMSYSERR_ALLOCATED:	return HP_ERR_ALLOCATED_DEVICE;
		case MMSYSERR_BADDEVICEID:	return HP_ERR_MIDI_DEVICE;
		case MMSYSERR_NOMEM:		return HP_ERR_MEMORY;
		case MMSYSERR_INVALPARAM:	return HP_ERR_PLAY;
		default:					return HP_ERR_PLAY;
		}
	}

	create_basic_playlist();

	hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)thread_player,this,0,&threadID);
	SetThreadPriority(hThread,THREAD_PRIORITY_HIGHEST);
    return HP_ERR_NONE;
}


//**************************************************************************
//*
//*	thread player, thread start routine 
//*
//**************************************************************************

void PlayClass::thread_player(PlayClass *pl)
{
/*
	if (!pl->thread_prepare_playlist())
	{
		ExitThread(HP_ERR_PLAY);
	}
*/
	pl->playlist = pl->basic_playlist;

	pl->is_playing = true;
	while (pl->delay_ms >= 0)
	{
		if (pl->single_event_length !=0)
		{ // insert sending single_event
			int save_delay_ms = pl->delay_ms;
			pl->delay_ms = 100;
			pl->thread_wait();
//			MIDIFile::message("[0]= %x, [1] = %x, [2] = %x, length = %i",
//				pl->single_event_data[0], pl->single_event_data[1], pl->single_event_data[2],
//				pl->single_event_length);
			pl->thread_midi_single_event(pl->single_event_data,	pl->single_event_length);
//			MIDIFile::message("[0]= %x, [1] = %x, [2] = %x, length = %i",
//				pl->single_event_data[0], pl->single_event_data[1], pl->single_event_data[2],
//				pl->single_event_length);
			pl->delay_ms = 200;
			pl->thread_wait();
			pl->single_event_data[0] = NULL;
			pl->single_event_length = 0;
			pl->delay_ms = save_delay_ms;
		}

		pl->thread_wait();
		pl->thread_send_next_event(); // an error or end is the same: delay_ms < 0
		if (pl->delay_ms < 0)
		{
			if (pl->new_play_start_time < 0) break;

			// continue at another time
			
			PlayEvent* pev = (pl->playlist)->get(pl->playlist_index);
			int act_event_time = pev->event_time;

			
			if (pl->new_play_start_time > 0)
			{	// new playlist needed: Create reduced playlist and copy pointer to playlist
				if (pl->last_new_play_start_time!= pl->new_play_start_time)
				{	   
					pl->last_new_play_start_time= pl->new_play_start_time;
					pl->thread_create_reduced_playlist(pl->new_play_start_time,act_event_time);
					pl->playlist = pl->reduced_playlist;
				}
				// else: reduced_playlist is OK, use it once more
			}
			else
			{	// start from 0: Use basic_playlist
				pl->playlist = pl->basic_playlist;
			}
			// initialize variables
			if (pl->new_play_start_time < act_event_time)
			{
				pl->play_time = 0;
			}
			pl->new_play_start_time = -1; 
			pl->delay_ms = 0;
			pl->playlist_index = -1;
			pl->corr_time = 0;
			pl->tick_number = 0;
			pl->remainded_sleep_time = 0;
			pl->thread_stop_play();
			pl->is_playing = true;
			pl->play_stop_wait_marker = HP_GO_PLAY; 
		}
	}

	pl->thread_stop_play();

	if (midi_handle != NULL)
	{
		midiOutClose(midi_handle);
		midi_handle = NULL;
	}

	pl->playlist = NULL;
	if (pl->basic_playlist != NULL) 
	{
		delete pl->basic_playlist;
		pl->basic_playlist = NULL;
	}
	if (pl->reduced_playlist != NULL) 
	{
		delete pl->reduced_playlist;
		pl->reduced_playlist = NULL;
	}

	ExitThread(HP_ERR_NONE);
}

//**************************************************************************
//*
//*	thread: create reduced playlist to play from new start-time  
//* the function copies all relevant events from basic_playlist into 
//* reduced_playlist. reduced_playlist are played after copy of the
//* pointer into playlist
//*
//**************************************************************************

bool PlayClass::thread_create_reduced_playlist(int new_time, int call_time)
{
	if (basic_playlist==NULL) return false;
	if (reduced_playlist != NULL)
	{
		delete reduced_playlist;
	}
	reduced_playlist = new Play_Array;


	int i;

	PlayEvent* pev; 
		
	for (i=0; i<basic_playlist->get_length(); i++)
	{
		pev = basic_playlist->get(i);

		if (new_time <= call_time)
		{
			if (pev->event_time < new_time)
			{
				if (pev->type != MIDIEVENT) continue;
				unsigned char status = pev->data[0];
				if ((status!=0xf0) && 
					((status&0xf0)!=0xb0) && 
					((status&0xf0)!=0xc0))   
				{
					continue;
				}
				else
				{
					reduced_playlist->add(pev);
					continue;
				}
			}
			else
			{
				reduced_playlist->add(pev);
				continue;
			}
		}
		else
		{
			if (pev->event_time < call_time) continue;
			if (pev->event_time < new_time)
			{
				if (pev->type != MIDIEVENT) continue;
				unsigned char status = pev->data[0];
				if ((status!=0xf0) && 
		  		   ((status&0xf0)!=0xb0) && 
				   ((status&0xf0)!=0xc0))   
				{
					continue;
				}
				else
				{
					reduced_playlist->add(pev);
					continue;
				}
			}
			else
			{
				reduced_playlist->add(pev);
				continue;
			}
		}
	}

	int delta_ticks = 0;
	for (i=0; i<reduced_playlist->get_length(); i++)
	{
		pev = reduced_playlist->get(i);
		pev->millisec_delta_time = delta_ticks;

		unsigned char status = pev->data[0];
		if ((pev->type == MIDIEVENT) &&
			(status!=0xf0) && 
  		   ((status&0xf0)!=0xb0) && 
		   ((status&0xf0)!=0xc0))
		{
			break;
		}
		delta_ticks = ((pev->type == MIDIEVENT)&&(status==0xf0))? delta_ticks_sysex:delta_ticks_else;
	}
	return true;
}

//**************************************************************************
//*
//*	thread: wait   
//*
//**************************************************************************


DWORD PlayClass::thread_wait()
{
	HRESULT hr;
	if (delay_ms>0)
	{
		REFERENCE_TIME before_wait, after_wait;
		REFERENCE_TIME wait_time = 
			play_tempo_factor * delay_ms - corr_time;
		if (wait_time <= 0)
		{	// return immediately
			corr_time += wait_time; // reduce corr_time
			return 1;
		}

		// wait implemented by advise and statement-loop
		REFERENCE_TIME advise_time;

		if (wait_time >= 5 * 10000)
		{
			advise_time = wait_time - 4 * 10000;
		}
		else
		{
			advise_time = 0;
		}

		irc->GetTime(&before_wait);
		if (advise_time > 0)
		{
			m_EventAdvise.Reset();
			hr = irc->AdviseTime(before_wait, 
							     advise_time,
					             (HEVENT)(HANDLE) m_EventAdvise,
						         &m_dwAdvise);

			if (SUCCEEDED(hr))
			{
				m_EventAdvise.Wait();
			}
			else
			{
//				MIDIFile::message("thread_wait: AdviseTime not succeeded");
				delay_ms = -1;
				return 1;
			}
		}
		irc->GetTime(&after_wait);
		
		if (after_wait-(before_wait+wait_time) < 0)
		{
			REFERENCE_TIME actual_time = after_wait;
			while (actual_time-(before_wait+wait_time) < 0)
			{
				irc->GetTime(&actual_time);
			}
			corr_time = 0;
		}
		else
		{
			corr_time = after_wait-(before_wait+wait_time); // timer-delay-time
		}
	}
	return 1;
}

//**************************************************************************
//*
//*	thread: stop  
//*
//**************************************************************************


void PlayClass::thread_stop_play()
{
	if (pm_instance != NULL)
	{
		pm_instance->HP_PlayTBMonitor(-1,-1);

		char * empty_text_ch = new char[1];
		empty_text_ch[0] = 0;
		pm_instance->HP_PlayChMonitor(empty_text_ch);

		char * empty_text_ly = new char[1];
		empty_text_ly[0] = 0;
		pm_instance->HP_PlayLyMonitor(empty_text_ly);
	}

	play_stop_wait_marker = HP_GO_PLAY;

	is_playing = false;
}


//**************************************************************************
//*
//*	thread: send next event  
//*
//**************************************************************************


void PlayClass::thread_send_next_event()
{
	if ((playlist == NULL) || (midi_handle==0))
	{
		delay_ms = -1;
		return;
	}

	// the first call
	if (playlist_index == -1)
	{
		tick_number = 0;
		play_time = 0;
		playlist_index++;
	}
	
	// the last call
	if (playlist_index >= playlist->get_length())
	{
		delay_ms = -1;
		return;
	}
	
	// look, if STOP or WAIT wanted
	if (sound_off_list_index >= 0)
	{	// yes, wanted; send sound off for all channels
		if (sound_off_list_index >= sound_off_list->get_length())
		{
			sound_off_list_index = -1; // all sound off events passed
		}
		else
		{	// send an sound-off event of sound_off_list
			if (!thread_midi_out_list_event(sound_off_list,sound_off_list_index))
			{
				delay_ms = -1;
				return;
			}
			sound_off_list_index++;
			if (sound_off_list_index >= sound_off_list->get_length())
			{
				sound_off_list_index = -1; // all sound off events passed
			}
			else
			{
				int sleep_time = sound_off_list->delta_time_millisec(sound_off_list_index);
				delay_ms = sleep_time;
				play_time += (int)((((double)play_tempo_factor)*((double)delay_ms))/10000.+0.5);
				return;
			}
		}
	}
	
	if (play_stop_wait_marker == HP_STOP_PLAY)
	{	delay_ms = -1;
		return;
	}

	if (play_stop_wait_marker == HP_WAIT_PLAY)
	{
		remainded_sleep_time += max_sleep_time; // initiate wait
	}

	if (new_play_start_time >=0)
	{
		if (delay_ms >= 0)
		{
			delay_ms = -1;
			return;
		}
	}

	if (remainded_sleep_time > 0)
	{	// works for wait and partitioned long sleep times
		int sleep_time = (remainded_sleep_time > max_sleep_time)?max_sleep_time:
		                       remainded_sleep_time;
		remainded_sleep_time -= sleep_time;
		delay_ms = sleep_time;
		play_time += (int)((((double)play_tempo_factor)*((double)delay_ms))/10000.+0.5);

		return;
	}
	
	// call with guilty play_list_index
	{ // sleep has gone, send MIDI-Event

		if (!thread_midi_out_list_event(playlist,playlist_index))
		{
			delay_ms = -1;
			return;
		}

		tick_number = playlist->event_time(playlist_index);
		playlist_index++;

		if (playlist_index >= playlist->get_length())
		{
			delay_ms = -1;
			return;
		}

		int sleep_time = playlist->delta_time_millisec(playlist_index);

		if (sleep_time > max_sleep_time)
		{
			remainded_sleep_time = sleep_time - max_sleep_time;
			sleep_time = max_sleep_time;
		}

		if (play_stop_wait_marker == HP_STOP_PLAY)
		{
			delay_ms = -1;
			return;
		}
		delay_ms = sleep_time;

		play_time += (int)((((double)play_tempo_factor)*((double)delay_ms))/10000.+0.5);
		return;
	}
	return;
}


//**************************************************************************
//*
//*	thread: send single event to midi  
//*
//**************************************************************************


bool PlayClass::thread_midi_single_event(unsigned char* event_data, int length)
{
//	MIDIFile::message("thread_midi_single_event; [0]= %x, [1] = %x, [2] = %x, single_event_length = %i",
//		single_event_data[0], single_event_data[1], single_event_data[2],
//		length);

	
	UINT			result;
	union 
	{
		DWORD			dwData;
		unsigned char	bData[4];
	} u;
		
	int k;
		
	if (length <= 0) return HP_ERR_NONE; // must be an error

	if (length <= 4)
	{
		// construct the MIDI short message 
		for (k=0; k<4; k++) u.bData[k] = 0;
		for (k=0; k<length; k++) u.bData[k] = event_data[k];

		int status = event_data[0]&0xf0;
		int chan = event_data[0]&0x0f;

		// look for channel mute while playing
		if (!(chan_mute_list[chan] && (status==0x90)))
		{
			// output the short event
			result = midiOutShortMsg(midi_handle, u.dwData);
	
			if (result !=MMSYSERR_NOERROR)
			{
				play_stop_wait_marker = HP_GO_PLAY;
				MessageBox(NULL,"Error midiOutShortMessage",NULL,MB_OK);
				is_playing = false;
				return false;
			}
		}
	}
	else
	{
		// unprepare  from last use
		if (!midiHdr_unprepared)
		{
			// Unprepare the buffer and MIDIHDR 
			if (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(midi_handle, &midiHdr, sizeof(MIDIHDR)))
			{
				int i = 0;
				Sleep(10);
				while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(midi_handle, &midiHdr, sizeof(MIDIHDR)))
				{
					i++;
					if (i==50) break;
					Sleep(10);
				}
				if (i==50)
				{
					MessageBox(NULL,"ErrorUnprepareHeader",NULL,MB_OK);
					is_playing = false;
					return false;
				}
			}
			midiHdr_unprepared = true;
		}

		// Store pointer in MIDIHDR 
		midiHdr.lpData = (LPSTR)event_data;
		// Store its size in the MIDIHDR 
		midiHdr.dwBufferLength = length;
		// Flags must be set to 0 
		midiHdr.dwFlags = 0;

		// Prepare the buffer and MIDIHDR 
		result = midiOutPrepareHeader(midi_handle,  &midiHdr, sizeof(MIDIHDR));

		if (result != MMSYSERR_NOERROR)
		{
			play_stop_wait_marker = HP_GO_PLAY;
			MessageBox(NULL,"Error midiOutPrepareHeader",NULL,MB_OK);
			is_playing = false;
			return false;
		}
		midiHdr_unprepared = false;

		// Output the SysEx message. Note that this could return immediately if
		// the device driver can output the message on its own in the background.
		// Otherwise, the driver may make us wait in this call until the entire
		//  data is output

		play_stop_wait_marker = HP_WAIT_PLAY;
		result = midiOutLongMsg(midi_handle, &midiHdr, sizeof(MIDIHDR));
		play_stop_wait_marker = HP_GO_PLAY;

		if (result != MMSYSERR_NOERROR)
		{
			MessageBox(NULL,"Error midiOutLongMessage",NULL,MB_OK);
			is_playing = false;
			return false;
		}
	}
	return true;
}

//**************************************************************************
//*
//*	thread: send to midi  
//*
//**************************************************************************


bool PlayClass::thread_midi_out_list_event(Play_Array *list, int list_index)
{
	int type = list->type(list_index);
	
	// Look for user takt/beat monitoring
	if (pm_instance != NULL)
	{
		if (type==TAKTBEAT)
		{
			int takt = list->takt(list_index);
			int beat = list->beat(list_index);
			pm_instance->HP_PlayTBMonitor(takt,beat);
			return true;
		}
		if (type==CHORD)
		{
			int length;
			char * list_data = (char *)(list->data(list_index,&length));
			char * chord_text = new char[length];
			strcpy(chord_text,list_data);
			pm_instance->HP_PlayChMonitor(chord_text);
			return true;
		}
		if (type==LYRIC)
		{
			int length;
			char * list_data = (char *)(list->data(list_index,&length));
			char * lyric_text = new char[length];
			strcpy(lyric_text,list_data);
			pm_instance->HP_PlayLyMonitor(lyric_text);
			return true;
		}
	}
	
	if (type==MIDIEVENT)
	{
		UINT			result;
		
		union 
		{
			DWORD			dwData;
			unsigned char	bData[4];
		} u;
		
		int k;
		unsigned char* data;
		int length;
		
		data = (list)->data(list_index,&length);
		
		if (length <= 0) return HP_ERR_NONE; // must be an error

		if (length <= 4)
		{
			// construct the MIDI short message 
			for (k=0; k<4; k++) u.bData[k] = 0;
			for (k=0; k<length; k++) u.bData[k] = data[k];

			int status = data[0]&0xf0;
			int chan = data[0]&0x0f;

			// look for transpose while playing			
			if (transpose_steps != 0)
			{
				if ((status==0x80)||(status==0x90))
				{
					if (chan_transpose_list[chan])
					{   // only transpose melody channel
						int new_note = data[1]+transpose_steps;
						if ((new_note < 0)||(new_note > 127))
						{
							if (status==0x90)
							{
								u.bData[2] = 1; // velocity quiet
							}
						}
						else
						{
							u.bData[1] = new_note;
						}
					}
				}
			}

			// look for channel mute while playing
			if (!(chan_mute_list[chan] && (status==0x90)))
			{
				// output the short event
				result = midiOutShortMsg(midi_handle, u.dwData);
			
				if (result !=MMSYSERR_NOERROR)
				{
					play_stop_wait_marker = HP_GO_PLAY;
					MessageBox(NULL,"Error midiOutShortMessage",NULL,MB_OK);
					is_playing = false;
					return false;
				}
			}
		}
		else
		{
			// unprepare  from last use
			if (!midiHdr_unprepared)
			{
				// Unprepare the buffer and MIDIHDR 
				if (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(midi_handle, &midiHdr, sizeof(MIDIHDR)))
				{
					int i = 0;
					Sleep(10);
					while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(midi_handle, &midiHdr, sizeof(MIDIHDR)))
					{
						i++;
						if (i==50) break;
						Sleep(10);
					}
					if (i==50)
					{
						MessageBox(NULL,"ErrorUnprepareHeader",NULL,MB_OK);
						is_playing = false;
						return false;
					}
				}
				midiHdr_unprepared = true;
			}

			// Store pointer in MIDIHDR 
			midiHdr.lpData = (LPSTR)data;
			// Store its size in the MIDIHDR 
			midiHdr.dwBufferLength = length;
			// Flags must be set to 0 
			midiHdr.dwFlags = 0;

			// Prepare the buffer and MIDIHDR 
			result = midiOutPrepareHeader(midi_handle,  &midiHdr, sizeof(MIDIHDR));

			if (result != MMSYSERR_NOERROR)
			{
				play_stop_wait_marker = HP_GO_PLAY;
				MessageBox(NULL,"Error midiOutPrepareHeader",NULL,MB_OK);
				is_playing = false;
				return false;
			}
			midiHdr_unprepared = false;

			// Output the SysEx message. Note that this could return immediately if
			// the device driver can output the message on its own in the background.
			// Otherwise, the driver may make us wait in this call until the entire
			//  data is output

			result = midiOutLongMsg(midi_handle, &midiHdr, sizeof(MIDIHDR));

			if (result != MMSYSERR_NOERROR)
			{
				play_stop_wait_marker = HP_GO_PLAY;
				MessageBox(NULL,"Error midiOutLongMessage",NULL,MB_OK);
				is_playing = false;
				return false;
			}
		}
		return true;
	}
	return true;
}



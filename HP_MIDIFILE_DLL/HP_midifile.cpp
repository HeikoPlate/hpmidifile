/************************************************************************/
/************************************************************************/
/*                                                                      */
/* HP_midifile.cpp: Implementation of Global User Functions             */
/*                                                                      */
/************************************************************************/
/************************************************************************/

#include "stdafx.h"
#include "HP_midifile.h"
#include "midifile.h"
#include "MIDIEvent.h"
#include "HP_Array.h"
#include "MetaEvent.h"
#include <mmsystem.h>


// Buffer for copy and paste

Mev_Array    copybuf;
int          copy_ppqn = -1;
Tempo_Array *copy_tempolist = NULL;


/************************************************************************/
/*                                                                      */
/* Version String, to be updated with a new version                      */
/*                                                                      */
/************************************************************************/

char hp_midifile_version[] = "HP MIDIFILE version 1.37";


/************************************************************************/
/*                                                                      */
/* HP_GetVersion                                                        */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API char* HP_GetVersion()
{
	return hp_midifile_version;
}

/************************************************************************/
/*                                                                      */
/* HP_Delete                                                            */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API void HP_Delete(void* pointer)
{
	delete[] pointer;
}


/************************************************************************/
/*                                                                      */
/* The Entry Point of the DLL                                           */
/*                                                                      */
/************************************************************************/

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

/************************************************************************/
/*                                                                      */
/* Init, creates a MIDIFile Object                                      */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API HP_MIDIFILE_MODULE *HP_Init()
{
//	MIDIFile::message("HP_Init");
	return (new MIDIFile());
}

/************************************************************************/
/*                                                                      */
/* Free, destroys MIDIFile Object                                       */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API UINT HP_Free(HP_MIDIFILE_MODULE * hphandle)
{
//	MIDIFile::message("HP_Free");

	if (hphandle != NULL)
	{
		delete hphandle;
		hphandle = NULL;
	}

	return HP_ERR_NONE;
}

/************************************************************************/
/*                                                                      */
/* Load, loads a MIDI-File                                              */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API	UINT		HP_Load(HP_MIDIFILE_MODULE * hphandle, const char *name) 
{
//	MIDIFile::message("HP_Load %s",name);
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	if ((hphandle->eventlist)->get_length()>0) return HP_ERR_LOAD;

	UINT result;

	try
	{
		result = hphandle->load(name);
	}
	catch( ... )
	{
		result = HP_ERR_EVENTDATA;
	}
	return result;
}

/************************************************************************/
/*                                                                      */
/* More data behind tracks?                                             */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API	UINT HP_MoreChunks(HP_MIDIFILE_MODULE * hphandle) 
{
//	MIDIFile::message("HP_MoreChunks");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->is_more_chunks();
}


/************************************************************************/
/*                                                                      */
/* Save, stores a MIDI-File                                              */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API	UINT		HP_Save(HP_MIDIFILE_MODULE * hphandle, const char *name, int format) // SMF0 oder SMF1
{
//	MIDIFile::message("HP_Save %s",name);
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->save(name, format);
}

/************************************************************************/
/*                                                                      */
/* SetFirst                                                             */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API	UINT	HP_SetFirst(HP_MIDIFILE_MODULE * hphandle, bool first)
{
//	MIDIFile::message("HP_SetFirst");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	hphandle->set_first(first);
	return HP_ERR_NONE;
}

/************************************************************************/
/*                                                                      */
/* Insert Events                                                        */
/*                                                                      */
/************************************************************************/
	
HP_MIDIFILE_API UINT HP_InsertNote(HP_MIDIFILE_MODULE * hphandle,int time, int length, int note, int chan, int velocity)
{
//	MIDIFile::message("HP_InsertNote");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_note(time, length, chan, note, velocity);
}

HP_MIDIFILE_API UINT HP_InsertAftertouch(HP_MIDIFILE_MODULE * hphandle, int time, int chan, int note, int pressure)
{
//	MIDIFile::message("HP_InsertAftertouch");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_aftertouch(time,chan,note, pressure);
}

HP_MIDIFILE_API UINT HP_InsertController(HP_MIDIFILE_MODULE * hphandle, int time, int chan, int type, int value, int controller_number)
{
//	MIDIFile::message("HP_InsertController");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_controller(time,chan,type,value,controller_number);
}

HP_MIDIFILE_API UINT HP_InsertRPNAbsolute(HP_MIDIFILE_MODULE * hphandle, int time, int chan, int number, int value)
{
//	MIDIFile::message("HP_InsertRPN");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->insert_rpn_nrpn(time,chan,HP_RPN, number,value, true);
}

HP_MIDIFILE_API UINT HP_InsertRPNRelative(HP_MIDIFILE_MODULE * hphandle, int time, int chan, int number, int value)
{
//	MIDIFile::message("HP_InsertRPNRelative");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_rpn_nrpn(time,chan,HP_RPN,number,value,false);
}

HP_MIDIFILE_API UINT	HP_InsertNRPNAbsolute(HP_MIDIFILE_MODULE * hphandle, int time, int chan, int number, int value)
{
//	MIDIFile::message("HP_InsertNRPN");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_rpn_nrpn(time,chan,HP_NRPN,number,value,true);
}

HP_MIDIFILE_API UINT	HP_InsertNRPNRelative(HP_MIDIFILE_MODULE * hphandle, int time, int chan, int number, int value)
{
//	MIDIFile::message("HP_InsertNRPN_Relative");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_rpn_nrpn(time,chan,HP_NRPN, number,value,false);
}

HP_MIDIFILE_API UINT HP_InsertProgramChange(HP_MIDIFILE_MODULE * hphandle, int time, int chan, int number)
{
//	MIDIFile::message("HP_InsertProgramChange");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_programchange(time,chan,number);
}

HP_MIDIFILE_API UINT	HP_InsertChannelPressure(HP_MIDIFILE_MODULE * hphandle, int time, int chan, int pressure)
{
//	MIDIFile::message("HP_InsertChannelPressure");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_channelpressure(time,chan,pressure);
}

HP_MIDIFILE_API UINT	HP_InsertPitchWheel(HP_MIDIFILE_MODULE * hphandle, int time, int chan, int value)
{
//	MIDIFile::message("HP_InsertPitchWheel");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_pitchwheel(time,chan,value);
}

HP_MIDIFILE_API UINT HP_InsertSysEx(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char* data)
{

//	MIDIFile::message("HP_InsertSysEx");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_sysex(time,data);
}

// Meta Events

HP_MIDIFILE_API UINT	HP_InsertSequenceNumber(HP_MIDIFILE_MODULE * hphandle, int number)
{
//	MIDIFile::message("HP_InsertSequenceNumber");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_sequencenumber(number);
}

HP_MIDIFILE_API UINT	HP_InsertText(HP_MIDIFILE_MODULE * hphandle, int time, char* text)
{
//	MIDIFile::message("HP_InsertText");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_texttype(time,HP_TEXT,text);
}

HP_MIDIFILE_API UINT	HP_InsertCopyright(HP_MIDIFILE_MODULE * hphandle, int time, char* text)
{
//	MIDIFile::message("HP_InsertCopyright");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_texttype(time,HP_COPYRIGHT,text);
}

HP_MIDIFILE_API UINT	HP_InsertSongTrackName(HP_MIDIFILE_MODULE * hphandle,char* text, int chan)
{
//	MIDIFile::message("HP_InsertSongTrackName");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_texttype(0,HP_SONG_TRACK_NAME,text,chan);
}

HP_MIDIFILE_API UINT	HP_InsertInstrument(HP_MIDIFILE_MODULE * hphandle, int time, char* text, int track)
{
//	MIDIFile::message("HP_InsertInstrument");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_texttype(time,HP_INSTRUMENT,text,track);
}

HP_MIDIFILE_API UINT	HP_InsertLyric(HP_MIDIFILE_MODULE * hphandle, int time, char* text)
{
//	MIDIFile::message("HP_InsertLyric");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_texttype(time,HP_LYRIC,text);
}

HP_MIDIFILE_API UINT	HP_InsertMarker(HP_MIDIFILE_MODULE * hphandle, int time, char* text)
{
//	MIDIFile::message("HP_InsertMarker");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_texttype(time,HP_MARKER,text);
}

HP_MIDIFILE_API UINT	HP_InsertCuePoint(HP_MIDIFILE_MODULE * hphandle, int time, char* text)
{
//	MIDIFile::message("HP_InsertCuePoint");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_texttype(time,HP_CUE_POINT,text);
}

HP_MIDIFILE_API UINT	HP_InsertMIDIPort(HP_MIDIFILE_MODULE * hphandle, int time, int track, int port)
{
//	MIDIFile::message("HP_InsertMIDIPort");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_midiport(time,track,port);
}

HP_MIDIFILE_API UINT	HP_InsertTempo(HP_MIDIFILE_MODULE * hphandle, int time, int bpm)
{
//	MIDIFile::message("HP_InsertTempo");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_tempo(time,bpm);
}

HP_MIDIFILE_API UINT	HP_InsertTempo(HP_MIDIFILE_MODULE * hphandle, int time, double bpm)
{
//	MIDIFile::message("HP_InsertTempo");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_tempo_double(time,bpm);
}

HP_MIDIFILE_API UINT HP_InsertSmpteOffset(HP_MIDIFILE_MODULE * hphandle, int hr, int mn, int se, int fr, int ff)
{
//	MIDIFile::message("HP_InsertSmpteOffset");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_smpte_offset(hr,mn,se,fr,ff);
}


HP_MIDIFILE_API UINT	HP_InsertTimeSignature(HP_MIDIFILE_MODULE * hphandle, int time, int num, int denum, int metronome_click, int n32)
{
//	MIDIFile::message("HP_InsertTimeSignature");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_timesignature(time,num,denum,metronome_click,n32);
}

HP_MIDIFILE_API UINT HP_InsertKeySignature(HP_MIDIFILE_MODULE * hphandle, int time, char * key)
{
//	MIDIFile::message("HP_InsertKeySignature");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_keysignature(time,key);
}

HP_MIDIFILE_API UINT HP_InsertScoreStartBar(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_ScoreStartBar");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time, HP_SCORE_START_BAR,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertQuickStart(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_QuickStart");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time, HP_QUICK_START,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertScoreLayout(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_SCORE_LAYOUT");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time, HP_SCORE_LAYOUT,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertKeyboardVoice(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_KeyboardVoice");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time, HP_KEYBOARD_VOICE,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertXFVersionID(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_XFVersionID");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time,HP_XF_VERSION_ID,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertChord(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char cr, unsigned char ct, unsigned char bn, unsigned char bt)
{
//	MIDIFile::message("HP_InsertChord");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_chord(time,cr,ct,bn,bt);
}

HP_MIDIFILE_API UINT HP_InsertRehearsalMark(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_RehearsalMark");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time,HP_REHEARSAL_MARK,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertPhraseMark(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_PhraseMark");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time,HP_PHRASE_MARK,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertMaxPhraseMark(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_MaxPhraseMark");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time,HP_MAX_PHRASE_MARK,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertFingeringNumber(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_FingeringNumber");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time,HP_FINGERING_NUMBER,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertGuideTrackFlag(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_GuideTrackFlag");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time,HP_GUIDE_TRACK_FLAG,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertGuitarInformationFlag(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_GuitarInformationFlag");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time,HP_GUITAR_INFORMATION_FLAG,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertChordVoicingForGuitar(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_ChordVoicingForGuitar");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time,HP_CHORD_VOICING_FOR_GUITAR,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertLyricsBitmap(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_LyricsBitmap");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time,HP_LYRICS_BITMAP,event_data,length);
}

HP_MIDIFILE_API UINT HP_InsertOtherYamahaMeta(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *event_data, int length)
{
//	MIDIFile::message("HP_OtherYamahaMeta");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_ymeta_event(time,HP_OTHER_YAMAHA_META,event_data,length);
}

HP_MIDIFILE_API	UINT	HP_InsertRawEvent(HP_MIDIFILE_MODULE * hphandle, int time, unsigned char *data, int length, int chan)
{
//	MIDIFile::message("HP_InsertRawEvent");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->insert_rawevent(time,data,length,chan);
}

/************************************************************************/
/*                                                                      */
/* Change Events                                                        */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API UINT HP_ChangePPQN(HP_MIDIFILE_MODULE *hphandle, int ppqn)
{
//	MIDIFile::message("HP_ChangePPQN");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->change_ppqn(ppqn);
}

HP_MIDIFILE_API UINT HP_ChangeChannel(HP_MIDIFILE_MODULE *hphandle, int chan)
{
//	MIDIFile::message("HP_ChangeChannel");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	hphandle->change_channel(chan);
	return HP_ERR_NONE;
}

HP_MIDIFILE_API void HP_ChangeChannelInBuffer(int chan)
{
	int i;
//	MIDIFile::message("HP_ChangeChannelInBuffer");

	for (i=0; i<copybuf.get_length(); i++)
	{
		MIDIEvent *mev = copybuf.get(i);
		{
			if (mev->chan >= 0)
			{
				mev->change_chan(chan);
			}
		}
	}
}

HP_MIDIFILE_API UINT HP_ChangeNote(HP_MIDIFILE_MODULE *hphandle, int id , int chan, int note, bool absolute)
{
//	MIDIFile::message("HP_ChangeNote");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_note(id,chan,note,absolute);
}

HP_MIDIFILE_API UINT HP_ChangeVelocity(HP_MIDIFILE_MODULE *hphandle, int id , int chan, int note, int value, bool percent)
{
//	MIDIFile::message("HP_ChangeVelocity");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_velocity(id,chan,note,value,percent);
}

HP_MIDIFILE_API UINT HP_ChangeVelocitySelected(HP_MIDIFILE_MODULE *hphandle, int value, bool percent)
{
//	MIDIFile::message("HP_ChangeVelocitySelected");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_velocity_selected(value,percent);
}

HP_MIDIFILE_API UINT HP_ChangeNoteLength(HP_MIDIFILE_MODULE *hphandle, int id, int chan, int note, int value, bool percent)
{
//	MIDIFile::message("HP_ChangeNoteLength");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_note_length(id,chan,note,value,percent);
}

HP_MIDIFILE_API UINT HP_TransposeSelected(HP_MIDIFILE_MODULE *hphandle, int steps)
{
//	MIDIFile::message("HP_TransposeSelected");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->transpose_selected(steps);
}

HP_MIDIFILE_API UINT HP_QuantisizeSelected(HP_MIDIFILE_MODULE *hphandle, int resolution, bool start, bool length)
{
//	MIDIFile::message("HP_QuantisizeSelected");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->quantisize_selected(resolution,start,length);
}

HP_MIDIFILE_API UINT HP_SettleOverlappingNotes(HP_MIDIFILE_MODULE *hphandle, bool method)
{
//	MIDIFile::message("HP_SettleOverlappingNotes");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->settle_overlapping_notes(method);
}


HP_MIDIFILE_API UINT HP_ShiftNotesSelected(HP_MIDIFILE_MODULE *hphandle, int ticks)
{
//	MIDIFile::message("HP_ShiftNotesSelected");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->shift_notes_selected(ticks);
}


HP_MIDIFILE_API UINT HP_ChangeAftertouch(HP_MIDIFILE_MODULE *hphandle, int id, int note, int value, bool percent)
{
//	MIDIFile::message("HP_ChangeAftertouch");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_aftertouch(id,note,value,percent);
}

HP_MIDIFILE_API UINT HP_ChangeController(HP_MIDIFILE_MODULE *hphandle, int id , UINT type, int value, bool percent)
{
//	MIDIFile::message("HP_ChangeController");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_controller(id,type,value,percent);
}

HP_MIDIFILE_API UINT HP_ChangeControllerSelected(HP_MIDIFILE_MODULE *hphandle, UINT type,int value, bool percent)
{
//	MIDIFile::message("HP_ChangeControllerSelected");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_controller_selected(type,value,percent);
}

HP_MIDIFILE_API UINT HP_ChangeRPNAbsolute(HP_MIDIFILE_MODULE *hphandle, int id , int value, bool percent)
{
//	MIDIFile::message("HP_ChangeRPNAbsolute");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_rpn_nrpn(HP_RPN,true,id,value,percent);
}

HP_MIDIFILE_API UINT HP_ChangeRPNRelative(HP_MIDIFILE_MODULE *hphandle, int id , int value)
{
//	MIDIFile::message("HP_ChangeRPNRelative");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_rpn_nrpn(HP_RPN,false,id,value,false);
}

HP_MIDIFILE_API UINT HP_ChangeNRPNAbsolute(HP_MIDIFILE_MODULE *hphandle, int id , int value, bool percent)
{
//	MIDIFile::message("HP_ChangeNRPNAbsolute");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_rpn_nrpn(HP_NRPN,true,id,value,percent);
}

HP_MIDIFILE_API UINT HP_ChangeNRPNRelative(HP_MIDIFILE_MODULE *hphandle, int id , int value)
{
//	MIDIFile::message("HP_ChangeNRPNRelative");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_rpn_nrpn(HP_NRPN,false,id,value,false);
}

HP_MIDIFILE_API UINT HP_ChangeProgram(HP_MIDIFILE_MODULE *hphandle, int id ,int number)
{
//	MIDIFile::message("HP_ChangeProgram");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_program(id,number);
}

HP_MIDIFILE_API UINT HP_ChangeChannelPressure(HP_MIDIFILE_MODULE *hphandle, int id ,int pressure)
{
//	MIDIFile::message("HP_ChangeChannelPressure");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_channel_pressure(id,pressure);
}

HP_MIDIFILE_API UINT HP_ChangePitchWheel(HP_MIDIFILE_MODULE *hphandle, int id ,int value)
{
//	MIDIFile::message("HP_ChangePitchWheel");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_pitch_wheel(id,value);
}

HP_MIDIFILE_API UINT HP_ChangeSysex(HP_MIDIFILE_MODULE *hphandle, int id, unsigned char* data)
{
//	MIDIFile::message("HP_ChangeSysex");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_sysex(id,data);

}

HP_MIDIFILE_API UINT HP_ChangeSequenceNumber(HP_MIDIFILE_MODULE *hphandle, int id, int number)
{
//	MIDIFile::message("HP_ChangeSequenceNumber");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_sequence_number(id,number);
}

HP_MIDIFILE_API UINT HP_ChangeText(HP_MIDIFILE_MODULE *hphandle, int id, char* new_text)
{
//	MIDIFile::message("HP_ChangeText");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_texttype(id,HP_TEXT,new_text);
}

HP_MIDIFILE_API UINT HP_ChangeCopyright(HP_MIDIFILE_MODULE *hphandle, int id,char* new_text)
{
//	MIDIFile::message("HP_ChangeCopyright");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_texttype(id,HP_COPYRIGHT,new_text);
}

HP_MIDIFILE_API UINT HP_ChangeSongTrackName(HP_MIDIFILE_MODULE *hphandle, int id,char* new_text)
{
//	MIDIFile::message("HP_ChangeSongTrackName");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_texttype(id,HP_SONG_TRACK_NAME,new_text);
}

HP_MIDIFILE_API UINT HP_ChangeInstrument(HP_MIDIFILE_MODULE *hphandle, int id,char* new_text)
{
//	MIDIFile::message("HP_ChangeInstrument");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_texttype(id,HP_INSTRUMENT,new_text);
}

HP_MIDIFILE_API UINT HP_ChangeLyric(HP_MIDIFILE_MODULE *hphandle, int id, char* new_text)
{
//	MIDIFile::message("HP_ChangeLyric");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_texttype(id,HP_LYRIC,new_text);
}

HP_MIDIFILE_API UINT HP_ChangeMarker(HP_MIDIFILE_MODULE *hphandle, int id, char* new_text)
{
//	MIDIFile::message("HP_ChangeMarker");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_texttype(id,HP_MARKER,new_text);
}

HP_MIDIFILE_API UINT HP_ChangeCuePoint(HP_MIDIFILE_MODULE *hphandle, int id,char* new_text)
{
//	MIDIFile::message("HP_ChangeCuepoint");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_texttype(id,HP_CUE_POINT,new_text);
}

HP_MIDIFILE_API UINT HP_ChangeMidiPort(HP_MIDIFILE_MODULE *hphandle, int id, int track, int port)
{
//	MIDIFile::message("HP_ChangeMidiPort");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_midiport(id,track,port);
}

HP_MIDIFILE_API UINT HP_ChangeTempo(HP_MIDIFILE_MODULE * hphandle, int id, int value, bool percent)
{
//	MIDIFile::message("HP_ChangeTempo");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_tempo(id, value, percent);
}

HP_MIDIFILE_API UINT HP_ChangeTempo(HP_MIDIFILE_MODULE * hphandle, int id, double value, bool percent)
{
//	MIDIFile::message("HP_ChangeTempo");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_tempo_double(id, value, percent);
}

HP_MIDIFILE_API UINT HP_ChangeSmpteOffset(HP_MIDIFILE_MODULE *hphandle, int id, int hr, int mn, int se, int fr, int ff)
{
//	MIDIFile::message("HP_ChangeSmpteOffset");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->change_smpte_offset(id,hr,mn,se,fr,ff);
}
 
HP_MIDIFILE_API UINT HP_ChangeTimeSignature(HP_MIDIFILE_MODULE *hphandle, int id, int new_num, int new_denum, int new_m_click, int new_n32)
{
//	MIDIFile::message("HP_ChangeTimeSignature");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_time_signature(id,new_num,new_denum,new_m_click,new_n32);
}

HP_MIDIFILE_API UINT HP_ChangeKeySignature(HP_MIDIFILE_MODULE *hphandle, int id, char *key)
{
//	MIDIFile::message("HP_ChangeKeySignature");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_key_signature(id,key);
}

HP_MIDIFILE_API UINT HP_ChangeChord(HP_MIDIFILE_MODULE *hphandle, int id, unsigned char cr, unsigned char ct, unsigned char bn, unsigned char bt)
{
//	MIDIFile::message("HP_ChangeChord");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->change_chord(id,cr,ct,bn,bt);
}

HP_MIDIFILE_API UINT HP_ShiftEvent(HP_MIDIFILE_MODULE *hphandle, int id, int ticks)
{
//	MIDIFile::message("HP_ShiftEvent");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->shift_event(id,ticks);
}

HP_MIDIFILE_API UINT HP_ShiftSelected(HP_MIDIFILE_MODULE *hphandle, int ticks)
{
//	MIDIFile::message("HP_ShiftSelected");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	return hphandle->shift_selected(ticks);
}

/************************************************************************/
/*                                                                      */
/* Read Events                                                          */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API UINT HP_ReadEventData(HP_MIDIFILE_MODULE *hphandle, int id, int *time, unsigned char** event_data, int *event_length)
{
//	MIDIFile::message("HP_ReadEventData");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_event_data(id,time,event_data,event_length);
}

HP_MIDIFILE_API	UINT HP_ReadPPQN(HP_MIDIFILE_MODULE *hphandle, int *ppqn)
{
//	MIDIFile::message("HP_ReadPPQN");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ppqn(ppqn);
}

HP_MIDIFILE_API UINT HP_ReadNote(HP_MIDIFILE_MODULE *hphandle, int id , int *time, int *chan, int *note, int *velocity, int *length)
{
//	MIDIFile::message("HP_ReadNote");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_note(id,time,chan,note,velocity,length);
}

HP_MIDIFILE_API UINT HP_ReadAftertouch(HP_MIDIFILE_MODULE *hphandle, int id, int *time, int *chan, int *note, int *pressure)
{
//	MIDIFile::message("HP_ReadAftertouch");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_aftertouch(id,time,chan,note,pressure);
}

HP_MIDIFILE_API UINT HP_ReadController(HP_MIDIFILE_MODULE *hphandle, int id, int *time, int *chan, int *number, int *value)
{
//	MIDIFile::message("HP_ReadController");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_controller(id,time,chan,number,value);
}

HP_MIDIFILE_API UINT HP_ReadRPN(HP_MIDIFILE_MODULE *hphandle, int id, int *time, int *chan, bool *absolute, int *number, int *value)
{
//	MIDIFile::message("HP_ReadRPN");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_rpn_nrpn(HP_RPN,id,time,chan,absolute,number,value);
}

HP_MIDIFILE_API UINT HP_ReadNRPN(HP_MIDIFILE_MODULE *hphandle, int id, int *time, int *chan, bool *absolute, int *number, int *value)
{
//	MIDIFile::message("HP_ReadNRPN");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_rpn_nrpn(HP_NRPN,id,time,chan,absolute,number,value);
}

HP_MIDIFILE_API UINT HP_ReadProgramChange(HP_MIDIFILE_MODULE *hphandle, int id, int *time, int *chan, int *number)
{
//	MIDIFile::message("HP_ReadProgramChange");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_program_change(id,time,chan,number);
}

HP_MIDIFILE_API UINT HP_ReadChannelPressure(HP_MIDIFILE_MODULE *hphandle, int id, int *time, int *chan, int *pressure)
{
//	MIDIFile::message("HP_ReadChannelPressure");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_channel_pressure(id,time,chan,pressure);
}

HP_MIDIFILE_API UINT HP_ReadPitchWheel(HP_MIDIFILE_MODULE *hphandle, int id, int *time, int *chan, int *value)
{
//	MIDIFile::message("HP_ReadPitchWheel");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_pitch_wheel(id,time,chan,value);
}

HP_MIDIFILE_API UINT HP_ReadSysex(HP_MIDIFILE_MODULE *hphandle, int id, int *time, unsigned char** data, int *length)
{
//	MIDIFile::message("HP_ReadSysex");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_sysex(id,time,data,length);
}

HP_MIDIFILE_API UINT HP_ReadSequenceNumber(HP_MIDIFILE_MODULE *hphandle, int id, int *time, int *number)
{
//	MIDIFile::message("HP_ReadSequenceNumber");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_sequence_number(id,time,number);
}

HP_MIDIFILE_API UINT HP_ReadText(HP_MIDIFILE_MODULE *hphandle, int id, int *time, char **text)
{
	int chan; // dummy
//	MIDIFile::message("HP_ReadText");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_text_from_meta(id,time,HP_TEXT,&chan,text);
}

HP_MIDIFILE_API UINT HP_ReadCopyright(HP_MIDIFILE_MODULE *hphandle, int id, int *time, char **text)
{
	int chan; // dummy
//	MIDIFile::message("HP_ReadCopyright");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_text_from_meta(id,time,HP_COPYRIGHT,&chan,text);
}

HP_MIDIFILE_API UINT HP_ReadSongTrackName(HP_MIDIFILE_MODULE *hphandle, int id, int *time, char **text)
{
	int chan; // dummy
//	MIDIFile::message("HP_ReadSongTrackName");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_text_from_meta(id,time,HP_SONG_TRACK_NAME,&chan,text);
}

HP_MIDIFILE_API UINT HP_ReadInstrument(HP_MIDIFILE_MODULE *hphandle, int id, int *time, char **text)
{
	int chan; // dummy
//	MIDIFile::message("HP_ReadInstrument");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_text_from_meta(id,time,HP_INSTRUMENT,&chan,text);
}

HP_MIDIFILE_API UINT HP_ReadLyric(HP_MIDIFILE_MODULE *hphandle, int id, int *time, char **text)
{
	int chan; // dummy
//	MIDIFile::message("HP_ReadLyric");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_text_from_meta(id,time,HP_LYRIC,&chan,text);
}

HP_MIDIFILE_API UINT HP_ReadMarker(HP_MIDIFILE_MODULE *hphandle, int id, int *time, char **text)
{
	int chan; // dummy
//	MIDIFile::message("HP_ReadMarker");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_text_from_meta(id,time,HP_MARKER,&chan,text);
}

HP_MIDIFILE_API UINT HP_ReadCuePoint(HP_MIDIFILE_MODULE *hphandle, int id, int *time, char **text)
{
	int chan; // dummy
//	MIDIFile::message("HP_ReadCuePoint");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_text_from_meta(id,time,HP_CUE_POINT,&chan,text);
}

HP_MIDIFILE_API UINT HP_ReadMidiPort(HP_MIDIFILE_MODULE *hphandle, int id, int *time, int *track, int *port)
{
//	MIDIFile::message("HP_ReadMidiPort");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_midiport(id,time,track,port);
}

HP_MIDIFILE_API UINT HP_ReadTempo(HP_MIDIFILE_MODULE *hphandle, int id , int *time, int *bpm, int *cps)
{
//	MIDIFile::message("HP_ReadTempo");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_tempo(id,time,bpm,cps);
}

HP_MIDIFILE_API UINT HP_ReadTempo(HP_MIDIFILE_MODULE *hphandle, int id , int *time, double *bpm, int *cps)
{
//	MIDIFile::message("HP_ReadTempo");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_tempo_double(id,time,bpm,cps);
}

HP_MIDIFILE_API UINT HP_ReadSmpteOffset(HP_MIDIFILE_MODULE *hphandle, int id , int *hr, int *mn, int *se, int *fr, int *ff)
{
//	MIDIFile::message("HP_SmpteOffset");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_smpte_offset(id,hr,mn,se,fr,ff);
}

HP_MIDIFILE_API UINT HP_ReadTimeSignature(HP_MIDIFILE_MODULE *hphandle, int id, int *time, int *num, int *denum, int *metronome_click, int *n32)
{
//	MIDIFile::message("HP_ReadTimeSignature");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_time_signature(id,time,num,denum,metronome_click,n32);
}

HP_MIDIFILE_API UINT HP_ReadKeySignature(HP_MIDIFILE_MODULE *hphandle, int id, int *time, char **key)
{
//	MIDIFile::message("HP_ReadKeySignature");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_key_signature(id,time,key);
}

HP_MIDIFILE_API UINT HP_ReadScoreStartBar(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadScoreStartBar");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_SCORE_START_BAR,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadQuickStart(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadQuickStart");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_QUICK_START,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadScoreLayout(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadScoreLayout");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_SCORE_LAYOUT,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadKeyboardVoice(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadKeyboardVoice");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_KEYBOARD_VOICE,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadXFVersionID(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadXFVersionID");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_XF_VERSION_ID, event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadChord(HP_MIDIFILE_MODULE *hphandle, int id, int *time, unsigned char* cr, unsigned char* ct, unsigned char* bn, unsigned char* bt,char** chord)
{
//	MIDIFile::message("HP_ReadChord");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_chord(id,time,cr,ct,bn,bt,chord);
}

HP_MIDIFILE_API UINT HP_ReadRehearsalMark(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadRehearsalMark");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_REHEARSAL_MARK,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadPhraseMark(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadPhraseMark");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_PHRASE_MARK,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadMaxPhraseMark(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadMaxPhraseMark");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_MAX_PHRASE_MARK,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadFingeringNumber(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadFingeringNumber");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_FINGERING_NUMBER,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadGuideTrackFlag(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadGuideTrackFlag");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_GUIDE_TRACK_FLAG,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadGuitarInformationFlag(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadGuitarInformationFlag");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_GUITAR_INFORMATION_FLAG,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadChordVoicingForGuitar(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadChordVoicingForGuitar");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_CHORD_VOICING_FOR_GUITAR,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadLyricsBitmap(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadLyricsBitmap");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_LYRICS_BITMAP,event_data,length);
}

HP_MIDIFILE_API UINT HP_ReadOtherYamahaMeta(HP_MIDIFILE_MODULE * hphandle, int id, int *time, unsigned char **event_data, int *length)
{
//	MIDIFile::message("HP_ReadOtherYamahaMeta");
	if (hphandle==NULL) return HP_ERR_UNINITIALIZED;
	return hphandle->read_ymeta_event(id,time,HP_OTHER_YAMAHA_META, event_data,length);
}

/************************************************************************/
/*                                                                      */
/* Utilities                                                            */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API UINT HP_GetTaktBeatTick(HP_MIDIFILE_MODULE * hphandle, 
										   int time , int *takt, int *beat, int *tick)
{
//	MIDIFile::message("HP_GetTaktBeatTick");
	if (hphandle==NULL) 
	{
		*takt = 0;
		*beat = 0;
		*tick = 0;
		return HP_ERR_UNINITIALIZED;
	}
	hphandle->get_takt_beat_tick(time, takt, beat, tick);
	return HP_ERR_NONE;
}

HP_MIDIFILE_API UINT HP_GetTaktBeatTick(HP_MIDIFILE_MODULE * hphandle, 
										int time, 
										int time_of_last_time_signature,
										int takt_of_last_time_signature,
										int num,
										int denum,
										int *takt, int *beat, int *tick)
{
//	MIDIFile::message("HP_GetTaktBeatTick");
	if (hphandle==NULL) 
	{
		*takt = 0;
		*beat = 0;
		*tick = 0;
		return HP_ERR_UNINITIALIZED;
	}
	hphandle->get_takt_beat_tick(time, 
		                         time_of_last_time_signature,
								 takt_of_last_time_signature,
								 num, denum, takt, beat, tick);
	return HP_ERR_NONE;
}

HP_MIDIFILE_API UINT HP_GetTimeFromTaktBeatTick(HP_MIDIFILE_MODULE * hphandle, 
										   int takt, int beat, int tick, int *time)
{
//	MIDIFile::message("HP_GetTimeFromTaktBeatTick");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->get_time_from_takt_beat_tick(takt, beat, tick, time);
}

HP_MIDIFILE_API UINT HP_GetLastTime(HP_MIDIFILE_MODULE * hphandle, int *time)
{
//	MIDIFile::message("HP_GetLastTime");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->get_last_time(time);
}

HP_MIDIFILE_API char* HP_TypeName(UINT type)
{
	const struct{UINT type; char* type_name;} name_list[] =
	{
		{HP_TYPE_MASK			        ,"HP_TYPE_MASK"},
		{HP_NOTE				        ,"HP_NOTE"},
		{HP_AFTERTOUCH			        ,"HP_AFTERTOUCH"},
		{HP_CONTROLLER_TYPE	            ,"HP_CONTROLLER_TYPE"},
		{HP_BANK_SELECT_MSB		        ,"HP_BANK_SELECT_MSB"},
		{HP_MODULATION			        ,"HP_MODULATION"},
		{HP_BREATH				        ,"HP_BREATH"},
		{HP_PORTAMENTO_TIME		        ,"HP_PORTAMENTO_TIME"},
		{HP_MAIN_VOLUME			        ,"HP_MAIN_VOLUME"},
		{HP_PANPOT				        ,"HP_PANPOT"},
		{HP_EXPRESSION			        ,"HP_EXPRESSION"},
		{HP_BANK_SELECT_LSB		        ,"HP_BANK_SELECT_LSB"},
		{HP_SUSTAIN				        ,"HP_SUSTAIN"},
		{HP_PORTAMENTO			        ,"HP_PORTAMENTO"},
		{HP_SOSTENUTO			        ,"HP_SOSTENUTO"},
		{HP_SOFT_PEDAL			        ,"HP_SOFT_PEDAL"},
		{HP_HARMONIC_CONTENT	        ,"HP_HARMONIC_CONTENT"},
		{HP_RELEASE_TIME		        ,"HP_RELEASE_TIME"},
		{HP_ATTACK_TIME			        ,"HP_ATTACK_TIME"},
		{HP_BRIGHTNESS			        ,"HP_BRIGHTNESS"},
		{HP_DECAY_TIME			        ,"HP_DECAY_TIME"},
		{HP_VIBRATO_RATE		        ,"HP_VIBRATO_RATE"},
		{HP_VIBRATO_DEPTH		        ,"HP_VIBRATO_DEPTH"},
		{HP_VIBRATO_DELAY		        ,"HP_VIBRATO_DELAY"},
		{HP_PORTAMENTO_CONTROL	        ,"HP_PORTAMENTO_CONTROL"},
		{HP_REVERB_SEND_LEVEL	        ,"HP_REVERB_SEND_LEVEL"},
		{HP_CHORUS_SEND_LEVEL	        ,"HP_CHORUS_SEND_LEVEL"},
		{HP_VARIATION_SEND_LEVEL        ,"HP_VARIATION_SEND_LEVEL"},
		{HP_NRPN				        ,"HP_NRPN"},
		{HP_RPN					        ,"HP_RPN"},
		{HP_ALL_SOUND_OFF		        ,"HP_ALL_SOUND_OFF"},
		{HP_RESET_ALL_CONTROLLERS       ,"HP_RESET_ALL_CONTROLLERS"},
		{HP_ALL_NOTES_OFF		        ,"HP_ALL_NOTES_OFF"},
		{HP_OMNI_OFF			        ,"HP_OMNI_OFF"},
		{HP_OMNI_ON				        ,"HP_OMNI_ON"},
		{HP_MONO				        ,"HP_MONO"},
		{HP_POLY				        ,"HP_POLY"},
		{HP_PROGRAM_CHANGE		        ,"HP_PROGRAM_CHANGE"},
		{HP_CHANNEL_PRESSURE	        ,"HP_CHANNEL_PRESSURE"},
		{HP_PITCH_WHEEL			        ,"HP_PITCH_WHEEL"},
		{HP_SYSEX				        ,"HP_SYSEX"},
		{HP_SEQUENCE_NUMBER		        ,"HP_SEQUENCE_NUMBER"},
		{HP_TEXT				        ,"HP_TEXT"},
		{HP_COPYRIGHT			        ,"HP_COPYRIGHT"},
		{HP_SONG_TRACK_NAME			    ,"HP_SONG_TRACK_NAME"},
		{HP_INSTRUMENT			        ,"HP_INSTRUMENT"},
		{HP_LYRIC				        ,"HP_LYRIC"},
		{HP_MARKER				        ,"HP_MARKER"},
		{HP_CUE_POINT			        ,"HP_CUE_POINT"},
		{HP_CHANNEL_PREFIX		        ,"HP_CHANNEL_PREFIX"},
		{HP_MIDI_PORT			        ,"HP_MIDI_PORT"},
		{HP_END_OF_TRACK		        ,"HP_END_OF_TRACK"},
		{HP_TEMPO				        ,"HP_TEMPO"},
		{HP_SMPTE_OFFSET		        ,"HP_SMPTE_OFFSET"},
		{HP_TIME_SIGNATURE		        ,"HP_TIME_SIGNATURE"},
		{HP_KEY_SIGNATURE		        ,"HP_KEY_SIGNATURE"},
		{HP_OTHER_META			        ,"HP_OTHER_META"},
		{HP_SCORE_START_BAR		        ,"HP_SCORE_START_BAR"},
		{HP_QUICK_START					,"HP_QUICK_START"},
		{HP_SCORE_LAYOUT		        ,"HP_SCORE_LAYOUT"},
		{HP_KEYBOARD_VOICE		        ,"HP_KEYBOARD_VOICE"},
		{HP_XF_VERSION_ID		        ,"HP_XF_VERSION_ID"},
		{HP_CHORD_NAME			        ,"HP_CHORD_NAME"},
		{HP_REHEARSAL_MARK		        ,"HP_REHEARSAL_MARK"},
		{HP_PHRASE_MARK			        ,"HP_PHRASE_MARK"},
		{HP_MAX_PHRASE_MARK		        ,"HP_MAX_PHRASE_MARK"},
		{HP_FINGERING_NUMBER	        ,"HP_FINGERING_NUMBER"},
		{HP_GUIDE_TRACK_FLAG	        ,"HP_GUIDE_TRACK_FLAG"},
		{HP_GUITAR_INFORMATION_FLAG     ,"HP_GUITAR_INFORMATION_FLAG"},
		{HP_CHORD_VOICING_FOR_GUITAR    ,"HP_CHORD_VOICING_FOR_GUITAR"},
		{HP_LYRICS_BITMAP		        ,"HP_LYRICS_BITMAP"},
		{HP_OTHER_YAMAHA_META	        ,"HP_OTHER_YAMAHA_META"},
		{HP_RAW_EVENT			        ,"HP_RAW_EVENT"}	// HP_RAW_EVENT must be last event
	};

	int i;
	for (i=0; true; i++)
	{
		if (type == (name_list[i]).type)
		{
			return (name_list[i]).type_name;
			break;
		}
		if (name_list[i-1].type == HP_RAW_EVENT) break; // last type in list
	}
	return "No Type Found";
}

HP_MIDIFILE_API char* HP_ErrText(int err_no)
{
	const struct{int err_no; char* err_text;} err_list[] =
	{
		{HP_ERR_NONE               ,"HP_ERR_NONE"},     
		{HP_ERR_BUSY               ,"HP_ERR_BUSY"},                                                           
		{HP_ERR_UNINITIALIZED      ,"HP_ERR_UNINITIALIZED"},   
		{HP_ERR_MIDI_DEVICE        ,"HP_ERR_MIDI_DEVICE"},     
		{HP_ERR_ALLOCATED_DEVICE   ,"HP_ERR_ALLOCATED_DEVICE"},
		{HP_ERR_PLAY               ,"HP_ERR_PLAY"},                                                           
		{HP_ERR_OUTPUT_FORMAT      ,"HP_ERR_OUTPUT_FORMAT"},   
		{HP_ERR_FILE_NOTFOUND      ,"HP_ERR_FILE_NOTFOUND"},   
		{HP_ERR_FILE_NOTOPEN       ,"HP_ERR_FILE_NOTOPEN"},    
		{HP_ERR_FILE_FORMAT        ,"HP_ERR_FILE_FORMAT"},     
		{HP_ERR_FILE_BAD           ,"HP_ERR_FILE_BAD"},        
		{HP_ERR_MEMORY             ,"HP_ERR_MEMORY"},                                 
		{HP_ERR_INVALID_PARAM      ,"HP_ERR_INVALID_PARAM"},   
		{HP_ERR_INVALID_DENUM      ,"HP_ERR_INVALID_DENUM"},   
		{HP_ERR_INVALID_PPQN       ,"HP_ERR_INVALID_PPQN"},    
		{HP_ERR_NO_PPQN				,"HP_ERR_NO_PPQN"},                                
		{HP_ERR_INVALID_SYSEX		,"HP_ERR_INVALID_SYSEX"},   
		{HP_ERR_EVENTTYPE			,"HP_ERR_EVENTTYPE"},       
		{HP_ERR_EVENTDATA			,"HP_ERR_EVENTDATA"},       
		{HP_ERR_SEQUENCENUMBER		,"HP_ERR_SEQUENCENUMBER"},  
		{HP_ERR_KEY					,"HP_ERR_KEY"},             
		{HP_ERR_BAD_ID				,"HP_ERR_BAD_ID"},          
		{HP_ERR_DELETE				,"HP_ERR_DELETE"},          
		{HP_ERR_BAD_CONTROLLER		,"HP_ERR_BAD_CONTROLLER"},  
		{HP_ERR_BAD_NEW_NOTE		,"HP_ERR_BAD_NEW_NOTE"},    
		{HP_ERR_BAD_SMPTE_OFFSET	,"HP_ERR_BAD_SMPTE_OFFSET"},
		{HP_ERR_BAD_SHIFT			,"HP_ERR_BAD_SHIFT"},       
		{HP_ERR_BAD_TRANSPOSE		,"HP_ERR_BAD_TRANSPOSE"},   
		{HP_ERR_QUANTISIZE			,"HP_ERR_QUANTISIZE"},
		{HP_ERR_LOAD				,"HP_ERR_LOAD"},
		{HP_ERR_CWINSFILE_NOTFOUND	,"HP_ERR_CWINSFILE_NOTFOUND"},
		{HP_ERR_CWINSFILE			,"HP_ERR_CWINSFILE"},
		{HP_ERR_CWINS_DEF_INDEX		,"HP_ERR_CWINS_DEF_INDEX"},
		{HP_ERR_CWINS_FILE_MISSED	,"HP_ERR_CWINS_FILE_MISSED"},
		{HP_ERR_CWINS_LABEL_MISSED	,"HP_ERR_CWINS_LABEL_MISSED"},
		{HP_ERR_CWINS_NO_INSTRUMENT_DEFINITION_LIST	,"HP_ERR_CWINS_NO_INSTRUMENT_DEFINITION_LIST"},
		{HP_ERR_CWINS_NO_NAME		,"HP_ERR_CWINS_NO_NAME"},
		{HP_ERR_CWINS_NO_INSTRUMENT	,"HP_ERR_CWINS_NO_INSTRUMENT"},
		{HP_ERR_CWINS_NO_DRUM_LINES ,"HP_ERR_CWINS_NO_DRUM_LINES"},
		{HP_ERR_PLAY_NO_MIDIEVENT   ,"HP_ERR_PLAY_NO_MIDIEVENT"},
		{HP_ERR_RANGE_EVENTLIST		,"HP_ERR_RANGE_EVENTLIST"} // last type in list
	};
	
	int i;
	for (i=0; true; i++)
	{
		if (err_no == (err_list[i]).err_no)
		{
			return (err_list[i]).err_text;
			break;
		}
		if (err_no == HP_ERR_RANGE_EVENTLIST) break; // last type in list
	}
	return "No Err Text Found";
}

HP_MIDIFILE_API UINT HP_ConvertChordToText(	  unsigned char cr,
											  unsigned char ct,
											  unsigned char bn,
											  unsigned char bt,
											  char ** chord_name)
{
	return ChordNameEvent::convert_chord_to_text(cr,ct,bn,bt,chord_name);
}



/************************************************************************/
/*                                                                      */
/* Stream Functions                                                    */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API UINT HP_Rewind(HP_MIDIFILE_MODULE * hphandle)
{
//	MIDIFile::message("HP_Rewind");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	hphandle->rewind();
	return HP_ERR_NONE;
}

HP_MIDIFILE_API UINT HP_ReadEvent(HP_MIDIFILE_MODULE * hphandle, int *id, int *chan, int *time, int *type)
{
//	MIDIFile::message("HP_ReadEvent");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->read_event(id,chan,time,type);
}

HP_MIDIFILE_API UINT HP_DeleteShortNotesSelected(HP_MIDIFILE_MODULE * hphandle, int limit_length)
{
//	MIDIFile::message("HP_DeleteShortNotesSelected");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->delete_short_notes_selected(limit_length);
}


HP_MIDIFILE_API UINT HP_DeleteEvent(HP_MIDIFILE_MODULE * hphandle, int id)
{
//	MIDIFile::message("HP_DeleteEvent");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->delete_event(id);
}

/************************************************************************/
/*                                                                      */
/* Select, Copy, Paste                                                  */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API UINT HP_Select(HP_MIDIFILE_MODULE * hphandle, int id)
{
//	MIDIFile::message("HP_Select");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->select(id);
}

HP_MIDIFILE_API UINT HP_SelectChanTime(HP_MIDIFILE_MODULE * hphandle, int chan, int time1, int time2)
{
//	MIDIFile::message("HP_SelectChanTime");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	hphandle->select_by_chan_time(chan,time1,time2);
	return HP_ERR_NONE;
}

HP_MIDIFILE_API UINT HP_Deselect(HP_MIDIFILE_MODULE * hphandle, int id)
{
//	MIDIFile::message("HP_Deselect");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return hphandle->deselect(id);
}

HP_MIDIFILE_API UINT HP_DeselectChan(HP_MIDIFILE_MODULE * hphandle, int chan)
{
//	MIDIFile::message("HP_Deselect Chan");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	hphandle->deselect_channel_events(chan);
	return HP_ERR_NONE;
}

HP_MIDIFILE_API UINT HP_Copy(HP_MIDIFILE_MODULE * hphandle)
{
//	MIDIFile::message("HP_Copy");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	while (copybuf.get_length()>0)
	{
		copybuf.delete_from_end();
	}

	copybuf.clear();
	copy_ppqn = -1;

	hphandle->copy_selected_to_buf(&copybuf);
	hphandle->read_ppqn(&copy_ppqn);
	if (copy_tempolist != NULL)
	{
		delete copy_tempolist;
	}
	copy_tempolist = new Tempo_Array(*hphandle->tempolist);

	return HP_ERR_NONE;
}

HP_MIDIFILE_API UINT HP_Paste(HP_MIDIFILE_MODULE * hphandle,int time, bool delete_in_dest, int del_events_chan)
{
//	MIDIFile::message("HP_Paste");
	int ppqn;

	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

	hphandle->copy_selected_from_buf(&copybuf,&copy_ppqn,time, delete_in_dest, del_events_chan);
	if (hphandle->tempolist != NULL)
	{
		delete hphandle->tempolist;
	}
	hphandle->read_ppqn(&ppqn);
	hphandle->tempolist = new Tempo_Array(*copy_tempolist);
	hphandle->reorganize_tempolist(hphandle->eventlist,ppqn);

	return HP_ERR_NONE;
}


/************************************************************************/
/*                                                                      */
/* Play Functions                                                       */
/*                                                                      */
/************************************************************************/

HP_MIDIFILE_API UINT HP_GetMIDIDevices(HP_DEVICE **devices, int *no_devices)
{
	MIDIOUTCAPS	moc;
	int	iNumDevs;
	int	actual_no_devices = 0;
	int	i;
	int k = 0;

	iNumDevs = midiOutGetNumDevs();

	for (i=-1; i<iNumDevs; i++)
	{
		if (!midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS)))
		{
			actual_no_devices++;
		}
	}

	*devices = new HP_DEVICE[actual_no_devices];

	for (i=-1; i<iNumDevs; i++)
	{
		if (midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS))==MMSYSERR_NOERROR)
		{
			(*devices)[k].device_id = i;
			strcpy((*devices)[k].device_name, moc.szPname);
			k++;
		}
	}
	*no_devices = actual_no_devices;
	return HP_ERR_NONE;
}


HP_MIDIFILE_API UINT HP_SelectMIDIDevice(int id)
{
	MIDIOUTCAPS	moc;
	int	iNumDevs;
	int	actual_no_devices = 0;
	int	i;
	int k = 0;

	int found = false;

	iNumDevs = midiOutGetNumDevs();

	for (i=-1; i<iNumDevs; i++)
	{
		if (!midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS)))
		{
			if (i == id)
			{
				found = true;
				break;
			}
		}
	}
	
	if (!found)
	{
		return HP_ERR_MIDI_DEVICE;
	}
	else
	{
		MIDIFile::MIDI_device = id;
		return HP_ERR_NONE;
	}
}

HP_MIDIFILE_API UINT HP_Play(HP_MIDIFILE_MODULE * hphandle, int selected, bool send_before)
{
//	MIDIFile::message("HP_Play");
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	
	// selected:	HP_ALL
	//				HP_SELECTED
	// send_before: HP_SEND_EFFECT_EVENTS_BEFORE
	//				HP_NO_SEND_BEFORE

	return hphandle->play(selected, send_before);
}


HP_MIDIFILE_API UINT HP_SetPlayStopWait(HP_MIDIFILE_MODULE * hphandle, int modus)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

    if ((modus < HP_GO_PLAY) || (modus > HP_WAIT_PLAY))
	{
		return HP_ERR_PLAY;
	}

	return hphandle->set_play_stop_wait(modus);
}


HP_MIDIFILE_API UINT HP_SetPlayTime(HP_MIDIFILE_MODULE * hphandle, int new_time)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}

/*
    if ((modus < HP_GO_PLAY) || (modus > HP_WAIT_PLAY))
	{
		return HP_ERR_PLAY;
	}
*/
	return hphandle->set_play_time(new_time);
}


HP_MIDIFILE_API bool HP_IsPlaying(HP_MIDIFILE_MODULE * hphandle)
{
	if (hphandle==NULL) 
	{
		return false;
	}
	return hphandle->is_playing(); 
}

HP_MIDIFILE_API long HP_Duration(HP_MIDIFILE_MODULE * hphandle, int modus)
{
	if (hphandle==NULL) 
	{
		return -1;
	}

	return hphandle->duration(modus); 
}

HP_MIDIFILE_API long HP_PlayTime(HP_MIDIFILE_MODULE * hphandle)
{
	if (hphandle==NULL) 
	{
		return -1;
	}
	return hphandle->get_play_time();
}

HP_MIDIFILE_API long HP_PlayEventTime(HP_MIDIFILE_MODULE * hphandle)
{
	if (hphandle==NULL) 
	{
		return -1;
	}
	return hphandle->get_play_event_time();
}

HP_MIDIFILE_API UINT HP_PlayTempo(HP_MIDIFILE_MODULE * hphandle, int percent)
{
	if (hphandle==NULL) 
	{
		return -1;
	}
	return hphandle->play_tempo(percent);
}

HP_MIDIFILE_API UINT HP_PlayTranspose(HP_MIDIFILE_MODULE * hphandle, int steps, const bool transp_list[16])
{
	if (hphandle==NULL) 
	{
		return -1;
	}
	return hphandle->play_transpose(steps,transp_list);
}

HP_MIDIFILE_API UINT HP_PlaySingleEvent(HP_MIDIFILE_MODULE * hphandle, unsigned char* event_data, int length)
{
	if (hphandle==NULL) 
	{
		return -1;
	}

	if ((event_data[0]&0xf0 == 0xf0) && (event_data[0]&0x0f != 0))
	{
		return HP_ERR_PLAY_NO_MIDIEVENT;
	}


	return hphandle->play_single_event(event_data, length);
}


HP_MIDIFILE_API UINT HP_PlayMute(HP_MIDIFILE_MODULE * hphandle, const bool mute_list[16])
{
	if (hphandle==NULL) 
	{
		return -1;
	}
	return hphandle->play_mute(mute_list);
}

HP_MIDIFILE_API UINT HP_InitPlayMonitor(HP_MIDIFILE_MODULE * hphandle, HP_PLAY_MONITOR * pm_instance)
{
	if (hphandle==NULL) 
	{
		return -1;
	}
	return hphandle->init_play_monitor(pm_instance);
}

HP_MIDIFILE_API UINT HP_CWInsLoad(HP_MIDIFILE_MODULE * hphandle, const char* name)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	return (hphandle->insfile)->load(name);
}

HP_MIDIFILE_API UINT HP_CWInsGetDefs(HP_MIDIFILE_MODULE * hphandle, HP_CWDEF** defs, int *no_defs)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	return (hphandle->insfile)->get_defs(defs,no_defs);
}

HP_MIDIFILE_API UINT HP_CWInsSetDef(HP_MIDIFILE_MODULE * hphandle, int def_index)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	return (hphandle->insfile)->set_def(def_index);
}

extern HP_MIDIFILE_API UINT HP_CWInsGetBankVoiceNoteName(HP_MIDIFILE_MODULE * hphandle, 
		long bank, int voice, int note, char** bank_name,char** voice_name, char** note_name)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_bank_voice_note_name(bank, voice, note, bank_name, voice_name, note_name);
}

extern HP_MIDIFILE_API UINT HP_CWInsGetNoteNames(HP_MIDIFILE_MODULE * hphandle, long bank, int voice, char** bank_name, char** voice_name, HP_CWNOTE **note_names)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_note_names(bank, voice, bank_name, voice_name, note_names);
}


extern HP_MIDIFILE_API UINT HP_CWInsGetBanks(HP_MIDIFILE_MODULE * hphandle, HP_CWBANK** banks, int *no_banks)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_banks(banks, no_banks);
}


extern HP_MIDIFILE_API UINT HP_CWInsGetBankVoices(HP_MIDIFILE_MODULE * hphandle, 
						long bank, char** bank_name,HP_CWVOICE ** voices)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_bank_voices(bank, bank_name, voices);
}

extern HP_MIDIFILE_API UINT HP_CWInsGetBankVoiceList(HP_MIDIFILE_MODULE* hphandle, 
			               char *search_string, HP_CWBANKVOICE** bank_voice_list,
						   long *bank_voice_list_len)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_bank_voice_list(search_string, bank_voice_list,bank_voice_list_len);
}


extern HP_MIDIFILE_API UINT HP_CWInsGetControllerName(HP_MIDIFILE_MODULE * hphandle, int controller, char** controller_name)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_controller_name(controller, controller_name);
}

extern HP_MIDIFILE_API UINT HP_CWInsGetControllerNames(HP_MIDIFILE_MODULE * hphandle, HP_CWCONTROLLER** controller_names)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_controller_names(controller_names);
}

extern HP_MIDIFILE_API UINT HP_CWInsGetRPNName(HP_MIDIFILE_MODULE * hphandle, long rpn, char** rpn_name)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_rpn_name(rpn, rpn_name);
}

extern HP_MIDIFILE_API UINT HP_CWInsGetRPNNames(HP_MIDIFILE_MODULE * hphandle, HP_CWRPN ** rpn_names, long *no_rpn)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_rpn_names(rpn_names, no_rpn);
}

extern HP_MIDIFILE_API UINT HP_CWInsGetNRPNName(HP_MIDIFILE_MODULE * hphandle, long nrpn, char** nrpn_name)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_nrpn_name(nrpn, nrpn_name);
}

extern HP_MIDIFILE_API UINT HP_CWInsGetNRPNNames(HP_MIDIFILE_MODULE * hphandle, HP_CWNRPN ** nrpn_names, long *no_nrpn)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}
	return (hphandle->insfile)->get_nrpn_names(nrpn_names, no_nrpn);
}

extern HP_MIDIFILE_API UINT HP_CWInsIsDrumVoice(HP_MIDIFILE_MODULE* hphandle, long bank_no, int voice_no, int *is_drum)
{
	if (hphandle==NULL) 
	{
		return HP_ERR_UNINITIALIZED;
	}
	if (!((hphandle->insfile)->is_insfile_open()))
	{
		return HP_ERR_CWINS_FILE_MISSED;
	}
	if (!((hphandle->insfile)->is_insdef_open()))
	{
		return HP_ERR_CWINS_NO_INSTRUMENT;
	}

	return (hphandle->insfile)->is_drum_voice(bank_no, voice_no, is_drum);
}


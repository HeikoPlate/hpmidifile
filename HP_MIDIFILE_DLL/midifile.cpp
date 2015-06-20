/************************************************************************/
/************************************************************************/
/*                                                                      */
/* midifile.cpp: Implementation Function of the basic class MIDIFile    */
/*                                                                      */
/************************************************************************/
/************************************************************************/

#include "stdAfx.h"

#include "midifile.h"
#include "HP_midifile.h"

#include "MIDIEvent.h"
#include "MetaEvent.h"
#include "SysExEvent.h"

int MIDIFile::MIDI_device = 0; // initial value: Default MIDI device

//**************************************************************************
//*
//*		Basic Constructor for MIDIFile DLL
//*
//**************************************************************************

MIDIFile::MIDIFile()
{

//	message("MIDIFile()");
	int i;

	event_data_max = 512;
	event_data = new unsigned char[event_data_max];

	no_tracks = -1;
	mtrk_data = NULL;
	fpos = 0;
	track_pos = 0;
	smf  = -1;
	ppqn = -1;
	cps  = -1;
	no_of_sequencenumbers = 0;

	insert_first = false;

	eventlist = new Mev_Array();
	tempolist = new Tempo_Array();
	pos = 0;

	open_note = new Mev_Array();

	for (i=0; i<16; i++)
	{
		rpn_opc[i].msb   = NULL;
		rpn_opc[i].lsb   = NULL;
		nrpn_opc[i].msb  = NULL;
		nrpn_opc[i].lsb  = NULL;
		last_rpn_missed_data_lsb[i]  = NULL;
		last_nrpn_missed_data_lsb[i] = NULL;
		old_rpn_msb[i] = -1;
		old_nrpn_msb[i] = -1;
	}

	insfile   = new InsFile();

	source_mf = NULL;

	play_object = NULL;

	pm_instance = NULL;
}

//**************************************************************************
//*
//*		Destructor
//*
//**************************************************************************

MIDIFile::~MIDIFile()
{
//	int i;

	while (eventlist->get_length() > 0)
	{
		eventlist->delete_from_end();
	}
	delete[] event_data;
	delete open_note;
	delete eventlist;
	delete tempolist;
	delete insfile;

	if (play_object != NULL)
	{
		delete (PlayClass *)play_object;
	}

//	message("~MIDIFile()");
}


/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  Public Extras                                                                  */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/

//**************************************************************************
//*
//*		get number of events
//*
//**************************************************************************

UINT MIDIFile::get_no_events()
{
	return eventlist->get_length();
}

//**************************************************************************
//*
//*		get event
//*
//**************************************************************************

MIDIEvent* MIDIFile::get_event(int index)
{
	return eventlist->get(index);
}

/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  File Interface Functions: Load and Save                                        */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/

//**************************************************************************
//*
//*		load source file
//*
//**************************************************************************

UINT MIDIFile::load(const char *name)
{
	int track;
	int i;

	unsigned char mthd_header[14];
	unsigned char mtrk_header[8];

	int resulting_time = 0;
	int mtrk_length = -1;

	unsigned char akt_status = 0;
	unsigned char old_status = 0;

	fpos = 0; // byte-no in file

	/* Open for read and binary, will fail if file does not exist */
	if( (source_mf  = fopen(name, "rb" )) == NULL ) return HP_ERR_FILE_NOTFOUND;

	// read MThd chunk header

    if (!read_some_bytes(mthd_header,14))
	{
		fclose(source_mf);
		source_mf = NULL;
		return  HP_ERR_FILE_BAD;        /* Error loading file */
	}

	if ((mthd_header[0]!='M') ||
		(mthd_header[1]!='T') ||
		(mthd_header[2]!='h') ||
		(mthd_header[3]!='d')   )
	{
		fclose(source_mf);
		source_mf = NULL;
		return HP_ERR_FILE_FORMAT;      /* Unknown file format */;
	}

	// get file-characteristics smf, ppqn, cps

	smf = mthd_header[9]==0?HP_SMF0:(mthd_header[9]==1?HP_SMF1:-1);
	no_tracks = (int)mthd_header[10] * 256 + (int) mthd_header[11];

	if ((mthd_header[12]&0x80) == 0)
	{
		ppqn = (mthd_header[12]*256)+mthd_header[13];
		tempolist->ppqn = ppqn;
	}
	else
	{
		cps = (mthd_header[12]&0x7f)*mthd_header[13];
	}

	// read tracks
	for (track=0; track<no_tracks; track++)
	{
		if (!read_some_bytes(mtrk_header,8))
		{
			fclose(source_mf);
			source_mf = NULL;
			return  HP_ERR_FILE_BAD;        /* Error loading file */
		}

		if ((mtrk_header[0]!='M') ||
			(mtrk_header[1]!='T') ||
			(mtrk_header[2]!='r') ||
			(mtrk_header[3]!='k')   )
		{
			fclose(source_mf);
			source_mf = NULL;
			return HP_ERR_FILE_FORMAT;      /* Unknown file format */;
		}

		mtrk_length =
			256*(256*(256*mtrk_header[4]+ mtrk_header[5])+mtrk_header[6])+mtrk_header[7];

		if (mtrk_data!=NULL) delete[] mtrk_data;
		mtrk_data = new unsigned char[mtrk_length];

		if (!read_some_bytes(mtrk_data,mtrk_length))
		{
			fclose(source_mf);
			source_mf = NULL;
			return  HP_ERR_FILE_BAD;        /* Error loading track data */
		}

		track_pos = 0;
		resulting_time = 0; // actual clock

		// Loop over <delta_time><event>

		do
		{
			// eval delta time

			int delta_time = 0;
			unsigned char next_status;
			unsigned char status_or_nextbyte;
			unsigned char c;
			if ((delta_time = get_byte()) & 0x80)
			{
				delta_time &= 0x7f;
				do
				{
					delta_time = (delta_time<<7) + ((c = get_byte()) &0x7f);
				} while (c & 0x80);
			}

			// increment resulting time

			resulting_time += delta_time;

			if ((akt_status & 0xf0) != 0xf0) old_status = akt_status;

			// status

			status_or_nextbyte = get_byte();

			if ((status_or_nextbyte&0x80) != 0x00)
			{	// new status, store
				next_status = status_or_nextbyte;
				akt_status = status_or_nextbyte;
			}
			else
			{
				next_status = old_status;
				track_pos--;
			}

			// first byte into event_data, the following with scan_???

			event_data[0] = next_status;
			event_data_length = 1;

			switch (event_data[0] & 0xf0) // first nibble
			{
			case 0x80: // Note off
				scan_note_off();
				{
					int j;
					bool on_found = false;
					NoteOffEvent *noteoff = new NoteOffEvent(resulting_time,event_data,event_data_length);

					for (j=0; j<open_note->get_length(); j++)
					{
						NoteOnEvent* noteon = (NoteOnEvent*)open_note->get(j);
						if((noteoff->chan==noteon->chan) && (noteoff->note()==noteon->note()))
						{
							noteon->assoc_noteoff = noteoff;
							noteoff->assoc_noteon = noteon;
							open_note->remove(j); // remove from open_note, no delete of event
							on_found = true;
							break;
						}
					}

					if (on_found)
					{
						eventlist->add(noteoff);
					}
					else
					{	// don't insert lonely note_off
						delete noteoff;
					}
				}
				break;
			case 0x90: // Note on
				scan_note_on();
				// Note On with velocity 0 = Note Off
				if (event_data[2] != 0)
				{
					if (event_data[2]>0x7f) event_data[2]=0x7f;
					NoteOnEvent *noteon = new NoteOnEvent(resulting_time,event_data,event_data_length);
					open_note->add(noteon);
					eventlist->add(noteon);
				}
				else
				{
					int j;
					bool on_found = false;

					NoteOffEvent *noteoff = new NoteOffEvent(resulting_time,event_data,event_data_length);

					for (j=0; j<open_note->get_length(); j++)
					{
						NoteOnEvent* noteon = (NoteOnEvent*)open_note->get(j);
						if((noteoff->chan==noteon->chan) && (noteoff->note()==noteon->note()))
						{
							noteon->assoc_noteoff = noteoff;
							noteoff->assoc_noteon = noteon;
							open_note->remove(j); // remove from open_note, no delete of event
							on_found = true;
							break;
						}
					}
					if (on_found)
					{
						eventlist->add(noteoff);
					}
					else
					{	// don't insert lonely note_off
						delete noteoff;
					}
				}
				break;
			case 0xa0: // Aftertouch
				{
					scan_aftertouch();
					eventlist->add(new AftertouchEvent(resulting_time,event_data,event_data_length));
					break;
				}
			case 0xb0: // Control Change
				{
					scan_control_change();
					ControlChangeEvent *cev = new ControlChangeEvent(resulting_time,event_data,event_data_length);
					if ((cev->type == HP_RPN_MSB) ||
						(cev->type == HP_RPN_LSB) ||
						(cev->type == HP_NRPN_MSB)||
						(cev->type == HP_NRPN_LSB)||
						(cev->type == HP_DATA_ENTRY_MSB) ||
						(cev->type == HP_DATA_ENTRY_LSB) ||
						(cev->type == HP_DATA_INCREMENT) ||
						(cev->type == HP_DATA_DECREMENT)    )
					{

//			message("event_data[0] %x event_data[1] %x event_data[2] %x event_data_length %i",
//    		event_data[0],event_data[1],event_data[2],event_data_length);

						compute_rpn_nrpn(cev,-1);
						break;
					}
					eventlist->add(cev);
					break;
				}
			case 0xc0: // Program (Patch) Change
				{
					scan_patch_change();
					eventlist->add(new ProgramChangeEvent(resulting_time,event_data,event_data_length));
					break;
				}

			case 0xd0: // Channel Pressure
				scan_channel_pressure();
				eventlist->add(new ChannelPressureEvent(resulting_time,event_data,event_data_length));
				break;
			case 0xe0: // Pitch Wheel
				scan_pitch_wheel();
				eventlist->add(new PitchWheelEvent(resulting_time,event_data,event_data_length));
				break;
			case 0xf0: // Status f0, f1, f2,...
				{
					// look at second nibble of status
					short fn_type = event_data[0] & 0x0f;
					scan_0xfn(); // get event_data[1] and following


					switch (fn_type)
					{
					case 0: // sysex
						eventlist->add(new SysExEvent(resulting_time,event_data,event_data_length));
						break;
					case 0x0f:
						switch (event_data[1]) // meta-key
						{
						case 0x00:
							if (no_of_sequencenumbers==0)
							{
								eventlist->add(new SequenceNumberEvent(resulting_time,event_data,event_data_length));
								no_of_sequencenumbers++;
							}
							break;
						case 0x01:
							if (event_data_length > 3)
							{
								eventlist->add(new TextEvent(resulting_time,event_data,event_data_length));
							}
							break;
						case 0x02:
							if (event_data_length > 3)
							{
								eventlist->add(new CopyrightEvent(resulting_time,event_data,event_data_length));
							}
							break;
						case 0x03:
							if (event_data_length > 3)
							{
								eventlist->add(new SongSequTrackNameEvent(resulting_time,event_data,event_data_length));
							}
							break;
						case 0x04:
							if (event_data_length > 3)
							{
								eventlist->add(new InstrumentNameEvent(resulting_time,event_data,event_data_length));
							}
							break;
						case 0x05:
							if (event_data_length > 3)
							{
								eventlist->add(new LyricEvent(resulting_time,event_data,event_data_length));
							}
							break;
						case 0x06:
							if (event_data_length > 3)
							{
								eventlist->add(new MarkerEvent(resulting_time,event_data,event_data_length));
							}
							break;
						case 0x07:
							if (event_data_length > 3)
							{
								eventlist->add(new CuePointEvent(resulting_time,event_data,event_data_length));
							}
							break;
						case 0x20:
							eventlist->add(new ChannelPrefixEvent(resulting_time,event_data,event_data_length));
							break;
						case 0x21:
							eventlist->add(new MidiPortEvent(resulting_time,event_data,event_data_length));
							break;
						case 0x2f:
//							eventlist->add(new EndMtrkEvent(resulting_time,event_data,event_data_length));
							continue;	// Last event of a track, do not store
						case 0x51:
							{
								TempoEvent *tempoevent = new TempoEvent(resulting_time,event_data,event_data_length);
								tempolist->add(resulting_time,(tempoevent->msq)/ppqn);
								eventlist->add(tempoevent);
							}
							break;
						case 0x54:
							eventlist->add(new SmpteOffsetEvent(resulting_time,event_data,event_data_length));
							break;
						case 0x58:
							eventlist->add(new TimeSignatureEvent(resulting_time,event_data,event_data_length));
							break;
						case 0x59:
							{
								try
								{
									eventlist->add(new KeySignatureEvent(resulting_time,event_data,event_data_length));
								}
								catch(bool correct_syntax)
								{
									if (!correct_syntax)
									{
										correct_syntax = false; // dummy assignment
									}
								}
							}
							break;
						case 0x7f:
							{
								// YAMAHA Meta and XF-Meta-Events
								if (!is_xg_event(event_data,event_data_length))
								{
									eventlist->add(new MetaEvent(resulting_time,event_data,event_data_length));
									break;
								}
								unsigned char ymeta_basictype;
								unsigned char ymeta_type;
								unsigned char ymeta_additional_type;
								{
									int j;
									for (j=3; j<event_data_length; j++)
									{
										if (event_data[j]==0x43)
										{
											ymeta_basictype = event_data[j+1];
											ymeta_type = event_data[j+2];
											ymeta_additional_type = event_data[j+4];
											break;
										}
									}
								}

								if (ymeta_basictype == 0x73)
								{
									switch (ymeta_type)
									{
									case 0x0a:
										switch (ymeta_additional_type)
										{
										case 0x04:
											eventlist->add(new QuickStartEvent(resulting_time,event_data,event_data_length));
											break;
										case 0x05:
											eventlist->add(new ScoreLayoutEvent(resulting_time,event_data,event_data_length));
											break;
										case 0x07:
											eventlist->add(new ScoreStartBarEvent(resulting_time,event_data,event_data_length));
											break;
										default:
											message("Unknown YMETA 73 Event, type %x, addional type %x",ymeta_type,ymeta_additional_type);
											eventlist->add(new YMetaEvent(resulting_time,event_data,event_data_length));
											break;
										}
										break;
									case 0x0d:
										eventlist->add(new KeyboardVoiceEvent(resulting_time,event_data,event_data_length));
										break;
									case 0x00:
									case 0x05:
									case 0x0c:
									case 0x21:
										// at this time unknown Y. meta event
										eventlist->add(new YMetaEvent(resulting_time,event_data,event_data_length));
										break;
									default:
										// additional ymetatypes found: 0x0x00, 0x0c, 0x21, 0x05
										message("Unknown YMETA 0x73 Event, type %x",ymeta_type);
										eventlist->add(new YMetaEvent(resulting_time,event_data,event_data_length));
										break;
									}
								}
								if (ymeta_basictype == 0x7b)
								{
									switch (ymeta_type)
									{
									case 0x00:
										eventlist->add(new XFVersionIdEvent(resulting_time,event_data,event_data_length));
										break;
									case 0x01:
										{
											try
											{
												eventlist->add(new ChordNameEvent(resulting_time,event_data,event_data_length));
											}
											catch(bool correct_syntax)
											{
												if (!correct_syntax)
												{
													correct_syntax = false; // dummy assignment
												}
											}
										}
										break;
									case 0x02:
										eventlist->add(new RehearsalMarkEvent(resulting_time,event_data,event_data_length));
										break;
									case 0x03:
										eventlist->add(new PhraseMarkEvent(resulting_time,event_data,event_data_length));
										break;
									case 0x04:
										eventlist->add(new MaxPhraseMarkEvent(resulting_time,event_data,event_data_length));
										break;
									case 0x05:
										eventlist->add(new FingeringNumberEvent(resulting_time,event_data,event_data_length));
										break;
									case 0x0c:
										eventlist->add(new GuideTrackFlagEvent(resulting_time,event_data,event_data_length));
										break;
									case 0x10:
										eventlist->add(new GuitarInformationFlagEvent(resulting_time,event_data,event_data_length));
										break;
									case 0x12:
										eventlist->add(new ChordVoicingForGuitarEvent(resulting_time,event_data,event_data_length));
										break;
									case 0x21:
										eventlist->add(new LyricsBitmapEvent(resulting_time,event_data,event_data_length));
										break;
									case 0x0a:
										// additional ymeta_type found: 0x0a
										eventlist->add(new YMetaEvent(resulting_time,event_data,event_data_length));
										break;
									default:

										message("Unknown YMETA 7b Event, type %x",ymeta_type);
										eventlist->add(new YMetaEvent(resulting_time,event_data,event_data_length));
										break;
									}
								}
							}
						}
						break;
						default:
							message("Unknown 0xf Event");
							eventlist->add(new MIDIEvent(resulting_time,event_data,event_data_length));
							break;
					}
				}
			}
		} while (track_pos < mtrk_length);
	}

	// any more data in midifile?
	data_behind_tracks = 1;
	unsigned char data = (unsigned char) fgetc(source_mf);
	data = (unsigned char) fgetc(source_mf);
	if (feof(source_mf) != 0) 
	{
		data_behind_tracks = 0;
	}

	if (no_tracks > 1) eventlist->sort();

	int ol = open_note->get_length();
	if (ol > 0)
	{
		// delete open notes
		open_note->sort();
		int i;		// open_note index
		int k = 0;	// eventlist index;
		MIDIEvent* open_note_on;

		for (i=0; i<ol; i++)
		{
			open_note_on = open_note->get(i);
			while ((eventlist->get(k) != open_note_on) && (k < eventlist->get_length()))
			{
				k++;
			}
			if (k >= eventlist->get_length()) break;
			eventlist->remove(k); // k points to next mev
			delete open_note_on;  // mev deleted, but not the pointer in open_note
		}
		open_note->clear();
	}
	eventlist->sort();
	eventlist->check();
	tempolist->check();

	fclose(source_mf);
	source_mf = NULL;

	// clear relicts in rpn/nrpn lists
	for (i=0; i<16; i++)
	{
		if (rpn_opc[i].msb != NULL)
		{
			if ((rpn_opc[i].msb)->assoc_data_fine != NULL)
			{
				delete (rpn_opc[i].msb)->assoc_data_fine;
				(rpn_opc[i].msb)->assoc_data_fine = NULL;
			}
			delete rpn_opc[i].msb;
			rpn_opc[i].msb = NULL;
		}

		if (rpn_opc[i].lsb != NULL)
		{
			delete rpn_opc[i].lsb;
			rpn_opc[i].lsb = NULL;
		}

		if (nrpn_opc[i].msb != NULL)
		{
			if ((nrpn_opc[i].msb)->assoc_data_fine != NULL)
			{
				delete (nrpn_opc[i].msb)->assoc_data_fine;
				(nrpn_opc[i].msb)->assoc_data_fine = NULL;
			}
			delete nrpn_opc[i].msb;
			nrpn_opc[i].msb = NULL;
		}

		if (nrpn_opc[i].lsb != NULL)
		{
			delete nrpn_opc[i].lsb;
			nrpn_opc[i].lsb = NULL;
		}

		last_rpn_missed_data_lsb[i] = NULL;
		last_nrpn_missed_data_lsb[i] = NULL;
	}

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		any data after tracks?
//*
//**************************************************************************

UINT MIDIFile::is_more_chunks()
{
	return data_behind_tracks;
}


//**************************************************************************
//*
//*		save destination file
//*
//**************************************************************************

UINT MIDIFile::save(const char *name, const int format)
{
	if (ppqn<0) return HP_ERR_NO_PPQN;

	eventlist->check();

	//delete all notes with zero length
	select(HP_ALL_EVENTS);
	delete_short_notes_selected(0);
	deselect(HP_ALL_EVENTS);

	eventlist->check();

	int i,k;
	int no_of_songsequtrackname = 0;
	int last_resulting_time;

	/*							0	  1     2     4     4     5     6     7     8     9    10  	 11    12    13   */
	unsigned char MThd[14] = {0x4d, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
    /*	                                                                      MIDI-format|no tracks | ***PPQN***  */

	unsigned char MTrk[8]  = {0x4d, 0x54, 0x72, 0x6b, 0x00, 0x00, 0x00, 0x00};
    /*                                                ***four bytes length**                                      */

	unsigned char End_Of_Track[4] = {0x00,0xff, 0x2f, 0x00};

	/* Open destination file for write and binary */
	if( (dest_mf  = fopen(name, "wb" )) == NULL ) return HP_ERR_FILE_NOTOPEN;

	if (format == HP_SMF0)
	{
		if (smf == HP_SMF1)
		{
			// New eventlist with only one SongSequTrackNameEvent and without any MIDIPortEvent
			Mev_Array * tmp = new Mev_Array; // save old list

			for (i=0; i<eventlist->get_length(); i++)
			{
				MIDIEvent *tmp_mev = eventlist->get(i);
				if (tmp_mev->sub1type()==HP_META_EVENT)
				{

					if (tmp_mev->type==HP_SONG_TRACK_NAME)
					{
						if (no_of_songsequtrackname>0)
						{
							continue;
						}
						else
						{
							no_of_songsequtrackname++;
						}
					}
//					if (tmp_mev->type==HP_MIDI_PORT)
//					{
//						continue;
//					}
					if (tmp_mev->type==HP_CHANNEL_PREFIX)
					{
						continue;
					}
				}
				tmp->add(tmp_mev);
			}

			delete eventlist;
			eventlist = tmp;
		}
		eventlist->check();

		Mev_Array *new_eventlist = prep_for_save(eventlist); // delete status bytes if possible

		int track_length = new_eventlist->no_bytes() + 4;

		MThd[9]  = 0; // SMF 0
		MThd[11] = 1; // no of tracks
		MThd[12] = ppqn/256;
		MThd[13] = ppqn%256;


		int temp_length = track_length;
		for (i = 7; i > 3; i--)
		{
			MTrk[i] = temp_length & 0xff;
			temp_length >>=8;
		}

		put_bytes(MThd,14);
		put_bytes(MTrk,8);

		last_resulting_time = 0;

		for (i=0; i< new_eventlist->get_length(); i++)
		{
			MIDIEvent *temp = new_eventlist->get(i);
			int delta_time = temp->resulting_time - last_resulting_time;
			insert_var_length(delta_time);
			last_resulting_time = temp->resulting_time;
			put_bytes(temp->event_data,temp->event_data_length);
		}

		put_bytes(End_Of_Track,4);

		// erase new_eventlist
		while (new_eventlist->get_length() > 0)
		{
			new_eventlist->delete_from_end();
		}
		delete new_eventlist;

		for (i=0; i<dest_bytelist.get_length(); i++)
		{
			fputc((unsigned char)dest_bytelist[i],dest_mf);
		}
		dest_bytelist.clear();
	}

	if (format == HP_SMF1)
	{
		Mev_Array *chan_eventlist[17];     // 0 master_events, 1...16 chan-events
		Mev_Array *new_chan_eventlist[17]; // 0 master_events, 1...16 chan-events
		for	(i=0; i<17; i++)
		{
			chan_eventlist[i] = new Mev_Array;
		}

		// separate events in tracks
		for (i=0; i<eventlist->get_length(); i++)
		{
			MIDIEvent *tmp_mev = eventlist->get(i);
			int chan = tmp_mev->chan;

			if (tmp_mev->sub1type()==HP_META_EVENT)
			{
				int track_no = ((MetaEvent*)tmp_mev)->track;
				if (track_no != -1)
				{
					chan = track_no-1;
				}
			}
			if (chan < 0)
			{
				chan_eventlist[0]->add(tmp_mev);
			}
			else
			{
				chan_eventlist[chan+1]->add(tmp_mev);
			}
		}

		for (i=0; i<17; i++)
		{
			chan_eventlist[i]->check();
		}

		for (i=0; i<17;i++)
		{
			new_chan_eventlist[i] = prep_for_save(chan_eventlist[i]); // delete status if possible
		}

		MThd[9]  = 1; // SMF 1
		MThd[11] = 17; // no of tracks
		MThd[12] = ppqn/256;
		MThd[13] = ppqn%256;

		// fill data
		put_bytes(MThd,14); // MThd

		int track_length;

		for (i=0; i<17; i++)
		{
			track_length = new_chan_eventlist[i]->no_bytes() + 4; // all events + End of Track

			int temp_length = track_length;
			for (k = 7; k > 3; k--)
			{
				MTrk[k] = temp_length & 0xff;
				temp_length >>=8;
			}
			put_bytes(MTrk,8);

			last_resulting_time = 0;
			for (k=0; k< new_chan_eventlist[i]->get_length(); k++)
			{
				MIDIEvent *temp = new_chan_eventlist[i]->get(k);
				int delta_time = temp->resulting_time - last_resulting_time;
				insert_var_length(delta_time);
				last_resulting_time = temp->resulting_time;
				put_bytes(temp->event_data,temp->event_data_length);
			}
			put_bytes(End_Of_Track,4);
		}

		for (i=0; i<17;i++)
		{
			// erase new_eventlists
			while (new_chan_eventlist[i]->get_length() > 0)
			{
				new_chan_eventlist[i]->delete_from_end();
			}
			delete new_chan_eventlist[i];
		}

		for (i=0; i<dest_bytelist.get_length(); i++)
		{
			fputc((unsigned char)dest_bytelist[i],dest_mf);
		}
		dest_bytelist.clear();
	}
	fclose(dest_mf);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		local function to assemble RPN and NRPN
//*
//**************************************************************************


void MIDIFile::compute_rpn_nrpn(ControlChangeEvent *cev, int new_time)
{
	int chan = cev->chan;
	int time = cev->resulting_time;
	
	// ================================================================
	
	if (cev->type == HP_RPN_MSB)
	{
		// open nrpn?
		if (nrpn_opc[chan].msb != 0)
		{
			if ((nrpn_opc[chan].msb)->assoc_data_fine != NULL)
			{
				delete (nrpn_opc[chan].msb)->assoc_data_fine;
				(nrpn_opc[chan].msb)->assoc_data_fine = NULL;
			}
			delete nrpn_opc[chan].msb;
			nrpn_opc[chan].msb = NULL;
		}
		if (nrpn_opc[chan].lsb != 0)
		{
			delete nrpn_opc[chan].lsb;
			nrpn_opc[chan].lsb = NULL;
		}

		// allow rpn/nrpn without data lsb 
		last_rpn_missed_data_lsb[chan] = NULL;
		last_nrpn_missed_data_lsb[chan] = NULL;
		
		if (rpn_opc[chan].msb==NULL)
		{
			rpn_opc[chan].msb=cev;
// new
			old_rpn_msb[chan] = (cev->event_data)[2];
			if (rpn_opc[chan].lsb!=NULL)
			{
				rpn_opc[chan].msb->assoc_fine = rpn_opc[chan].lsb;
			}
			return;
		}

		// now rpn_opc[chan].msb already set

		delete rpn_opc[chan].msb;
		rpn_opc[chan].msb=cev;

		if (rpn_opc[chan].lsb!=NULL)
		{
			rpn_opc[chan].msb->assoc_fine = rpn_opc[chan].lsb;
		}
		return;
	}
	// ================================================================
	
	if (cev->type == HP_NRPN_MSB)
	{
		// open rpn?
		if (rpn_opc[chan].msb != 0)
		{
			if ((rpn_opc[chan].msb)->assoc_data_fine != NULL)
			{
				delete (rpn_opc[chan].msb)->assoc_data_fine;
				(rpn_opc[chan].msb)->assoc_data_fine = NULL;
			}
			delete rpn_opc[chan].msb;
			rpn_opc[chan].msb = NULL;
		}
		if (rpn_opc[chan].lsb != 0)
		{
			delete rpn_opc[chan].lsb;
			rpn_opc[chan].lsb = NULL;
		}

		// allow rpn/nrpn without data lsb 
		last_rpn_missed_data_lsb[chan] = NULL;
		last_nrpn_missed_data_lsb[chan] = NULL;
		
		if (nrpn_opc[chan].msb==NULL)
		{
			nrpn_opc[chan].msb=cev;
			if (nrpn_opc[chan].lsb!=NULL)
			{
				nrpn_opc[chan].msb->assoc_fine = nrpn_opc[chan].lsb;
			}
			return;
		}

		// now nrpn_opc[chan].msb already set

		delete nrpn_opc[chan].msb;
		nrpn_opc[chan].msb=cev;

		if (nrpn_opc[chan].lsb!=NULL)
		{
			nrpn_opc[chan].msb->assoc_fine = nrpn_opc[chan].lsb;
		}
		return;
	}

	// ================================================================
	
	if (cev->type == HP_RPN_LSB)
	{
		// open nrpn?
		if (nrpn_opc[chan].msb != 0)
		{
			if ((nrpn_opc[chan].msb)->assoc_data_fine != NULL)
			{
				delete (nrpn_opc[chan].msb)->assoc_data_fine;
				(nrpn_opc[chan].msb)->assoc_data_fine = NULL;
			}
			delete nrpn_opc[chan].msb;
			nrpn_opc[chan].msb = NULL;
		}
		if (nrpn_opc[chan].lsb != 0)
		{
			delete nrpn_opc[chan].lsb;
			nrpn_opc[chan].lsb = NULL;
		}

		// allow rpn/nrpn without data lsb 
		last_rpn_missed_data_lsb[chan] = NULL;
		last_nrpn_missed_data_lsb[chan] = NULL;
		
		if (rpn_opc[chan].lsb==NULL)
		{
			rpn_opc[chan].lsb=cev;
			if (rpn_opc[chan].msb!=NULL)
			{
				rpn_opc[chan].msb->assoc_fine = rpn_opc[chan].lsb;
			}
			else
			{
				// RPN_LSB without starting RPN_MSB 
				// if old_rpn_msb[chan] >= 0
					// create *cev1 (or needed event ?) with event_data 0xb<chan>, 0x65, 0x<old_rpn_msb>, length=3;
					// call recursively compute_rpn_nrpn with cev1 followed by  add(cev1) and same call 
					//with cev as before
				// else
				// return;
/*
				if (old_rpn_msb[chan] < 0)
				{
					return;
				}
				// create cev by msb of old_rpn_msb
				unsigned char event_data1[3]; 
				event_data1[0] = 0xbf&(unsigned char)chan;
				event_data1[1] = 0x65;
				event_data1[2] = old_rpn_msb[chan];
				int event_data1_length  = 3;
				ControlChangeEvent *cev1 = new ControlChangeEvent(time-1, event_data1, event_data1_length);
				eventlist->add(cev1);
				eventlist->sort();
				compute_rpn_nrpn(cev1,-1);
				compute_rpn_nrpn(cev,-1);
*/
			}

			return;
		}

		// now rpn_opc[chan].lsb already set

		delete rpn_opc[chan].lsb;
		rpn_opc[chan].lsb=cev;

		if (rpn_opc[chan].msb!=NULL)
		{
			rpn_opc[chan].msb->assoc_fine = rpn_opc[chan].lsb;
		}
		return;
	}

	// ================================================================
	
	if (cev->type == HP_NRPN_LSB)
	{
		// open rpn?
		if (rpn_opc[chan].msb != 0)
		{
			if ((rpn_opc[chan].msb)->assoc_data_fine != NULL)
			{
				delete (rpn_opc[chan].msb)->assoc_data_fine;
				(rpn_opc[chan].msb)->assoc_data_fine = NULL;
			}
			delete rpn_opc[chan].msb;
			rpn_opc[chan].msb = NULL;
		}
		if (rpn_opc[chan].lsb != 0)
		{
			delete rpn_opc[chan].lsb;
			rpn_opc[chan].lsb = NULL;
		}

		// allow rpn/nrpn without data lsb 
		last_rpn_missed_data_lsb[chan] = NULL;
		last_nrpn_missed_data_lsb[chan] = NULL;
		
		if (nrpn_opc[chan].lsb==NULL)
		{
			nrpn_opc[chan].lsb=cev;
			if (nrpn_opc[chan].msb!=NULL)
			{
				nrpn_opc[chan].msb->assoc_fine = nrpn_opc[chan].lsb;
			}
			return;
		}

		// now nrpn_opc[chan].lsb already set

		delete nrpn_opc[chan].lsb;
		nrpn_opc[chan].lsb=cev;

		if (nrpn_opc[chan].msb!=NULL)
		{
			nrpn_opc[chan].msb->assoc_fine = nrpn_opc[chan].lsb;
		}
		return;
	}

	// ================================================================
	
	if (cev->type == HP_DATA_ENTRY_MSB)
	{
		if ((rpn_opc[chan].msb!=NULL)&&(rpn_opc[chan].lsb!=NULL))
		{	// rpn must be completed and and add to eventlist
			(rpn_opc[chan].msb)->assoc_data_coarse = cev;

			(rpn_opc[chan].msb)->resulting_time = new_time==-1?time:new_time;
			eventlist->add(rpn_opc[chan].msb);

			(rpn_opc[chan].lsb)->resulting_time = (rpn_opc[chan].msb)->resulting_time;
			eventlist->add(rpn_opc[chan].lsb);
			cev->resulting_time = (rpn_opc[chan].msb)->resulting_time;
			eventlist->add(cev);

			if ((rpn_opc[chan].msb)->assoc_data_fine != NULL)
			{
				((rpn_opc[chan].msb)->assoc_data_fine)->resulting_time = 
					(rpn_opc[chan].msb)->resulting_time;
				eventlist->add((rpn_opc[chan].msb)->assoc_data_fine);
			}
			else
			{	// data fine may come later
				last_rpn_missed_data_lsb[chan] = rpn_opc[chan].msb;
			}
			// clear rpn
			rpn_opc[chan].msb = NULL;
			rpn_opc[chan].lsb = NULL;
			return;
		}
		if ((nrpn_opc[chan].msb!=NULL)&&(nrpn_opc[chan].lsb!=NULL))
		{	// nrpn must be completed and and add to eventlist
			(nrpn_opc[chan].msb)->assoc_data_coarse = cev;

			(nrpn_opc[chan].msb)->resulting_time = new_time==-1?time:new_time;
			eventlist->add(nrpn_opc[chan].msb);

			(nrpn_opc[chan].lsb)->resulting_time = (nrpn_opc[chan].msb)->resulting_time;
			eventlist->add(nrpn_opc[chan].lsb);
			cev->resulting_time = (nrpn_opc[chan].msb)->resulting_time;
			eventlist->add(cev);

			if ((nrpn_opc[chan].msb)->assoc_data_fine != NULL)
			{
				((nrpn_opc[chan].msb)->assoc_data_fine)->resulting_time = 
					(nrpn_opc[chan].msb)->resulting_time;
				eventlist->add((nrpn_opc[chan].msb)->assoc_data_fine);
			}
			else
			{	// data fine may come later
				last_nrpn_missed_data_lsb[chan] = nrpn_opc[chan].msb;
			}
			// clear nrpn
			nrpn_opc[chan].msb = NULL;
			nrpn_opc[chan].lsb = NULL;
			return;
		}
		
		// either rpn nor nrpn: delete data msb
		delete cev;
		return;
	}

	// ================================================================
	
	if (cev->type == HP_DATA_ENTRY_LSB)
	{
		if (last_rpn_missed_data_lsb[chan]!=0)
		{
			(last_rpn_missed_data_lsb[chan])->assoc_data_fine = cev;
			cev->resulting_time = (last_rpn_missed_data_lsb[chan])->resulting_time;
			last_rpn_missed_data_lsb[chan] = NULL;
			eventlist->add(cev);
			return;
		}
		if (last_nrpn_missed_data_lsb[chan]!=0)
		{
			(last_nrpn_missed_data_lsb[chan])->assoc_data_fine = cev;
			cev->resulting_time = (last_nrpn_missed_data_lsb[chan])->resulting_time;
			last_nrpn_missed_data_lsb[chan] = NULL;
			eventlist->add(cev);
			return;
		}
		if ((rpn_opc[chan].msb != NULL)&&(rpn_opc[chan].lsb != NULL))
		{
			if((rpn_opc[chan].msb)->assoc_data_fine==NULL)
			{
				(rpn_opc[chan].msb)->assoc_data_fine = cev;
			}
			else
			{
				delete (rpn_opc[chan].msb)->assoc_data_fine;
				(rpn_opc[chan].msb)->assoc_data_fine = cev;
			}
			cev->resulting_time = (rpn_opc[chan].msb)->resulting_time;
//			eventlist->add(cev);
			return;
		}
		if ((nrpn_opc[chan].msb != NULL)&&(nrpn_opc[chan].lsb != NULL))
		{
			if((nrpn_opc[chan].msb)->assoc_data_fine==NULL)
			{
				(nrpn_opc[chan].msb)->assoc_data_fine = cev;
			}
			else
			{
				delete (nrpn_opc[chan].msb)->assoc_data_fine;
				(nrpn_opc[chan].msb)->assoc_data_fine = cev;
			}
			cev->resulting_time = (nrpn_opc[chan].msb)->resulting_time;
//			eventlist->add(cev);
			return;
		}
		// either rpn nor nrpn: delete data lsb
		delete cev;
		return;
	}

	// ================================================================
	
	if ((cev->type == HP_DATA_INCREMENT)||(cev->type == HP_DATA_DECREMENT))
	{	// complete and save
		if ((rpn_opc[chan].msb != NULL)&&(rpn_opc[chan].lsb != NULL))
		{
			if ((rpn_opc[chan].msb)->assoc_data_fine != NULL)
			{
				delete (rpn_opc[chan].msb)->assoc_data_fine;
			}

			(rpn_opc[chan].msb)->assoc_data_idcrement = cev;

			(rpn_opc[chan].msb)->resulting_time = new_time==-1?time:new_time;
			eventlist->add(rpn_opc[chan].msb);

			(rpn_opc[chan].lsb)->resulting_time = (rpn_opc[chan].msb)->resulting_time;
			eventlist->add(rpn_opc[chan].lsb);
			cev->resulting_time = (rpn_opc[chan].msb)->resulting_time;
			eventlist->add(cev);

			rpn_opc[chan].msb = NULL;
			rpn_opc[chan].lsb = NULL;
			last_rpn_missed_data_lsb[chan] = NULL;
			return;
		}
		if ((nrpn_opc[chan].msb != NULL)&&(nrpn_opc[chan].lsb != NULL))
		{
			if ((nrpn_opc[chan].msb)->assoc_data_fine != NULL)
			{
				delete (nrpn_opc[chan].msb)->assoc_data_fine;
			}

			(nrpn_opc[chan].msb)->assoc_data_idcrement = cev;

			(nrpn_opc[chan].msb)->resulting_time = new_time==-1?time:new_time;
			eventlist->add(nrpn_opc[chan].msb);

			(nrpn_opc[chan].lsb)->resulting_time = (nrpn_opc[chan].msb)->resulting_time;
			eventlist->add(nrpn_opc[chan].lsb);
			
			cev->resulting_time = (nrpn_opc[chan].msb)->resulting_time;
			eventlist->add(cev);

			nrpn_opc[chan].msb = NULL;
			nrpn_opc[chan].lsb = NULL;
			last_nrpn_missed_data_lsb[chan] = NULL;
			return;
		}
		// either rpn nor nrpn: delete data lsb
		delete cev;
		return;
	}
}

/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  Utilities for user                                                                      */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/

//**************************************************************************
//*
//*		convert time to takt/beat/tick
//*
//**************************************************************************

UINT MIDIFile::get_takt_beat_tick(int resulting_time, int *takt, int *beat, int *tick)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	int i;
	MIDIEvent *mev;
	bool sort_needed = true;

	int last_time  = 0; // time of last time-signature
	int next_time;
	int last_num   = 4; // num of last time-signature (default at start: 4/4
	int next_num;
	int last_denum = 4; // denum of last time-signature
	int next_denum;
	int last_no_sixteenth_takt = (16*last_num)/last_denum;
	int next_no_sixteenth_takt;
	int last_takt  = 0; // takt of last time-signature, count from 0
	int next_takt;

	for (i=0; i<eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if (resulting_time < mev->resulting_time) break; // no relevant time-signature follows
		if ((mev->sub1type()==HP_META_EVENT) && (mev->type==HP_TIME_SIGNATURE))
		{
			next_time = mev->resulting_time;
			next_num = ((TimeSignatureEvent*)mev)->num;
			next_denum = ((TimeSignatureEvent*)mev)->denum;
			if (next_denum > 16)
			{
				message("denum > 16 in Time Signature");
				return HP_ERR_INVALID_DENUM;
			}

			next_no_sixteenth_takt = (16*next_num)/next_denum;
			next_takt =
				(next_time - last_time)==0?last_takt:
				last_takt+((4*(next_time-last_time))/(ppqn*last_no_sixteenth_takt));
			int must_next_time = last_time + ((next_takt-last_takt)*last_no_sixteenth_takt*ppqn)/4;

			if (must_next_time != next_time)
			{
				// posit mev at must_next_time
				mev->resulting_time = must_next_time;
				sort_needed = true;
			}

			last_time = next_time;
			last_num = next_num;
			last_denum = next_denum;
			last_no_sixteenth_takt = next_no_sixteenth_takt;
			last_takt = next_takt;
		}
	}
	if (sort_needed) eventlist->sort();
	return get_takt_beat_tick(resulting_time,last_time,last_takt,last_num,last_denum,takt,beat,tick);
}

//**************************************************************************
//*
//*		convert time to takt/beat/tick; an efficient version
//*
//**************************************************************************

UINT MIDIFile::get_takt_beat_tick(int resulting_time,
								  int time_of_taktchange,
								  int takt_of_taktchange,
								  int num,
								  int denum,
								  int *takt, int *beat, int *tick)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	int delta_time = resulting_time - time_of_taktchange;
	int ticks_in_takt = (4*num*ppqn)/denum;
	int ticks_in_beat = (4*ppqn)/denum;
	int result_takt = takt_of_taktchange + delta_time/ticks_in_takt;
	int result_beat = (delta_time/ticks_in_beat)%num;
	int result_tick = (delta_time < ticks_in_beat)?delta_time:
	                     delta_time%ticks_in_beat;

	*takt = result_takt;
	*beat = result_beat;
	*tick = result_tick;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		convert takt/beat/tick to time
//*
//**************************************************************************

UINT MIDIFile::get_time_from_takt_beat_tick(int takt, int beat, int tick, int *time)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	int i;
	MIDIEvent *mev;

	bool sort_needed = false;

	int last_time  = 0; // time of last time-signature
	int next_time;
	int last_num   = 4; // num of last time-signature (default at start: 4/4
	int next_num;
	int last_denum = 4; // denum of last time-signature
	int next_denum;
	int last_no_sixteenth_takt = (16*last_num)/last_denum;
	int next_no_sixteenth_takt;
	int last_takt  = 0; // takt of last time-signature, count from 0
	int next_takt;

	for (i=0; i<eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if ((mev->sub1type()==HP_META_EVENT) && (mev->type==HP_TIME_SIGNATURE))
		{
			next_time = mev->resulting_time;
			next_num = ((TimeSignatureEvent*)mev)->num;
			next_denum = ((TimeSignatureEvent*)mev)->denum;
			if (next_denum > 16)
			{
				message("denum > 16 in time-signature");
				return HP_ERR_INVALID_DENUM;
			}

			next_no_sixteenth_takt = (16*next_num)/next_denum;
			next_takt =
				(next_time - last_time)==0?last_takt:
				last_takt+((4*(next_time-last_time))/(ppqn*last_no_sixteenth_takt));

			int must_next_time = last_time + ((next_takt-last_takt)*last_no_sixteenth_takt*ppqn)/4;

			if (must_next_time != next_time)
			{
				// mev must be at must_next_time
				mev->resulting_time = must_next_time;
				sort_needed = true;
			}

			if (takt < next_takt) break; // no relevant time-signature follows

			last_time = next_time;
			last_num = next_num;
			last_denum = next_denum;
			last_no_sixteenth_takt = next_no_sixteenth_takt;
			last_takt = next_takt;
		}
	}

	// actual time signature arrived

	*time = last_time + (takt-last_takt)*last_no_sixteenth_takt*(ppqn/4)+(4*beat*ppqn)/last_denum + tick;

	if (sort_needed) eventlist->sort();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get last time of MIDI-file
//*
//**************************************************************************

UINT MIDIFile::get_last_time(int *time)
{
	if (ppqn < 0) return HP_ERR_NO_PPQN;

	MIDIEvent *mev = eventlist->get_last();
	*time = mev->resulting_time;
	return HP_ERR_NONE;
}


/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  Set Switches, for user                                                                   */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/

//**************************************************************************
//*
//*		insert at beginning (true) or end (false) of all events with same time
//*
//**************************************************************************

void MIDIFile:: set_first(bool first)
{
	insert_first = first;
}

/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  Stream Functions                                                                       */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/

//**************************************************************************
//*
//*		set stream position to beginning
//*
//**************************************************************************

void MIDIFile::rewind()
{
	pos = 0;
}

//**************************************************************************
//*
//*		set stream position at selected id
//*
//**************************************************************************

UINT MIDIFile::seek(int id)
{
	if ((id < 0) || (id >= eventlist->get_length()))
	{
		return HP_ERR_RANGE_EVENTLIST;
	}
	pos = id;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		settle_overlapping_notes(bool method)
//*		method = HP_ASSEMBLE (whole length) or
//**    method = HP_NO_ASSEMBLE (only first note remains)
//*
//**************************************************************************

UINT MIDIFile::settle_overlapping_notes(bool method)
{
	MIDIEvent *mev;

	int c; // channel
	int n; // note
	int i; // index eventlist
	int note;
	int note_on_time;
	int note_off_time;

	struct {MIDIEvent* nev; int on_time; int off_time;} last_note[128];

	for (c=0; c < 16; c++)
	{	// loop over all channels

		for (n=0; n<128; n++)
		{ // initalize last_note
			last_note[n].nev = NULL;
		}

		for (i=0; i<eventlist->get_length();i++)
		{
			mev = eventlist->get(i);
			if ((mev->type != HP_NOTE_ON)||(mev->chan != c))
			{	// loop over all note on of channel c
				continue;
			}
			note = ((NoteOnEvent*)mev)->note();

			note_on_time = mev->resulting_time;
			note_off_time = (((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time;
			
			if (note_on_time >= last_note[note].off_time)
			{
				last_note[note].nev = NULL;
			}

			if (last_note[note].nev == NULL)
			{	// no last note
				last_note[note].nev = mev;
				last_note[note].on_time = note_on_time;
				last_note[note].off_time = note_off_time;
				continue;
			}

			if (last_note[note].off_time <= note_on_time)
			{	// no overlappimg
				last_note[note].nev = NULL;
				continue;
			}

			if (note_off_time <= last_note[note].off_time)
			{	// actual note inside last note; delete this note
				delete_event(i);
				i--;
				continue;
			}
			else
			{  // actual note starts inside and ends later last note
				if (method == HP_NO_ASSEMBLE)
				{ // only old note remains
					delete_event(i);
					i--;
					continue;
				}
				if (method == HP_ASSEMBLE)
				{	// last note ends with off of actual note
					(((NoteOnEvent*)last_note[note].nev)->assoc_noteoff)->resulting_time =
						(((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time;
					delete_event(i);
					i--;
					last_note[note].nev = NULL;
					continue;
				}
			}
		}
	}
	eventlist->sort();
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		delete_short_notes_selected
//*
//**************************************************************************

UINT MIDIFile::delete_short_notes_selected(int limit_length)
{
	MIDIEvent *mev;

	int note_length;
	int i;

	for (i=0; i < eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if ((mev->type != HP_NOTE_ON)||(mev->selected==0))
		{
			continue;
		}

		note_length = ((((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time) -
			                  (mev->resulting_time);
		if (note_length <= limit_length)
		{
			delete_event(i);
			i--;
		}
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		delete event
//*
//**************************************************************************

UINT MIDIFile::delete_event(int id)
{
	int i,l;

	if ((id < 0) || (id >= eventlist->get_length()))
	{
		return HP_ERR_RANGE_EVENTLIST;
	}

	MIDIEvent *mev = eventlist->get(id);

	if (mev->type == HP_NOTE_OFF)
	{
		return HP_ERR_DELETE;
	}

	if (mev->type == HP_TEMPO)
	{
		tempolist->remove(mev->resulting_time);
	}

	if (mev->type == HP_NOTE_ON)
	{   // delete associated note off
		NoteOffEvent *noteoff = (NoteOffEvent*)(((NoteOnEvent*)mev)->assoc_noteoff);
		for (i=id; i<eventlist->get_length(); i++)
		{
			if (eventlist->get(i) == noteoff)
			{
				delete noteoff;
				eventlist->remove(i);
				if (i < pos) // allow stream functions furtheron
				{
					pos--;
				}
				break;
			}
		}
	}

	if ((mev->type==HP_RPN_LSB)||
		(mev->type==HP_NRPN_LSB)||
		(mev->type==HP_DATA_ENTRY_MSB)||
		(mev->type==HP_DATA_ENTRY_LSB)||
		(mev->type==HP_DATA_INCREMENT)||
		(mev->type==HP_DATA_DECREMENT)   )
	{
		return HP_ERR_DELETE;
	}

	if ((mev->type == HP_RPN_MSB) || (mev->type == HP_NRPN_MSB))
	{
		int k;

		ControlChangeEvent *delete_list[4] = {NULL,NULL,NULL,NULL};
		delete_list[0] = ((ControlChangeEvent*)mev)->assoc_fine;
		delete_list[1] = ((ControlChangeEvent*)mev)->assoc_data_coarse;
		delete_list[2] = ((ControlChangeEvent*)mev)->assoc_data_fine;
		delete_list[3] = ((ControlChangeEvent*)mev)->assoc_data_idcrement;

		// delete all associated controller events
		for (k=0; k<4; k++)
		{
			if (delete_list[k]==NULL) continue;
			for (l=0; l<eventlist->get_length(); l++)
			{
				if (eventlist->get(l) == delete_list[k])
				{
					delete delete_list[k];
					eventlist->remove(l);
					if (l < pos) // allow stream functions furtheron
					{
						pos--;
					}
					break;
				}
			}
		} // end for k loop
	}

	for (l=0; l<eventlist->get_length(); l++)
	{
		if (eventlist->get(l)==mev)
		{
			eventlist->remove(l);
			break;
		}
	}
	delete mev; // delete MIDI_Event

	if (l < pos) // allow stream functions furtheron
	{
		pos--;
	}
	return HP_ERR_NONE;
}

/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  Select- and Buffer Functions                                                               */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/

//**************************************************************************
//*
//*		select events
//*
//**************************************************************************


UINT MIDIFile::select(int index)
{
	int i;
	MIDIEvent* mev;

	if (index == HP_ALL_EVENTS)
	{
		for (i=0; i < eventlist->get_length(); i++)
		{
			mev = eventlist->get(i);
			mev->selected = 1;
			if (mev->type == HP_NOTE)
			{
				MIDIEvent *mev_noff = ((NoteOnEvent*)mev)->assoc_noteoff;
				if (mev_noff != NULL)
				{
					mev->selected = mev_noff->resulting_time;
					mev_noff->selected = 1;
				}
			}
			if ((mev->type == HP_RPN_MSB)||(mev->type == HP_NRPN_MSB))
			{
				if (((ControlChangeEvent*)mev)->assoc_fine!= NULL)
				{
					(((ControlChangeEvent*)mev)->assoc_fine)->selected = 1;
				}
				if (((ControlChangeEvent*)mev)->assoc_data_coarse!=NULL)
				{
					(((ControlChangeEvent*)mev)->assoc_data_coarse)->selected = 1;
				}
				if (((ControlChangeEvent*)mev)->assoc_data_fine!= NULL)
				{
					(((ControlChangeEvent*)mev)->assoc_data_fine)->selected = 1;
				}
				if (((ControlChangeEvent*)mev)->assoc_data_idcrement!=NULL)
				{
					(((ControlChangeEvent*)mev)->assoc_data_idcrement)->selected = 1;
				}
			}
		}
	}
	else
	{
		if (index >= eventlist->get_length())
		{
			return HP_ERR_RANGE_EVENTLIST;
		}
		mev = eventlist->get(index);
		mev->selected = 1;
		if (mev->type == HP_NOTE_ON)
		{
			int last_time = (((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time;
			mev->selected = last_time;
			(((NoteOnEvent*)mev)->assoc_noteoff)->selected = 1;

		}
		if ((mev->type == HP_RPN_MSB)||(mev->type == HP_NRPN_MSB))
		{
			if (((ControlChangeEvent*)mev)->assoc_fine!= NULL)
			{
				(((ControlChangeEvent*)mev)->assoc_fine)->selected = 1;
			}
			if (((ControlChangeEvent*)mev)->assoc_data_coarse!=NULL)
			{
				(((ControlChangeEvent*)mev)->assoc_data_coarse)->selected = 1;
			}
			if (((ControlChangeEvent*)mev)->assoc_data_fine!= NULL)
			{
				(((ControlChangeEvent*)mev)->assoc_data_fine)->selected = 1;
			}
			if (((ControlChangeEvent*)mev)->assoc_data_idcrement!=NULL)
			{
				(((ControlChangeEvent*)mev)->assoc_data_idcrement)->selected = 1;
			}
		}
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		select all events of a time interval and associated with a channel
//*
//**************************************************************************

void MIDIFile::select_by_chan_time(int chan, int time1, int time2)
{
	int i;
	MIDIEvent* mev;
	int time;

	for (i=0; i<eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		time = mev->resulting_time;

		if (time < time1) continue;
		if ((time2 != HP_NO_TIME_LIMIT)&&(time > time2)) continue;

		if (mev->chan==chan)
		{
			if ((mev->type == HP_NOTE_ON) && (time2 != HP_NO_TIME_LIMIT))
			{
				int off_time = (((NoteOnEvent*)mev)->assoc_noteoff)->resulting_time;
				if (off_time > time2)
				{
					mev->selected = time2;
				}
				else
				{
					mev->selected = off_time;
				}
				continue;
			}
			mev->selected = 1;
		}
	}
}

//**************************************************************************
//*
//*		deselect event
//*
//**************************************************************************

UINT MIDIFile::deselect(int id)
{
	int i;
	MIDIEvent* mev;

	if (id == HP_ALL_EVENTS)
	{
		for (i=0; i < eventlist->get_length(); i++)
		{
			mev = eventlist->get(i);
			mev->selected = 0;
		}
	}
	else
	{
		if ((id < 0) || (id >= eventlist->get_length()))
		{
			return HP_ERR_RANGE_EVENTLIST;
		}
		mev = eventlist->get(id);
		mev->selected = 0;
		if (mev->type == HP_NOTE_ON)
		{
			(((NoteOnEvent*)mev)->assoc_noteoff)->selected = 0;
		}
		if ((mev->type == HP_RPN_MSB)||(mev->type == HP_NRPN_MSB))
		{
			if (((ControlChangeEvent*)mev)->assoc_fine!= NULL)
			{
				(((ControlChangeEvent*)mev)->assoc_fine)->selected = 0;
			}
			if (((ControlChangeEvent*)mev)->assoc_data_coarse!=NULL)
			{
				(((ControlChangeEvent*)mev)->assoc_data_coarse)->selected = 0;
			}
			if (((ControlChangeEvent*)mev)->assoc_data_fine!= NULL)
			{
				(((ControlChangeEvent*)mev)->assoc_data_fine)->selected = 0;
			}
			if (((ControlChangeEvent*)mev)->assoc_data_idcrement!=NULL)
			{
				(((ControlChangeEvent*)mev)->assoc_data_idcrement)->selected = 0;
			}
		}
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		deselect all events associated with a channel
//*
//**************************************************************************

void MIDIFile::deselect_channel_events(int chan)
{
	int i;
	MIDIEvent* mev;

	for (i=0; i<eventlist->get_length(); i++)
	{
		mev = eventlist->get(i);
		if (mev->chan==chan)
		{
			mev->selected = 0;
		}
	}
}

//**************************************************************************
//*
//*		copy selected events to an external buffer
//*
//**************************************************************************

void MIDIFile::copy_selected_to_buf(Mev_Array * dest_mevs)
{
	int i;

	if (open_note->get_length() > 0)
	{
		for (i=0; i<open_note->get_length(); i++)
		{
			delete open_note->get(i);
		}
		open_note->clear();
	}

	for (i=0; i < eventlist->get_length(); i++)
	{
		MIDIEvent *mev = eventlist->get(i);
		if ( mev->selected > 0)
		{
			int j;
			int type = mev->type;
			switch (type)
			{
			case HP_NOTE_OFF:
				{
					NoteOffEvent *noteoff  = new NoteOffEvent();
					*noteoff = *((NoteOffEvent*)mev);

					for (j=0; j<open_note->get_length(); j++)
					{
						NoteOnEvent* noteon = (NoteOnEvent*)open_note->get(j);
						if((noteoff->chan==noteon->chan) && (noteoff->note()==noteon->note()))
						{
							noteon->assoc_noteoff = noteoff;
							noteoff->assoc_noteon = noteon;
							open_note->remove(j); // remove from open_note, no delete of event
							dest_mevs->add(noteoff);
							break;
						}
					}
					break;
				}
			case HP_NOTE_ON:
				{
					NoteOnEvent *noteon = new NoteOnEvent();
					*noteon = *((NoteOnEvent*)mev);
					open_note->add(noteon);
					dest_mevs->add(noteon);
					break;
				}
			case HP_AFTERTOUCH:
				{
					AftertouchEvent * aftertouch = new AftertouchEvent();
					*aftertouch = *((AftertouchEvent*)mev);
					dest_mevs->add(aftertouch);
					break;
				}
			case HP_BANK_SELECT_MSB:
			case HP_MODULATION:
			case HP_BREATH:
			case HP_PORTAMENTO_TIME:
			case HP_MAIN_VOLUME:
			case HP_PANPOT:
			case HP_EXPRESSION:
			case HP_BANK_SELECT_LSB:
			case HP_SUSTAIN:
			case HP_PORTAMENTO:
			case HP_SOSTENUTO:
			case HP_SOFT_PEDAL:
			case HP_HARMONIC_CONTENT:
			case HP_RELEASE_TIME:
			case HP_ATTACK_TIME:
			case HP_BRIGHTNESS:
			case HP_DECAY_TIME:
			case HP_VIBRATO_RATE:
			case HP_VIBRATO_DEPTH:
			case HP_VIBRATO_DELAY:
			case HP_PORTAMENTO_CONTROL:
			case HP_REVERB_SEND_LEVEL:
			case HP_CHORUS_SEND_LEVEL:
			case HP_VARIATION_SEND_LEVEL:
			case HP_ALL_SOUND_OFF:
			case HP_RESET_ALL_CONTROLLERS:
			case HP_ALL_NOTES_OFF:
			case HP_OMNI_OFF:
			case HP_OMNI_ON:
			case HP_MONO:
			case HP_POLY:
			case HP_CONTROLLER_TYPE: // Unknown controller number

			case HP_DATA_ENTRY_MSB: // Look for RPN, NRPN
			case HP_DATA_ENTRY_LSB: // Look for RPN, NRPN
			case HP_NRPN_LSB:		// Look for RPN, NRPN
			case HP_NRPN_MSB:
			case HP_RPN_LSB:		// Look for RPN, NRPN
			case HP_RPN_MSB:
			case HP_DATA_INCREMENT: // Look for RPN, NRPN
			case HP_DATA_DECREMENT: // Look for RPN, NRPN
				{
					ControlChangeEvent * cev = new ControlChangeEvent();
					*cev = *((ControlChangeEvent*)mev);
					dest_mevs->add(cev);
					break;
				}
			case HP_PROGRAM_CHANGE:
				{
					ProgramChangeEvent * pce = new ProgramChangeEvent();
					*pce = *((ProgramChangeEvent*)mev);
					dest_mevs->add(pce);
					break;
				}
			case HP_CHANNEL_PRESSURE:
				{
					ChannelPressureEvent * cpe = new ChannelPressureEvent();
					*cpe = *((ChannelPressureEvent*)mev);
					dest_mevs->add(cpe);
					break;
				}
			case HP_PITCH_WHEEL:
				{
					PitchWheelEvent * pwe = new PitchWheelEvent();
					*pwe = *((PitchWheelEvent*)mev);
					dest_mevs->add(pwe);
					break;
				}
			case HP_SYSEX:
				{
					SysExEvent * sev = new SysExEvent();
					*sev = *((SysExEvent*)mev);
					dest_mevs->add(sev);
					break;
				}
			case HP_SEQUENCE_NUMBER:
				{
					SequenceNumberEvent * sev = new SequenceNumberEvent();
					*sev = *((SequenceNumberEvent*)mev);
					dest_mevs->add(sev);
					break;
				}
			case HP_TEXT:
				{
					TextEvent * tev = new TextEvent();
					*tev = *((TextEvent*)mev);
					dest_mevs->add(tev);
					break;
				}
			case HP_COPYRIGHT:
				{
					CopyrightEvent * cev = new CopyrightEvent();
					*cev = *((CopyrightEvent*)mev);
					dest_mevs->add(cev);
					break;
				}
			case HP_SONG_TRACK_NAME:
				{
					SongSequTrackNameEvent * tev = new SongSequTrackNameEvent();
					*tev = *((SongSequTrackNameEvent*)mev);
					dest_mevs->add(tev);
					break;
				}
			case HP_INSTRUMENT:
				{
					InstrumentNameEvent * iev = new InstrumentNameEvent();
					*iev = *((InstrumentNameEvent*)mev);
					dest_mevs->add(iev);
					break;
				}
			case HP_LYRIC:
				{
					LyricEvent * iev = new LyricEvent();
					*iev = *((LyricEvent*)mev);
					dest_mevs->add(iev);
					break;
				}
			case HP_MARKER:
				{
					MarkerEvent * mav = new MarkerEvent();
					*mav = *((MarkerEvent*)mev);
					dest_mevs->add(mav);
					break;
				}
			case HP_CUE_POINT:
				{
					CuePointEvent * cpv = new CuePointEvent();
					*cpv = *((CuePointEvent*)mev);
					dest_mevs->add(cpv);
					break;
				}
			case HP_CHANNEL_PREFIX:
				{
					ChannelPrefixEvent * cpv = new ChannelPrefixEvent();
					*cpv = *((ChannelPrefixEvent*)mev);
					dest_mevs->add(cpv);
					break;
				}
			case HP_MIDI_PORT:
				{
					MidiPortEvent * mpe = new MidiPortEvent();
					*mpe = *((MidiPortEvent*)mev);
					dest_mevs->add(mpe);
					break;
				}
			case HP_TEMPO:
				{
					TempoEvent * tpe = new TempoEvent();
					*tpe = *((TempoEvent*)mev);
					dest_mevs->add(tpe);
					break;
				}
			case HP_SMPTE_OFFSET:
				{
					SmpteOffsetEvent * soe = new SmpteOffsetEvent();
					*soe = *((SmpteOffsetEvent*)mev);
					dest_mevs->add(soe);
					break;
				}
			case HP_TIME_SIGNATURE:
				{
					TimeSignatureEvent * tse = new TimeSignatureEvent();
					*tse = *((TimeSignatureEvent*)mev);
					dest_mevs->add(tse);
					break;
				}
			case HP_KEY_SIGNATURE:
				{
					KeySignatureEvent * kse = new KeySignatureEvent();
					*kse = *((KeySignatureEvent*)mev);
					dest_mevs->add(kse);
					break;
				}
			case HP_OTHER_META:
				{
					MetaEvent * me = new MetaEvent();
					*me = *((MetaEvent*)mev);
					dest_mevs->add(me);
					break;
				}
			case HP_SCORE_START_BAR:
				{
					ScoreStartBarEvent * se = new ScoreStartBarEvent();
					*se = *((ScoreStartBarEvent*)mev);
					dest_mevs->add(se);
					break;
				}
			case HP_QUICK_START:
				{
					QuickStartEvent * se = new QuickStartEvent();
					*se = *((QuickStartEvent*)mev);
					dest_mevs->add(se);
					break;
				}
			case HP_SCORE_LAYOUT:
				{
					ScoreLayoutEvent * se = new ScoreLayoutEvent();
					*se = *((ScoreLayoutEvent*)mev);
					dest_mevs->add(se);
					break;
				}
			case HP_KEYBOARD_VOICE:
				{
					KeyboardVoiceEvent * ke = new KeyboardVoiceEvent();
					*ke = *((KeyboardVoiceEvent*)mev);
					dest_mevs->add(ke);
					break;
				}
			case HP_OTHER_YAMAHA_META:
				{
					YMetaEvent * ye = new YMetaEvent();
					*ye = *((YMetaEvent*)mev);
					dest_mevs->add(ye);
					break;
				}
			case HP_XF_VERSION_ID:
				{
					XFVersionIdEvent * xe = new XFVersionIdEvent();
					*xe = *((XFVersionIdEvent*)mev);
					dest_mevs->add(xe);
					break;
				}
			case HP_CHORD_NAME:
				{
					ChordNameEvent * ce = new ChordNameEvent();
					*ce = *((ChordNameEvent*)mev);
					dest_mevs->add(ce);
					break;
				}
			case HP_REHEARSAL_MARK:
				{
					RehearsalMarkEvent * re = new RehearsalMarkEvent();
					*re = *((RehearsalMarkEvent*)mev);
					dest_mevs->add(re);
					break;
				}
			case HP_PHRASE_MARK:
				{
					PhraseMarkEvent * pe = new PhraseMarkEvent();
					*pe = *((PhraseMarkEvent*)mev);
					dest_mevs->add(pe);
					break;
				}
			case HP_MAX_PHRASE_MARK:
				{
					MaxPhraseMarkEvent * me = new MaxPhraseMarkEvent();
					*me = *((MaxPhraseMarkEvent*)mev);
					dest_mevs->add(me);
					break;
				}
			case HP_FINGERING_NUMBER:
				{
					FingeringNumberEvent * fe = new FingeringNumberEvent();
					*fe = *((FingeringNumberEvent*)mev);
					dest_mevs->add(fe);
					break;
				}
			case HP_GUIDE_TRACK_FLAG:
				{
					GuideTrackFlagEvent * ge = new GuideTrackFlagEvent();
					*ge = *((GuideTrackFlagEvent*)mev);
					dest_mevs->add(ge);
					break;
				}
			case HP_GUITAR_INFORMATION_FLAG:
				{
					GuideTrackFlagEvent * ge = new GuideTrackFlagEvent();
					*ge = *((GuideTrackFlagEvent*)mev);
					dest_mevs->add(ge);
					break;
				}
			case HP_CHORD_VOICING_FOR_GUITAR:
				{
					ChordVoicingForGuitarEvent * ce = new ChordVoicingForGuitarEvent();
					*ce = *((ChordVoicingForGuitarEvent*)mev);
					dest_mevs->add(ce);
					break;
				}
			case HP_LYRICS_BITMAP:
				{
					LyricsBitmapEvent * le = new LyricsBitmapEvent();
					*le = *((LyricsBitmapEvent*)mev);
					dest_mevs->add(le);
					break;
				}

			case HP_RAW_EVENT:
				{
					MIDIEvent * me = new MIDIEvent();
					*me = *mev;
					dest_mevs->add(me);
					break;
				}
			default:
				{
					message("copy_selected_to: type not found %x",type);
					MIDIEvent * me = new MIDIEvent();
					*me = *mev;
					dest_mevs->add(me);
					break;
				}
			}
		}
	}

	// look for open note events

	unsigned char noteoff_data[] = {0x80, 0x00, 0x00};
	int noteoff_length = 3;

	for (i=0; i<open_note->get_length(); i++)
	{
		NoteOnEvent* noteon = (NoteOnEvent*)open_note->get(i);
		int last_time = noteon->selected;
		int note = noteon->note();
		int chan = noteon->chan;
		noteoff_data[0] = 0x80;
		noteoff_data[0] |= chan & 0x0f;
		noteoff_data[1] = note % 0x7f;
		NoteOffEvent* noteoff = new NoteOffEvent(last_time, noteoff_data, noteoff_length);
		noteon->assoc_noteoff = noteoff;
		noteoff->assoc_noteon = noteon;
		dest_mevs->add(noteoff);
	}

	for (i=open_note->get_length()-1; i>=0; i--)
	{
		open_note->remove(i);
	}
	open_note->clear();
}

//**************************************************************************
//*
//*		copy all selected events from buf
//*
//**************************************************************************

void MIDIFile::copy_selected_from_buf(Mev_Array *copybuf, int *copy_ppqn, int time, bool delete_in_dest, int del_events_chan)
{
	int i;
	int buf_length = copybuf->get_length();

	if (buf_length == 0) return;

	// initialize open-note buffer
	if (open_note->get_length() > 0)
	{
		for (i=0; i<open_note->get_length(); i++)
		{
			delete open_note->get(i);
		}
		open_note->clear();
	}

	// initialize rpn/nrpn-buffer
	for (i=0; i<16; i++)
	{
		rpn_opc[i].msb   = NULL;
		rpn_opc[i].lsb   = NULL;
		nrpn_opc[i].msb  = NULL;
		nrpn_opc[i].lsb  = NULL;
		last_rpn_missed_data_lsb[i] = NULL;
		last_nrpn_missed_data_lsb[i] = NULL;
	}

	// different ppqn in copybuf?

	if (ppqn != *copy_ppqn)
	{
		if (ppqn == -1)
		{	// set ppqn from buf
			ppqn = *copy_ppqn;
		}
		else
		{ // convert all times and ppqn of copy_buf
			change_ppqn_basic(copybuf,ppqn,copy_ppqn);
		}
	}

	int first_time = (copybuf->get(0))->resulting_time;


	// Something to be deleted in destination before?

	if (delete_in_dest == HP_DELETE)
	{
		int new_first_time = time;
		int new_last_time = (copybuf->get(buf_length-1))->resulting_time + time - first_time;

		int id, chan, time, etype;
		int old_pos = pos;
		rewind();
		while (read_event(&id,&chan,&time,&etype)==HP_ERR_NONE)
		{
			if (time < new_first_time) continue;
			if (time > new_last_time) break;

			MIDIEvent *mev = eventlist->get(id);

			if (del_events_chan==HP_NO_CHAN)
			{ // delete event with no channel
				if (mev->chan == -1) delete_event(id);
			}
			else if (del_events_chan==HP_ALL_EVENTS)
			{	// delete this event
				delete_event(id);
			}
			else if ((0<=del_events_chan) && (del_events_chan<=15))
			{
				if (mev->chan == del_events_chan) delete_event(id);
			}
		}
		pos = old_pos;
	}

	for (i=0; i<buf_length; i++)
	{
		int j;

		MIDIEvent* bev = copybuf->get(i);
		int new_time = (bev->resulting_time) + (time - first_time);
		int type = bev->type;
		switch (type)
		{
		case HP_NOTE_OFF:
			{
				NoteOffEvent *noteoff  = new NoteOffEvent();
				*noteoff = *((NoteOffEvent*)bev);

				for (j=0; j<open_note->get_length(); j++)
				{
					NoteOnEvent* noteon = (NoteOnEvent*)open_note->get(j);
					if((noteoff->chan==noteon->chan) && (noteoff->note()==noteon->note()))
					{
						noteon->assoc_noteoff = noteoff;
						noteoff->assoc_noteon = noteon;
						open_note->remove(j); // remove from open_note, no delete of event
						break;
					}
				}

				eventlist->add(noteoff);
				break;
			}
		case HP_NOTE_ON:
			{
				NoteOnEvent *noteon = new NoteOnEvent();
				*noteon = *((NoteOnEvent*)bev);
				open_note->add(noteon);
				eventlist->add(noteon);
				break;
			}
//***********************************

			case HP_AFTERTOUCH:
			{
				AftertouchEvent * aftertouch = new AftertouchEvent();
				*aftertouch = *((AftertouchEvent*)bev);
				eventlist->add(aftertouch);
				break;
			}

			case HP_BANK_SELECT_MSB:
			case HP_MODULATION:
			case HP_BREATH:
			case HP_PORTAMENTO_TIME:
			case HP_MAIN_VOLUME:
			case HP_PANPOT:
			case HP_EXPRESSION:
			case HP_BANK_SELECT_LSB:
			case HP_SUSTAIN:
			case HP_PORTAMENTO:
			case HP_SOSTENUTO:
			case HP_SOFT_PEDAL:
			case HP_HARMONIC_CONTENT:
			case HP_RELEASE_TIME:
			case HP_ATTACK_TIME:
			case HP_BRIGHTNESS:
			case HP_DECAY_TIME:
			case HP_VIBRATO_RATE:
			case HP_VIBRATO_DEPTH:
			case HP_VIBRATO_DELAY:
			case HP_PORTAMENTO_CONTROL:
			case HP_REVERB_SEND_LEVEL:
			case HP_CHORUS_SEND_LEVEL:
			case HP_VARIATION_SEND_LEVEL:
			case HP_ALL_SOUND_OFF:
			case HP_RESET_ALL_CONTROLLERS:
			case HP_ALL_NOTES_OFF:
			case HP_OMNI_OFF:
			case HP_OMNI_ON:
			case HP_MONO:
			case HP_POLY:
			case HP_CONTROLLER_TYPE:
			{
				ControlChangeEvent * cev = new ControlChangeEvent();
				*cev = *((ControlChangeEvent*)bev);
				eventlist->add(cev);
				break;
			}
			case HP_DATA_ENTRY_MSB: // Look for RPN, NRPN
			case HP_DATA_ENTRY_LSB: // Look for RPN, NRPN
			case HP_NRPN_LSB:		// Look for RPN, NRPN
			case HP_NRPN_MSB:
			case HP_RPN_LSB:		// Look for RPN, NRPN
			case HP_RPN_MSB:
			case HP_DATA_INCREMENT: // Look for RPN, NRPN
			case HP_DATA_DECREMENT: // Look for RPN, NRPN
			{
				ControlChangeEvent * cev = new ControlChangeEvent();
				*cev = *((ControlChangeEvent*)bev);
				compute_rpn_nrpn(cev,new_time);
				break;
			}
			case HP_PROGRAM_CHANGE:
			{
				ProgramChangeEvent * pce = new ProgramChangeEvent();
				*pce = *((ProgramChangeEvent*)bev);
				eventlist->add(pce);
				break;
			}
			case HP_CHANNEL_PRESSURE:
			{
				ChannelPressureEvent * cpe = new ChannelPressureEvent();
				*cpe = *((ChannelPressureEvent*)bev);
				eventlist->add(cpe);
				break;
			}
			case HP_PITCH_WHEEL:
			{
				PitchWheelEvent * pwe = new PitchWheelEvent();
				*pwe = *((PitchWheelEvent*)bev);
				eventlist->add(pwe);
				break;
			}
			case HP_SYSEX:
			{
				SysExEvent * sev = new SysExEvent();
				*sev = *((SysExEvent*)bev);
				eventlist->add(sev);
				break;
			}
			case HP_SEQUENCE_NUMBER:
			{
				SequenceNumberEvent * sev = new SequenceNumberEvent();
				*sev = *((SequenceNumberEvent*)bev);
				eventlist->add(sev);
				break;
			}
			case HP_TEXT:
			{
				TextEvent * tev = new TextEvent();
				*tev = *((TextEvent*)bev);
				eventlist->add(tev);
				break;
			}
			case HP_COPYRIGHT:
			{
				CopyrightEvent * cev = new CopyrightEvent();
				*cev = *((CopyrightEvent*)bev);
				eventlist->add(cev);
				break;
			}
			case HP_SONG_TRACK_NAME:
			{
				SongSequTrackNameEvent * tev = new SongSequTrackNameEvent();
				*tev = *((SongSequTrackNameEvent*)bev);
				eventlist->add(tev);
				break;
			}
			case HP_INSTRUMENT:
			{
				InstrumentNameEvent * iev = new InstrumentNameEvent();
				*iev = *((InstrumentNameEvent*)bev);
				eventlist->add(iev);
				break;
			}
			case HP_LYRIC:
			{
				LyricEvent * iev = new LyricEvent();
				*iev = *((LyricEvent*)bev);
				eventlist->add(iev);
				break;
			}
			case HP_MARKER:
			{
				MarkerEvent * mav = new MarkerEvent();
				*mav = *((MarkerEvent*)bev);
				eventlist->add(mav);
				break;
			}
			case HP_CUE_POINT:
			{
				CuePointEvent * cpv = new CuePointEvent();
				*cpv = *((CuePointEvent*)bev);
				eventlist->add(cpv);
				break;
			}
			case HP_CHANNEL_PREFIX:
			{
				ChannelPrefixEvent * cpv = new ChannelPrefixEvent();
				*cpv = *((ChannelPrefixEvent*)bev);
				eventlist->add(cpv);
				break;
			}
			case HP_MIDI_PORT:
			{
				MidiPortEvent * mpe = new MidiPortEvent();
				*mpe = *((MidiPortEvent*)bev);
				eventlist->add(mpe);
				break;
			}
			case HP_TEMPO:
			{
				TempoEvent * tpe = new TempoEvent();
				*tpe = *((TempoEvent*)bev);
				eventlist->add(tpe);
				break;
			}
			case HP_SMPTE_OFFSET:
			{
				SmpteOffsetEvent * soe = new SmpteOffsetEvent();
				*soe = *((SmpteOffsetEvent*)bev);
				eventlist->add(soe);
				break;
			}
			case HP_TIME_SIGNATURE:
			{
				TimeSignatureEvent * tse = new TimeSignatureEvent();
				*tse = *((TimeSignatureEvent*)bev);
				eventlist->add(tse);
				break;
			}
			case HP_KEY_SIGNATURE:
			{
				KeySignatureEvent * kse = new KeySignatureEvent();
				*kse = *((KeySignatureEvent*)bev);
				eventlist->add(kse);
				break;
			}
			case HP_OTHER_META:
			{
				MetaEvent * me = new MetaEvent();
				*me = *((MetaEvent*)bev);
				eventlist->add(me);
				break;
			}
			case HP_SCORE_START_BAR:
			{
				ScoreStartBarEvent * se = new ScoreStartBarEvent();
				*se = *((ScoreStartBarEvent*)bev);
				eventlist->add(se);
				break;
			}
			case HP_QUICK_START:
			{
				QuickStartEvent * se = new QuickStartEvent();
				*se = *((QuickStartEvent*)bev);
				eventlist->add(se);
				break;
			}
			case HP_SCORE_LAYOUT:
			{
				ScoreLayoutEvent * se = new ScoreLayoutEvent();
				*se = *((ScoreLayoutEvent*)bev);
				eventlist->add(se);
				break;
			}
			case HP_KEYBOARD_VOICE:
			{
				KeyboardVoiceEvent * ke = new KeyboardVoiceEvent();
				*ke = *((KeyboardVoiceEvent*)bev);
				eventlist->add(ke);
				break;
			}
			case HP_OTHER_YAMAHA_META:
			{
				YMetaEvent * ye = new YMetaEvent();
				*ye = *((YMetaEvent*)bev);
				eventlist->add(ye);
				break;
			}
			case HP_XF_VERSION_ID:
			{
				XFVersionIdEvent * xe = new XFVersionIdEvent();
				*xe = *((XFVersionIdEvent*)bev);
				eventlist->add(xe);
				break;
			}
			case HP_CHORD_NAME:
			{
				ChordNameEvent * ce = new ChordNameEvent();
				*ce = *((ChordNameEvent*)bev);
				eventlist->add(ce);
				break;
			}
			case HP_REHEARSAL_MARK:
			{
				RehearsalMarkEvent * re = new RehearsalMarkEvent();
				*re = *((RehearsalMarkEvent*)bev);
				eventlist->add(re);
				break;
			}
			case HP_PHRASE_MARK:
			{
				PhraseMarkEvent * pe = new PhraseMarkEvent();
				*pe = *((PhraseMarkEvent*)bev);
				eventlist->add(pe);
				break;
			}
			case HP_MAX_PHRASE_MARK:
			{
				MaxPhraseMarkEvent * me = new MaxPhraseMarkEvent();
				*me = *((MaxPhraseMarkEvent*)bev);
				eventlist->add(me);
				break;
			}
			case HP_FINGERING_NUMBER:
			{
				FingeringNumberEvent * fe = new FingeringNumberEvent();
				*fe = *((FingeringNumberEvent*)bev);
				eventlist->add(fe);
				break;
			}
			case HP_GUIDE_TRACK_FLAG:
			{
				GuideTrackFlagEvent * ge = new GuideTrackFlagEvent();
				*ge = *((GuideTrackFlagEvent*)bev);
				eventlist->add(ge);
				break;
			}
			case HP_GUITAR_INFORMATION_FLAG:
			{
				GuideTrackFlagEvent * ge = new GuideTrackFlagEvent();
				*ge = *((GuideTrackFlagEvent*)bev);
				eventlist->add(ge);
				break;
			}
			case HP_CHORD_VOICING_FOR_GUITAR:
			{
				ChordVoicingForGuitarEvent * ce = new ChordVoicingForGuitarEvent();
				*ce = *((ChordVoicingForGuitarEvent*)bev);
				eventlist->add(ce);
				break;
			}
			case HP_LYRICS_BITMAP:
			{
				LyricsBitmapEvent * le = new LyricsBitmapEvent();
				*le = *((LyricsBitmapEvent*)bev);
				eventlist->add(le);
				break;
			}
			case HP_RAW_EVENT:
			{
				MIDIEvent * me = new MIDIEvent();
				*me = *bev;
				eventlist->add(me);
				break;
			}
			default:
			{
				message("copy_selected_from: type not found");
				MIDIEvent * me = new MIDIEvent();
				*me = *bev;
				eventlist->add(me);
				break;
			}
		}

/*!*/	if (eventlist->get_length() > 0)
		{
			if(	(type != HP_DATA_ENTRY_MSB) && 
				(type != HP_DATA_ENTRY_LSB) && 
				(type != HP_NRPN_LSB) &&		
				(type != HP_NRPN_MSB) &&
				(type != HP_RPN_LSB) &&		
				(type != HP_RPN_MSB) &&
				(type != HP_DATA_INCREMENT) && 
				(type != HP_DATA_DECREMENT)    ) // compute_rpn_nrpn sets new_time
			{
				(eventlist->get_last())->resulting_time = new_time;
			}
		}
	}
	eventlist->sort();

	// clear relicts in rpn/nrpn lists
	for (i=0; i<16; i++)
	{
		if (rpn_opc[i].msb != NULL)
		{
			if ((rpn_opc[i].msb)->assoc_data_fine != NULL)
			{
				delete (rpn_opc[i].msb)->assoc_data_fine;
				(rpn_opc[i].msb)->assoc_data_fine = NULL;
			}
			delete rpn_opc[i].msb;
			rpn_opc[i].msb = NULL;
		}

		if (rpn_opc[i].lsb != NULL)
		{
			delete rpn_opc[i].lsb;
			rpn_opc[i].lsb = NULL;
		}

		if (nrpn_opc[i].msb != NULL)
		{
			if ((nrpn_opc[i].msb)->assoc_data_fine != NULL)
			{
				delete (nrpn_opc[i].msb)->assoc_data_fine;
				(nrpn_opc[i].msb)->assoc_data_fine = NULL;
			}
			delete nrpn_opc[i].msb;
			nrpn_opc[i].msb = NULL;
		}

		if (nrpn_opc[i].lsb != NULL)
		{
			delete nrpn_opc[i].lsb;
			nrpn_opc[i].lsb = NULL;
		}

		last_rpn_missed_data_lsb[i] = NULL;
		last_nrpn_missed_data_lsb[i] = NULL;
	}
}


//**************************************************************************
//**************************************************************************
//*
//*	play functions
//*
//**************************************************************************
//**************************************************************************

//**************************************************************************
//*
//*	start play
//*
//**************************************************************************

UINT MIDIFile::play(int selected, bool send_before)
{
	// play_object must be global member of MIDIFile
	// play_object will be set to NULL by the destructor of MIDIFile only

	UINT result;
	if (play_object != NULL)
	{
		delete (PlayClass *)play_object;
		play_object = NULL;
	}

	play_object = (void *)new PlayClass(this);
	if (play_object == NULL) return HP_ERR_PLAY;

	PlayClass *pl = (PlayClass *)play_object;
	pl->pm_instance = pm_instance; // send takt/beat-monitor-instance

	result = pl->play(selected,send_before);
	if (result != HP_ERR_NONE)
	{
		delete pl;
		play_object = NULL;
	}
	return result;
}

//**************************************************************************
//*
//*	duration of the whole (HP_ALL) or the selected parts of the song
//*
//**************************************************************************

long MIDIFile::duration(int modus)
{
	int i;
	long f_duration = 0;
	int last_time = 0;
	int actual_time = 0;

	if (modus == HP_ALL)
	{
		for (i=0; i<eventlist->get_length(); i++)
		{
			actual_time = (eventlist->get(i))->resulting_time;
			if (last_time < actual_time)
			{
				int millisec_delta_time =
					tempolist->millisec_delta_time(last_time,actual_time); // no of millisecs since preceeding event
				f_duration += (long) millisec_delta_time;
				last_time = actual_time;
			}
		}
		return f_duration;
	}

	if (modus == HP_SELECTED)
	{
		MIDIEvent * mev;
		long non_selected_time = 0;
		bool selected_found = false;

		for (i=0; i<eventlist->get_length(); i++)
		{
			mev = eventlist->get(i);

			if (selected_found)
			{
				actual_time = mev->resulting_time;
				if (mev->selected > 0)
				{
					if (last_time < actual_time)
					{
						int millisec_delta_time =
							tempolist->millisec_delta_time(last_time,actual_time); // no of millisecs since preceeding event
						f_duration += (long) millisec_delta_time;
						last_time = actual_time;
					}

					if (non_selected_time > 0)
					{	// selected after non_selected
						f_duration += non_selected_time;
						non_selected_time = 0;
					}
				}
				else
				{	// not selected after selected
					if (last_time < actual_time)
					{
						int millisec_delta_time =
							tempolist->millisec_delta_time(last_time,actual_time); // no of millisecs since preceeding event
						non_selected_time += (long) millisec_delta_time;
						last_time = actual_time;
					}
				}
			}
			else
			{	// not selected found
				if (mev->selected > 0)
				{
					selected_found = true;
					last_time = mev->resulting_time ;
				}
				else
				{ // up to here no selection happened
					continue;
				}
			}
		}
		return f_duration;
	}
	return 0;
}

//**************************************************************************
//*
//*	actual time played
//*
//**************************************************************************

long MIDIFile::get_play_time()
{
	if (play_object == NULL) return -1;

	PlayClass *pl = (PlayClass *)play_object;
	return pl->get_play_time();
}


//**************************************************************************
//*
//*	actual event_time played
//*
//**************************************************************************

long MIDIFile::get_play_event_time()
{
	if (play_object == NULL) return -1;

	PlayClass *pl = (PlayClass *)play_object;
	return pl->get_tick_number();
}

//**************************************************************************
//*
//*	playing equal instance of Play exist
//*
//**************************************************************************

bool MIDIFile::is_playing()
{
	if (play_object==NULL) return false;

	PlayClass *pl = (PlayClass *)play_object;
	return pl->is_playing;
}

//**************************************************************************
//*
//*	stop, go on or wait play
//*
//**************************************************************************


UINT MIDIFile::set_play_stop_wait(int modus)
{
	if (play_object == NULL) return HP_ERR_PLAY;
	PlayClass *pl = (PlayClass *)play_object;
	return pl->set_play_stop_wait(modus);
}

//**************************************************************************
//*
//*	set a new time position for play
//*
//**************************************************************************


UINT MIDIFile::set_play_time(int new_time)
{
	UINT result;

	if (play_object == NULL) return HP_ERR_PLAY;

	int last_time;
	get_last_time(&last_time);
	if ((new_time < 0) && (new_time >= last_time)) return HP_ERR_PLAY;


	PlayClass *pl = (PlayClass *)play_object;
	pl->set_play_stop_wait(HP_WAIT_PLAY);
	result = pl->set_play_time(new_time);
	if (result == HP_ERR_NONE)
	{
		pl->set_play_stop_wait(HP_GO_PLAY);
	}
	return result;
}


//**************************************************************************
//*
//*	change tempo of playing
//*
//**************************************************************************

UINT MIDIFile::play_tempo(int percent)
{
	if (play_object == NULL) return HP_ERR_PLAY;
	PlayClass *pl = (PlayClass *)play_object;
	return pl->play_tempo(percent);
}

//**************************************************************************
//*
//*	transpose while playing
//*
//**************************************************************************

UINT MIDIFile::play_transpose(int steps, const bool transp_list[16])
{
	UINT result;
	if (play_object == NULL) return HP_ERR_PLAY;
	PlayClass *pl = (PlayClass *)play_object;
	pl->set_play_stop_wait(HP_WAIT_PLAY);
	result = pl->play_transpose(steps, transp_list);
	pl->set_play_stop_wait(HP_GO_PLAY);
	return result;
}

//**************************************************************************
//*
//*	send single midievent while playing
//*
//**************************************************************************

UINT MIDIFile::play_single_event(unsigned char* event_data, int length)
{
	UINT result;
	if (play_object == NULL) return HP_ERR_PLAY;
	PlayClass *pl = (PlayClass *)play_object;
//	pl->set_play_stop_wait(HP_WAIT_PLAY);
	result = pl->play_single_event(event_data, length);
//	pl->set_play_stop_wait(HP_GO_PLAY);
	return result;
}


//**************************************************************************
//*
//*	mute while playing
//*
//**************************************************************************

UINT MIDIFile::play_mute(const bool mute_list[16])
{
	UINT result;
	if (play_object == NULL) return HP_ERR_PLAY;
	PlayClass *pl = (PlayClass *)play_object;
	pl->set_play_stop_wait(HP_WAIT_PLAY);
	result = pl->play_mute(mute_list);
	pl->set_play_stop_wait(HP_GO_PLAY);
	return result;
}

//**************************************************************************
//*
//*	Initialize play monitor
//*
//**************************************************************************

UINT MIDIFile::init_play_monitor(HP_PLAY_MONITOR * pmonitor_instance)
{
//	message("init_play_monitor");
	pm_instance = pmonitor_instance; // save pointer

	return HP_ERR_NONE;
}

/***********************************************************************************/
/***********************************************************************************/
/*                                                                                 */
/*  Local Help Function                                                            */
/*                                                                                 */
/***********************************************************************************/
/***********************************************************************************/

//**************************************************************************
//*
//*		reads length bytes from source file
//*
//**************************************************************************

bool MIDIFile::read_some_bytes(unsigned char data[], int length)
{
	int i;
	if (source_mf==NULL) return false;
	fseek(source_mf,fpos,0);

	for (i=0; i<length; i++)
	{
		data[i] = (unsigned char) getc(source_mf);
		if (ferror(source_mf)) return false;
	}

	fpos += length;
	return true;
}

//**************************************************************************
//*
//*		reads one byte from source file
//*
//**************************************************************************

unsigned char MIDIFile::get_byte(bool back)
{
	if (back)
	{
		return mtrk_data[track_pos];
	}
	else
	{
		track_pos++;
		return mtrk_data[track_pos-1];
	}
}

//**************************************************************************
//*
//*		writes length bytes  in destination list
//*
//**************************************************************************

void MIDIFile::put_bytes(unsigned char *bytes, int length)
{
	int i;
	for (i=0; i<length; i++)
	{
		dest_bytelist.add(bytes[i]);
	}
}

//**************************************************************************
//*
//*		writes one byte in destination list
//*
//**************************************************************************

void MIDIFile::put_byte(unsigned char byte)
{
	dest_bytelist.add(byte); // not source_mf!!
}

//**************************************************************************
//*
//*		reads length bytes from track data
//*
//**************************************************************************

void MIDIFile::get_bytes(unsigned char *data, int length)
{
	int i;
	for (i=0; i<length; i++)
	{
		track_pos++;
		data[i]= mtrk_data[track_pos-1];
	}
}

//**************************************************************************
//*
//*		convert VarLen-Bytes to time
//*
//**************************************************************************

int MIDIFile::eval_var_len(int len, unsigned char *var_len_bytes)
{
	int i;
	int no_bytes = 1;
	unsigned long buffer;
	buffer = len & 0x7f;

	while ( (len >>=7))
	{
		buffer <<=8;
		buffer |= ((len & 0x7f) | 0x80);
		no_bytes ++;
	}

	// put var_length-bytes array

	for (i=0; i<no_bytes; i++)
	{
		var_len_bytes[i] = (unsigned char)(buffer & 0xff);
		if (buffer & 0x80)
		{
			buffer >>= 8;
		}
		else
			break;
	}

	return no_bytes;
}

//**************************************************************************
//*
//*		convert time to VarLen bytes and store
//*
//**************************************************************************

int MIDIFile::insert_var_length(int delta_time)
{
	unsigned char var_len_bytes[16];
	int no_bytes = eval_var_len(delta_time, var_len_bytes);
	put_bytes(var_len_bytes,no_bytes);
	return no_bytes;
}

//**************************************************************************
//*
//*		convert VarLen bytes to int
//*
//**************************************************************************

int MIDIFile::convert_var_len_to_int(unsigned char *var_len_bytes)
{
	int i = 0;
	int value;
	unsigned char c;

	value = var_len_bytes[i];
	if ((value & 0x80) != 0)
	{
		value &= 0x7f;
		do
		{
			i++;
			c = var_len_bytes[i];
			value = (value<<7) + (c & 0x7f);
		} while ((c & 0x80) != 0);
	}
	return value;
}

//**************************************************************************
//*
//*		convert key to intern
//*
//**************************************************************************

UINT MIDIFile::convert_key_to_intern(const char *key, unsigned char * sf, unsigned char * mi)
{
	const struct { char* key; unsigned char sharp_flat; } list[30] =
	{
		{"CB",0xf9},
		{"GB",0xfa},
		{"DB",0xfb},
		{"AB",0xfc},
		{"EB",0xfd},
		{"BB",0xfe},
		{"F",0xff},
		{"C",0x00},
		{"G",0x01},
		{"D",0x02},
		{"A",0x03},
		{"E",0x04},
		{"B",0x05},
		{"F#",0x06},
		{"C#",0x07},
		{"CBM",0x02},
		{"GBM",0x03},
		{"DBM",0x04},
		{"ABM",0x05},
		{"EBM",0xfa},
		{"BBM",0xfb},
		{"FM",0xfc},
		{"CM",0xfd},
		{"GM",0xfe},
		{"DM",0xff},
		{"AM",0x00},
		{"EM",0x01},
		{"BM",0x02},
		{"F#M",0x03},
		{"C#M",0x04}
	};

	char * upper_key =  _strupr( _strdup(key));
	char majmin = 0;


	if (strlen(upper_key) == 3)
	{
		if (upper_key[2] == 'M')
		{
			majmin = 'm';
		}
		else
		{
			return HP_ERR_KEY;
		}
	}
	if (strlen(upper_key) == 2)
	{
		if (upper_key[1] == 'M')
		{
			majmin = 'm';
		}
	}

	int i;
	int keyfound = false;

	for (i=0; i < 30; i++)
	{
		if (strcmp(upper_key,list[i].key)==0)
		{
			keyfound = true;
			break;
		}
	}

	if (!keyfound) return HP_ERR_KEY;

	*sf = (unsigned char)(list[i].sharp_flat) & 0xff;
	*mi = (majmin=='m') ? 0x01:0x00;

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		extracts sysex data for user (without var len) from sysex-data
//*
//**************************************************************************

UINT MIDIFile::eval_sysex_data(unsigned char **sysex, int *sysex_length, const unsigned char * data)
{
	Uch_Array s_data;
	if (data[0] != 0xf0)
	{
		return HP_ERR_INVALID_SYSEX;
	}

	s_data.add(0xf0);

	int i=1;
	unsigned char byte;

	do
	{
		byte = data[i];
		if ((data[i]&0x80)!=0) break;
		s_data.add(byte);
		i++;
	} while (true);

	if (byte != 0xf7)
	{
		return HP_ERR_INVALID_SYSEX;
	}
	else
	{
		s_data.add(byte);
	}

	// eval var len
	unsigned char var_len_bytes[64];
	int no_bytes_var_len = eval_var_len(s_data.get_length()-1,var_len_bytes);

	for (i=0; i<no_bytes_var_len; i++)
	{
		s_data.insert(var_len_bytes[i],i+1);
	}

	*sysex = new unsigned char[s_data.get_length()];

	for (i=0; i< s_data.get_length(); i++)
	{
		(*sysex)[i] = s_data[i];
	}

	*sysex_length = s_data.get_length();

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		SCAN FUNCTIONS
//*		Scan events from source file
//*
//**************************************************************************

//**************************************************************************
//*
//*		scan_0xfn
//*		get f0, f1,...ff events, fill sequ only
//*
//**************************************************************************

void MIDIFile::scan_0xfn()
{
	int i;

	switch (event_data[0] & 0x0f)
	{
	case 0:		// SysEx
		{
			event_data[1] = get_byte();
			int event_data_free = 2;
			// read variable length
			int scr =  event_data[1];
			unsigned char c;

			if ((scr&0x80)!=0)
			{
				scr &=0x7f;
				do
				{
					c = get_byte();
					event_data[event_data_free] = c;
					event_data_free++;
					scr = (scr <<7) + (c & 0x7f);
				} while ((c & 0x80)!=0);
			}
			if (event_data_free+scr > event_data_max)
			{
				unsigned char* temp = event_data; // save old pointer
				event_data_max = event_data_free+scr + 256;
				event_data = new unsigned char[event_data_max];
				for (i=0; i<event_data_free; i++)
				{
					event_data[i] = temp[i];
				}
				delete[] temp;
			}
			get_bytes(&event_data[event_data_free],scr);
			event_data_length = event_data_free+scr;
		}
		break;
	case 1:		// Quarter Frame
		event_data[1] = get_byte();
		event_data_length = 2;
		break;
	case 2:		// Position Pointer
		get_bytes(&event_data[1],2);
		event_data_length = 3;
		break;
	case 3:		// Song Select
		event_data[1] = get_byte();
		event_data_length = 2;
		break;
	case 6:		// Tune Request
		break;
	case 8:		// Midi Clock
		break;
	case 9:		// Tick
		break;
	case 0x0a:	// Midi Start
		break;
	case 0x0b:	// Midi Continue
		break;
	case 0x0c:	// Midi Stop
		break;
	case 0x0e:	// Active Sense
		break;
	case 0x0f:	// Meta
		{
			get_bytes(&event_data[1],2); // type + first byte of len of meta-event

			// read variable length
			int scr =  event_data[2]; // first byte of len
			unsigned char c;
			int index = 3;

			if ((scr&0x80)!=0)
			{
				scr &=0x7f;
				do
				{
					c = get_byte();
					scr = (scr <<7) + (c & 0x7f);
					event_data[index] = c;
					index++;
				} while ((c & 0x80)!=0);
			}

			if (index+scr > event_data_max)
			{
				unsigned char* temp = event_data; // save old pointer
				event_data_max = index + scr + 256;
				event_data = new unsigned char[event_data_max];
				for (i=0; i<index; i++)
				{
					event_data[i] = temp[i];
				}
				delete[] temp;
			}

			get_bytes(&event_data[index],scr);
			event_data_length = index+scr;
		}
		break;
    default:
		break;
	}
}

//**************************************************************************
//*
//*		scan_sysex
//*		Reads the bytes behind 0xf0 upto 0xf7 and fills array of event_data
//*
//**************************************************************************

bool MIDIFile::scan_sysex()
{
	for (int i = 1; i < 256; i++)
	{
		event_data[i] = get_byte();

		if (event_data[i] == 0xf7)
		{
			event_data_length = i+1;
			return true;
		}
	}

	// no sysex end
	event_data_length = 0;

	return false;
}

//**************************************************************************
//*
//*		scan_note_off
//*		get note off events
//*
//**************************************************************************

void MIDIFile::scan_note_off()
{
	get_bytes(&event_data[1],2);
	event_data_length += 2;
}

//**************************************************************************
//*
//*		scan_note_on
//*		get note on events
//*
//**************************************************************************

void MIDIFile::scan_note_on()
{
	get_bytes(&event_data[1],2);
	event_data_length += 2;
}

//**************************************************************************
//*
//*		scan_aftertouch
//*		get aftertouch events
//*
//**************************************************************************

void MIDIFile::scan_aftertouch()
{
	get_bytes(&event_data[1],2);
	event_data_length += 2;
}

//**************************************************************************
//*
//*		scan_control_change
//*		get control change events
//*
//**************************************************************************

void MIDIFile::scan_control_change()
{
	get_bytes(&event_data[1],2);
	event_data_length += 2;
}

//**************************************************************************
//*
//*		scan_patch_change
//*		get patch change
//*
//**************************************************************************

void MIDIFile::scan_patch_change()
{
	get_bytes(&event_data[1],1);
	event_data_length++;
}

//**************************************************************************
//*
//*		scan_channel_pressure
//*		get channel pressure events
//*
//**************************************************************************

void MIDIFile::scan_channel_pressure()
{
	get_bytes(&event_data[1],1);
	event_data_length++;
}

//**************************************************************************
//*
//*		scan_pitch_wheel
//*		get pitch wheel events
//*
//**************************************************************************

void MIDIFile::scan_pitch_wheel()
{
	get_bytes(&event_data[1],2);
	event_data_length += 2;
}

//**************************************************************************
//*
//*		change_ppqn_basic
//*		change in given list
//*
//**************************************************************************

void MIDIFile::change_ppqn_basic(Mev_Array *mevlist, int newppqn, int *ppqn)
{
	int i;
	for (i=0; i < mevlist->get_length(); i++)
	{
		double o_time = (double)((mevlist->get(i))->resulting_time);
		int new_time = (int)(o_time*(double)newppqn/(double)(*ppqn));
		(mevlist->get(i))->resulting_time = new_time;
	}

	*ppqn = newppqn;
}

void MIDIFile::reorganize_tempolist(Mev_Array *mevlist, int ppqn)
{
	int i;
	if (tempolist != NULL) delete tempolist;

	tempolist = new Tempo_Array();
	tempolist->ppqn = ppqn;

	for (i=0; i < mevlist->get_length(); i++)
	{
		MIDIEvent *mev = mevlist->get(i);

		if (mev->type==HP_TEMPO)
		{

			tempolist->add(mev->resulting_time,(((TempoEvent*)mev)->msq)/ppqn);
		}
	}
	tempolist->sort();
}

//**************************************************************************
//*
//*	is_xg_event
//* look for 0x43 in meta-event
//*
//**************************************************************************

bool MIDIFile::is_xg_event(unsigned char* eventdata, int length)
{
	int k;
	// var len
	unsigned char len_byte;
	int meta_len = 0;

	for (k=2; k < length; k++)
	{
		len_byte = eventdata[k];
		if ((len_byte&0x80)==0)
		{
			meta_len = 128*meta_len+(int)len_byte;
			break;
		}
		else
		{
			meta_len = 128*meta_len+(int)(len_byte&0x7f);
		}
	}

	if (k+1 >= length) return false;
	if (eventdata[k+1] != 0x43) return false;
	if (length != k+1+meta_len) return false;

	return true;
}



//**************************************************************************
//*
//*	prepare track for save
//*
//**************************************************************************

Mev_Array* MIDIFile::prep_for_save(Mev_Array *track)
{
	int i,k;
	unsigned char status;
	unsigned char old_status = 0x00;
	bool del_status;
	Mev_Array * new_track  = new Mev_Array;

	MIDIEvent* mev;
	MIDIEvent* new_mev;

	for (i=0; i < track->get_length(); i++)
	{
		mev = track->get(i);
		status = mev->event_data[0];
		switch (status & 0xf0)
		{
		case 0x80:
		case 0x90:
		case 0xa0:
		case 0xb0:
		case 0xc0:
		case 0xd0:
		case 0xe0:
			del_status = (status == old_status);
			break;
		default:
			del_status = false;
			break;
		}
		old_status = status;

		int new_event_data_length = mev->event_data_length - (del_status?1:0);
		unsigned char* new_event_data = new unsigned char[new_event_data_length];

		for (k=(del_status?1:0); k< mev->event_data_length; k++)
		{
			new_event_data[k-(del_status?1:0)] = mev->event_data[k];
		}

		new_mev = new MIDIEvent(mev->resulting_time,new_event_data,new_event_data_length);

		new_track->add(new_mev);
		delete[] new_event_data;
	}
	return new_track;
}


//**************************************************************************
//*
//*	print formatted messages
//*
//**************************************************************************

void MIDIFile::message(const char* fmt,...)
{
	#ifdef HP_MESSAGE
	char buffer[129];
	va_list args;
	va_start(args, fmt);

	_vsnprintf(buffer,128, fmt,args);
	va_end(args);

	MessageBox(NULL, buffer,"HP_MIDIFILE_MESSAGE",MB_OK);
	#endif
}

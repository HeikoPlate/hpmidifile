// InsDef.cpp: Implementation of all classes associated with 
// an instrument definition
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InsDef.h"
#include "midifile.h"
//**************************************************************************
//**************************************************************************
//*
//* CwDefaults , singleton with constant lists
//*
//**************************************************************************
//**************************************************************************

//**************************************************************************
//*
//*		initial notes
//*		list of all note-names for melody voices
//*
//**************************************************************************

const std::string CwDefaults::initial_notes[128] =
{
/*  0*/	"C0","Db0","D0","Eb0","E0","F0","F#0","G0","Ab0","A0","Bb0","B0", 
/* 12*/	"C1","Db1","D1","Eb1","E1","F1","F#1","G1","Ab1","A1","Bb1","B1", 
/* 24*/	"C2","Db2","D2","Eb2","E2","F2","F#2","G2","Ab2","A2","Bb2","B2", 
/* 36*/	"C3","Db3","D3","Eb3","E3","F3","F#3","G3","Ab3","A3","Bb3","B3", 
/* 48*/	"C4","Db4","D4","Eb4","E4","F4","F#4","G4","Ab4","A4","Bb4","B4", 
/* 60*/	"C5","Db5","D5","Eb5","E5","F5","F#5","G5","Ab5","A5","Bb5","B5", 
/* 82*/	"C6","Db6","D6","Eb6","E6","F6","F#6","G6","Ab6","A6","Bb6","B6", 
/* 84*/	"C7","Db7","D7","Eb7","E7","F7","F#7","G7","Ab7","A7","Bb7","B7", 
/* 96*/	"C8","Db8","D8","Eb8","E8","F8","F#8","G8","Ab8","A8","Bb8","B8", 
/*108*/	"C9","Db9","D9","Eb9","E9","F9","F#9","G9","Ab9","A9","Bb9","B9", 
/*120*/	"C10","Db10","D10","Eb10","E10","F10","F#10","G10"
};

//**************************************************************************
//*
//*		GMPercussion
//*		list of all GM Percussion keys
//*
//**************************************************************************

const std::string CwDefaults::GMPercussion[128] =
{
/*  0*/	"0","1","2","3","4","5","6","7","8","9","10","11", 
/* 12*/	"12","13","14","15","16","17","18","19","20","21","22","23", 
/* 24*/	"24","25","26","27","28","29","30","31","32","33","34","Bass Drum",		
/* 36*/	"Bass Drum 1",		// C3
		"Side Stick",
		"Acoustic Snare",
		"Hand Clap",
		"Electric Snare",	
		"Low Floor Tom",
		"Closed Hi-Hat",
		"High Floor Tom",
		"Pedal Hi-Hat",
		"Low Tom",		
		"Open Hi-Hat",
		"Low-Mid Tom",
/* 48*/	"Hi-Mid Tom",		//	C4
		"Crash Cymbal 1",
		"High Tom",		
		"Ride Cymbal 1",
		"Chinese Cymbal",
		"Ride Bell",
		"Tambourine",
		"Splash Cymbal",	
		"Cowbell",
		"Crash Cymbal 2",
		"Vibraslap",
		"Ride Cymbal 2",
/* 60*/	"Hi Bongo",			// C5		
		"Low Bongo",
		"Mute Hi Conga",
		"Open Hi Conga",
		"Low Conga",
		"High Timbale",	
		"Low Timbale",
		"High Agogo",
		"Low Agogo",
		"Cabasa",
		"Maracas",	
		"Short Whistle",
/* 82*/	"Long Whistle",		// C6
		"Short Guiro",
		"Long Guiro",
		"Claves",	
		"Hi Wood Block",
		"Low Wood Block",
		"Mute Cuica",
		"Open Cuica",
		"Mute Triangle",
		"Open Triangle",
/* 82*/	"82","83",
/* 84*/ "84","85","86","87","88","89","90","91","92","93","94","95",
/* 96*/	"96","97","98","99","100","101","102","103","104","105","106","107",
/*108*/	"108","109","110","111","112","113","114","115","116","117","118","119",
/*120*/	"120","121","122","123","124","125","126","127"
};

//**************************************************************************
//*
//*		initial_items
//*		list of all number-names from"0" to "127"
//*
//**************************************************************************

const std::string CwDefaults::initial_items[128] =
{
	"0","1","2","3","4","5","6","8","8","9",
	"10","11","12","13","14","15","16","18","18","19",
	"20","21","22","23","24","25","26","28","28","29",
	"30","31","32","33","34","35","36","38","38","39",
	"40","41","42","43","44","45","46","48","48","49",
	"50","51","52","53","54","55","56","58","58","59",
	"60","61","62","63","64","65","66","68","68","69",
	"70","71","72","73","74","75","76","78","78","79",
	"80","81","82","83","84","85","86","88","88","89",
	"90","91","92","93","94","95","96","98","98","99",
	"100","101","102","103","104","105","106","108","108","109",
	"110","111","112","113","114","115","116","118","118","119",
	"120","121","122","123","124","125","126","127"
	};

//**************************************************************************
//*
//*		GMPatch
//*		list of all General MIDI voices
//*
//**************************************************************************

const std::string CwDefaults::GMPatch[128] =
{
  "Acoustic Grand",
  "Bright Acoustic",
  "Electric Grand",
  "Honky-Tonk",
  "Electric Piano1",
  "Electric Piano2",
  "Harpsichord",
  "Clav",
  "Celesta",
  "Glockenspiel",
  "Music Box",
  "Vibraphone",
  "Marimba",
  "Xylophone", 
  "Tubular Cells",
  "Dulcimer",
  "Drawbar Organ",
  "Percussive Organ",
  "Rock Organ",
  "Church Organ",
  "Reed Organ",
  "Accordian",
  "Harmonica",
  "Tango Accordian",
  "Acoustic Guitar(nylon)",
  "Acoustic Guitar(steel)",
  "Electric Guitar(jazz)",
  "Electric Guitar(clean)",
  "Electric Guitar(muted)",
  "Overdriven Guitar",
  "Distortion Guitar",
  "Guitar Harmonics",
  "Acoustic Bass",
  "Electric Bass(finger)",
  "Electric Bass(pick)",
  "Fretless Bass",
  "Slap Bass 1",
  "Slap Bass 2",
  "Synth Bass 1",
  "Synth Bass 2",
  "Violin",
  "Viola",
  "Cello",
  "Contrabass",
  "Tremolo Strings",
  "Pizzicato Strings",
  "Orchestral Strings",
  "Timpani",
  "String Ensemble 1",
  "String Ensemble 2",
  "SynthStrings 1",
  "SynthStrings 2",
  "Choir Aahs",
  "Voice Oohs",
  "Synth Voice",
  "Orchestra Hit",
  "Trumpet",
  "Trombone",
  "Tuba",
  "Muted Trumpet",
  "French Horn",
  "Brass Section",
  "SynthBrass 1",
  "SynthBrass 2",
  "Soprano Sax",
  "Alto Sax",
  "Tenor Sax",
  "Baritone Sax",
  "Oboe",
  "English Horn",
  "Bassoon",
  "Clarinet",
  "Piccolo",
  "Flute",
  "Recorder",
  "Pan Flute",
  "Blown Bottle",
  "Shakuhachi",
  "Whistle",
  "Ocarina",
  "Lead 1 (square)",
  "Lead 2 (sawtooth)",
  "Lead 3 (calliope)",
  "Lead 4 (chiff)",
  "Lead 5 (charang)",
  "Lead 6 (voice)",
  "Lead 7 (fifth)",
  "Lead 8 (bass+lead)",
  "Pad 1 (new age)",
  "Pad 2 (warm)",
  "Pad 3 (polysynth)",
  "Pad 4 (choir)",
  "Pad 5 (bowed)",
  "Pad 6 (metallic)",
  "Pad 7 (halo)",
  "Pad 8 (sweep)",
  "FX 1 (rain)",
  "FX 2 (soundtrack)",
  "FX 3 (crystal)",
  "FX 4 (atmosphere)",
  "FX 5 (brightness)",
  "FX 6 (goblins)",
  "FX 7 (echoes)",
  "FX 8 (sci-fi)",
  "Sitar",
  "Banjo",
  "Shamisen",
  "Koto",
  "Kalimba",
  "Bagpipe",
  "Fiddle",
  "Shanai",
  "Tinkle Bell",
  "Agogo",
  "Steel Drums",
  "Woodblock",
  "Taiko Drum",
  "Melodic Tom",
  "Synth Drum",
  "Reverse Cymbal",
  "Guitar Fret Noise",
  "Breath Noise",
  "Seashore",
  "Bird Tweet",
  "Telephone Ring",
  "Helicopter",
  "Applause",
  "Gunshot"
};

//**************************************************************************
//*
//*		is_initial_note
//*		true, if the given note-name is an initial-note
//*
//**************************************************************************

bool CwDefaults::is_initial_note(int index, std::string note_name)
{
	return (initial_notes[index]==note_name);
}

//**************************************************************************
//*
//*		is_initial_item
//*		true, if the given item-name ist the initial-item
//*
//**************************************************************************

bool CwDefaults::is_initial_item(int index, std::string item_name)
{
	return (initial_items[index]==item_name);
}

//**************************************************************************
//**************************************************************************
//*
//* CwLabelList, to hold all labels [name] of the instrument definition file
//*
//**************************************************************************
//**************************************************************************

//**************************************************************************
//*
//*		Standard Constructor
//*
//**************************************************************************

CwLabelList::CwLabelList()
{
}

//**************************************************************************
//*
//*		Constructor
//*
//**************************************************************************

CwLabelList::CwLabelList(std::ifstream *ifile)
{
	long initial_pos = ifile->tellg();

	ifile->clear();
	ifile->seekg(0);

	char line[256];
	std::string str_line;

	while (true)
	{
		ifile->getline(line,256);

		if (ifile->eof())
		{
			ifile->clear();
			break;
		}
		
		if (line[0] != '[')
		{
			continue;
		}
		
		str_line = line;

		int last = str_line.find("]");

		label_elem.label = str_line.substr(1,last-1);
		label_elem.pos = ifile->tellg();

		label_list.insert(label_list.end(),label_elem);
	}
	ifile->seekg(initial_pos);
}

//**************************************************************************
//*
//*		Destructor
//*
//**************************************************************************

CwLabelList::~CwLabelList()
{
	label_list.clear();
}

//**************************************************************************
//*
//*		label_pos
//*		delivers the file-position of [name] of a given label name
//*
//**************************************************************************

long CwLabelList::label_pos(std::string name)
{
	long pos = -1;

	for (list_l=label_list.begin(); list_l != label_list.end(); list_l++)
	{
		if (list_l->label == name)
		{
			pos = list_l->pos;
			break;
		}
	}
	return pos;
}

//**************************************************************************
//**************************************************************************
//*
//* CwVoice, to hold all data of a voice
//*
//**************************************************************************
//**************************************************************************

//**************************************************************************
//*
//*		Standard Constructor
//*
//**************************************************************************

CwVoice::CwVoice()
{
	label.erase();
}

//**************************************************************************
//*
//*		Constructor
//*
//**************************************************************************

CwVoice::CwVoice(std::ifstream *ifile, CwLabelList *label_list, std::string name)
{
	label = name;

	int i;

	// initialize notes
	for (i=0; i<128; i++)
	{
		notes[i].note_name = CwDefaults::initial_notes[i];
	}
}

//**************************************************************************
//*
//*		Destructor
//*
//**************************************************************************

CwVoice::~CwVoice()
{

}

//**************************************************************************
//*
//*		add_notes
//*		fills list of all note-names of the voice
//*
//**************************************************************************

void CwVoice::add_notes(std::ifstream *ifile, CwLabelList *label_list, std::string voice_name)
{
	char line[256];
	std::string str_line;

	const std::string based_on = "BasedOn=";
	const int len_based_on = based_on.length();

	long pos = (label_list->label_pos)(voice_name);
	if (pos == -1)
	{
		MIDIFile::message("No voice-note_label %s",voice_name.c_str());
		return; // no notes
	}

//	MIDIFile::message("CwVoice::add_notes from %s",voice_name.c_str());
	long initial_pos = ifile->tellg();

	ifile->clear();
	ifile->seekg(pos); // start at the label

	bool is_error;
	while (true)
	{
		ifile->getline(line,256);

		is_error = false;
		if (ifile->eof())
		{
			ifile->clear();
			break;
		}

		if (line[0] == '[')
		{
			break;
		}

		str_line = line;
		if (str_line.substr(0,len_based_on)==based_on)
		{
			std::string based_on_voice = str_line.substr(len_based_on);
			add_notes(ifile,label_list,based_on_voice);
			continue;
		}

		char ch = line[0];
		if ((ch<'0')||(ch>'9'))
		{
			continue;
		}

		int last = str_line.find("=");

		if (last < 0)
		{
			continue;
		}

		int no = 0;
		int i;
		
		for (i=0; i<last; i++)
		{
			ch = line[i];
			if ((ch<'0')||(ch>'9'))
			{
				MIDIFile::message("error note number, line %s",line);
				is_error = true;
				break;
			}
			no = 10*no+(ch-'0');
		}

		if (is_error) 
		{
			break;
		}

		if (no>128)
		{
			MIDIFile::message("error note number");
			break;
		}

		std::string text = str_line.substr(last+1);
		notes[no].note_name = text;
	}

	ifile->seekg(initial_pos);
}

//**************************************************************************
//*
//*		get_note_name
//*		delivers the name of the note with a given note-number
//*
//**************************************************************************

UINT CwVoice::get_note_name(int note, char** note_name)
{
	if ((note<0)||(note>128))
	{
		*note_name = new char[1];
		(*note_name)[0] = (char)0;
		MIDIFile::message("Note %i not found",note);
		return HP_ERR_CWINS_NO_NAME;
	}

	std::string str_note_name = notes[note].note_name;

	*note_name = new char[str_note_name.length()+1];
	strcpy(*note_name, str_note_name.c_str());

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get_note_names
//*		delivers a list of all notes of the voice
//*
//**************************************************************************

UINT CwVoice::get_note_names(HP_CWNOTE** note_names)
{
	*note_names = new HP_CWNOTE[128];
	int i;
	std::string short_notename;

	for (i=0; i<128; i++)
	{
		short_notename = (notes[i].note_name).substr(0,63);
		strcpy((*note_names)[i].note_name,short_notename.c_str());
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//**************************************************************************
//*
//* CwBank, to hold all about a Voice-Bank
//*
//**************************************************************************
//**************************************************************************

//**************************************************************************
//*
//*		Standard Constructor
//*
//**************************************************************************

CwBank::CwBank()
{
	label.erase();
}

//**************************************************************************
//*
//*		Constructor
//*
//**************************************************************************

CwBank::CwBank(std::ifstream *ifile, CwLabelList *label_list, std::string name)
{
	label = name;
	int i;

	const std::string based_on = "BasedOn=";
	const int len_based_on = based_on.length();
	based_on_bank = NULL;

	// initialize voices
	for (i=0; i<128; i++)
	{
		voices[i].voice_name = CwDefaults::initial_items[i];
		voices[i].voice = NULL;
	}

	// look for voices in file
	char line[256];
	std::string str_line;

	long pos = label_list->label_pos(name);
	if (pos == -1)
	{
//		MIDIFile::message("No Label %s",name.c_str());
		return;
	}

	long initial_pos = ifile->tellg();
	ifile->clear();
	ifile->seekg(pos); // start at the label

	// look for label "name"
	bool is_error;

	while (true)
	{
		ifile->getline(line,256);

		is_error = false;
		if (ifile->eof())
		{
			ifile->clear();
			break;
		}

		if (line[0] == '[')
		{
			break;
		}

		str_line = line;

		if (str_line.substr(0,len_based_on)==based_on)
		{
			std::string bank = str_line.substr(len_based_on);
			based_on_bank = new CwBank(ifile,label_list,bank);
		}

		char ch = line[0];
		if ((ch<'0')||(ch>'9'))
		{
			continue;
		}

		int last = str_line.find("=");

		if (last < 0)
		{
			continue;
		}

		int no = 0;
		
		for (i=0; i<last; i++)
		{
			ch = line[i];
			if ((ch<'0')||(ch>'9'))
			{
				MIDIFile::message("error CW Controller, line %s",line);
				is_error = true;
				break;
			}
			no = 10*no+(ch-'0');
		}

		if (is_error) 
		{
			break;
		}

		if (no>128)
		{
			MIDIFile::message("error CW Controller number");
			break;
		}

		std::string text = str_line.substr(last+1);
		voices[no].voice_name = text;
		voices[no].voice = new CwVoice(ifile,label_list,text);
	}

	// substitute BasedOn voices
	for (i=0; i<128; i++)
	{
		if ((voices[i].voice_name ==  CwDefaults::initial_items[i]) &&
			(based_on_bank != NULL) && (voices[i].voice != NULL))
		{
			voices[i].voice_name = based_on_bank->voice(i);
		}
	}

	if (based_on_bank != NULL)
	{
		delete based_on_bank;
		based_on_bank = NULL;
	}

	ifile->seekg(initial_pos);
}

//**************************************************************************
//*
//*		Destructor
//*
//**************************************************************************

CwBank::~CwBank()
{
	int i;
	for (i=0; i<128; i++)
	{
		if (voices[i].voice != NULL)
		{
			delete voices[i].voice;
		}
	}
	if (based_on_bank != NULL)
	{
		delete based_on_bank;
	}
}

//**************************************************************************
//*
//*		add_notes
//*		fills note-list of a given voice 
//*
//**************************************************************************

void CwBank::add_notes(std::ifstream *ifile, CwLabelList *label_list, int voice_no,std::string voice_name)
{
	if (voices[voice_no].voice != NULL)
	{
		(voices[voice_no].voice)->add_notes(ifile,label_list,voice_name);
	}
}

//**************************************************************************
//*
//*		get_voices
//*		delivers a list of all voices of the bank
//*
//**************************************************************************

UINT CwBank::get_voices(char **bank_name,HP_CWVOICE ** voice_array)
{
	int i;

	*bank_name = new char[label.length()+1];
	strcpy(*bank_name, label.c_str());

	*voice_array = new HP_CWVOICE[128];
	std::string short_voicename;
	for (i=0; i<128; i++)
	{
		short_voicename = ((voices[i]).voice_name).substr(0,63);
		strcpy(((*voice_array)[i]).voice_name,short_voicename.c_str());
	}

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get_note_names
//*		delivers a list of all note-names of a given voice
//*
//**************************************************************************

UINT CwBank::get_note_names(int voice, char ** voice_name, HP_CWNOTE** note_names)
{
	if ((voice<0)||(voice>128))
	{
		MIDIFile::message("Voice %i not found",voice);
		return HP_ERR_CWINS_NO_NAME;
	}

	CwVoice * voice_instance = voices[voice].voice;

	if (voice_instance==NULL)
	{
		*note_names = new HP_CWNOTE[128];
		int i;
		for (i=0; i<128;i++)
		{
			strcpy((*note_names)[i].note_name,(CwDefaults::initial_notes[i]).c_str());
		}
		*voice_name = new char[1];
		(*voice_name)[0] = (char)0;
		return HP_ERR_NONE;
	}

	std::string str_voice_name = voices[voice].voice_name;
	*voice_name = new char[str_voice_name.length()+1];
	strcpy (*voice_name, str_voice_name.c_str());
	return voice_instance->get_note_names(note_names);
}

//**************************************************************************
//*
//*		get_voice-note_name
//*		delivers a list of all note-names of a given voice-number
//*
//**************************************************************************

UINT CwBank::get_voice_note_name(int voice, int note,
		                       char** voice_name, char** note_name)
{
	if ((voice<0)||(voice>128))
	{
		*voice_name = new char[1];
		(*voice_name)[0] = (char)0;
		*note_name = new char[1];
		(*note_name)[0] = (char)0;
		MIDIFile::message("Voice %i not found",voice);
		return HP_ERR_CWINS_NO_NAME;
	}

	std::string str_voice_name = voices[voice].voice_name;
	*voice_name = new char[str_voice_name.length()+1];
	strcpy (*voice_name, str_voice_name.c_str());

	CwVoice * voice_instance = voices[voice].voice;

	if (voice_instance==NULL)
	{
		std::string str_note_name = 
			((note<0)||(note>128))?"":CwDefaults::initial_notes[note];
		*note_name = new char[str_note_name.length()+1];
		strcpy(*note_name,str_note_name.c_str());
		return HP_ERR_NONE;
	}

	return voice_instance->get_note_name(note,note_name);
}

//**************************************************************************
//*
//*		voice
//*		delivers the name of a voice by a given voice-number
//*
//**************************************************************************

std::string CwBank::voice(int voice_no)
{
	return voices[voice_no].voice_name;
}

//**************************************************************************
//**************************************************************************
//*
//* CwController, to hold all about controllers of an instrument definition
//*
//**************************************************************************
//**************************************************************************

//**************************************************************************
//*
//*		Standard Constructor
//*
//**************************************************************************

CwController::CwController()
{
	label.erase();
}

//**************************************************************************
//*
//*		Constructor
//*
//**************************************************************************

CwController::CwController(std::ifstream *ifile, CwLabelList *label_list, std::string name)
{
	label = name;
	int i;

	for (i=0; i<128; i++)
	{
		ctrl_names[i] = CwDefaults::initial_items[i];
	}

	add_names(ifile, label_list, name);
}

//**************************************************************************
//*
//*		add_names
//*		fills the internal list of all controller-names
//*
//**************************************************************************

void CwController::add_names(std::ifstream *ifile, CwLabelList *label_list, std::string name)
{
	int i;
	const std::string based_on = "BasedOn=";
	const int len_based_on = based_on.length();
	based_on_name = "";

	char line[256];
	std::string str_line;

	unsigned char ch;
	bool is_error = false;
	long pos = label_list->label_pos(name);

	if (pos == -1)
	{
//		MIDIFile::message("No Label %s",name.c_str());
		return;
	}

	long initial_pos = ifile->tellg();
	ifile->clear();
	ifile->seekg(pos);

	// look for controller names 

	while (true)
	{
		ifile->getline(line,256);
	
		is_error = false;

		if (ifile->eof())
		{
			ifile->clear();
			break;
		}

		if (line[0] == '[')
		{
			break;
		}

		str_line = line;

		if (str_line.substr(0,len_based_on)==based_on)
		{
			based_on_name = str_line.substr(len_based_on);
			continue;
		}

		ch = line[0];
		if ((ch<'0')||(ch>'9'))
		{
			continue;
		}

		int last = str_line.find("=");

		if (last < 0)
		{
			continue;
		}

		int no = 0;
		
		for (i=0; i<last; i++)
		{
			ch = line[i];
			if ((ch<'0')||(ch>'9'))
			{
				MIDIFile::message("error CW Controller, line %s",line);
				is_error = true;
				break;
			}
			no = 10*no+(ch-'0');
		}

		if (is_error) 
		{
			break;
		}

		if (no>128)
		{
			MIDIFile::message("error CW Controller number");
			break;
		}

		if (ctrl_names[no] == CwDefaults::initial_items[no])
		{
			ctrl_names[no] = str_line.substr(last+1);
		}
	}
	if (based_on_name != "")
	{
		add_names(ifile, label_list,based_on_name);
	}

	ifile->seekg(initial_pos);
}

//**************************************************************************
//*
//*		Destructor
//*
//**************************************************************************

CwController::~CwController()
{

}

//**************************************************************************
//*
//*		get_name
//*		delivers the controller-name by a given controller-number
//*
//**************************************************************************

std::string CwController::get_name(int no)
{
	return ctrl_names[no];
}

//**************************************************************************
//**************************************************************************
//*
//* CwRPN_NRPN, to hold all about RPN or NRPN of an instrument definition
//*
//**************************************************************************
//**************************************************************************

//**************************************************************************
//*
//*		Standard Constructor
//*
//**************************************************************************

CwRPN_NRPN::CwRPN_NRPN()
{
	based_on_name = "";
}

//**************************************************************************
//*
//*		Constructor
//*
//**************************************************************************

CwRPN_NRPN::CwRPN_NRPN(std::ifstream *ifile, CwLabelList *label_list, std::string name)
{
	based_on_name = "";
	label = name;
	add_rpn_nrpn(ifile,label_list,name);
}

//**************************************************************************
//*
//*		add_rpn_nrpn
//*		fills the internal list of all RPN/NRPN-names
//*
//**************************************************************************

void CwRPN_NRPN::add_rpn_nrpn(std::ifstream *ifile, CwLabelList *label_list, std::string name)
{
	int i;
	char line[256];
	std::string str_line;
	bool is_error;

	const std::string based_on = "BasedOn=";
	const int len_based_on = based_on.length();
	based_on_name = "";

	long pos = label_list->label_pos(name);

	if (pos == -1)
	{
//		MIDIFile::message("No Label %s",name.c_str());
		return;
	}

	long initial_pos = ifile->tellg();
	ifile->clear();
	ifile->seekg(pos);

	// look for rpn/nrpn names 

	while (true)
	{
		ifile->getline(line,256);
		is_error = false;

		if (ifile->eof())
		{
			ifile->clear();
			break;
		}

		if (line[0] == '[')
		{
			break;
		}

		str_line = line;

		if (str_line.substr(0,len_based_on)==based_on)
		{
			based_on_name = str_line.substr(len_based_on);
			continue;
		}

		if ((line[0]<'0')||(line[0]>'9'))
		{
			continue;
		}

		int last = str_line.find("=");

		if (last < 0)
		{
			continue;
		}

		long no = 0;
		
		for (i=0; i<last; i++)
		{
			if ((line[i]<'0')||(line[i]>'9'))
			{
				MIDIFile::message("error RPN/NRPN, line %s",line);
				is_error = true;
				break;
			}
			no = 10*no+(line[i]-'0');
		}

		if (is_error) 
		{
			break;
		}

		if (no<0)
		{
			MIDIFile::message("error RPN/NRPN number");
			break;
		}

		list_elem.no = no;
		list_elem.name = str_line.substr(last+1);
		rpn_nrpn_list.insert(rpn_nrpn_list.end(),list_elem);
	}

	if (based_on_name != "")
	{
		add_rpn_nrpn(ifile,label_list,based_on_name);
	}

	ifile->seekg(initial_pos);
}

//**************************************************************************
//*
//*		Destructor
//*
//**************************************************************************

CwRPN_NRPN::~CwRPN_NRPN()
{
	rpn_nrpn_list.clear();
}

//**************************************************************************
//*
//*		get_name
//*		delivers the name of a RPN/NRPN by a given RPN/NRPN-number
//*
//**************************************************************************

std::string CwRPN_NRPN::get_name(long no)
{

	for (list_i=rpn_nrpn_list.begin(); list_i != rpn_nrpn_list.end(); list_i++)
	{
		if (list_i->no == no)
		{
			return list_i->name;
		}
	}
	return "";
}

//**************************************************************************
//*
//*		get_rpn_names
//*		only for RPN: delivers a list of all RPN-Names
//*
//**************************************************************************

UINT CwRPN_NRPN::get_rpn_names(HP_CWRPN** rpn_names, long *no_rpn)
{
	long list_len = rpn_nrpn_list.size();
	*rpn_names = new HP_CWRPN[list_len];
	int i = 0;
	std::string short_name;
	for (list_i=rpn_nrpn_list.begin(); list_i != rpn_nrpn_list.end(); list_i++)
	{
		(*rpn_names)[i].rpn_number = list_i->no;
		short_name = (list_i->name).substr(0,63);
		strcpy((*rpn_names)[i].rpn_name,short_name.c_str());
		i++;
	}
	*no_rpn = i;
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get_nrpn_names
//*		only for NRPN: delivers a list of all NRPN-Names
//*
//**************************************************************************

UINT CwRPN_NRPN::get_nrpn_names(HP_CWNRPN** nrpn_names, long *no_nrpn)
{
	long list_len = rpn_nrpn_list.size();
	*nrpn_names = new HP_CWNRPN[list_len];
	int i = 0;
	std::string short_name;
	for (list_i=rpn_nrpn_list.begin(); list_i != rpn_nrpn_list.end(); list_i++)
	{
		(*nrpn_names)[i].nrpn_number = list_i->no;
		short_name = (list_i->name).substr(0,63);
		strcpy((*nrpn_names)[i].nrpn_name,short_name.c_str());
		i++;
	}
	*no_nrpn = i;
	return HP_ERR_NONE;
}

//**************************************************************************
//**************************************************************************
//*
//* CwInsdef:	To hold all informations about an instrument definition
//*				Basic class of this file
//*
//**************************************************************************
//**************************************************************************

//**************************************************************************
//*
//*		Standard Constructor
//*
//**************************************************************************

InsDef::InsDef()
{
	cw_ctrl = NULL;
	cw_rpn = NULL;
	cw_nrpn = NULL;
	drum_array = NULL;
	bank_list.clear();
	no_banks = 0;
}

//**************************************************************************
//*
//*		Constructor
//*
//**************************************************************************

InsDef::InsDef(std::ifstream *ifile, CwLabelList *label_list, long def_pos)
{
	cw_ctrl = NULL;
	cw_rpn = NULL;
	cw_nrpn = NULL;
	drum_array = new CWDrum_Array;
	bank_list.clear();
	no_banks = 0;
	std::string str_line;
	char line[256];
	
	const std::string control = "Control=";
	const int ctr_len = control.length();
	const std::string rpn = "RPN=";
	const int rpn_len = rpn.length();
	const std::string nrpn = "NRPN=";
	const int nrpn_len = nrpn.length();
	const std::string patch = "Patch[";
	const int patch_len = patch.length();
	const std::string key = "Key[";
	const int key_len = key.length();
	const std::string drum = "Drum[";
	const int drum_len = drum.length();
	
	ifile->clear();
	ifile->seekg(def_pos);
	ifile->getline(line,256); // skip insdef label

	while (true)
	{
		ifile->getline(line,256);

		if (ifile->eof())
		{
			ifile->clear();
			break;
		}
		if (line[0] == '[')
		{
			break;
		}
		str_line = line;

		// controller ===============================================
		if (str_line.compare(0,ctr_len,control)==0)
		{
			std::string control_name = str_line.substr(ctr_len);
			if (cw_ctrl == NULL)
			{
				cw_ctrl = new CwController(ifile,label_list,control_name);
			}
			continue;
		}

		// RPN ======================================================
		if (str_line.compare(0,rpn_len,rpn)==0)
		{
			std::string rpn_name = str_line.substr(rpn_len);
			if (cw_rpn==NULL)
			{
				cw_rpn = new CwRPN_NRPN(ifile,label_list,rpn_name);
			}
			continue;
		}

		// NRPN ======================================================
		if (str_line.compare(0,nrpn_len,nrpn)==0)
		{
			std::string nrpn_name = str_line.substr(nrpn_len);
			if (cw_nrpn==NULL)
			{
				cw_nrpn = new CwRPN_NRPN(ifile,label_list,nrpn_name);
			}
			continue;
		}

		// banks ====================================================
		if (str_line.compare(0,patch_len,patch)==0)
		{
			insdef_bank.bank_no = 0;
			int pos = patch_len;

			char ch = str_line.at(pos);
			if (ch == '*')
			{
				// ???
				continue;
			}

			while ((ch >= '0')&&(ch <= '9'))
			{
				insdef_bank.bank_no = 10*insdef_bank.bank_no + (ch-'0');
				pos++;
				ch = str_line.at(pos);
			}
			insdef_bank.bank_name = str_line.substr(pos+2);
			insdef_bank.bank = new CwBank(ifile,label_list,insdef_bank.bank_name);
			bank_list.insert(bank_list.end(),insdef_bank);
			no_banks++;
			continue;
		}

		// key ======================================================
		if (str_line.compare(0,key_len,key)==0)
		{
			long bank_no = 0;
			int voice_no = 0;
			std::string voice_name;
			int pos = key_len;

			char ch = str_line.at(pos);
			if (ch == '*')
			{
				MIDIFile::message("Key[*... in %s",line);
				continue;
			}
			while ((ch >= '0')&&(ch <= '9'))
			{
				bank_no = 10*bank_no + (ch-'0');
				pos++;
				ch = str_line.at(pos);
			}
			if (ch != ',')
			{
				MIDIFile::message("Missing , in %s",line);
				continue;
			}
			pos++;
			ch = str_line.at(pos);

			if (ch == '*')
			{
				MIDIFile::message("Key[?,* in %s",line);
				continue;
			}

			while ((ch >= '0')&&(ch <= '9'))
			{
				voice_no = 10*voice_no + (ch-'0');
				pos++;
				ch = str_line.at(pos);
			}
			if (ch != ']')
			{
				MIDIFile::message("Missing ] in %s",line);
				continue;
			}
			voice_name = str_line.substr(pos+2);
//			MIDIFile::message("%i %i %s",bank_no, voice_no, voice_name.c_str());

			// activate notes for bank bank_no and voice voice_no
			// find bank
			
			bool bank_found = false;
			for (list_i=bank_list.begin(); list_i != bank_list.end(); list_i++)
			{
				if (list_i->bank_no == bank_no)
				{
//					MIDIFile::message("Bank found %i:%s",list_i->bank_no,(list_i->bank_name).c_str());
					bank_found = true;
					break;
				}
			}
			
			if (!bank_found)
			{
				MIDIFile::message("bank %i not found",bank_no);
				continue;
			}
			(list_i->bank)->add_notes(ifile,label_list,voice_no,voice_name);
			continue;
		}
	
		// drum ======================================================
		if (str_line.compare(0,drum_len,drum)==0)
		{
			long bank_no = 0;
			int voice_no = 0;
			int is_drum;

			int pos = drum_len;

			char ch = str_line.at(pos);

			while ((ch=='*')||((ch >= '0')&&(ch <= '9')))
			{
				if (ch=='*')
				{
					bank_no = -1;
				}
				else
				{
					bank_no = 10*bank_no + (ch-'0');
				}
				pos++;
				ch = str_line.at(pos);
			}

			if (ch != ',')
			{
				MIDIFile::message("Missing , in %s",line);
				continue;
			}
			pos++;

			ch = str_line.at(pos);
			if (ch == '*')
			{
				voice_no = -1;
				pos++;
				ch = str_line.at(pos);
			}
			else
			{
				while ((ch >= '0')&&(ch <= '9'))
				{
					voice_no = 10*voice_no + (ch-'0');
					pos++;
					ch = str_line.at(pos);
				}
			}

			if (ch != ']')
			{
				MIDIFile::message("Missing ] in %s",line);
				continue;
			}
			pos++;
			ch = str_line.at(pos);

			if (ch != '=')
			{
				MIDIFile::message("Missing = in %s",line);
				continue;
			}
			pos++;
			ch = str_line.at(pos);

			if ((ch!='0')&&(ch!='1'))
			{
				MIDIFile::message("Missing =0 or =1 in %s",line);
				continue;
			}

			is_drum = (ch=='1');

//			MIDIFile::message("bank_no = %i, voice_no = %i, is_drum %i",bank_no, voice_no, is_drum?1:0);
			
			HP_CWDRUM *elem = new HP_CWDRUM;
			elem->bank_no = bank_no;
			elem->voice_no = voice_no;
			elem->is_drum = is_drum;
			
			drum_array->add(elem);

			continue;
		}
	}
}

//**************************************************************************
//*
//*		Destructor
//*
//**************************************************************************

InsDef::~InsDef()
{
	if (cw_ctrl != NULL)
	{
		delete cw_ctrl;
	}
	if (cw_rpn != NULL)
	{
		delete cw_rpn;
	}
	if (cw_nrpn != NULL)
	{
		delete cw_nrpn;
	}

	if (drum_array != NULL)
	{
		delete drum_array;
	}

	for (list_i=bank_list.begin(); list_i != bank_list.end(); list_i++)
	{
		 delete list_i->bank;
	}

	bank_list.clear();
}

//**************************************************************************
//*
//*		get_banks
//*		delivers a list of all banks
//*
//**************************************************************************

UINT InsDef::get_banks(HP_CWBANK** banks, int* no_b)
{
	*no_b = no_banks;
	*banks = new HP_CWBANK[no_banks];
	int i = 0;
	std::string short_bankname;
	for (list_i=bank_list.begin(); list_i != bank_list.end(); list_i++)
	{
		(*banks)[i].bank_number = list_i->bank_no;
		short_bankname = (list_i->bank_name).substr(0,63);
		strcpy((*banks)[i].bank_name,short_bankname.c_str());
		i++;
	}
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get_note_names
//*		delivers a list of all notenames of a given voice
//*
//**************************************************************************

UINT InsDef::get_note_names(long bank, int voice, char** bank_name, char** voice_name, HP_CWNOTE** note_names)
{
	bool bank_found = false;
	std::string str_bank_name;
	for (list_i=bank_list.begin(); list_i != bank_list.end(); list_i++)
	{
		if (list_i->bank_no == bank)
		{
//			MIDIFile::message("%i:%s",list_i->bank_no,(list_i->bank_name).c_str());
			str_bank_name = list_i->bank_name;
			bank_found = true;
			break;
		}
	}

	if (!bank_found)
	{
		MIDIFile::message("Bank %i not found",bank);
		return HP_ERR_CWINS_NO_NAME;
	}

	*bank_name = new char[str_bank_name.length()+1];
	strcpy(*bank_name,str_bank_name.c_str());

	CwBank *bank_instance = list_i->bank;
	return bank_instance->get_note_names(voice,voice_name,note_names);
}

//**************************************************************************
//*
//*		get_rpn_names
//*		delivers a list of all RPN names
//*
//**************************************************************************

UINT InsDef::get_rpn_names(HP_CWRPN** rpn_names, long *no_rpn)
{
	if (cw_rpn != NULL)
	{
		return cw_rpn->get_rpn_names(rpn_names, no_rpn);
	}
	else
	{
		return HP_ERR_CWINS_NO_NAME;
	}

	return HP_ERR_NONE;
}

UINT InsDef::get_nrpn_names(HP_CWNRPN** nrpn_names, long *no_nrpn)
{
	if (cw_nrpn != NULL)
	{
		return cw_nrpn->get_nrpn_names(nrpn_names, no_nrpn);
	}
	else
	{
		return HP_ERR_CWINS_NO_NAME;
	}

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get_bank_voices
//*		delivers a list of all voice names of a given bank
//*
//**************************************************************************

UINT InsDef::get_bank_voices(long bank,char **bank_name,HP_CWVOICE ** voices)
{
	for (list_i=bank_list.begin(); list_i != bank_list.end(); list_i++)
	{
		if (list_i->bank_no == bank)
		{
//			MIDIFile::message("%i:%s",list_i->bank_no,(list_i->bank_name).c_str());
			break;
		}
	}

	if (list_i == bank_list.end())
	{
		return HP_ERR_CWINS_NO_NAME;
	}

	return (list_i->bank)->get_voices(bank_name, voices);
}

//**************************************************************************
//*
//*		fill_bankvoice_array
//*		fills the array with all elems with voice-names
//*
//**************************************************************************

void InsDef::fill_bankvoice_array(CWBankVoice_Array *bankvoice_array)
{
	char *bank_name;
	char voice_name[64] = "empty";
	long bank_no;
	int voice_no;

	for (list_i=bank_list.begin(); list_i != bank_list.end(); list_i++)
	{
		bank_no = list_i->bank_no;
		CwBank *bank_instance = list_i->bank;
		HP_CWVOICE *voices = NULL;
		bank_instance->get_voices(&bank_name, &voices);
		for (voice_no=0; voice_no<128; voice_no++)
		{
			strcpy(voice_name,voices[voice_no].voice_name);

			if ((int)strlen(voice_name)<4)
			{
				if ((voice_name[0] >= '0') && (voice_name[0] <= '9'))
				{
					continue;
				}
			}

//			MIDIFile::message("Bank %i: %s , Voice %i: %s", bank_no, bank_name, voice_no, voice_name);

			HP_CWBANKVOICE *cwbankvoice = new HP_CWBANKVOICE;
			strcpy(cwbankvoice->bank_name,bank_name);
			cwbankvoice->bank_number = bank_no;
			strcpy(cwbankvoice->voice_name,voice_name);
			cwbankvoice->voice_number = voice_no;
			bankvoice_array->add(cwbankvoice);
		}

		delete bank_name;
		delete voices;
	}
	bankvoice_array->sort();
/*
	int i;
	for (i=0; i<bankvoice_array->get_length(); i++)
	{
		char voice_name[64];
		strcpy(voice_name,(*bankvoice_array)[i]->voice_name);

		MIDIFile::message("Bank %i: %s , Voice %i: %s", 
			((*bankvoice_array)[i])->bank_number,
			((*bankvoice_array)[i])->bank_name,
			((*bankvoice_array)[i])->voice_number,
			((*bankvoice_array)[i])->voice_name);
	}
*/
}


//**************************************************************************
//*
//*		get_bank_voice_note_name
//*		delivers the names of a bank, the voice and the note
//*
//**************************************************************************

UINT InsDef::get_bank_voice_note_name(long bank, int voice, int note, 
				  char** bank_name, char** voice_name, char** note_name)
{
	bool bank_found = false;
	for (list_i=bank_list.begin(); list_i != bank_list.end(); list_i++)
	{
		if (list_i->bank_no == bank)
		{
//			MIDIFile::message("%i:%s",list_i->bank_no,(list_i->bank_name).c_str());
			bank_found = true;
			break;
		}
	}

	if (!bank_found)
	{
		*bank_name = new char[1];
		(*bank_name)[0] = (char)0;
		*voice_name = new char[1];
		(*voice_name)[0] = (char)0;
		*note_name = new char[1];
		(*note_name)[0] = (char)0;

		MIDIFile::message("Bank %i not found",bank);

		return HP_ERR_CWINS_NO_NAME;
	}

	std::string str_bank_name = list_i->bank_name;
	*bank_name = new char[str_bank_name.length()+1];
	strcpy(*bank_name,str_bank_name.c_str());

	CwBank *bank_instance = list_i->bank;
	return bank_instance->get_voice_note_name(voice,note,voice_name,note_name);
}

//**************************************************************************
//*
//*		is_drum_voice
//*		finds out if a voice is a drumkit of SFX
//*
//**************************************************************************

UINT InsDef::is_drum_voice(long bank, int voice, int *is_drum)
{
	*is_drum = 0;
	int i;

	bool is_bank = false;
	bool is_voice = false;
	int act_drum = 0;



	if (drum_array->get_length() == 0)
	{
		return HP_ERR_CWINS_NO_DRUM_LINES;
	}

	for (i=0; i<drum_array->get_length(); i++)
	{
		long bank_no = ((*drum_array)[i]->bank_no);
		int voice_no = ((*drum_array)[i]->voice_no);
		int local_drum = ((*drum_array)[i]->is_drum);

		is_bank = false;
		is_voice = false;

		if ((bank==bank_no) || (bank_no==-1))
		{
			is_bank = true;
		}
		if ((voice==voice_no) || (voice_no==-1))
		{
			is_voice = true;
		}
		if (is_bank && is_voice)
		{
			act_drum = local_drum;
		}
	}
	*is_drum = act_drum;

	return HP_ERR_NONE;
}




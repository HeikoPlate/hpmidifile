// InsFile.cpp: Implementierung der Klasse InsFile.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InsFile.h"
#include "midifile.h"


//**************************************************************************
//*
//*		Constructor
//*
//**************************************************************************

InsFile::InsFile()
{
	insfile = NULL;
	def_pos = -1;
	insdef = NULL;
	instrument_list.clear();
	no_insdefs = 0;
	label_list = NULL;
	file_name = "";
	bankvoice_array = new CWBankVoice_Array;
	drum_array = new CWDrum_Array;
}

//**************************************************************************
//*
//*		Destructor
//*
//**************************************************************************

InsFile::~InsFile()
{
	if (ifile.is_open()) ifile.close();
	if (insdef != NULL)
	{
		delete insdef;
		insdef = NULL;
	}
	instrument_list.clear();
	delete bankvoice_array;
	delete drum_array;
}

//**************************************************************************
//*
//*		is_insfile_open
//*		true, if insfile has been loaded
//*
//**************************************************************************

bool InsFile::is_insfile_open()
{
	return ifile.is_open();
}

//**************************************************************************
//*
//*		is_insdef_open
//*		true, if instrument definition has been selected
//*
//**************************************************************************

bool InsFile::is_insdef_open()
{
	return (def_pos > 0);
}

//**************************************************************************
//*
//*		load
//*		opens instrument definition file
//*
//**************************************************************************

UINT InsFile::load(const char* name)
{
	std::string instrument_definitions = ".Instrument Definitions";
	std::string str_line;
	std::string instrument_name;
	int strlen_i_def = instrument_definitions.length(); 

	char line[256];
	long instrument_line_pos;
	bool instrument_line_found = false;
	long instrument_name_pos;

	instrument_list.clear();
	no_insdefs = 0;
	delete bankvoice_array;
	bankvoice_array = new CWBankVoice_Array;
	if (ifile.is_open())
	{
		ifile.close();
		def_pos = -1;
		if (insdef != NULL)
		{
			delete insdef;
			insdef = NULL;
		}
	}

	ifile.open(name);	
	if (ifile.fail())
	{
		MIDIFile::message("Instrumentdefinition-File %s not found",file_name);
		return HP_ERR_CWINSFILE_NOTFOUND;
	}

	file_name = name;

	while (true)
	{
		instrument_line_pos = ifile.tellg();
		ifile.getline(line,256);
		if (ifile.eof()) 
		{
			ifile.clear();
			break;
		}
		str_line = line;
		
		if (str_line.compare(0,strlen_i_def,instrument_definitions)==0)
		{
			instrument_line_found = true;
			break;
		}
	}

	if (!instrument_line_found)
	{
		ifile.close();
		MIDIFile::message("Instrumentdefinition %s not correct (1)",file_name);
		return 	HP_ERR_CWINSFILE;
	}
	
	while (true)
	{
		instrument_name_pos = ifile.tellg();
		ifile.getline(line,256);
		if (ifile.eof()) 
		{
			ifile.clear();
			break;
		}
		str_line = line;
		if (line[0]=='[')
		{
			int last = str_line.find("]");
			instrument_name = str_line.substr(1,last-1);

			insfile_instrument.instrument_name = instrument_name;
			insfile_instrument.pos = instrument_name_pos;
			instrument_list.insert(instrument_list.end(),insfile_instrument);
		}	
	}
	no_insdefs = instrument_list.size();

	// fill label list
	if (label_list != NULL)
	{
		delete label_list;
	}
	label_list = new CwLabelList(&ifile);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get_defs
//*		delivers list of all intrument definitions of the file
//*
//**************************************************************************

UINT InsFile::get_defs(HP_CWDEF **def_list, int* no_defs)
{
	*def_list = new HP_CWDEF[no_insdefs];
	int i=0;

	for (list_i=instrument_list.begin(); list_i != instrument_list.end(); list_i++)
	{
		strcpy((*def_list)[i].def_name, (list_i->instrument_name).c_str());
		i++;
	}

	*no_defs = no_insdefs;
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		set_def
//*		With given index of the def-list selects the associated instrument
//*		definition
//*
//**************************************************************************

UINT InsFile::set_def(int def_index)
{
	if ((def_index < 0) || (def_index >= no_insdefs))
	{
		MIDIFile::message("set_def: bad def_index");
		return HP_ERR_CWINS_DEF_INDEX;
	}

	def_pos = -1;
	int i = 0;

	for (list_i=instrument_list.begin(); list_i != instrument_list.end(); list_i++)
	{
		if (i==def_index)
		{
			def_pos = list_i->pos;
			break;
		}
		else
		{
			i++;
		}
	}

	if (def_pos<0)
	{
		MIDIFile::message("set_def: no instrument");
		return HP_ERR_CWINS_DEF_INDEX;
	}

	// read insdef and fill all class-instances

	if (insdef != NULL)
	{
		delete insdef;
	}

	insdef = new InsDef(&ifile,label_list,def_pos);
	
	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get_controller_name
//*		With given controller-number deliver the name of the Controller
//*
//**************************************************************************

UINT InsFile::get_controller_name(int controller, char** controller_name)
{
	if ((controller< 0) || (controller>127))
	{
		return HP_ERR_INVALID_PARAM;
	}

	char name[128];

	if (insdef->cw_ctrl != NULL)
	{
		strcpy(name,((insdef->cw_ctrl)->get_name(controller)).c_str());
	}
	else
	{
		sprintf(name,"%i",controller);
	}

	*controller_name = new char[strlen(name)+1];
	strcpy(*controller_name,name);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get_controller_names
//*		Delivers a list with all controller-names
//*
//**************************************************************************

UINT InsFile::get_controller_names(HP_CWCONTROLLER** controller_names)
{
	if (insdef->cw_ctrl != NULL)
	{
		int i;
		std::string short_name;
		*controller_names = new HP_CWCONTROLLER[128];
		for (i=0; i<128; i++)
		{
			short_name = ((insdef->cw_ctrl)->get_name(i)).substr(0,63);
			strcpy((*controller_names)[i].controller_name,short_name.c_str());
		}
		return HP_ERR_NONE;
	}
	else
	{
		return HP_ERR_CWINS_NO_NAME;
	}
}

//**************************************************************************
//*
//*		get_rpn_name
//*		With given RPN-number deliver the name of the RPN
//*
//**************************************************************************

UINT InsFile::get_rpn_name(long rpn, char** rpn_name)
{
	if (rpn< 0)
	{
		return HP_ERR_INVALID_PARAM;
	}

	char name[128];

	if (insdef->cw_rpn != NULL)
	{
		strcpy(name,((insdef->cw_rpn)->get_name(rpn)).c_str());
	}
	else
	{
		sprintf(name,"%i",rpn);
	}

	*rpn_name = new char[strlen(name)+1];
	strcpy(*rpn_name,name);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get_rpn_names
//*		Delivers a list with all RPN-names
//*
//**************************************************************************

UINT InsFile::get_rpn_names(HP_CWRPN** rpn_names, long *no_rpn)
{
	return 	(insdef->get_rpn_names)(rpn_names, no_rpn);
}

//**************************************************************************
//*
//*		get_nrpn_name
//*		With given NRPN-number deliver the name of the NRPN
//*
//**************************************************************************

UINT InsFile::get_nrpn_name(long nrpn, char** nrpn_name)
{
	if (nrpn< 0)
	{
		return HP_ERR_INVALID_PARAM;
	}

	char name[128];

	if (insdef->cw_nrpn != NULL)
	{
		strcpy(name,((insdef->cw_nrpn)->get_name(nrpn)).c_str());
	}
	else
	{
		sprintf(name,"%i",nrpn);
	}

	*nrpn_name = new char[strlen(name)+1];
	strcpy(*nrpn_name,name);

	return HP_ERR_NONE;
}

//**************************************************************************
//*
//*		get_nrpn_names
//*		Delivers a list with all NRPN-names
//*
//**************************************************************************

UINT InsFile::get_nrpn_names(HP_CWNRPN** nrpn_names, long *no_nrpn)
{
	return 	(insdef->get_nrpn_names)(nrpn_names, no_nrpn);
}

//**************************************************************************
//*
//*		get_bank_voice_note_name
//*		With given bank-, voice- and note-number the names of
//*		the bank, patch and note are stored in 
//*		bank_name, voice_name and note_name
//*
//**************************************************************************

UINT InsFile::get_bank_voice_note_name(long bank, int voice, int note, char** bank_name, char** voice_name, char** note_name)
{
	if (bank == HP_GM_VOICE_BANK) // General Midi Voice Bank
	{
		int len_bank_name = strlen("General Midi");
		*bank_name = new char[len_bank_name+1];
		strcpy(*bank_name,"General Midi");

		if ((voice<0)||(voice>127))
		{
			*voice_name = new char[1];
			(*voice_name)[0] = (char)0;
		}
		else
		{
			int len_voice_name = (CwDefaults::GMPatch[voice]).length();
			*voice_name = new char[len_voice_name+1];
			strcpy(*voice_name,(CwDefaults::GMPatch[voice]).c_str());
		}

		std::string str_note_name = 
			((note<0)||(note>127))?"":CwDefaults::initial_notes[note];
		int len_note_name = str_note_name.length();
		*note_name = new char[len_note_name+1];
		strcpy(*note_name, str_note_name.c_str());
		return HP_ERR_NONE;
	}


	if (bank == HP_GM_PERC_BANK) // General Midi Percussion Bank
	{
		int len_bank_name = strlen("GM Percussion");
		*bank_name = new char[len_bank_name+1];
		strcpy(*bank_name,"GM Percussion");

		*voice_name = new char[1];
		(*voice_name)[0] = (char)0;

		std::string str_note_name = 
			((note<0)||(note>127))?"":CwDefaults::GMPercussion[note];
		int len_note_name = str_note_name.length();

		*note_name = new char[len_note_name+1];
		strcpy(*note_name, str_note_name.c_str());
		return HP_ERR_NONE;
	}

	return insdef->get_bank_voice_note_name(bank,voice,note,
								bank_name,voice_name,note_name);
}

//**************************************************************************
//*
//*		get_banks
//*		Get all bank-names
//*
//**************************************************************************

UINT InsFile::get_banks(HP_CWBANK** banks, int* no_banks)
{
	return insdef->get_banks(banks,no_banks);
}

//**************************************************************************
//*
//*		is_drum_voice
//*		analyzes Drum[...] line
//*
//**************************************************************************

UINT InsFile::is_drum_voice(long bank, int voice, int *is_drum)
{
	return insdef->is_drum_voice(bank,voice,is_drum);
}

//**************************************************************************
//*
//*		get_bank_voices
//*		Get bank-name and all voice-names of a given bank
//*		Default-Banks are used if necessary.
//*
//**************************************************************************

UINT InsFile::get_bank_voices(long bank,char **bank_name, HP_CWVOICE ** voices)
{
	int i;
	char* gm = "General Midi";
	char * gm_perc = "General Midi Percussion";

	if (bank == HP_GM_VOICE_BANK) // General Midi Voice Bank
	{
		*bank_name = new char[strlen(gm)+1];
		*voices = new HP_CWVOICE[128];

		strcpy(*bank_name,gm);

		for (i=0; i<128; i++)
		{
			strcpy((*voices)[i].voice_name,(CwDefaults::GMPatch[i]).c_str());
		}
		return HP_ERR_NONE;
	}
		
	if (bank == HP_GM_PERC_BANK) // General Midi Percussion Bank
	{
		*bank_name = new char[strlen(gm)+1];
		*voices = new HP_CWVOICE[128];

		strcpy(*bank_name,gm);

		for (i=0; i<128; i++)
		{
			strcpy((*voices)[i].voice_name,gm_perc); 
		}
		return HP_ERR_NONE;
	}

	return insdef->get_bank_voices(bank, bank_name, voices);
}



//**************************************************************************
//*
//*		get_bank_voice_list
//*		Delivers a list of all voices with a given part-string and its
//*     voice-numbers, banks and banknames. With bank_voice_list_len == 0
//*		a new search is started, otherwise the given list will be refined.
//*
//**************************************************************************


UINT InsFile::get_bank_voice_list(char *search_string, HP_CWBANKVOICE **bank_voice_list, long *bank_voice_list_len)
{
	int i;
	int k = 0;
	char *pdest;
	char search_string_lower[64];
	char voice_name_lower[64];

	strcpy(search_string_lower,search_string);
	_strlwr(search_string_lower);

	if (bankvoice_array->get_length()==0)
	{
		insdef->fill_bankvoice_array(bankvoice_array);
	}

	if (*bank_voice_list_len==0)
	{	
		*bank_voice_list = new HP_CWBANKVOICE[bankvoice_array->get_length()];
		for (i=0; i<bankvoice_array->get_length(); i++)
		{
			char *voice_name = ((*bankvoice_array)[i])->voice_name;
			strcpy(voice_name_lower,voice_name);
			_strlwr(voice_name_lower);
			if (search_string[0] != 0)
			{
				pdest = strstr(voice_name_lower,search_string_lower);
				if( pdest == NULL )
				{
					continue;
				}
			}
			strcpy((*bank_voice_list)[k].bank_name,((*bankvoice_array)[i])->bank_name);
			(*bank_voice_list)[k].bank_number = ((*bankvoice_array)[i])->bank_number;
			strcpy((*bank_voice_list)[k].voice_name,((*bankvoice_array)[i])->voice_name);
			(*bank_voice_list)[k].voice_number = ((*bankvoice_array)[i])->voice_number;
			k++;
		}
		*bank_voice_list_len = k;
	}
	else
	{
		for (i=0; i<*bank_voice_list_len; i++)
		{
			char *voice_name = (*bank_voice_list)[i].voice_name;
			strcpy(voice_name_lower,voice_name);
			_strlwr(voice_name_lower);
			pdest = strstr(voice_name_lower,search_string_lower);
			if( pdest == NULL )
			{
				continue;
			}
			strcpy((*bank_voice_list)[k].bank_name,(*bank_voice_list)[i].bank_name);
			(*bank_voice_list)[k].bank_number = (*bank_voice_list)[i].bank_number;
			strcpy((*bank_voice_list)[k].voice_name,(*bank_voice_list)[i].voice_name);
			(*bank_voice_list)[k].voice_number = (*bank_voice_list)[i].voice_number;
			k++;
		}
		*bank_voice_list_len = k;
	}
	return HP_ERR_NONE;
}


//**************************************************************************
//*
//*		get_note_names
//*		Delivers a list of all note-names of a given voice
//*
//**************************************************************************

UINT InsFile::get_note_names(long bank, int voice, char** bank_name, char** voice_name, HP_CWNOTE** note_names)
{
	int i;

	if (bank == HP_GM_VOICE_BANK) // General Midi Voice Bank
	{
		std::string str_bank_name = "General MIDI";
		*bank_name = new char[str_bank_name.length()+1];
		strcpy (*bank_name, str_bank_name.c_str());

		std::string str_voice_name = CwDefaults::GMPatch[voice];
		*voice_name = new char[str_voice_name.length()+1];
		strcpy (*voice_name, str_voice_name.c_str());

		*note_names = new HP_CWNOTE[128];
		for (i=0; i<128; i++)
		{
			strcpy((*note_names)[i].note_name,(CwDefaults::initial_notes[i]).c_str());
		}
		return HP_ERR_NONE;
	}
		
	if (bank == HP_GM_PERC_BANK) // General Midi Percussion Bank
	{
		std::string str_bank_name = "General MIDI";
		*bank_name = new char[str_bank_name.length()+1];
		strcpy (*bank_name, str_bank_name.c_str());

		std::string str_voice_name = "GM Percussion";
		*voice_name = new char[str_voice_name.length()+1];
		strcpy (*voice_name, str_voice_name.c_str());
		*note_names = new HP_CWNOTE[128];
		for (i=0; i<128; i++)
		{
			strcpy((*note_names)[i].note_name,(CwDefaults::GMPercussion[i]).c_str()); 
		}
		return HP_ERR_NONE;
	}

	return insdef->get_note_names(bank, voice, bank_name, voice_name, note_names);
}

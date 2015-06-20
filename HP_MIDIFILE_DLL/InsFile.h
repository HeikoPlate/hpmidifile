// InsFile.h: Schnittstelle für die Klasse InsFile.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSFILE_H__2F1A2A81_1178_11D6_9348_0080AD7896CF__INCLUDED_)
#define AFX_INSFILE_H__2F1A2A81_1178_11D6_9348_0080AD7896CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include "HP_midifile.h"
#include "InsDef.h"
#include "HP_Array.h"
#include <string>
#include <list>
#include <fstream>

class CwInstrument
{
public:
	std::string instrument_name;
	long pos;
};

class InsFile  
{

	typedef std::list<CwInstrument> LISTINSTRUMENT;

public:

	InsFile();
	virtual ~InsFile();
	bool is_insfile_open();
	bool is_insdef_open();

	UINT load(const char* name);
	UINT get_defs(HP_CWDEF** def_list, int* no_defs);
	UINT get_controller_name(int controller, char** controller_name);
	UINT get_controller_names(HP_CWCONTROLLER** controller_names);
	UINT get_rpn_name(long rpn, char** rpn_name);
	UINT get_rpn_names(HP_CWRPN** rpn_names, long *no_rpn);
	UINT get_nrpn_name(long nrpn, char** nrpn_name);
	UINT get_nrpn_names(HP_CWNRPN** nrpn_names, long *no_nrpn);
	UINT get_bank_voice_note_name(long bank, int voice, int note, 
				char** bank_name, char** voice_name, char** note_name);
	UINT get_banks(HP_CWBANK** banks, int* no_banks);
	UINT get_bank_voices(long bank,char **bank_name,HP_CWVOICE** voices);
	UINT get_bank_voice_list(char *search_string, HP_CWBANKVOICE **bank_voice_list, long *bank_voice_list_len);
	UINT get_note_names(long bank, int voice, char** bank_name, char** voice_name, HP_CWNOTE** note_names);
	UINT is_drum_voice(long bank, int voice, int *is_drum);
	UINT set_def(int insdef_index);
	
private:

	int no_insdefs;
	std::string file_name;
	std::string def_name;	

	FILE* insfile;
	CwLabelList *label_list;

	CwInstrument insfile_instrument;
	LISTINSTRUMENT::iterator list_i;
	std::ifstream ifile;
	LISTINSTRUMENT instrument_list;
	long def_pos;
	InsDef *insdef;
	CWBankVoice_Array *bankvoice_array;
	CWDrum_Array * drum_array;
};

#endif // !defined(AFX_INSFILE_H__2F1A2A81_1178_11D6_9348_0080AD7896CF__INCLUDED_)

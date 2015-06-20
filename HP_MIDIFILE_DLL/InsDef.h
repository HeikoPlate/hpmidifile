// InsDef.h: Schnittstelle für die Klasse InsDef.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSDEF_H__3CC4E48D_6ABE_45F5_A627_17B4C6B0D05E__INCLUDED_)
#define AFX_INSDEF_H__3CC4E48D_6ABE_45F5_A627_17B4C6B0D05E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>
#include <fstream>
#include "HP_midifile.h"
#include "HP_Array.h"

class CwDefaults
{
public:

	static const std::string initial_notes[128];
	static const std::string initial_items[128];
	static bool is_initial_note(int index, std::string note_name);
	static bool is_initial_item(int index, std::string item_name);
	static const std::string GMPatch[128];
	static const std::string GMPercussion[128]; 
};

// =======================================================================

class CwLabel
{
public:
	std::string label;
	long pos;
};

class CwLabelList
{
public:
	CwLabelList();
	CwLabelList(std::ifstream *ifile);
	virtual ~CwLabelList();

	long label_pos(std::string name);
private:
	typedef std::list<CwLabel> LISTLABEL;
	CwLabel label_elem;
	LISTLABEL label_list;
	LISTLABEL::iterator list_l;
};


// =======================================================================

class CwNoteElem
{
public:
	std::string note_name;

	static const std::string initial_notes[128];
};

// =======================================================================

class CwVoice
{
public:
	CwVoice();
	CwVoice(std::ifstream *ifile, CwLabelList *label_list, std::string name);
	UINT get_note_name(int note, char** note_name);
	UINT get_note_names(HP_CWNOTE** note_names);

	virtual  ~CwVoice();
	void add_notes(std::ifstream *ifile, CwLabelList *label_list, std::string voice_name);
	std::string label;

private:
	CwNoteElem notes[128];
};

class CwVoiceElem
{
public:
	std::string voice_name;
	CwVoice *voice;
};

// =======================================================================

class CwBank
{
public:
	CwBank();
	CwBank(std::ifstream *ifile, CwLabelList *label_list, std::string name);
	virtual  ~CwBank();
	void add_notes(std::ifstream *ifile, CwLabelList *label_list, int voice_no,std::string voice_name);

	std::string label;
	CwBank* based_on_bank;
	UINT get_voices(char **bank_name,HP_CWVOICE ** voice_array);
	UINT get_voice_note_name(int voice, int note,
		                       char** voice_name, char** note_name);
	UINT get_note_names(int voice, char** voice_name, HP_CWNOTE** note_names);


private:
	CwVoiceElem voices[128];
	std::string voice(int voice_no);
};

class CwBankElem
{
public:
	long bank_no;
	std::string bank_name;
	CwBank *bank;
};

// =======================================================================

class CwController  
{
public:
	CwController();
	CwController(std::ifstream *ifile, CwLabelList *label_list, std::string name);
	virtual ~CwController();
	std::string get_name(int no);
	std::string label;
	std::string ctrl_names[128];
private:
	void add_names(std::ifstream *ifile, CwLabelList *label_list, std::string name);
	std::string based_on_name;
};

// =======================================================================

class CwRPN_NRPN_Elem
{
public:
	long no;
	std::string name;
};

class CwRPN_NRPN  
{

typedef std::list<CwRPN_NRPN_Elem> RPN_NRPN_LIST;

public:
	CwRPN_NRPN();
	CwRPN_NRPN(std::ifstream *ifile, CwLabelList *label_list, std::string name);
	virtual ~CwRPN_NRPN();
	std::string get_name(long no);
	std::string label;
	UINT get_rpn_names(HP_CWRPN** rpn_names, long *no_rpn);
	UINT get_nrpn_names(HP_CWNRPN** nrpn_names, long *no_nrpn);

private:
	void add_rpn_nrpn(std::ifstream *ifile, CwLabelList *label_list, std::string name);
    CwRPN_NRPN_Elem list_elem;
	RPN_NRPN_LIST rpn_nrpn_list;
	RPN_NRPN_LIST::iterator list_i;
	std::string based_on_name;
};

// =======================================================================

class InsDef  
{
	typedef std::list<CwBankElem> BANKS;


public:
	InsDef();
	InsDef(std::ifstream *ifile, CwLabelList *label_list, long def_pos);
	virtual ~InsDef();

	CwController *cw_ctrl;
	CwRPN_NRPN *cw_rpn;
	CwRPN_NRPN *cw_nrpn;
	CWDrum_Array *drum_array;


	UINT get_banks(HP_CWBANK** banks, int* no_b);
	UINT get_bank_voices(long bank,char **bank_name,HP_CWVOICE ** voices);
	UINT get_bank_voice_note_name(long bank, int voice, int note,
				char** bank_name, char** voice_name, char** note_name);
	UINT get_note_names(long bank, int voice, char** bank_name, char** voice_name, HP_CWNOTE** note_names);
	UINT get_rpn_names(HP_CWRPN** rpn_names, long *no_rpn);
	UINT get_nrpn_names(HP_CWNRPN** nrpn_names, long *no_nrpn);
	UINT is_drum_voice(long bank, int voice, int *is_drum);
	void fill_bankvoice_array(CWBankVoice_Array *bankvoice_array);

private:
	CwBankElem insdef_bank;
	BANKS::iterator list_i;
	BANKS bank_list;
	int no_banks;
};

#endif // !defined(AFX_INSDEF_H__3CC4E48D_6ABE_45F5_A627_17B4C6B0D05E__INCLUDED_)

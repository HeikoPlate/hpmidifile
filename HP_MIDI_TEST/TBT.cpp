// TBT.cpp: Implementierung der Klasse TBT.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "TBT.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

TBT::TBT(int takt=0, int beat=0, int tick=0)
{
	tbt_takt = takt;
	tbt_beat = beat;
	tbt_tick = tick;
}




TBT::~TBT()
{

}

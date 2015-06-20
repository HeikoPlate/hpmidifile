// TBT.h: Schnittstelle für die Klasse TBT.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TBT_H__DFEFF77F_F71A_4F92_86FB_251FDE67FA1C__INCLUDED_)
#define AFX_TBT_H__DFEFF77F_F71A_4F92_86FB_251FDE67FA1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TBT  
{
public:
	TBT(int takt, int beat, int tick);
	virtual ~TBT();
private:
	int tbt_takt, tbt_beat, tbt_tick;

};

#endif // !defined(AFX_TBT_H__DFEFF77F_F71A_4F92_86FB_251FDE67FA1C__INCLUDED_)

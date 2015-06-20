# Microsoft Developer Studio Generated NMAKE File, Based on HP_MIDI_TEST.dsp
!IF "$(CFG)" == ""
CFG=HP_MIDI_TEST - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. HP_MIDI_TEST - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "HP_MIDI_TEST - Win32 Release" && "$(CFG)" != "HP_MIDI_TEST - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "HP_MIDI_TEST.mak" CFG="HP_MIDI_TEST - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "HP_MIDI_TEST - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "HP_MIDI_TEST - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HP_MIDI_TEST - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\HP_MIDI_TEST.exe"


CLEAN :
	-@erase "$(INTDIR)\HP_MIDI_TEST.obj"
	-@erase "$(INTDIR)\HP_MIDI_TEST.pch"
	-@erase "$(INTDIR)\HP_MIDI_TEST.res"
	-@erase "$(INTDIR)\HP_MIDI_TESTDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\HP_MIDI_TEST.exe"
	-@erase "$(OUTDIR)\HP_MIDI_TEST.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\HP_MIDI_TEST.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\HP_MIDI_TEST.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HP_MIDI_TEST.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\HP_MIDI_TEST.pdb" /map:"$(INTDIR)\HP_MIDI_TEST.map" /machine:I386 /out:"$(OUTDIR)\HP_MIDI_TEST.exe" 
LINK32_OBJS= \
	"$(INTDIR)\HP_MIDI_TEST.obj" \
	"$(INTDIR)\HP_MIDI_TESTDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\HP_MIDI_TEST.res"

"$(OUTDIR)\HP_MIDI_TEST.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "HP_MIDI_TEST - Win32 Debug"

OUTDIR=.
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.
# End Custom Macros

ALL : "$(OUTDIR)\HP_MIDI_TEST.exe" "$(OUTDIR)\HP_MIDI_TEST.bsc"


CLEAN :
	-@erase "$(INTDIR)\HP_MIDI_TEST.obj"
	-@erase "$(INTDIR)\HP_MIDI_TEST.pch"
	-@erase "$(INTDIR)\HP_MIDI_TEST.res"
	-@erase "$(INTDIR)\HP_MIDI_TEST.sbr"
	-@erase "$(INTDIR)\HP_MIDI_TESTDlg.obj"
	-@erase "$(INTDIR)\HP_MIDI_TESTDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Debug\HP_MIDI_TEST.map"
	-@erase "$(OUTDIR)\HP_MIDI_TEST.bsc"
	-@erase "$(OUTDIR)\HP_MIDI_TEST.exe"
	-@erase "$(OUTDIR)\HP_MIDI_TEST.ilk"
	-@erase "$(OUTDIR)\HP_MIDI_TEST.pdb"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\HP_MIDI_TEST.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\HP_MIDI_TEST.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HP_MIDI_TEST.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\HP_MIDI_TEST.sbr" \
	"$(INTDIR)\HP_MIDI_TESTDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\HP_MIDI_TEST.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=..\HP_MIDIFILE_DLL\Debug\HP_midifile.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\HP_MIDI_TEST.pdb" /map:"$(INTDIR)\HP_MIDI_TEST.map" /debug /machine:I386 /out:"$(OUTDIR)\HP_MIDI_TEST.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\HP_MIDI_TEST.obj" \
	"$(INTDIR)\HP_MIDI_TESTDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\HP_MIDI_TEST.res"

"$(OUTDIR)\HP_MIDI_TEST.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("HP_MIDI_TEST.dep")
!INCLUDE "HP_MIDI_TEST.dep"
!ELSE 
!MESSAGE Warning: cannot find "HP_MIDI_TEST.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "HP_MIDI_TEST - Win32 Release" || "$(CFG)" == "HP_MIDI_TEST - Win32 Debug"
SOURCE=.\HP_MIDI_TEST.cpp

!IF  "$(CFG)" == "HP_MIDI_TEST - Win32 Release"


"$(INTDIR)\HP_MIDI_TEST.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HP_MIDI_TEST.pch"


!ELSEIF  "$(CFG)" == "HP_MIDI_TEST - Win32 Debug"


"$(INTDIR)\HP_MIDI_TEST.obj"	"$(INTDIR)\HP_MIDI_TEST.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HP_MIDI_TEST.pch"


!ENDIF 

SOURCE=.\HP_MIDI_TEST.rc

"$(INTDIR)\HP_MIDI_TEST.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\HP_MIDI_TESTDlg.cpp

!IF  "$(CFG)" == "HP_MIDI_TEST - Win32 Release"


"$(INTDIR)\HP_MIDI_TESTDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HP_MIDI_TEST.pch"


!ELSEIF  "$(CFG)" == "HP_MIDI_TEST - Win32 Debug"


"$(INTDIR)\HP_MIDI_TESTDlg.obj"	"$(INTDIR)\HP_MIDI_TESTDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\HP_MIDI_TEST.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "HP_MIDI_TEST - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\HP_MIDI_TEST.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\HP_MIDI_TEST.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "HP_MIDI_TEST - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\HP_MIDI_TEST.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\HP_MIDI_TEST.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 


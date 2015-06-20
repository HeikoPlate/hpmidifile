; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
LastClass=CCreate_songDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "create_song.h"

ClassCount=2
Class1=CCreate_songApp
Class2=CCreate_songDlg

ResourceCount=4
Resource2=IDR_MAINFRAME
Resource3=IDD_CREATE_SONG_DIALOG
Resource4=IDD_CREATE_SONG_DIALOG (Englisch (USA))

[CLS:CCreate_songApp]
Type=0
HeaderFile=create_song.h
ImplementationFile=create_song.cpp
Filter=N

[CLS:CCreate_songDlg]
Type=0
HeaderFile=create_songDlg.h
ImplementationFile=create_songDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDOK



[DLG:IDD_CREATE_SONG_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CCreate_songDlg

[DLG:IDD_CREATE_SONG_DIALOG (Englisch (USA))]
Type=1
Class=CCreate_songDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308865


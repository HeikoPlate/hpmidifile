; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
LastClass=CMidifile_playerDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "midifile_player.h"

ClassCount=4
Class1=CMidifile_playerApp
Class2=CMidifile_playerDlg
Class3=CAboutDlg

ResourceCount=6
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MIDIFILE_PLAYER_DIALOG
Resource4=IDD_ABOUTBOX (Englisch (USA))
Resource5=IDD_SELECT_MIDI_DEVICE
Class4=SelectMIDIDevice
Resource6=IDD_MIDIFILE_PLAYER_DIALOG (Englisch (USA))

[CLS:CMidifile_playerApp]
Type=0
HeaderFile=midifile_player.h
ImplementationFile=midifile_player.cpp
Filter=N

[CLS:CMidifile_playerDlg]
Type=0
HeaderFile=midifile_playerDlg.h
ImplementationFile=midifile_playerDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_LOAD

[CLS:CAboutDlg]
Type=0
HeaderFile=midifile_playerDlg.h
ImplementationFile=midifile_playerDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_MIDIFILE_PLAYER_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CMidifile_playerDlg

[DLG:IDD_MIDIFILE_PLAYER_DIALOG (Englisch (USA))]
Type=1
Class=CMidifile_playerDlg
ControlCount=19
Control1=IDOK,button,1342242817
Control2=IDC_LOAD,button,1342242816
Control3=IDC_PLAY,button,1342242816
Control4=IDC_INFO,button,1342242816
Control5=IDC_PAUSE,button,1342242816
Control6=IDC_GO_ON,button,1342242816
Control7=IDC_STOP,button,1342242816
Control8=IDC_MIDI_DEVICE,button,1342242816
Control9=IDC_PROGRESS,msctls_progress32,1342177280
Control10=IDC_PLAY_TIME,edit,1342244993
Control11=IDC_SONG_TIME,edit,1342244993
Control12=IDC_LAST_TAKT_TEXT,static,1342308352
Control13=IDC_LAST_TAKT,edit,1342244993
Control14=IDC_TAKT_BEAT_TEXT,static,1342308352
Control15=IDC_TAKT_BEAT,edit,1342244993
Control16=IDC_LYRIC_TEXT,static,1342308353
Control17=IDC_LYRIC,edit,1342244993
Control18=IDC_CHORD_TEXT,static,1342308353
Control19=IDC_CHORD,edit,1342244993

[DLG:IDD_ABOUTBOX (Englisch (USA))]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308481
Control3=IDC_STATIC,static,1342308353
Control4=IDOK,button,1342373889
Control5=IDC_TITLE_VERSION_TEXT,static,1342308353

[DLG:IDD_SELECT_MIDI_DEVICE]
Type=1
Class=SelectMIDIDevice
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_DEVICE_LIST,listbox,1352728833

[CLS:SelectMIDIDevice]
Type=0
HeaderFile=SelectMIDIDevice.h
ImplementationFile=SelectMIDIDevice.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_DEVICE_LIST
VirtualFilter=dWC


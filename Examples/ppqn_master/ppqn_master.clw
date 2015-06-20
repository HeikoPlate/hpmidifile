; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
LastClass=CPpqn_masterDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ppqn_master.h"

ClassCount=5
Class1=CPpqn_masterApp
Class2=CPpqn_masterDlg
Class3=CAboutDlg

ResourceCount=7
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_PPQN_MASTER_DIALOG
Resource4=IDD_SELECT_PPQN
Resource5=IDD_SELECT_MIDIFORMAT
Class4=SelectFormat
Resource6=IDD_ABOUTBOX (Englisch (USA))
Class5=Select_PPQN
Resource7=IDD_PPQN_MASTER_DIALOG (Englisch (USA))

[CLS:CPpqn_masterApp]
Type=0
HeaderFile=ppqn_master.h
ImplementationFile=ppqn_master.cpp
Filter=N

[CLS:CPpqn_masterDlg]
Type=0
HeaderFile=ppqn_masterDlg.h
ImplementationFile=ppqn_masterDlg.cpp
Filter=D
LastObject=IDC_INFO
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=ppqn_masterDlg.h
ImplementationFile=ppqn_masterDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_PPQN_MASTER_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CPpqn_masterDlg

[DLG:IDD_PPQN_MASTER_DIALOG (Englisch (USA))]
Type=1
Class=CPpqn_masterDlg
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDC_TITEL,static,1342308353
Control3=IDC_LOAD,button,1342242816
Control4=IDC_SAVE,button,1342242816
Control5=IDC_PPQN,button,1342242816
Control6=IDC_INFO,button,1342242816

[DLG:IDD_ABOUTBOX (Englisch (USA))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SELECT_MIDIFORMAT]
Type=1
Class=SelectFormat
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SMF0,button,1342177289
Control4=IDC_SMF1,button,1342177289

[CLS:SelectFormat]
Type=0
HeaderFile=SelectFormat.h
ImplementationFile=SelectFormat.cpp
BaseClass=CDialog
Filter=D
LastObject=IDOK
VirtualFilter=dWC

[DLG:IDD_SELECT_PPQN]
Type=1
Class=Select_PPQN
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_PPQN_LIST,listbox,1350631680

[CLS:Select_PPQN]
Type=0
HeaderFile=Select_PPQN.h
ImplementationFile=Select_PPQN.cpp
BaseClass=CDialog
Filter=D
LastObject=IDCANCEL
VirtualFilter=dWC


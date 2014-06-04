# Microsoft Developer Studio Project File - Name="xmiladv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=xmiladv - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "xmiladv.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "xmiladv.mak" CFG="xmiladv - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "xmiladv - Win32 Release" ("Win32 (x86) Console Application" 用)
!MESSAGE "xmiladv - Win32 Debug" ("Win32 (x86) Console Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xmiladv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\adv"
# PROP Intermediate_Dir "..\obj\vc\advrel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\Win32s" /I ".\patch" /I ".\vram" /I ".\fdd" /I "..\\" /I "..\common" /I "..\z80c" /I "..\io" /I "..\sound" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "TRACE" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib ddraw.lib dxguid.lib dsound.lib /nologo /subsystem:console /map /machine:I386

!ELSEIF  "$(CFG)" == "xmiladv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\adv"
# PROP Intermediate_Dir "..\obj\vc\advdbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\\" /I ".\Win32s" /I ".\patch" /I ".\vram" /I ".\fdd" /I "..\\" /I "..\common" /I "..\z80c" /I "..\io" /I "..\sound" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib ddraw.lib dxguid.lib dsound.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\bin\adv\xmiladvd.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xmiladv - Win32 Release"
# Name "xmiladv - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\COMMON\MILSTR.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\PARTS.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\STRRES.C
# End Source File
# End Group
# Begin Group "cpu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Z80C\Z80C.C
# End Source File
# Begin Source File

SOURCE=..\Z80C\Z80C_CB.C
# End Source File
# Begin Source File

SOURCE=..\Z80C\Z80C_IX.C
# End Source File
# Begin Source File

SOURCE=..\Z80C\Z80C_IY.C
# End Source File
# Begin Source File

SOURCE=..\Z80C\Z80C_MN.C
# End Source File
# Begin Source File

SOURCE=..\Z80C\Z80C_SB.C
# End Source File
# Begin Source File

SOURCE=..\Z80C\Z80DMAP.C
# End Source File
# Begin Source File

SOURCE=..\Z80C\Z80MEM.C
# End Source File
# End Group
# Begin Group "io"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\IO\CGROM.C
# End Source File
# Begin Source File

SOURCE=..\IO\CMT.C
# End Source File
# Begin Source File

SOURCE=..\IO\CRTC.C
# End Source File
# Begin Source File

SOURCE=..\IO\CTC.C
# End Source File
# Begin Source File

SOURCE=..\IO\DIPSW.C
# End Source File
# Begin Source File

SOURCE=..\IO\DMAC.C
# End Source File
# Begin Source File

SOURCE=..\IO\FDC.C
# End Source File
# Begin Source File

SOURCE=.\PATCH\IOCORE.C
# End Source File
# Begin Source File

SOURCE=.\patch\MEMIO.C
# End Source File
# Begin Source File

SOURCE=..\IO\PCG.C
# End Source File
# Begin Source File

SOURCE=..\IO\PPI.C
# End Source File
# Begin Source File

SOURCE=..\IO\SIO.C
# End Source File
# Begin Source File

SOURCE=.\PATCH\SNDBOARD.C
# End Source File
# Begin Source File

SOURCE=..\IO\SUBCPU.C
# End Source File
# Begin Source File

SOURCE=.\patch\VRAMIO.C
# End Source File
# End Group
# Begin Group "vram"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VRAM\MAKE15.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\MAKEATR.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\MAKEC16.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\MAKEC8.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\MAKECS.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\MAKEMIX.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\MAKESCRN.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\MAKESUB.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\MAKETXTL.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\PALETTES.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\VRAM.C
# End Source File
# Begin Source File

SOURCE=.\VRAM\VSYNCFF.C
# End Source File
# End Group
# Begin Group "fdd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FDD\FDD_2D.C
# End Source File
# Begin Source File

SOURCE=.\FDD\FDD_D88.C
# End Source File
# Begin Source File

SOURCE=.\FDD\FDDFILE.C
# End Source File
# End Group
# Begin Group "win32s"

# PROP Default_Filter ""
# Begin Group "advsys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Win32s\ADVKEY.CPP
# End Source File
# Begin Source File

SOURCE=.\Win32s\ADVSYS.CPP
# End Source File
# Begin Source File

SOURCE=.\Win32s\ADVUSER.CPP
# End Source File
# Begin Source File

SOURCE=.\Win32s\ADVVIDEO.CPP
# End Source File
# Begin Source File

SOURCE=.\Win32s\DOSIO.CPP
# End Source File
# Begin Source File

SOURCE=.\Win32s\WIN9XSUB.CPP
# End Source File
# End Group
# Begin Source File

SOURCE=.\ADVPSG.C
# End Source File
# Begin Source File

SOURCE=.\PATCH\FONT.C
# End Source File
# Begin Source File

SOURCE=.\JOYMNG.C
# End Source File
# Begin Source File

SOURCE=.\MOUSEMNG.C
# End Source File
# Begin Source File

SOURCE=.\SYSMNG.C
# End Source File
# Begin Source File

SOURCE=.\TIMEMNG.C
# End Source File
# Begin Source File

SOURCE=.\TRACE.C
# End Source File
# Begin Source File

SOURCE=.\XMIL.C
# End Source File
# End Group
# Begin Source File

SOURCE=..\CALENDAR.C
# End Source File
# Begin Source File

SOURCE=..\IEVENT.C
# End Source File
# Begin Source File

SOURCE=..\KEYSTAT.C
# End Source File
# Begin Source File

SOURCE=..\NEVENT.C
# End Source File
# Begin Source File

SOURCE=..\PCCORE.C
# End Source File
# Begin Source File

SOURCE=..\TIMING.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

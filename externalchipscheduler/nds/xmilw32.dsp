# Microsoft Developer Studio Project File - Name="xmilw32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xmilw32 - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "xmilw32.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "xmilw32.mak" CFG="xmilw32 - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "xmilw32 - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "xmilw32 - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xmilw32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj\vc\xmilnds"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\patch" /I ".\win32s" /I "..\\" /I "..\common" /I "..\z80ac" /I "..\io" /I ".\vram" /I "..\sound" /I ".\fdd" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "TRACE" /D MAKESCRN_VRAMFF=1 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib ddraw.lib dxguid.lib dsound.lib /nologo /subsystem:console /map /machine:I386 /out:"..\bin/xmilnds/xmilw32.exe"

!ELSEIF  "$(CFG)" == "xmilw32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj\vc\xmilndsd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\\" /I ".\patch" /I ".\win32s" /I "..\\" /I "..\common" /I "..\z80ac" /I "..\io" /I ".\vram" /I "..\sound" /I ".\fdd" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D MAKESCRN_VRAMFF=1 /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib winmm.lib ddraw.lib dxguid.lib dsound.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\bin/xmilnds/xmilw32d.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xmilw32 - Win32 Release"
# Name "xmilw32 - Win32 Debug"
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

SOURCE=..\z80ac\z80c.c
# End Source File
# Begin Source File

SOURCE=..\z80ac\z80c_cb.c
# End Source File
# Begin Source File

SOURCE=..\z80ac\z80c_ix.c
# End Source File
# Begin Source File

SOURCE=..\z80ac\z80c_iy.c
# End Source File
# Begin Source File

SOURCE=..\z80ac\z80c_mn.c
# End Source File
# Begin Source File

SOURCE=..\z80ac\z80c_sb.c
# End Source File
# Begin Source File

SOURCE=..\z80ac\z80dmap.c
# End Source File
# Begin Source File

SOURCE=..\z80ac\z80mem.c
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

SOURCE=.\patch\CTC.C
# End Source File
# Begin Source File

SOURCE=..\IO\DIPSW.C
# End Source File
# Begin Source File

SOURCE=..\IO\DMAC.C
# End Source File
# Begin Source File

SOURCE=.\patch\FDC.C
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

SOURCE=.\patch\PPI.C
# End Source File
# Begin Source File

SOURCE=..\IO\SIO.C
# End Source File
# Begin Source File

SOURCE=.\patch\SNDBOARD.C
# End Source File
# Begin Source File

SOURCE=.\patch\SUBCPU.C
# End Source File
# Begin Source File

SOURCE=.\patch\VRAMIO.C
# End Source File
# End Group
# Begin Group "vram"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\vram\make15.cpp
# End Source File
# Begin Source File

SOURCE=.\vram\makeatr.cpp
# End Source File
# Begin Source File

SOURCE=.\vram\makec16.cpp
# End Source File
# Begin Source File

SOURCE=.\vram\makec8.cpp
# End Source File
# Begin Source File

SOURCE=.\vram\makecs.cpp
# End Source File
# Begin Source File

SOURCE=.\vram\makemix.cpp
# End Source File
# Begin Source File

SOURCE=.\vram\makescrn.cpp
# End Source File
# Begin Source File

SOURCE=.\vram\makesub.cpp
# End Source File
# Begin Source File

SOURCE=.\vram\maketxtl.cpp
# End Source File
# Begin Source File

SOURCE=.\vram\palettes.cpp
# End Source File
# Begin Source File

SOURCE=.\vram\vsyncff.cpp
# End Source File
# End Group
# Begin Group "fdd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fdd\FDD_2D.C
# End Source File
# Begin Source File

SOURCE=.\fdd\FDD_D88.C
# End Source File
# Begin Source File

SOURCE=.\fdd\FDDFILE.C
# End Source File
# End Group
# Begin Group "nds"

# PROP Default_Filter ""
# Begin Group "sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sound\nds7psg.cpp
# End Source File
# Begin Source File

SOURCE=.\sound\nds9psg.cpp
# End Source File
# End Group
# Begin Group "win32s"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32s\dosio.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\extrom.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndsconsole.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndscore.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndsdma.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndsinput.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndsipc.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndsirq.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndsmem.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndsreg.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndssound.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndsswi.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndssys.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndstouch.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\ndsvideo.cpp
# End Source File
# Begin Source File

SOURCE=.\win32s\trace.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\bss.x86

!IF  "$(CFG)" == "xmilw32 - Win32 Release"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\xmilnds
InputPath=.\bss.x86
InputName=bss

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -o $(IntDir)\$(InputName).obj -i.\x86\ -i..\i286x\ -i..\io\x86\ $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "xmilw32 - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\xmilndsd
InputPath=.\bss.x86
InputName=bss

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -o $(IntDir)\$(InputName).obj -i.\x86\ -i..\i286x\ -i..\io\x86\ $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\joymng.cpp
# End Source File
# Begin Source File

SOURCE=.\mousemng.cpp
# End Source File
# Begin Source File

SOURCE=.\softkbd7.cpp
# End Source File
# Begin Source File

SOURCE=.\softkbd9.cpp
# End Source File
# Begin Source File

SOURCE=.\timemng.cpp
# End Source File
# Begin Source File

SOURCE=.\xmil7.cpp
# End Source File
# Begin Source File

SOURCE=.\xmil9.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\CALENDAR.C
# End Source File
# Begin Source File

SOURCE=..\DEBUGSUB.C
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

# Microsoft Developer Studio Project File - Name="xmil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xmil - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "xmil.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "xmil.mak" CFG="xmil - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "xmil - Win32 Release JP" ("Win32 (x86) Application" 用)
!MESSAGE "xmil - Win32 Release World" ("Win32 (x86) Application" 用)
!MESSAGE "xmil - Win32 Trace" ("Win32 (x86) Application" 用)
!MESSAGE "xmil - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xmil - Win32 Release JP"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseJP"
# PROP BASE Intermediate_Dir "ReleaseJP"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj\vc\win9xreljp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\dialog" /I "..\\" /I "..\common" /I "..\z80x" /I "..\io" /I "..\vram" /I "..\sound" /I "..\font" /I "..\fdd" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "xmil - Win32 Release World"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseWorld"
# PROP BASE Intermediate_Dir "ReleaseWorld"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj\vc\win9xrelworld"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\dialog" /I "..\\" /I "..\common" /I "..\z80x" /I "..\io" /I "..\vram" /I "..\sound" /I "..\font" /I "..\fdd" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib /nologo /subsystem:windows /map /machine:I386 /out:"..\bin/xmilw.exe"

!ELSEIF  "$(CFG)" == "xmil - Win32 Trace"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Trace"
# PROP BASE Intermediate_Dir "Trace"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj\vc\win9xtrc"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\\" /I ".\dialog" /I "..\\" /I "..\common" /I "..\z80x" /I "..\io" /I "..\vram" /I "..\sound" /I "..\font" /I "..\fdd" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "TRACE" /D "SUPPORT_TURBOZ" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib /nologo /subsystem:windows /map /machine:I386 /out:"..\bin/xmilt.exe"

!ELSEIF  "$(CFG)" == "xmil - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj\vc\win9xdbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\\" /I ".\dialog" /I "..\\" /I "..\common" /I "..\z80x" /I "..\io" /I "..\vram" /I "..\sound" /I "..\font" /I "..\fdd" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\bin/xmild.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xmil - Win32 Release JP"
# Name "xmil - Win32 Release World"
# Name "xmil - Win32 Trace"
# Name "xmil - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\COMMON\_MEMORY.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\BMPDATA.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\MILSTR.C
# End Source File
# Begin Source File

SOURCE=..\COMMON\PARTS.C
# End Source File
# Begin Source File

SOURCE=..\common\profile.c
# End Source File
# Begin Source File

SOURCE=..\common\profile.h
# End Source File
# Begin Source File

SOURCE=..\COMMON\STRRES.C
# End Source File
# Begin Source File

SOURCE=..\common\textfile.c
# End Source File
# Begin Source File

SOURCE=..\common\textfile.h
# End Source File
# End Group
# Begin Group "cpu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Z80X\Z80DMAP.C
# End Source File
# Begin Source File

SOURCE=..\Z80X\Z80X.C
# End Source File
# Begin Source File

SOURCE=..\Z80X\Z80X_CB.X86

!IF  "$(CFG)" == "xmil - Win32 Release JP"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xreljp
InputPath=..\Z80X\Z80X_CB.X86
InputName=Z80X_CB

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Release World"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xrelworld
InputPath=..\Z80X\Z80X_CB.X86
InputName=Z80X_CB

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Trace"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xtrc
InputPath=..\Z80X\Z80X_CB.X86
InputName=Z80X_CB

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xdbg
InputPath=..\Z80X\Z80X_CB.X86
InputName=Z80X_CB

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Z80X\Z80X_IF.X86

!IF  "$(CFG)" == "xmil - Win32 Release JP"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xreljp
InputPath=..\Z80X\Z80X_IF.X86
InputName=Z80X_IF

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Release World"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xrelworld
InputPath=..\Z80X\Z80X_IF.X86
InputName=Z80X_IF

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Trace"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xtrc
InputPath=..\Z80X\Z80X_IF.X86
InputName=Z80X_IF

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xdbg
InputPath=..\Z80X\Z80X_IF.X86
InputName=Z80X_IF

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Z80X\Z80X_IX.X86

!IF  "$(CFG)" == "xmil - Win32 Release JP"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xreljp
InputPath=..\Z80X\Z80X_IX.X86
InputName=Z80X_IX

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Release World"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xrelworld
InputPath=..\Z80X\Z80X_IX.X86
InputName=Z80X_IX

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Trace"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xtrc
InputPath=..\Z80X\Z80X_IX.X86
InputName=Z80X_IX

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xdbg
InputPath=..\Z80X\Z80X_IX.X86
InputName=Z80X_IX

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Z80X\Z80X_IY.X86

!IF  "$(CFG)" == "xmil - Win32 Release JP"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xreljp
InputPath=..\Z80X\Z80X_IY.X86
InputName=Z80X_IY

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Release World"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xrelworld
InputPath=..\Z80X\Z80X_IY.X86
InputName=Z80X_IY

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Trace"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xtrc
InputPath=..\Z80X\Z80X_IY.X86
InputName=Z80X_IY

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xdbg
InputPath=..\Z80X\Z80X_IY.X86
InputName=Z80X_IY

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Z80X\Z80X_MN.X86

!IF  "$(CFG)" == "xmil - Win32 Release JP"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xreljp
InputPath=..\Z80X\Z80X_MN.X86
InputName=Z80X_MN

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Release World"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xrelworld
InputPath=..\Z80X\Z80X_MN.X86
InputName=Z80X_MN

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Trace"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xtrc
InputPath=..\Z80X\Z80X_MN.X86
InputName=Z80X_MN

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xdbg
InputPath=..\Z80X\Z80X_MN.X86
InputName=Z80X_MN

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Z80X\Z80X_SB.X86

!IF  "$(CFG)" == "xmil - Win32 Release JP"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xreljp
InputPath=..\Z80X\Z80X_SB.X86
InputName=Z80X_SB

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Release World"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xrelworld
InputPath=..\Z80X\Z80X_SB.X86
InputName=Z80X_SB

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Trace"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xtrc
InputPath=..\Z80X\Z80X_SB.X86
InputName=Z80X_SB

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xdbg
InputPath=..\Z80X\Z80X_SB.X86
InputName=Z80X_SB

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Z80X\Z80XMEM.X86

!IF  "$(CFG)" == "xmil - Win32 Release JP"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xreljp
InputPath=..\Z80X\Z80XMEM.X86
InputName=Z80XMEM

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Release World"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xrelworld
InputPath=..\Z80X\Z80XMEM.X86
InputName=Z80XMEM

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Trace"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xtrc
InputPath=..\Z80X\Z80XMEM.X86
InputName=Z80XMEM

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xdbg
InputPath=..\Z80X\Z80XMEM.X86
InputName=Z80XMEM

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

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

SOURCE=..\IO\IOCORE.C
# End Source File
# Begin Source File

SOURCE=..\IO\MEMIO.C
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

SOURCE=..\IO\SNDBOARD.C
# End Source File
# Begin Source File

SOURCE=..\IO\SUBCPU.C
# End Source File
# Begin Source File

SOURCE=..\IO\VRAMIO.C
# End Source File
# End Group
# Begin Group "vram"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\VRAM\MAKE15.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKE24.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKEATR.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKECHR.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKEMIX.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKESCRN.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKESUB.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKETXTH.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\MAKETXTL.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\PALETTES.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\SCRNDRAW.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\SCRNSAVE.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\SDRAW.C
# End Source File
# Begin Source File

SOURCE=..\VRAM\VRAM.C
# End Source File
# End Group
# Begin Group "sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ext\opm.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\opm.h
# End Source File
# Begin Source File

SOURCE=..\sound\opmgen.h
# End Source File
# Begin Source File

SOURCE=..\sound\opmgenc.c
# End Source File
# Begin Source File

SOURCE=.\x86\opmgeng.x86

!IF  "$(CFG)" == "xmil - Win32 Release JP"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xreljp
InputPath=.\x86\opmgeng.x86
InputName=opmgeng

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Release World"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xrelworld
InputPath=.\x86\opmgeng.x86
InputName=opmgeng

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Trace"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xtrc
InputPath=.\x86\opmgeng.x86
InputName=opmgeng

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ELSEIF  "$(CFG)" == "xmil - Win32 Debug"

# Begin Custom Build - ｱｾﾝﾌﾞﾙ中... $(InputPath)
IntDir=.\..\obj\vc\win9xdbg
InputPath=.\x86\opmgeng.x86
InputName=opmgeng

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 $(InputPath) -o $(IntDir)\$(InputName).obj

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ext\psg.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\psg.h
# End Source File
# Begin Source File

SOURCE=..\sound\psggen.h
# End Source File
# Begin Source File

SOURCE=..\sound\psggenc.c
# End Source File
# Begin Source File

SOURCE=..\sound\psggeng.c
# End Source File
# Begin Source File

SOURCE=..\SOUND\SNDCTRL.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\SOUND.C
# End Source File
# Begin Source File

SOURCE=..\SOUND\X1F.C
# End Source File
# End Group
# Begin Group "fdd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\FDD\DISKDRV.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_2D.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_D88.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_INI.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDD_MTR.C
# End Source File
# Begin Source File

SOURCE=..\FDD\FDDFILE.C
# End Source File
# Begin Source File

SOURCE=..\FDD\NEWDISK.C
# End Source File
# End Group
# Begin Group "font"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\FONT\FONT.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTDATA.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTMAKE.C
# End Source File
# Begin Source File

SOURCE=..\FONT\FONTX1.C
# End Source File
# End Group
# Begin Group "win9x"

# PROP Default_Filter ""
# Begin Group "dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DIALOG\D_ABOUT.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_BMP.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_CFG.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_DISK.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\D_SOUND.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\DIALOGS.CPP
# End Source File
# Begin Source File

SOURCE=.\DIALOG\EXTCLASS.CPP
# End Source File
# End Group
# Begin Group "ext"

# PROP Default_Filter ""
# Begin Group "romeo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ext\romeo\juliet.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\romeo\juliet.h
# End Source File
# Begin Source File

SOURCE=.\ext\romeo\romeo.h
# End Source File
# End Group
# Begin Group "scci"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ext\scci\scci.h
# End Source File
# Begin Source File

SOURCE=.\ext\scci\SCCIDefines.h
# End Source File
# Begin Source File

SOURCE=.\ext\scci\scciif.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\scci\scciif.h
# End Source File
# End Group
# Begin Group "c86ctl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ext\c86ctl\c86ctl.h
# End Source File
# Begin Source File

SOURCE=.\ext\c86ctl\c86ctlif.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\c86ctl\c86ctlif.h
# End Source File
# Begin Source File

SOURCE=.\ext\c86ctl\cbus_boardtype.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ext\externalchip.h
# End Source File
# Begin Source File

SOURCE=.\ext\externalchipmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\externalchipmanager.h
# End Source File
# Begin Source File

SOURCE=.\ext\externalopl3.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\externalopl3.h
# End Source File
# Begin Source File

SOURCE=.\ext\externalopm.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\externalopm.h
# End Source File
# Begin Source File

SOURCE=.\ext\externalopna.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\externalopna.h
# End Source File
# Begin Source File

SOURCE=.\ext\externalpsg.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\externalpsg.h
# End Source File
# End Group
# Begin Group "misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\misc\guard.h
# End Source File
# Begin Source File

SOURCE=.\misc\threadbase.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\threadbase.h
# End Source File
# Begin Source File

SOURCE=.\misc\tickcounter.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\tickcounter.h
# End Source File
# Begin Source File

SOURCE=.\misc\trace.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\trace.h
# End Source File
# Begin Source File

SOURCE=.\misc\tstring.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\tstring.h
# End Source File
# Begin Source File

SOURCE=.\misc\vc6macros.h
# End Source File
# Begin Source File

SOURCE=.\misc\WndBase.h
# End Source File
# Begin Source File

SOURCE=.\misc\wndloc.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\wndloc.h
# End Source File
# Begin Source File

SOURCE=.\misc\WndProc.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\WndProc.h
# End Source File
# End Group
# Begin Group "subwnd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\subwnd\dclock.cpp
# End Source File
# Begin Source File

SOURCE=.\subwnd\dclock.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DOSIO.CPP
# End Source File
# Begin Source File

SOURCE=.\FONTMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\INI.CPP
# End Source File
# Begin Source File

SOURCE=.\JOYMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\MENU.CPP
# End Source File
# Begin Source File

SOURCE=.\MOUSEMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\SCRNMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\SOUNDMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\SYSMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\TIMEMNG.CPP
# End Source File
# Begin Source File

SOURCE=.\WINKBD.CPP
# End Source File
# Begin Source File

SOURCE=.\XMIL.CPP
# End Source File
# Begin Source File

SOURCE=.\Xmil.rc
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

SOURCE=..\STATSAVE.C
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
# Begin Source File

SOURCE=.\Icons\CURSOR1.CUR
# End Source File
# Begin Source File

SOURCE=.\Icons\vinered.ico
# End Source File
# End Group
# End Target
# End Project

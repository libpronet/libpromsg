# Microsoft Developer Studio Project File - Name="pro_msg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pro_msg - Win32 Debug_MD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pro_msg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pro_msg.mak" CFG="pro_msg - Win32 Debug_MD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pro_msg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pro_msg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "pro_msg - Win32 Release_MD" (based on "Win32 (x86) Static Library")
!MESSAGE "pro_msg - Win32 Debug_MD" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pro_msg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../_release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../../../libpronet/pub/inc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0501 /D for="if (0) {} else for" /FR /Fd"../_release/pro_msg_s-mt.pdb" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../_release/pro_msg_s-mt.lib"

!ELSEIF  "$(CFG)" == "pro_msg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../_debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../../libpronet/pub/inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0501 /D for="if (0) {} else for" /FR /Fd"../_debug/pro_msg_s-mt.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../_debug/pro_msg_s-mt.lib"

!ELSEIF  "$(CFG)" == "pro_msg - Win32 Release_MD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "pro_msg___Win32_Release_MD"
# PROP BASE Intermediate_Dir "pro_msg___Win32_Release_MD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../_release-md"
# PROP Intermediate_Dir "Release_MD"
# PROP Target_Dir ""
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /O2 /I "../../../src/mbedtls/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0501 /D for="if (0) {} else for" /FR /Fd"../_release/pro_msg_s.pdb" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../../../libpronet/pub/inc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0501 /D for="if (0) {} else for" /FR /Fd"../_release-md/pro_msg_s-md.pdb" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../_release/pro_msg_s-mt.lib"
# ADD LIB32 /nologo /out:"../_release-md/pro_msg_s-md.lib"

!ELSEIF  "$(CFG)" == "pro_msg - Win32 Debug_MD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "pro_msg___Win32_Debug_MD"
# PROP BASE Intermediate_Dir "pro_msg___Win32_Debug_MD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../_debug-md"
# PROP Intermediate_Dir "Debug_MD"
# PROP Target_Dir ""
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../src/mbedtls/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0501 /D for="if (0) {} else for" /FR /Fd"../_debug/pro_msg_s.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../libpronet/pub/inc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0501 /D for="if (0) {} else for" /FR /Fd"../_debug-md/pro_msg_s-md.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../_debug/pro_msg_s-mt.lib"
# ADD LIB32 /nologo /out:"../_debug-md/pro_msg_s-md.lib"

!ENDIF 

# Begin Target

# Name "pro_msg - Win32 Release"
# Name "pro_msg - Win32 Debug"
# Name "pro_msg - Win32 Release_MD"
# Name "pro_msg - Win32 Debug_MD"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\pro_msg\msg_client.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pro_msg\msg_server.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\pro_msg\msg_client.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pro_msg\msg_server.h
# End Source File
# End Group
# End Target
# End Project

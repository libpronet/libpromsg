@echo off
set THIS_DIR=%~sdp0

copy /y %THIS_DIR%..\..\build\windows-vs6\_debug\pro_msg_s-mt.lib    %THIS_DIR%windows-vs6\x86\
copy /y %THIS_DIR%..\..\build\windows-vs6\_debug\pro_msg_s-mt.pdb    %THIS_DIR%windows-vs6\x86\
copy /y %THIS_DIR%..\..\build\windows-vs6\_debug\pro_msg_jni.dll     %THIS_DIR%windows-vs6\x86\
copy /y %THIS_DIR%..\..\build\windows-vs6\_debug\pro_msg_jni.lib     %THIS_DIR%windows-vs6\x86\
copy /y %THIS_DIR%..\..\build\windows-vs6\_debug\pro_msg_jni.map     %THIS_DIR%windows-vs6\x86\
copy /y %THIS_DIR%..\..\build\windows-vs6\_debug\pro_msg_jni.pdb     %THIS_DIR%windows-vs6\x86\

copy /y %THIS_DIR%..\..\build\windows-vs6\_debug-md\pro_msg_s-md.lib %THIS_DIR%windows-vs6\x86\
copy /y %THIS_DIR%..\..\build\windows-vs6\_debug-md\pro_msg_s-md.pdb %THIS_DIR%windows-vs6\x86\

pause

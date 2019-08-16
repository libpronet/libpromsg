@echo off
set THIS_DIR=%~sdp0

copy /y %THIS_DIR%..\..\build\windows-vs2010\_release\pro_msg_s-mt.lib       %THIS_DIR%windows-vs2010\x86\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release\pro_msg_s-mt.pdb       %THIS_DIR%windows-vs2010\x86\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release\pro_msg_jni.dll        %THIS_DIR%windows-vs2010\x86\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release\pro_msg_jni.lib        %THIS_DIR%windows-vs2010\x86\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release\pro_msg_jni.map        %THIS_DIR%windows-vs2010\x86\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release\pro_msg_jni.pdb        %THIS_DIR%windows-vs2010\x86\

copy /y %THIS_DIR%..\..\build\windows-vs2010\_release-md\pro_msg_s-md.lib    %THIS_DIR%windows-vs2010\x86\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release-md\pro_msg_s-md.pdb    %THIS_DIR%windows-vs2010\x86\

copy /y %THIS_DIR%..\..\build\windows-vs2010\_release64x\pro_msg_s-mt.lib    %THIS_DIR%windows-vs2010\x86_64\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release64x\pro_msg_s-mt.pdb    %THIS_DIR%windows-vs2010\x86_64\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release64x\pro_msg_jni.dll     %THIS_DIR%windows-vs2010\x86_64\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release64x\pro_msg_jni.lib     %THIS_DIR%windows-vs2010\x86_64\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release64x\pro_msg_jni.map     %THIS_DIR%windows-vs2010\x86_64\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release64x\pro_msg_jni.pdb     %THIS_DIR%windows-vs2010\x86_64\

copy /y %THIS_DIR%..\..\build\windows-vs2010\_release64x-md\pro_msg_s-md.lib %THIS_DIR%windows-vs2010\x86_64\
copy /y %THIS_DIR%..\..\build\windows-vs2010\_release64x-md\pro_msg_s-md.pdb %THIS_DIR%windows-vs2010\x86_64\

pause

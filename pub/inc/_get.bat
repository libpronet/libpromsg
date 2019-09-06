@echo off
set THIS_DIR=%~sdp0

copy /y %THIS_DIR%..\..\src\pro_msg\msg_client.h                   %THIS_DIR%promsg\
copy /y %THIS_DIR%..\..\src\pro_msg\msg_client2.h                  %THIS_DIR%promsg\
copy /y %THIS_DIR%..\..\src\pro_msg\msg_server.h                   %THIS_DIR%promsg\

copy /y %THIS_DIR%..\..\src\pro_msg_jni\com\pro\msg\ProMsgJni.java %THIS_DIR%com\pro\msg\

pause

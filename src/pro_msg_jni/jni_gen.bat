@echo off
set THIS_DIR=%~sdp0

cd %THIS_DIR%
javac %THIS_DIR%com\pro\msg\ProMsgJni.java

javah -d %THIS_DIR%com\pro\msg -classpath %THIS_DIR% com.pro.msg.ProMsgJni
javap -s                       -classpath %THIS_DIR% com.pro.msg.ProMsgJni.PRO_MSG_USER
javap -s                       -classpath %THIS_DIR% com.pro.msg.ProMsgJni.MsgClientListener
javap -s                       -classpath %THIS_DIR% com.pro.msg.ProMsgJni.MsgServerListener

copy /y %THIS_DIR%com\pro\msg\com_pro_msg_ProMsgJni.h %THIS_DIR%
del %THIS_DIR%com\pro\msg\*.h
del %THIS_DIR%com\pro\msg\*.class

pause

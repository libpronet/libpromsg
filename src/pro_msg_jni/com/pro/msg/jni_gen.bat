@echo off
set THIS_DIR=%~sdp0

cd %THIS_DIR%
javac ProMsgJni.java

cd ..\..\..
javah -d com\pro\msg com.pro.msg.ProMsgJni
javap -s com.pro.msg.ProMsgJni.PRO_MSG_USER
javap -s com.pro.msg.ProMsgJni.MsgClientListener
javap -s com.pro.msg.ProMsgJni.MsgServerListener

cd com\pro\msg
copy /y com_pro_msg_ProMsgJni.h ..\..\..\
del *.h
del *.class

pause

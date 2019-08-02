procfgdir = ${prefix}/libpromsg/cfg

#############################################################################

procfg_DATA = ../../../../pub/cfg/ca.crt              \
              ../../../../pub/cfg/server.crt          \
              ../../../../pub/cfg/server.key          \
              ../../../../pub/cfg/msg_server.cfg      \
              ../../../../pub/cfg/msg_server-java.cfg \
              ../../../../pub/cfg/msg_client.cfg      \
              ../../../../pub/cfg/set1_sys.sh         \
              ../../../../pub/cfg/set2_proc.sh

install-data-hook:
	chmod +x ${procfgdir}/set1_sys.sh
	chmod +x ${procfgdir}/set2_proc.sh

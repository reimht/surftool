#!/bin/sh

INIFILE='/usr/local/www/surftool/surftool.ini'
STDAEMON='/usr/local/share/surftool/surftooldaemon.sh'
LOGFILE='/var/log/surftool.log'


rc_start() {

        if ! [ -f "$STDAEMON" ]; then
                echo "Error surftooldaemon executable: file '$STDAEMON' does not exist "
        fi

        if ! [ -f "$INIFILE" ]; then
                echo "Error surftooldaemon config file '$INIFILE' does not exist "
        fi

        #start daemon
        $STDAEMON start $INIFILE >> $LOGFILE 2>&1 &


}

rc_stop() {
        $STDAEMON stop $INIFILE
}

rc_status() {
        $STDAEMON status $INIFILE
}

case $1 in
        start)
                rc_start
                ;;
        stop)
                rc_stop
                ;;
        status)
                rc_status
                ;;
        restart)
                rc_stop
                rc_start
                ;;
esac


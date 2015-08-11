#!/bin/sh

stoppfile="/tmp/surftool/stoppdaemon"

rm -f "$stoppfile"

while [ ! -f "$stoppfile" ]
do
  /usr/local/bin/php --no-header /usr/local/www/surftool/check_and_activate.php >> /var/log/surftool.log
  sleep 1
done

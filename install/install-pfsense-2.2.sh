#!/bin/sh

echo "copy web page"
mkdir -p /usr/local/www/surftool/
mv /tmp/surftool-master/*.php /usr/local/www/surftool/
mv /tmp/surftool-master/*.ini /usr/local/www/surftool/
mv /tmp/surftool-master/*.inc /usr/local/www/surftool/

echo "copy daemon"
mkdir -p /usr/local/share/surftool/
mv /tmp/surftool-master/surftooldaemon.sh /usr/local/share/surftool/
chmod +x /usr/local/share/surftool/surftooldaemon.sh

echo "copy init.d script"
cp /tmp/surftool-master/init.d/surftool-pfsense.sh /usr/local/etc/rc.d/surftool.sh
chmod +x /usr/local/etc/rc.d/surftool.sh

echo "start service"
service surftool.sh start


#!/bin/bash
sudo killall dcerpcd lwiod lwregd lwsmd eventlogd netlogond
sudo mkdir /lwcifs
sudo chmod 0777 /lwcifs
sudo apt-get purge likewise-open
sudo dpkg --install  /work/samba/likewise/likewise-open-6.1.0.406/debug/package/deb/likewise-open/likewise-open_6.1.0-0_amd64.deb
sudo /opt/likewise/bin/lw-lsa mod-user --enable-user Administrator
sudo /opt/likewise/bin/lw-lsa  mod-user --set-password springpath Administrator
sudo /opt/likewise/bin/lwsm start srvsvc
sudo /opt/likewise/bin/lwsm start dfs
sudo /opt/likewise/bin/lwsm list

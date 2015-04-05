#!/bin/bash

sudo mv /opt/likewise /opt/likewise-install
sudo ln -s /work/samba/likewise/likewise-open-6.1.0.406/debug/stage/opt/likewise/ /opt/

sudo mv /etc/likewise /etc/likewise-install
sudo ln -s /work/samba/likewise/likewise-open-6.1.0.406/debug/stage/etc/likewise/ /etc/

sudo mv /var/lib/likewise /var/lib/likewise-install
sudo ln -s  /work/samba/likewise/likewise-open-6.1.0.406/debug/stage/var/lib/likewise/ /var/lib

cd /etc/pam.d/
cp ./common-password.lwidentity.orig   ./common-password
cp ./samba.lwidentity.orig		    ./samba
cp ./other.lwidentity.orig		    ./other
cp ./cups.lwidentity.orig		    ./cups
cp ./sudo.lwidentity.orig		    ./sudo
cp ./ppp.lwidentity.orig		    ./ppp
cp ./su.lwidentity.orig		    ./su
cp ./login.lwidentity.orig		    ./login
cp ./chsh.lwidentity.orig		    ./chsh
cp ./atd.lwidentity.orig		    ./atd
cp ./sshd.lwidentity.orig		    ./sshd
cp ./lightdm-autologin.lwidentity.orig ./lightdm-autologin
cp ./common-auth.lwidentity.orig	    ./common-auth
cp ./xscreensaver.lwidentity.orig	    ./xscreensaver
cp ./lightdm.lwidentity.orig	    ./lightdm
cp ./cron.lwidentity.orig		    ./cron
cp ./chfn.lwidentity.orig		    ./chfn
cp ./polkit-1.lwidentity.orig          ./polkit-1

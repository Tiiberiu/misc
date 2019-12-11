#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

read -p "Enter desired hostname: "  hostname
read -p "Enter admin username: "  adminuser
read -p "Enter username to be allowed: "  alloweduser

nmcli con mod ens160 ipv4.dns "your_ip"
nmcli con mod ens160 ipv4.ignore-auto-dns yes
nmcli con mod ens160 ipv4.dns-search "domain.local"

systemctl restart network

hostnamectl set-hostname $hostname-LNX

hostname -F /etc/hostname

yum -y update

yum -y install realmd sssd samba-common samba-common-tools krb5-workstation adcli oddjob oddjob-mkhomedir openldap-clients policycoreutils-python

sed -i '3,6 s/^/#/' /etc/chrony.conf

echo "server domain_controller.domain_name.local" | sudo tee -a /etc/chrony.conf

systemctl restart chronyd

timedatectl set-ntp true

realm -v join YONDER.LOCAL -U $adminuser@YONDER.LOCAL --computer-ou=OU=.Linux,OU=Computers,OU=Yonder --membership-software=adcli

rm -f /etc/sssd/sssd.conf

touch /etc/sssd/sssd.conf

chmod 0600 /etc/sssd/sssd.conf

echo "[sssd]
domains = yonder.local
config_file_version = 2
services = nss, pam

[domain/yonder.local]
ad_domain = yonder.local
krb5_realm = YONDER.LOCAL
realmd_tags = manages-system joined-with-adcli
cache_credentials = True
id_provider = ad
krb5_store_password_if_offline = True
default_shell = /bin/bash
ldap_id_mapping = True
access_provider = ad
use_fully_qualified_names = False
fallback_homedir = /home/%d/%u
dyndns_update = False
ad_access_filter = (|(memberOf=CN=<PATH>)(distinguishedName=<PATH>))" | tee -a /etc/sssd/sssd.conf

systemctl restart sssd

touch /etc/sudoers.d/myfile

echo "%linux_admins ALL=(ALL) ALL
$alloweduser ALL=(ALL) ALL " | tee -a /etc/sudoers.d/myfile

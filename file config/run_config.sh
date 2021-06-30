echo -e "RD:1">> /etc/mosquitto/passwd.txt
mosquitto_passwd -U passwd.txt
/etc/init.d/mosquitto enable
/etc/init.d/mosquitto restart
rm /etc/rc.button/wps
touch /etc/rc.button/wps
echo -e "#!/bin/sh

[ \"\${ACTION}\" = \"released\" -o \"\${ACTION}\" = \"timeout\" ] || exit 21

. /lib/functions.sh

logger \"\$BUTTON pressed for \$SEEN seconds\"

# factory reset
if [ \"\$SEEN\" -gt 20 ]
then
	[ -f /tmp/.factory_reset ] && return
	echo timer > /sys/class/leds/linkit-smart-7688\:orange\:wifi/trigger
	echo 100 > /sys/class/leds/linkit-smart-7688\:orange\:wifi/delay_on
	echo 100 > /sys/class/leds/linkit-smart-7688\:orange\:wifi/delay_off
	echo \"FACTORY RESET\" > /dev/console
	firstboot -y && reboot now
	
# wifi reset (back to ap mode)
elif [ \"\$SEEN\" -gt 5 ]
then
	echo none > /sys/class/leds/linkit-smart-7688\:orange\:wifi/trigger
	echo 1 > /sys/class/leds/linkit-smart-7688\:orange\:wifi/brightness
	sleep 1
	echo \"DISABLE WIFI STATION MODE\" > /dev/console
	uci set network.wan.ifname='eth0'
	uci commit network
	/etc/init.d/network restart
	uci del wireless.wifinet1
	uci commit wireless
	wifi	
	
# print wifi information (show mac under ap mode / ip under sta mode
elif [ \"\$SEEN\" -lt 1 ]
then
	echo \"REBOOT\" > /dev/console
	reboot
fi

return 0">> /etc/rc.button/wps
chmod +x /etc/rc.button/wps
touch /etc/bb.txt
awk '{print toupper($0)}' < /sys/class/net/eth0/address > /etc/bb.txt
read MAC </etc/bb.txt
Mac1=${MAC:12:2}
Mac2=${MAC:15:2}
temp="$Mac1$Mac2"
rm /etc/bb.txt
mv /etc/RDhcPy /root
mv /etc/run.sh /root
touch /etc/crontabs/root
echo -e "*/1 * * * * /root/run.sh">> /etc/crontabs/root
/etc/init.d/cron restart
uci set wireless.default_radio0.ssid="RD_HC_$temp"
uci set system.@system[0].hostname="RD_HC_$temp"
uci set wireless.radio0.disabled='0'
uci set network.wan=interface
uci set network.wan.proto='dhcp'
uci set system.@system[0].zonename='Asia/Ho Chi Minh'
uci set system.@system[0].timezone='<+7>-7'
uci set network.wan.ifname='eth0'
uci set network.lan.macaddr="$MAC"
uci set network.lan.force_link='1'
uci set network.lan.ipaddr='10.10.10.1'
uci del network.lan.ifname
uci commit
/etc/init.d/wireless restart
/etc/init.d/network restart
/etc/init.d/system restart
reboot

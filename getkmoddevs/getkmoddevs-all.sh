#!/bin/sh
#
# Author:
# Tuan Hoang <tqhoang@elrepo.org>
#
# Description:
# This script loops over all instaled kmods and calls getkmoddevs-single.sh on each *.ko file
#
# Note:
# - Applies a blacklist filter for *.ko files that do not have device info
# - Applies a quirklist workaround for drivers missing device info
#
# Assumes:
# - Only ELRepo kmods are installed
#
# Usage:
# getkmoddevs-all.sh
#


# Array for RHEL kmods blacklist
kmod_blacklist=("kvdo.ko" "uds.ko" "oracleasm.ko")

# Append ELRepo EL9 and EL10 kmods
kmod_blacklist+=("b43.ko" "b43legacy.ko" "drbd.ko" "drbd_transport_lb-tcp.ko" "drbd_transport_rdma.ko" "drbd_transport_tcp.ko" "dvb-core.ko" "ecryptfs.ko" "em28xx-alsa.ko" "em28xx-dvb.ko" "em28xx-rc.ko" "em28xx-v4l.ko" "floppy.ko" "hfs.ko" "hfsplus.ko" "hid-mcp2221.ko" "led-class-multicolor.ko" "leds-gpio.ko" "leds-pca9532.ko" "libsas.ko" "lgdt330x.ko" "megaraid_mm.ko" "mlx4_en.ko" "mlx4_ib.ko" "mptctl.ko" "mt792x-usb.ko" "nvidia-drm.ko" "nvidia-modeset.ko" "nvidia-peermem.ko" "nvidia-uvm.ko" "ocfs2_dlmfs.ko" "ocfs2_dlm.ko" "ocfs2.ko" "ocfs2_nodemanager.ko" "ocfs2_stackglue.ko" "ocfs2_stack_o2cb.ko" "ovpn-dco-v2.ko" "ovpn.ko" "rc-core.ko" "rc-pinnacle-pctv-hd.ko" "rtw88_8723d.ko" "rtw88_8812a.ko" "rtw88_8814a.ko" "rtw88_8821a.ko" "rtw88_8821c.ko" "rtw88_88xxa.ko" "rtw88_usb.ko" "rtw89_8851b.ko" "rtw89_8852bt.ko" "rtw89_8922a.ko" "rtw89_usb.ko" "s5h1411.ko" "tda10048.ko" "tda18271.ko" "tvp5150.ko" "usbip-core.ko" "usbip-host.ko" "vhci-hcd.ko" "v4l2-async.ko" "v4l2-fwnode.ko" "v4l2loopback.ko" "xc2028.ko" "xt_time.ko" "xt_u32.ko" "zl10353.ko")

# Append ELRepo EL8-only kmods
kmod_blacklist+=("ath.ko" "bnxt_re.ko" "ftsteutates.ko" "handshake.ko" "iwlegacy.ko" "jfs.ko" "lru_cache.ko" "sch_cake.ko" "sysv.ko" "wireguard.ko")

# Array for kmod quirklist
kmod_quirklist=("a2818.ko" "si2157.ko" "wl.ko")

# Array for kmod RPM names
kmod_rpmlist=()

# Function to check if the array has an element
set +e #otherwise the script will exit on error
function containsElement () {
	local e match="$1"
	shift
	for e; do [[ "$e" == "$match" ]] && return 0; done
	return 1
}


# Loop over all kmods for the kernel
for kmod in `find /lib/modules/*/extra -name "*.ko" | sort`
do
	# filter blacklist
	containsElement "`basename $kmod`" "${kmod_blacklist[@]}"
	if [ $? == 0 ]
	then
		continue
	fi
	
	# get the RPM name
	KMOD_RPM=`rpm --queryformat "%{name}" -qf ${kmod}`

	# only print the RPM name once
	containsElement "${KMOD_RPM}" "${kmod_rpmlist[@]}"
	if [ $? == 1 ]
	then
		kmod_rpmlist+=(${KMOD_RPM})
		echo "===== ${KMOD_RPM} ====="
	fi

	# print the kmod name
	echo "(`basename $kmod`) \\\\"

	# check quirklist
	containsElement "`basename $kmod`" "${kmod_quirklist[@]}"
	if [ $? == 1 ]
	then
		./getkmoddevs-single.sh $kmod
	else
		# hack for kmod-a2818
		if [ "`basename $kmod`" == "a2818.ko" ]
		then
			./lsdevname -n -v 10B5 -d 9054
			echo " \\\\"
			echo " \\\\"
		elif [ "`basename $kmod`" == "si2157.ko" ]
		then
			echo "[i2c:si2141] I2C UNKNOWN DEVICE si2141 \\\\"
			echo "[i2c:si2146] I2C UNKNOWN DEVICE si2146 \\\\"
			echo "[i2c:si2157] I2C UNKNOWN DEVICE si2157 \\\\"
			echo "[i2c:si2177] I2C UNKNOWN DEVICE si2177 \\\\"
			echo " \\\\"
		elif [ "`basename $kmod`" == "wl.ko" ]
		then
			./lsdevname -n -v 14e4 -d 4311
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4312
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4313
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4315
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4727
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4328
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4329
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 432a
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 432b
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 432c
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 432d
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4365
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 0576
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4353
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4357
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4358
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4359
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4331
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 43b1
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 4360
			echo " \\\\"
			./lsdevname -n -v 14e4 -d 43a0
			echo " \\\\"
			echo " \\\\"
		fi
	fi
done

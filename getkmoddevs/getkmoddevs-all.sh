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
kmod_blacklist+=("drbd.ko" "drbd_transport_lb-tcp.ko" "drbd_transport_rdma.ko" "drbd_transport_tcp.ko" "dvb-core.ko" "ecryptfs.ko" "em28xx-alsa.ko" "em28xx-dvb.ko" "em28xx-rc.ko" "em28xx-v4l.ko" "floppy.ko" "hfs.ko" "hfsplus.ko" "hid-mcp2221.ko" "led-class-multicolor.ko" "leds-gpio.ko" "leds-pca9532.ko" "libsas.ko" "lgdt330x.ko" "megaraid_mm.ko" "mlx4_en.ko" "mlx4_ib.ko" "mptctl.ko" "mt792x-usb" "nvidia-drm.ko" "nvidia-modeset.ko" "nvidia-peermem.ko" "nvidia-uvm.ko" "ovpn-dco-v2.ko" "ovpn.ko" "rc-core.ko" "rc-pinnacle-pctv-hd.ko" "rtw88_8723d.ko" "rtw88_8812a.ko" "rtw88_8814a.ko" "rtw88_8821a.ko" "rtw88_8821c.ko" "rtw88_88xxa.ko" "rtw88_usb.ko" "si2157.ko" "tvp5150.ko" "usbip-core.ko" "usbip-host.ko" "vhci-hcd.ko" "v4l2-async.ko" "v4l2-fwnode.ko" "v4l2loopback.ko" "xc2028.ko" "xt_time.ko" "xt_u32.ko" "zl10353.ko")
# Append ELRepo EL8-only kmods
kmod_blacklist+=("ath.ko" "bnxt_re.ko" "ftsteutates.ko" "handshake.ko" "iwlegacy.ko" "jfs.ko" "lru_cache.ko" "sch_cake.ko" "sysv.ko" "wireguard.ko")

# Array for kmod quirklist
kmod_quirklist=("a2818.ko")

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
		./lsdevname -n -v 10B5 -d 9054
		echo " \\\\"
		echo " \\\\"
	fi
done

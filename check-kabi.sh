#!/bin/bash
#
# Author: Phil Perry <phil@elrepo.org>
#
# Description: Script to check kABI compatibility of kmod packages.
#
# v0.1 - 09 Oct 2021.
# v0.2 - 24 Oct 2021.
#
# Usage: just run the script in a dir containing kmod packages
# The script will compare the required kernel symbol hashes from
# 'rpm -qp --requires' against those listed in Module.symvers for the running kernel

errors="0"
kversion=$(uname -r)
pkg=""

if [ -n "$(ls -1U ./ | grep kmod-.*\.rpm$)" ]; then
	echo "kmod packages found - running kABI compatibility tests with kernel-$kversion"
else
	echo "No kmod packages found to test"
	exit 1
fi

if [ -e ./Module.symvers ]; then
	rm ./Module.symvers
fi
cp /usr/src/kernels/$kversion/Module.symvers ./Module.symvers

for kmod in `ls kmod-*.rpm`; do
	ksym_hashes=`rpm -qp --requires $kmod | grep kernel\( | awk -F"= " '{print $2}'`
	# echo "$ksym_hashes"
	# 0xeae3dfd6
	for hash in $ksym_hashes; do
		if grep -q $hash ./Module.symvers; then
			# echo "$hash found in Module.symvers"
			continue
		else
			if [ "$pkg" != "$kmod" ]; then
				echo -e "\nPackage $kmod failed kABI check"
				pkg="$kmod"
			fi
			failed_ksym=`rpm -qp --requires $kmod | grep $hash`
			echo "$failed_ksym"
			# echo "$hash not found in Module.symvers"
			errors=$(( $errors + 1 ))
		fi
	done
done
echo -e "\nTotal number of errors: $errors"
exit 0

#!/bin/sh
#
# Author:
# Tuan Hoang <tqhoang@elrepo.org>
#
# Description:
# This script prints all the device info for a single kmod filename (*.ko)
#
# Output format:
# (<kmod filename>)
# [<vendor ID>:<device ID>] <vendor name> <device name> \\
# ..
# [<vendor ID>:<device ID>] <vendor name> <device name> \\
# \\


# Function to print the PCI info
function printPciInfo() {
   arr=("$@")
   for i in "${arr[@]}";
      do
          vendorID=`echo "$i" | cut -d: -f1`
          deviceID=`echo "$i" | cut -d: -f2`

	  # for some reason there is an empty element
	  if [ ! -z "${vendorID}" ]
	  then
	  	if [ ! -z "${deviceID}" ]
		then
			./lsdevname -p -n -v ${vendorID} -d ${deviceID}
		else
			./lsdevname -p -n -v ${vendorID}
		fi
	  	echo " \\\\"
	  fi
      done
}


# Function to print the USB info
function printUsbInfo() {
   arr=("$@")
   for i in "${arr[@]}";
      do
          vendorID=`echo "$i" | cut -d: -f1`
          deviceID=`echo "$i" | cut -d: -f2`

	  # for some reason there is an empty element
	  if [ ! -z "${vendorID}" ]
	  then
	  	if [ ! -z "${deviceID}" ]
		then
			./lsdevname -u -n -v ${vendorID} -d ${deviceID}
		else
			./lsdevname -u -n -v ${vendorID}
		fi
	  	echo " \\\\"
	  fi
      done
}


# PCI Device ID's print 8 characters
pci_array+=( $(modinfo $1 | grep alias | grep -e pci | grep "d\*sv" | awk '{print substr($0,26,4)":"}' | sort | uniq) )
pci_array+=( $(modinfo $1 | grep alias | grep -e pci | grep -v "d\*sv" | awk '{print substr($0,26,4)":"substr($0,35,4)}' | sort | uniq) )

# will echo number of elements in array
# echo ${#pci_array[@]}
# will dump all elements of the array
# echo "${pci_array[@]}"


# USB Device ID's only print 4 characters
usb_array+=( $(modinfo $1 | grep alias | grep -e usb | grep "d\*sv" | awk '{print substr($0,22,4)":"}' | sort | uniq) )
usb_array+=( $(modinfo $1 | grep alias | grep -e usb | grep -v "d\*sv" | awk '{print substr($0,22,4)":"substr($0,27,4)}' | sort | uniq) )

# will echo number of elements in array
# echo ${#usb_array[@]}
# will dump all elements of the array
# echo "${usb_array[@]}"


# Print it all
printPciInfo "${pci_array[@]}"
printUsbInfo "${usb_array[@]}"
echo " \\\\ "

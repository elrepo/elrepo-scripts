#!/bin/bash

# Script to create a Driver Update Disk for RHEL.
#
# v1.0 June 2019.
#

PROGNAME=$(basename $0)

if [ $# -ne 1 ]; then
	echo "Usage: $PROGNAME kmod-package" >&2
	exit 1
fi

KMOD_RPM=$1

if [ ! -e $KMOD_RPM ]; then
	echo "$PROGNAME: $KMOD_RPM -- no such package." >&2
	exit 2
fi

FTWO=$(echo $KMOD_RPM | cut -d\- -f2)

KMOD_SRPM=$(echo $KMOD_RPM | sed "s/^kmod\-/$FTWO-/; s/\-$FTWO/-kmod/; s/x86_64\.rpm$/src.rpm/")

if [ ! -e $KMOD_SRPM ]; then
	KMOD_SRPM=$(echo $KMOD_RPM | sed "s/x86_64\.rpm$/src.rpm/")
fi

if [ ! -e $KMOD_SRPM ]; then
	echo "$PROGNAME: The source package for $KMOD_RPM is not present." >&2
	exit 3
fi

PACKNAME=$(echo $KMOD_RPM | sed "s/^kmod/dd/; s/x86_64\.rpm$/iso/")

if [ ! -x /usr/bin/createrepo ]; then
	echo "$PROGNAME: Please install the createrepo package." >&2
	exit 4
fi

# Remove any stale dd directory structure.
rm -fr ./dd

# Create the dd directory structure.
mkdir -p ./dd/rpms/x86_64/repodata/
mkdir -p ./dd/src/

# Populate the dd directory structure.
echo -e "Driver Update Disk version 3\c" > ./dd/rhdd3
cp $KMOD_RPM ./dd/rpms/x86_64/
cp $KMOD_SRPM ./dd/src/

# Create the repodata.
createrepo -v --workers=1 ./dd/rpms/x86_64/
if [ $? -ne 0 ]; then
	echo "$PROGNAME: createrepo has failed." >&2
	exit 5
fi

# Create the ISO9660 image.
mkisofs -lR -V OEMDRV -input-charset utf8 -o $PACKNAME ./dd
if [ $? -ne 0 ]; then
	echo "$PROGNAME: mkisofs has failed." >&2
	exit 6
else
	rm -fr ./dd
fi

exit 0

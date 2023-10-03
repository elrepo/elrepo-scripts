#!/bin/bash

# Script to create a Driver Update Disk for RHEL.
#
# v1.1 July 2019.
# v1.2 June 2021 - add ISO signing
#
# From Phil:
# To create and sign DUDs, place all kmod RPMs and SRPMs in a dir and use
# thus:
# 
# for i in *.x86_64.rpm; do ./mkdd-v1.2 $i; done
# 
# You can also then verify the signatures of all asc files:
# 
# for i in *.asc; do gpg --verify $i; done
#
# Then publish them
#
# $ cp *.iso *.asc /home/buildsys/localrepo/dud/el8/x86_64/   (el8)
# $ cp *.iso *.asc /home/buildsys/localrepo/dud/el9/x86_64/   (el9)

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
createrepo -q --workers=1 ./dd/rpms/x86_64/
if [ $? -ne 0 ]; then
	echo "$PROGNAME: createrepo has failed." >&2
	exit 5
fi

# Create the ISO9660 image.
mkisofs -quiet -lR -V OEMDRV -input-charset utf8 -o $PACKNAME ./dd
if [ $? -ne 0 ]; then
	echo "$PROGNAME: mkisofs has failed." >&2
	exit 6
else
	rm -fr ./dd
fi

# Create signature file
SHA256NAME=$(echo $PACKNAME | sed "s/iso/SHA256SUM/")
sha256sum $PACKNAME > $SHA256NAME
gpg --clearsign -a $SHA256NAME
rm $SHA256NAME

exit 0

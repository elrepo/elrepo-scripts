# getkmoddevs

This app retrieves all of the kmod device info and prints the information that is used for the [ELRepo DeviceIDs](https://elrepo.org/wiki/doku.php?id=deviceids) page.


## Prerequisite
Install all ELRepo kmods to the target hosts
```
sudo dnf install kmod-\*
```


## Build
Compile `lsdevname`
```
make lsdevname
```

## Usage
1. Run the script on the primary host (ex. EL9) and redirect to a file
```
./getkmoddevs-all.sh > kmod-deviceinfo-el9.txt
```

2. Run on other target host(s) (ex. EL8)
```
./getkmoddevs-all.sh > kmod-deviceinfo-el8.txt
```

3. Manually cherrypick the deviceinfo from the other target hosts and merge into main deviceinfo file

4. Edit the wiki page and overwrite the kmod section


## Optional
Update the hwdata files under */usr/share/hwdata*
- [PCI devices](https://pci-ids.ucw.cz/)
- [USB devices](http://www.linux-usb.org/usb-ids.html)

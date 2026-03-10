#!/bin/sh

mkdir -p build
cd build
make
./grub-mkimage -O x86_64-efi -o grubx64.efi -p '(tftp,192.168.190.2)/test19/accek-tb' -c ../../boot/grub-accek-tb-bootstrap-xen.cfg -d grub-core all_video boot btrfs cat chain configfile echo efifwsetup efinet ext2 fat font gfxmenu gfxterm gzio halt hfsplus iso9660 jpeg loadenv loopback lvm mdraid09 mdraid1x minicmd normal part_apple part_msdos part_gpt password_pbkdf2 png reboot regexp search search_fs_uuid search_fs_file search_label serial sleep syslinuxcfg test tftp video xfs backtrace http linux usb usbserial_common usbserial_pl2303 usbserial_ftdi usbserial_usbdebug keylayouts at_keyboard multiboot2 efifb


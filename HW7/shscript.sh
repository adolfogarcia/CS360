#! /bin/bash
dd if=/dev/zero of=mydisk bs=1024 count=1440
mkfs -b 1024 mydisk 1440
mount mydisk /mnt
(cd /mnt; mkdir dir1 dir2; touch file1 file2; mkdir dir1/dir3; ls -l);
umount /mnt

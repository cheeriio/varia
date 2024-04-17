(cd / && patch -p1) <mp438675.patch
cd /usr/src/minix/include/minix && make includes
cd /usr/src/include && make includes
cd /usr/src/minix/fs/procfs && make && make install
cd /usr/src/minix/servers/pm && make && make install
cd /usr/src/minix/drivers/storage/ramdisk && make && make install
cd /usr/src/minix/drivers/storage/memory && make && make install
cd /usr/src/lib/libc && make && make install
cd /usr/src/releasetools && make do-hdboot

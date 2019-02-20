Steps to run

gcc _fs.c -w -g -o _fs `pkg-config fuse --cflags --libs`
mkdir mountpoint
gdb _fs
run -d -f mountpoint/


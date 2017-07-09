#!/bin/sh

find /dev/ > /tmp/a
lsusb > /tmp/a2
read -p "Plug in your device and press any key to continue... " x
find /dev/ > /tmp/b
lsusb > /tmp/b2
echo ""
echo "/dev/ changes:"
diff /tmp/a /tmp/b
echo ""
echo "lsusb changes:"
diff /tmp/a2 /tmp/b2

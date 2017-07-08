#!/bin/sh

read -p "Unplug your device and press any key to begin ... " x
find /dev/ > /tmp/a
read -p "Plug in your device and press any key to continue... " x
find /dev/ > /tmp/b
echo "Your device's has added the following to /dev/"
echo ""
diff /tmp/a /tmp/b
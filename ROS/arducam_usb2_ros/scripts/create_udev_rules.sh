#!/bin/bash

echo "start copy arducam.rules to  /etc/udev/rules.d/"
echo "`rospack find arducam_usb2_ros`/scripts/arducam.rules"
sudo cp `rospack find arducam_usb2_ros`/scripts/arducam.rules  /etc/udev/rules.d
echo " "
echo "Restarting udev"
echo ""
sudo service udev reload
sudo service udev restart
echo "finish "

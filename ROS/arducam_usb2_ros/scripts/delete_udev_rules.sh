#!/bin/bash

echo "deleting /etc/udev/rules.d/arducam.rules"
sudo rm /etc/udev/rules.d/arducam.rules
echo " "
echo "Restarting udev"
echo ""
sudo service udev reload
sudo service udev restart
echo "finish  delete"

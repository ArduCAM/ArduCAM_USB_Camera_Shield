#!/bin/bash

sudo cp scripts/arducam.rules  /etc/udev/rules.d
sudo service udev reload
sudo service udev restart
chmod +x src/arducam_ros_node.py
chmod +x src/arducam_trigger_ros_node.py
chmod +x src/display_test.py

# arducam_usb2_ros
ArduCAM USB2 Shield ROS node

Setup:
1. run create_udev_rules.sh in scripts folder
2. run 'chmod +x arducam_ros_node.py' command in src folder

Usage:
1. Change "camera_config" argument in arducam_node.launch file to corresponding Arducam camera.
2. roslaunch arducam_usb2_ros arducam_node.launch

Topic publish:
/arducam/camera/image_raw

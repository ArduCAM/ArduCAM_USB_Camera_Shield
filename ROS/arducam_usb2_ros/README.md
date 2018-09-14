# ArduCAM USB2 Shield ROS node

(Tested on Ubuntu 16.04/Linux Mint 18, ROS Kinetic)

## Setup:
1. Copy arducam_usb2_ros to your catkin workspace directory
2. Run catkin_make
3. Run setup.sh in arducam_usb2_ros folder

## Usage:
1. Change "camera_config" argument in arducam_node.launch file to your corresponding Arducam camera.
2. Change "serial_number" argument in arducam_node.launch file to your corresponding Arducam's serial number in the form of XXXX-XXXX-XXXX, leave it blank if there is only one camera.
3. Change "horizontal_flip" or "vertical_flip" arguments in arducam_node.launch file to change image horizontal or vertical orientation.
4. roslaunch arducam_usb2_ros arducam_node.launch

## From default arducam_node.launch file
- ### Topic publish:
/cam0/arducam/camera/image_raw

/cam0/arducam/camera/captured

- ### Read value from sensor register:
rosservice call /cam0/arducam/read_reg *(register address)*

Example, to obtain chip version:
rosservice call /cam0/arducam/read_reg 0

- ### Write value to sensor register:
rosservice call /cam0/arducam/write_reg *(register address) (value)*

Example, to adjust exposure on MT9N001:
rosservice call /cam0/arducam/write_reg 12306 100

- ### Capture image:
rosservice call /cam0/arducam/capture

(Image published to /cam0/arducam/camera/captured)


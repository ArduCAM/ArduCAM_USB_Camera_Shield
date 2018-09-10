# ArduCAM USB2 Shield ROS node

## Setup:
1. Copy arducam_usb2_ros to your catkin workspace directory
2. Run catkin_make
3. Run setup.sh in arducam_usb2_ros folder

## Usage:
1. Change "camera_config" argument in arducam_node.launch file to your corresponding Arducam camera.
3. Change "serial_number" argument in arducam_node.launch file to your corresponding Arducam's serial number in the form of XXXX-XXXX-XXXX-XXXX, leave it blank if there is only one camera.
2. roslaunch arducam_usb2_ros arducam_node.launch

## With default arducam_node.launch file,
- ### Topic publish:
cam0/arducam/camera/image_raw

- ### Read value from sensor register:
rosservice call cam0/arducam/read_reg *(register address)*

Example, to obtain chip version:
rosservice call cam0/arducam/read_reg 0

- ### Write value to sensor register:
rosservice call cam0/arducam/write_reg *(register address) (value)*

Example, to adjust exposure on MT9N001:
rosservice call cam0/arducam/write_reg 12306 100

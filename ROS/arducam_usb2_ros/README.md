# ArduCAM USB2 Shield ROS node

(Tested on Ubuntu 16.04/Linux Mint 18, ROS Kinetic)

## Setup:
1. Copy arducam_usb2_ros to your catkin workspace directory
2. Run catkin_make
3. Run setup.sh in arducam_usb2_ros folder

## Usage:
1. Change "camera_config" argument in the launch file to your corresponding Arducam camera JSON config file.
2. Change "serial_number" argument in the launch file to your corresponding Arducam's serial number in the form of XXXX-XXXX-XXXX, leave it blank if there is only one camera.
3. Change "horizontal_flip" or "vertical_flip" arguments in the launch file to change image horizontal or vertical orientation.
4. Change "frame_id" argument in the launch file to change topic's frame_id of your like.
5. Single camera example: roslaunch arducam_usb2_ros arducam_node.launch<br/>
Multiple cameras example: roslaunch arducam_usb2_ros multiarducam_node.launch<br/>
External/software trigger image capture example: roslaunch arducam_usb2_ros trigger_capture_node.launch

## Single camera example
- ### Topic publish:
/cam0/arducam/camera/image_raw

/cam0/arducam/captured/camera/image_raw

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

## External/software trigger image capture example
- ### Topic publish:
/cam0/arducam/triggered/camera/image_raw

- ### Read value from sensor register:
rosservice call /cam0/arducam/read_reg *(register address)*

Example, to obtain chip version:
rosservice call /cam0/arducam/read_reg 0

- ### Write value to sensor register:
rosservice call /cam0/arducam/write_reg *(register address) (value)*

Example, to adjust exposure on MT9N001:
rosservice call /cam0/arducam/write_reg 12306 100

- ### Software trigger image capture:
rosservice call /cam0/arducam/trigger

*Software trigger or external trigger (pull TRIGGER pin to HIGH) captured image will be published to /cam0/arducam/triggered/camera/image_raw*


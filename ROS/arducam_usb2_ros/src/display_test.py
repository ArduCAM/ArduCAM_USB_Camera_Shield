#!/usr/bin/env python
import rospy
import rosbag, sys, yaml, os, cv2
import numpy as np
from sensor_msgs.msg import CompressedImage, Image
from std_msgs.msg import String
import sys

class ImageConversions():
    # We only instantiate the bridge once
    bridge = None
    
def get_cv_bridge():
    if ImageConversions.bridge is None:
        from cv_bridge import CvBridge  # @UnresolvedImport
        ImageConversions.bridge = CvBridge()
    return ImageConversions.bridge

def bgr_from_imgmsg(msg):
    bridge = get_cv_bridge()
    return bridge.imgmsg_to_cv2(msg)
    
def rgb_from_imgmsg(msg):
    bridge = get_cv_bridge()
    return bridge.imgmsg_to_cv2(msg, "rgb8")
    
def callback(data):
    cv2.namedWindow("ArduCam Demo",1)
    image = bgr_from_imgmsg(data)
    cv2.imshow("ArduCam Demo",image)
    cv2.waitKey(10)
    
    #rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)
    
def listener(index):

    # In ROS, nodes are uniquely named. If two nodes with the same
    # node are launched, the previous one is kicked off. The
    # anonymous=True flag means that rospy will choose a unique
    # name for our 'listener' node so that multiple listeners can
    # run simultaneously.
    
    rospy.init_node('listener', anonymous=True)

    rospy.Subscriber("/cam{}/arducam/camera/image_raw".format(index), Image, callback)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    index = 0
    if len(sys.argv) ==  2:
        index = int(sys.argv[1])
    listener(index)

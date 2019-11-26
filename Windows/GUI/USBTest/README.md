# Overview

Windows demo code for the following cameras:

- MT9V034 (0.36MP Global Shutter Camera)
- AR0134  (1.2MP Global Shutter Camera)
- MT9N001 (9MP Color Rolling Shutter Camera)
- MT9J001 (10MP Mono Rolling Shutter Camera)
- MT9J003 (10MP Color Rolling Shutter Camera)
- MT9F002 (14MP Color Rolling Shutter Camera)
- OV7675  (0.3MP Rolling Shutter Camera)
- OV2640  (2MP Rolling Shutter Camera)
- OV5640  (5MP Rolling Shutter Camera)


# Usuage

1. For 64bit Windows run the demo excutable file from : ``` \x64\Release\USBTest.exe ```
or for 32bit Windows run the demo excutable file from : ``` \Release\USBTest.exe ```
![Demo][Demo]

[Demo]:http://www.arducam.com/wp-content/uploads/2017/09/USB_Camera_Demo.jpg "Demo"


2. Select the Sensor type from the configuration file drop list.

3. Click "auto-Open" or "scan" then "open" button to open the camera.

4. Click the "play" / "stop" button to start/stop video streaming. 

5. Click the "shot" button to take a snapshot with BMP or JPEG format.

6. Save option "Display" only display realtime video, "Save Image" only save continuous BMP/JPEG images, "Save Data" only save RAW images.

7. Fill in "RegAddr" and "Value" text box in decimal or hex(start with 0x) format to read and write sensor register value.


## Window Control
Using mice's scroll wheel to do video zoom in/out.
Checkthe "Full Screen" to display the video in full window.


# Troubleshooting
You might come across missing MSVCF100.DLL file or similar.
Please install the Microsoft Visual C++ 2010 SP1 Redistributable Package from
[Microsoft](https://www.microsoft.com/en-US/download/details.aspx?id=8328).


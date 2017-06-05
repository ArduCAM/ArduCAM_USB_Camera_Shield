# Overview

Windows demo code for the following cameras:

- MT9V034 (0.36MP Global Shutter Camera)
- AR0134 (1.2MP Global Shutter Camera)
- MT9N001 （9MP Rolling Shutter Camera)
- MT9J001 （10MP Rolling Shutter Camera)
- MT9F002 （14MP Rolling Shutter Camera)

# Usuage

1. Run the demo excutable file from :

``` \x64\Release\USB_Demo.exe ```

2. Select the Sensor type from the drop list.

3. Click "auto-Open" button to open the camera.

4. Click the "play" / "stop" button to start/stop video streaming. 

5. Click the "shot" button to take a snapshot with RAW and BMP format.

6. Fill in "RegAddr" and "Value" text box in decimal format to read and write sensor register value.


## Window Control
Using mice's scroll wheel to do video zoom in/out.


# Troubleshooting
You might come across missing MSVCF100.DLL file or similar.
Please install the Microsoft Visual C++ 2010 SP1 Redistributable Package from

``` \x64\Release\vcredist_x86.exe ```

or from [Microsoft](https://www.microsoft.com/en-US/download/details.aspx?id=8328).


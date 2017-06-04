# Overview

Python demo code for the following cameras:

- MT9V034 (0.36MP Global Shutter Camera)
- AR0134 (1.2MP Global Shutter Camera)
- MT9F002 （14MP Rolling Camera)


# Usuage

- Realtime preview

``` sudo python MT9V034 ```

``` sudo python AR0134 ```

- Snapshot

``` sudo python MT9F002 --type jpg --name sample_image ```

## Parameters
--type : jpg, bmp, png

--name : file name without extension

## Window Control
The realtime preview mode support zoom in/out cwith control the keyboard

- Zoom in key ' <- '
- Zoom out key ' -> '
- Quit key ' q '

# Troubleshooting
The keyboard event number might not match your system, using the following command to determine which is your keyboard event

``` cat /proc/bus/input/devices ```

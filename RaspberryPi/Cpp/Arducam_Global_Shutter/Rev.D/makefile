CC			= g++

LIBS		= -lArduCamLib   -lpthread `pkg-config --cflags --libs opencv` -lzbar
FLAGS		= -L. -I. -I../Arducam_SDK -std=gnu++11 -g
LDIR		= ../Arducam_SDK

ArduCam_Global_Shutter_Demo: ArduCam_Global_Shutter_Demo.cpp  $(LDIR)/ArduCamLib.h
	$(CC) $^ -o $@ $(LIBS) $(FLAGS) 

.PHONY:clean

clean:
	rm -f  ArduCam_Global_Shutter_Demo

.PHONY:install-sdk

install-sdk:
	sudo cp -d $(LDIR)/lib*.so* /usr/lib

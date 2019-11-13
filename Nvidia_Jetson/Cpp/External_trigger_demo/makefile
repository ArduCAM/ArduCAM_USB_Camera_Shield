CC			= g++

LIBS		= -lArduCamLib -lusb-1.0 -larducam_config_parser -lpthread `pkg-config --cflags --libs opencv`
FLAGS		= -L. -I. -std=gnu++11 -g
LDIR		= ./Arducam_SDK

ArduCam_Ext_Trigger_Demo: ArduCam_Ext_Trigger_Demo.cpp  $(LDIR)/ArduCamLib.h
	$(CC) $^ -o $@ $(LIBS) $(FLAGS) 

.PHONY:clean

clean:
	rm -f  ArduCam_Ext_Trigger_Demo

.PHONY:install-sdk

install-sdk:
	sudo cp -d $(LDIR)/lib*.so* /usr/lib

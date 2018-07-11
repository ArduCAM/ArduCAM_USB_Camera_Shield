//Arducam SDK multi-thread example for MT9N001 color camera
//This demo is only demonstrate how to use the Arducam SDK functions, 
//Tested under Pine64 ubuntuMATE16.04 OS Opencv 2.4.13
//Compile with command: g++ MT9N001_demo.cpp -o Test `pkg-config --cflags --libs opencv` -lArduCamLib -lusb-1.0  -lpthread -L. -I. -std=gnu++11
//Copy libArduCamLib.so libusb-1.0.so to /lib folder
//Running the camera with command: sudo ./Test

#include <opencv2/opencv.hpp>
#include "ArduCamLib.h"
#include <iostream>
#include <unistd.h>  
#include <pthread.h>
#include <thread>
#include <termios.h>

using namespace std;
using namespace cv;

ArduCamHandle useHandle;
ArduCamCfg useCfg;
Uint32 pUsbIdxArray[10];

pthread_t ntid1;
pthread_t ntid2;

int running = 0;
char key = 0;
int save_image = 0;

Mat BytestoMat(Uint8* bytes,int width,int height)
{
    Mat image = Mat(height,width,CV_8UC1,bytes);
    return image;
}

//Capture Thread 
void *fnCapture_Thread(void *para)
{ 

    //1. Call beginCapture to start a capture task
    ArduCam_beginCapture(useHandle);
    while(running)
    {
	//2. Continue with capture task
	ArduCam_capture(useHandle);
	//cout << "                         capture thread" << endl;
    }
    //3. Call endCapture to stop and delete the capture task
    ArduCam_endCapture(useHandle );

    pthread_exit(NULL);  
}

//Display Thread 
//Add your image processing here
void *fnDisplay_Thread(void *para)
{
    Uint8* pu8FrameData;
    Mat raw_image;
    Mat rgb_image;
    Mat resize_image;
    char file_name[64];
    int count = 0;
    while(running)
    {
	if ( ArduCam_available(useHandle) > 0)
	{
	        cout << "frame available\n";
	        //5. Read a frame
	        ArduCam_read(useHandle, pu8FrameData );
	        
	        //6. Display the frame
	        raw_image = BytestoMat(pu8FrameData, 3488, 2616);
	        if ( !raw_image.data )
		{
			cout << "No image data \n";
			//return -1;
		}
		//cout << image.data[100] << image.data[101] << "\n";
		namedWindow("Display Image", WINDOW_AUTOSIZE );
		
		//Select one of the following format to convert
		//CV_BayerBG2RGB =CV_BayerRG2BGR,  
		//CV_BayerGB2RGB =CV_BayerGR2BGR,  
		//CV_BayerRG2RGB =CV_BayerBG2BGR,  
		//CV_BayerGR2RGB =CV_BayerGB2BGR, 

		cvtColor(raw_image, rgb_image, CV_BayerBG2BGR);
		resize(rgb_image, resize_image, cv::Size(640, 480), (0,0), (0,0), cv::INTER_NEAREST);
		imshow("Display Image", resize_image);
		cvWaitKey(200);

		if(save_image)
		{
		    sprintf(file_name, "image_%d.jpg", count++);
		    imwrite(file_name, rgb_image);
		    save_image = 0;
		}

	        //7. Delete the old frame
	        ArduCam_del(useHandle);
	        cout << "Delete frame \n";
	}
    	//cout << "display thread" << endl;
    //usleep(100);
    }
    pthread_exit(NULL);  
}


int main(int argc, char** argv )
{
    unsigned int u32Value = 0xFFFFFFFF;
    bool open_flag = 0;
    int number = 0;
    int i ;
    int ret;	  
    unsigned char VRCMD_Val[3] = { 0x03, 0x04, 0x0C};
    unsigned int size;
    static struct termios oldt, newt;

    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON);          

    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	  
    //1. Scan the camera for index number for manual open API
    number = ArduCam_scan( pUsbIdxArray, 0x52cb );
    cout << "Device found = " << number << " \n";
	  
    useCfg.u16Vid = 0x52cb;
    useCfg.u32Width = 3488;
    useCfg.u32Height = 2616;
    useCfg.u8PixelBytes = 1;
    useCfg.u8PixelBits = 8;
    useCfg.emI2cMode = I2C_MODE_16_16;
    useCfg.u32SensorShipAddr = 0x20;
    
    //2. Open the camera to get the handle using auto open API
    u32Value = ArduCam_autoopen( useHandle, &useCfg );
    //u32Value = ArduCam_open( useHandle, &useCfg, 0 );

    //3. Download the sensor register settings
    ArduCam_writeReg_8_8( useHandle, 0x46, 1, 0x05);
    ArduCam_writeReg_8_8( useHandle, 0x46, 2, 0x00);
    ArduCam_writeReg_8_8( useHandle, 0x46, 3, 0x00); 
    ArduCam_VRCMD(useHandle, 0xF6, 0, 0, 0, 3, VRCMD_Val, &size);  
    // [MT9N001 (MI-9130) Register Wizard Defaults]
    ArduCam_writeSensorReg( useHandle,0x0100, 0x0); 			// Mode Select = 0x0
    ArduCam_writeSensorReg( useHandle,0x0300, 0x4); 			// vt_pix_clk_div = 0x4
    ArduCam_writeSensorReg( useHandle,0x0302, 0x01); 		// vt_sys_clk_div = 0x1
    ArduCam_writeSensorReg( useHandle,0x0304, 0x07); 		// pre_pll_clk_div = 0x2
    ArduCam_writeSensorReg( useHandle,0x0306, 0x40); 		// pll_multiplier = 0x40
    ArduCam_writeSensorReg( useHandle,0x0308, 0x08); 		// op_pix_clk_div = 0x8
    ArduCam_writeSensorReg( useHandle,0x030A, 0x01); 		// op_sys_clk_div = 0x1
    sleep(1);							// DELAY = 1   // Allow PLL to lock
    ArduCam_writeSensorReg( useHandle,0x3064, 0x805); 		// RESERVED_MFR_3064 = 0x805
    ArduCam_writeSensorReg( useHandle,0x0104, 0x1); 			// Grouped Parameter Hold = 0x1
    ArduCam_writeSensorReg( useHandle,0x3016, 0x111); 		// Row Speed = 0x111

    // LOAD= Frame Rate Improvement

    ArduCam_writeSensorReg( useHandle,0x0344, 0x008); 		// Column Start = 0x8
    ArduCam_writeSensorReg( useHandle,0x0348, 0xDA7); 		// Column End = 0xDA7
    ArduCam_writeSensorReg( useHandle,0x0346, 0x008); 		// Row Start = 0x8
    ArduCam_writeSensorReg( useHandle,0x034A, 0xA3F); 		// Row End = 0xA3F
    ArduCam_writeSensorReg( useHandle,0x3040, 0x0041); 		// Read Mode = 0x41
    ArduCam_writeSensorReg( useHandle,0x0400, 0x0); 			// Scaling Mode = 0x0
    ArduCam_writeSensorReg( useHandle,0x0404, 0x10); 		// Scale_M = 0x10
    ArduCam_writeSensorReg( useHandle,0x034C, 0xDA0); 		// Output Width = 0xDA0
    ArduCam_writeSensorReg( useHandle,0x034E, 0xA38); 		// Output Height = 0xA38
    ArduCam_writeSensorReg( useHandle,0x0342, 0x202B); 		// Line Length = 0x202B
    ArduCam_writeSensorReg( useHandle,0x0340, 0x0AC7); 		// Frame Lines = 0xAC7
    //    ArduCam_writeSensorReg( useHandle,0x0202, 0x0000); 		// Coarse Integration Time (OPTIONAL) = 0x0
    ArduCam_writeSensorReg( useHandle,0x3014, 0x056A); 		// Fine Integration Time = 0x56A
    ArduCam_writeSensorReg( useHandle,0x3010, 0x0100); 		// Fine Correction = 0x100
    ArduCam_writeSensorReg( useHandle,0x3018, 0x0000); 		// Extra Delay = 0x0
    ArduCam_writeSensorReg( useHandle,0x30D4, 0x1080); 		// Cols Dbl Sampling = 0x1080
    ArduCam_writeSensorReg( useHandle,0x0104, 0x0); 			// Grouped Parameter Hold = 0x0
    ArduCam_writeSensorReg( useHandle,0x0100, 0x1); 			// Mode Select = 0x1

    ArduCam_writeSensorReg( useHandle,0x0306, 0x0040); 

    ArduCam_writeSensorReg( useHandle,0x3012, 500); 
    ArduCam_writeSensorReg( useHandle,0x301A, 0x5CCC); 
    ArduCam_writeSensorReg( useHandle,0x0206, 33); 
    ArduCam_writeSensorReg( useHandle,0x0208, 50); 
    ArduCam_writeSensorReg( useHandle,0x020a, 50); 
    ArduCam_writeSensorReg( useHandle,0x020c, 33); 

    //4. Create multi-threads for capture and processing
    if (u32Value == 0)
    {
        open_flag = 1;
        cout << "ArduCam_open successful" << endl ;
	running = 1;
    	ret = pthread_create(&ntid1,NULL,fnCapture_Thread,NULL);
	if(ret != 0)
	{
	  cout << "can't create capture thread:" << ret << endl;
	  return -1;
	}
	else
	  cout << "create capture thread successfully" << endl;

    	ret = pthread_create(&ntid2,NULL,fnDisplay_Thread,NULL);
	if(ret != 0)
	{
	  cout << "can't create display thread:" << ret << endl;
	  return -1;
	}
	else
	  cout << "create display thread successfully" << endl;
    }
    else
    {
        open_flag = 0;
        cout << "ArduCam_open failed" << endl;
	return -1;
    }


    //5. Waiting for user command
    while(open_flag == 1)
    {
	//cout << "main thread " << endl;
	usleep(1000);    
	cin >> key;
	if(key == 'q')
	{
	    running = 0;
	    break;
	}
	else if(key == 's')
	    save_image = 1;
    }

    cout << "ArduCam closed" << endl;
    //6. Close the camera handle
    ArduCam_close(useHandle);
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
      
}




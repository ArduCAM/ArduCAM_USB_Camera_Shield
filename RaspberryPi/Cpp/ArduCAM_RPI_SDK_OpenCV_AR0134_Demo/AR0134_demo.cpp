//Arducam SDK multi-thread example for AR0134 camera
//This demo is only demonstrate how to use the Arducam SDK functions, 
//Tested under Raspberry Pi3 Raspbian OS Opencv 2.4.13
//Compile with command: g++ AR0134_demo.cpp -o Test `pkg-config --cflags --libs opencv` -lArduCamLib -lusb-1.0  -lpthread -L. -I. -std=gnu++11
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
int write_image = 0;

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
	        raw_image = BytestoMat(pu8FrameData, 1280, 964);
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

		cvtColor(raw_image, rgb_image, CV_BayerGR2RGB);
		resize(rgb_image, resize_image, cv::Size(640, 480), (0,0), (0,0), cv::INTER_NEAREST);
		imshow("Display Image", resize_image);
		cvWaitKey(80);
		
		if(write_image)
		{
		    sprintf(file_name, "image_%d.jpg", count++);
		    imwrite(file_name, rgb_image);
		    write_image = 0;
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

    static struct termios oldt, newt;

    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON);          

    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	  
    //1. Scan the camera for index number for manual open API
    number = ArduCam_scan( pUsbIdxArray, 0x52cb );
    cout << "Device found = " << number << " \n";
	  
	  
    useCfg.u16Vid = 0x52cb;
    useCfg.u32Width = 1280;
    useCfg.u32Height = 964;
    useCfg.u8PixelBytes = 1;
    useCfg.u8PixelBits = 8;
    useCfg.emI2cMode = I2C_MODE_16_16;
    useCfg.u32SensorShipAddr = 0x20;
    
    //2. Open the camera to get the handle using auto open API
    u32Value = ArduCam_autoopen( useHandle, &useCfg );
    //u32Value = ArduCam_open( useHandle, &useCfg, 0 );

    //3. Download the sensor register settings
    ArduCam_writeReg_8_8( useHandle, 0x46, 1, 0x05);
    //Change camera exposure value
    ArduCam_writeSensorReg( useHandle, 0x0B,  0x200 );
    ArduCam_writeSensorReg( useHandle,0x3028, 0x0010); 		// ROW_SPEED = 16

    ArduCam_writeSensorReg( useHandle,0x302E, 0x0002); 		// PRE_PLL_CLK_DIV = 2 (N)
    ArduCam_writeSensorReg( useHandle,0x3030, 0x002C); 		// PLL_MULTIPLIER = 44 (M)
    ArduCam_writeSensorReg( useHandle,0x302C, 0x0001); 		// VT_SYS_CLK_DIV = 1  (P1)
    ArduCam_writeSensorReg( useHandle,0x302A, 0x0010); 		// VT_PIX_CLK_DIV = 16 (P2)

    ArduCam_writeSensorReg( useHandle,0x3032, 0x0000); 		// DIGITAL_BINNING = 0
    ArduCam_writeSensorReg( useHandle,0x30B0, 0x0080); 		// DIGITAL_TEST = 128
    
    // [Timing_settings]
    ArduCam_writeSensorReg( useHandle,0x301A, 0x10DC);  		// RESET_REGISTER = 4312
    ArduCam_writeSensorReg( useHandle,0x301A, 0x1980);  		// R0x301A[12] = 1, R0x301A[11] = 1, R0x301A[8] = 1, R0x301A[7] = 1, R0x301A[2] = 0
    ArduCam_writeSensorReg( useHandle,0x300C, 0x0672);  		// LINE_LENGTH_PCK = 1650

    ArduCam_writeSensorReg( useHandle,0x3002, 0x0000);  		// Y_ADDR_START = 0
    ArduCam_writeSensorReg( useHandle,0x3004, 0x0000);  		// X_ADDR_START = 0
    ArduCam_writeSensorReg( useHandle,0x3006, 0x03BF);  		// Y_ADDR_END = 959
    ArduCam_writeSensorReg( useHandle,0x3008, 0x04FF);  		// X_ADDR_END = 1279
    ArduCam_writeSensorReg( useHandle,0x300A, 0x03DE);  		// FRAME_LENGTH_LINES = 990
    ArduCam_writeSensorReg( useHandle,0x3012, 0x0064);  		// COARSE_INTEGRATION_TIME = 100
    ArduCam_writeSensorReg( useHandle,0x3014, 0x00C0);  		// FINE_INTEGRATION_TIME = 192
    ArduCam_writeSensorReg( useHandle,0x30A6, 0x0001);  		// Y_ODD_INC = 1

    ArduCam_writeSensorReg( useHandle,0x308C, 0x0000);  		// Y_ADDR_START_CB = 0
    ArduCam_writeSensorReg( useHandle,0x308A, 0x0000);  		// X_ADDR_START_CB = 0
    ArduCam_writeSensorReg( useHandle,0x3090, 0x03BF);  		// Y_ADDR_END_CB = 959
    ArduCam_writeSensorReg( useHandle,0x308E, 0x04FF);  		// X_ADDR_END_CB = 1279
    ArduCam_writeSensorReg( useHandle,0x30AA, 0x03DE);  		// FRAME_LENGTH_LINES_CB = 990
    ArduCam_writeSensorReg( useHandle,0x3016, 0x0064);  		// COARSE_INTEGRATION_TIME_CB = 100
    ArduCam_writeSensorReg( useHandle,0x3018, 0x00C0);  		// FINE_INTEGRATION_TIME_CB = 192
    ArduCam_writeSensorReg( useHandle,0x30A8, 0x0001);  		// Y_ODD_INC_CB = 1

    ArduCam_writeSensorReg( useHandle,0x3040, 0x0000);  		// READ_MODE = 0
    ArduCam_writeSensorReg( useHandle,0x3064, 0x1982);  		// EMBEDDED_DATA_CTRL = 6530
    ArduCam_writeSensorReg( useHandle,0x31C6, 0x8000);  		// HISPI_CONTROL_STATUS = 32768 (DEFAULT)

    //ArduCam_writeSensorReg( useHandle,0x3002, 0  ); 			// Y_ADDR_START
    ArduCam_writeSensorReg( useHandle,0x3012, 800);			// Exposure 
    ArduCam_writeSensorReg( useHandle,0x3056, 90);
    ArduCam_writeSensorReg( useHandle,0x3058, 112);
    ArduCam_writeSensorReg( useHandle,0x305a, 112);
    ArduCam_writeSensorReg( useHandle,0x305c, 90);

    ArduCam_writeSensorReg( useHandle,0x3046, 0x0100);  		// en_flash/Flash indicator

    ArduCam_writeSensorReg( useHandle,0x301a, 0x10DC);

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
	    write_image = 1;
    }

    cout << "ArduCam closed" << endl;
    //6. Close the camera handle
    ArduCam_close(useHandle);
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
      
}




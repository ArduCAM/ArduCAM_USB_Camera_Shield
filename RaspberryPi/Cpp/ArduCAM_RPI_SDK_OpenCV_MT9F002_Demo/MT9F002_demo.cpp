			//return -1;
			cout << "No image data \n";
		
		    imwrite(file_name, rgb_image);
		    save_image = 0;
		    sprintf(file_name, "image_%d.jpg", count++);
		//CV_BayerBG2RGB =CV_BayerRG2BGR,  
		//CV_BayerGB2RGB =CV_BayerGR2BGR,  
		//CV_BayerGR2RGB =CV_BayerGB2BGR, 
		//CV_BayerRG2RGB =CV_BayerBG2BGR,  
		//Select one of the following format to convert
		//cout << image.data[100] << image.data[101] << "\n";
		cvWaitKey(500);
		cvtColor(raw_image, rgb_image, CV_BayerBG2BGR);
		if(save_image)
		imshow("Display Image", resize_image);
		namedWindow("Display Image", WINDOW_AUTOSIZE );
		resize(rgb_image, resize_image, cv::Size(640, 480), (0,0), (0,0), cv::INTER_NEAREST);
		{
		{
		}
		}
	  
	        
	        //5. Read a frame
	        //6. Display the frame
	        //7. Delete the old frame
	        ArduCam_del(useHandle);
	        ArduCam_read(useHandle, pu8FrameData );
	        cout << "Delete frame \n";
	        cout << "frame available\n";
	        if ( !raw_image.data )
	        raw_image = BytestoMat(pu8FrameData, 4384, 3288);
	    break;
	    running = 0;
	    save_image = 1;
	  cout << "can't create capture thread:" << ret << endl;
	  cout << "can't create display thread:" << ret << endl;
	  cout << "create capture thread successfully" << endl;
	  cout << "create display thread successfully" << endl;
	  return -1;
	  return -1;
	//2. Continue with capture task
	//cout << "                         capture thread" << endl;
	//cout << "main thread " << endl;
	ArduCam_capture(useHandle);
	cin >> key;
	else
	else
	else if(key == 's')
	if ( ArduCam_available(useHandle) > 0)
	if(key == 'q')
	if(ret != 0)
	if(ret != 0)
	return -1;
	running = 1;
	usleep(1000);    
	{
	{
	{
	{
	}
	}
	}
	}

































    	//cout << "display thread" << endl;
    	ret = pthread_create(&ntid1,NULL,fnCapture_Thread,NULL);
    	ret = pthread_create(&ntid2,NULL,fnDisplay_Thread,NULL);
    
      
        cout << "ArduCam_open failed" << endl;
        cout << "ArduCam_open successful" << endl ;
        open_flag = 0;
        open_flag = 1;
    //1. Call beginCapture to start a capture task
    //1. Scan the camera for index number for manual open API
    //2. Open the camera to get the handle using auto open API
    //3. Call endCapture to stop and delete the capture task
    //3. Download the sensor register settings
    //4. Create multi-threads for capture and processing
    //5. Waiting for user command
    //6. Close the camera handle
    //Change camera exposure value
    //DELAY = 300
    //sleep(1);
    //u32Value = ArduCam_open( useHandle, &useCfg, 0 );
    //usleep(100);
    ArduCam_VRCMD(useHandle, 0xF6, 0, 0, 0, 3, VRCMD_Val, &size);  
    ArduCam_beginCapture(useHandle);
    ArduCam_close(useHandle);
    ArduCam_endCapture(useHandle );
    ArduCam_writeReg_8_8( useHandle, 0x46, 1, 0x05);
    ArduCam_writeReg_8_8( useHandle, 0x46, 2, 0x00);
    ArduCam_writeReg_8_8( useHandle, 0x46, 3, 0x00); 
    ArduCam_writeSensorReg( useHandle,0x0100, 0x0	 );	  //Mode Select = 0x0
    ArduCam_writeSensorReg( useHandle,0x0100, 0x1 	);	//Mode Select = 0x1
    ArduCam_writeSensorReg( useHandle,0x0103, 0x1);	   //Software Reset = 0x1
    ArduCam_writeSensorReg( useHandle,0x0104, 0x0 	);	//Grouped Parameter Hold = 0x0
    ArduCam_writeSensorReg( useHandle,0x0104, 0x1	 );	  //Grouped Parameter Hold = 0x1
    ArduCam_writeSensorReg( useHandle,0x0112, 0x0C0C );	  	//CCP Data Format = 0xC0C
    ArduCam_writeSensorReg( useHandle,0x0206, 33 );
    ArduCam_writeSensorReg( useHandle,0x0208, 50 );
    ArduCam_writeSensorReg( useHandle,0x020a, 50 );
    ArduCam_writeSensorReg( useHandle,0x020c, 33 );
    ArduCam_writeSensorReg( useHandle,0x0300, 0x4	 );	  //vt_pix_clk_div = 0x4
    ArduCam_writeSensorReg( useHandle,0x0302, 0x01	  );	 //vt_sys_clk_div = 0x1
    ArduCam_writeSensorReg( useHandle,0x0304, 0x03	 );	  //pre_pll_clk_div = 0x3
    ArduCam_writeSensorReg( useHandle,0x0304, 8 );
    ArduCam_writeSensorReg( useHandle,0x0306, 0x6E	  );	 //pll_multiplier = 0x6E
    ArduCam_writeSensorReg( useHandle,0x0308, 0x0C	  );	 //op_pix_clk_div = 0xC
    ArduCam_writeSensorReg( useHandle,0x030A, 0x01	  );	 //op_sys_clk_div = 0x1
    ArduCam_writeSensorReg( useHandle,0x0340, 0x0D67  );	 	//Frame Lines = 0xD67
    ArduCam_writeSensorReg( useHandle,0x0342, 0x33C1  );	 	//Line Length = 0x33C1
    ArduCam_writeSensorReg( useHandle,0x0344, 0x090	  );	 //Column Start = 0x90
    ArduCam_writeSensorReg( useHandle,0x0346, 0x020	  );	 //Row Start = 0x20
    ArduCam_writeSensorReg( useHandle,0x0348, 0x11AF  );	 	//Column End = 0x11AF
    ArduCam_writeSensorReg( useHandle,0x034A, 0xCF7	  );	 //Row End = 0xCF7
    ArduCam_writeSensorReg( useHandle,0x034C, 0x1120  );	 	//Output Width = 0x1120
    ArduCam_writeSensorReg( useHandle,0x034E, 0xCD8	  );	 //Output Height = 0xCD8
    ArduCam_writeSensorReg( useHandle,0x0400, 0x0	  );	 //Scaling Mode = 0x0
    ArduCam_writeSensorReg( useHandle,0x0404, 0x10	  );	 //Scale_M = 0x10
    ArduCam_writeSensorReg( useHandle,0x3010, 0x00CF  );	 	//Fine Correction = 0xCF
    ArduCam_writeSensorReg( useHandle,0x3012, 0x0010  );	 	//Coarse Integration Time = 0x10
    ArduCam_writeSensorReg( useHandle,0x3012, 0x0300 );
    ArduCam_writeSensorReg( useHandle,0x3014, 0x1F50  );	 	//Fine Integration Time = 0x1F50
    ArduCam_writeSensorReg( useHandle,0x3016, 0x111	  );	 //Row Speed = 0x111
    ArduCam_writeSensorReg( useHandle,0x3018, 0x0000  );	 	//Extra Delay = 0x0
    ArduCam_writeSensorReg( useHandle,0x301A, 0x0010 );	  	//Reset Register = 0x10
    ArduCam_writeSensorReg( useHandle,0x301A, 0x0018);
    ArduCam_writeSensorReg( useHandle,0x301A, 0x10DC );	  	//Reset Register = 0x10DC
    ArduCam_writeSensorReg( useHandle,0x301E, 0x00A8 );	  	//Data Pedestal = 0xA8
    ArduCam_writeSensorReg( useHandle,0x301a, 0x5ccc  );	  //reset_register
    ArduCam_writeSensorReg( useHandle,0x3040, 0x0041  );	 	//Read Mode = 0x41
    ArduCam_writeSensorReg( useHandle,0x3064, 0x0805 );	  	//SMIA Test = 0x805
    ArduCam_writeSensorReg( useHandle,0x30E8, 0x8001  );	//Power Mode Context 1
    ArduCam_writeSensorReg( useHandle,0x31AE, 0x301	);	   //Serial Format = 0x301
    Mat image = Mat(height,width,CV_8UC1,bytes);
    Mat raw_image;
    Mat resize_image;
    Mat rgb_image;
    Uint8* pu8FrameData;
    bool open_flag = 0;
    char file_name[64];
    cout << "ArduCam closed" << endl;
    cout << "Device found = " << number << " \n";
    else
    if (u32Value == 0)
    int count = 0;
    int i ;
    int number = 0;
    int ret;	  
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    number = ArduCam_scan( pUsbIdxArray, 0x52cb );
    pthread_exit(NULL);  
    pthread_exit(NULL);  
    return image;
    static struct termios oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    u32Value = ArduCam_autoopen( useHandle, &useCfg );
    unsigned char VRCMD_Val[3] = { 0x03, 0x04, 0x0C};
    unsigned int size;	  
    unsigned int u32Value = 0xFFFFFFFF;
    useCfg.emI2cMode = I2C_MODE_16_16;
    useCfg.u16Vid = 0x52cb;
    useCfg.u32Height = 3288;
    useCfg.u32SensorShipAddr = 0x20;
    useCfg.u32Width = 4384;
    useCfg.u8PixelBits = 8;
    useCfg.u8PixelBytes = 1;
    while(open_flag == 1)
    while(running)
    while(running)
    {
    {
    {
    {
    {
    }
    }
    }
    }
    }
#include "ArduCamLib.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <termios.h>
#include <thread>
#include <unistd.h>  
//Add your image processing here
//Arducam SDK multi-thread example for MT9F002 color camera
//Capture Thread 
//Compile with command: g++ MT9F002_demo.cpp -o Test `pkg-config --cflags --libs opencv` -lArduCamLib -lusb-1.0  -lpthread -L. -I. -std=gnu++11
//Copy libArduCamLib.so libusb-1.0.so to /lib folder
//Display Thread 
//Running the camera with command: sudo ./Test
//Tested under Raspberry Pi3 Raspbian OS Opencv 2.4.13
ArduCamCfg useCfg;
ArduCamHandle useHandle;
Mat BytestoMat(Uint8* bytes,int width,int height)
Uint32 pUsbIdxArray[10];
char key = 0;
int main(int argc, char** argv )
int running = 0;
int save_image = 0;
pthread_t ntid1;
pthread_t ntid2;
using namespace cv;
using namespace std;
void *fnCapture_Thread(void *para)
void *fnDisplay_Thread(void *para)
{
{
{
{ 
}
}
}
}

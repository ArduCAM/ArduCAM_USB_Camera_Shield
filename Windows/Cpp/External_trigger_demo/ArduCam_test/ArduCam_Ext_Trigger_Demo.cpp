// ArduCam_test.cpp : Defines the entry point for the console application.
//
#ifdef linux
#include "Arducam_SDK/ArduCamLib.h"
#include <unistd.h>
#include <termios.h>
#endif

#ifdef _WIN32
#include "stdafx.h"
#include <Windows.h>
#include "ArduCamlib.h"
#include <io.h>
#include <direct.h> 
#endif
#include <opencv2/opencv.hpp>
#include <thread>
#include <time.h>
#include <iostream>
#include <istream>
#include <string>

#include <sys/types.h> 
#include <sys/stat.h> 

#include <signal.h>

#include "arducam_config_parser.h"
//#define USE_SOFT_TRIGGER

using namespace std;
using namespace cv;

ArduCamCfg cameraCfg;
volatile bool _running = true;
bool save_raw = false;
bool save_flag = true;

int color_mode = 0;

void showHelp(){
	printf(" usage: sudo ./ArduCam_Ext_Trigger_Demo <path/config-file-name>	\
			\n\n example: sudo ./ArduCam_Ext_Trigger_Demo ./../../cpp_config/AR0134_960p_Color.yml	\
			\n\n");
}


cv::Mat JPGToMat(Uint8* bytes,int length){
	cv::Mat image = Mat(1,length,CV_8UC1,bytes);
	if(length <= 0){
		image.data = NULL;
	    return image;
	}

	image = imdecode(image,IMREAD_ANYCOLOR);
	return image;
}
cv::Mat YUV422toMat(Uint8* bytes, int width, int height)
{
	cv::Mat image = cv::Mat(height, width, CV_8UC2, bytes);
	cv::cvtColor(image,image,cv::COLOR_YUV2BGR_YUYV);
	return image;
}

cv::Mat separationImage(Uint8* bytes, int width, int height)
{
	int width_d = width << 1;
	unsigned char* temp1,*temp2;
	temp1 = (unsigned char*)malloc(width);
	temp2 = (unsigned char*)malloc(width);
	
	for(int k = 0 ; k < height ;k++){
		for(int i = 0 ,j =0; i < width_d ;i+=2){
			temp1[j] = bytes[i + (k * width_d)];
			temp2[j++] = bytes[i + 1 + (k * width_d)];
		}
		memcpy(bytes + (k * width_d),temp1,width);
		memcpy(bytes + (k * width_d + width),temp2,width);
	}
	cv::Mat image = cv::Mat( height, width_d,CV_8UC1,bytes);
	free(temp1);
	free(temp2);
	return image;
}


#define RGB565_RED		0xf800
#define RGB565_GREEN	0x07e0
#define RGB565_BLUE		0x001f
cv::Mat RGB565toMat(Uint8* bytes, int width, int height) {
	unsigned char* temp_data, *ptdata, *data, *data_end;

	data = bytes;
	data_end = bytes + (width * height * 2);

	temp_data = (unsigned char*)malloc(cameraCfg.u32Width * cameraCfg.u32Height * 3);
	ptdata = temp_data;

	Uint8 r, g, b;
	while (data < data_end) {
		unsigned short temp;

		temp = (*data << 8) | *(data + 1);
		r = (temp & RGB565_RED	)	>> 8;
		g = (temp & RGB565_GREEN)	>> 3;
		b = (temp & RGB565_BLUE	)	<< 3;

		switch(color_mode){
		case 1:
			*ptdata++ = r;
			*ptdata++ = g;
			*ptdata++ = b;
			break;
		case 0:
		default:
			*ptdata++ = b;
			*ptdata++ = g;
			*ptdata++ = r;
			break;
		}
		data += 2;
	}

	cv::Mat image = cv::Mat(height, width, CV_8UC3);
	memcpy(image.data, temp_data, cameraCfg.u32Height * cameraCfg.u32Width * 3);
	cv::flip(image, image, 0);
	free(temp_data);
	return image;
}
cv::Mat dBytesToMat(Uint8* bytes,int bit_width,int width,int height){
    unsigned char* temp_data = (unsigned char*)malloc(width * height);
    int index = 0;
    for(int i = 0 ; i < width * height * 2 ;i+=2){
        unsigned char temp = ((bytes[i + 1] << 8 | bytes[i]) >> (bit_width - 8)) & 0xFF;
        temp_data[index++] = temp;
    }
    cv::Mat image = cv::Mat(height, width, CV_8UC1);
	memcpy(image.data, temp_data, cameraCfg.u32Height * cameraCfg.u32Width);
    free(temp_data);
    return image;
}

cv::Mat BytestoMat(Uint8* bytes, int width, int height)
{
	cv::Mat image = cv::Mat(height, width, CV_8UC1, bytes);
	return image;
}

void configBoard(ArduCamHandle &cameraHandle, Config config) {
	uint8_t u8Buf[10];
	for (int n = 0; n < config.params[3]; n++) {
		u8Buf[n] = config.params[4 + n];
	}
	ArduCam_setboardConfig(cameraHandle, config.params[0], config.params[1],
		config.params[2], config.params[3], u8Buf);
}

/**
 * read config file and open the camera.
 * @param filename : path/config_file_name
 * @param cameraHandle : camera handle
 * @param cameraCfg :camera config struct
 * @return TURE or FALSE
 * */
bool camera_initFromFile(std::string filename, ArduCamHandle &cameraHandle, ArduCamCfg &cameraCfg, int index) {
	CameraConfigs cam_cfgs;
	memset(&cam_cfgs, 0x00, sizeof(CameraConfigs));
	if (arducam_parse_config(filename.c_str(), &cam_cfgs)) {
		std::cout << "Cannot find configuration file." << std::endl << std::endl;
		showHelp();
		return false;
	}
	CameraParam *cam_param = &cam_cfgs.camera_param;
	Config *configs = cam_cfgs.configs;
	int configs_length = cam_cfgs.configs_length;

	switch (cam_param->i2c_mode) {
	case 0: cameraCfg.emI2cMode = I2C_MODE_8_8; break;
	case 1: cameraCfg.emI2cMode = I2C_MODE_8_16; break;
	case 2: cameraCfg.emI2cMode = I2C_MODE_16_8; break;
	case 3: cameraCfg.emI2cMode = I2C_MODE_16_16; break;
	default: break;
	}

	color_mode = cam_param->format & 0xFF;
	switch (cam_param->format >> 8) {
	case 0: cameraCfg.emImageFmtMode = FORMAT_MODE_RAW; break;
	case 1: cameraCfg.emImageFmtMode = FORMAT_MODE_RGB; break;
	case 2: cameraCfg.emImageFmtMode = FORMAT_MODE_YUV; break;
	case 3: cameraCfg.emImageFmtMode = FORMAT_MODE_JPG; break;
	case 4: cameraCfg.emImageFmtMode = FORMAT_MODE_MON; break;
	case 5: cameraCfg.emImageFmtMode = FORMAT_MODE_RAW_D; break;
	case 6: cameraCfg.emImageFmtMode = FORMAT_MODE_MON_D; break;
	default: break;
	}

	cameraCfg.u32Width = cam_param->width;
	cameraCfg.u32Height = cam_param->height;

	cameraCfg.u32I2cAddr = cam_param->i2c_addr;
	cameraCfg.u8PixelBits = cam_param->bit_width;
	cameraCfg.u32TransLvl = cam_param->trans_lvl;

	if (cameraCfg.u8PixelBits <= 8) {
		cameraCfg.u8PixelBytes = 1;
	}
	else if (cameraCfg.u8PixelBits > 8 && cameraCfg.u8PixelBits <= 16) {
		cameraCfg.u8PixelBytes = 2;
		save_raw = true;
	}

	int ret_val = ArduCam_open(cameraHandle, &cameraCfg, index);
	if (ret_val == USB_CAMERA_NO_ERROR) {
		//ArduCam_enableForceRead(cameraHandle);	//Force display image
		Uint8 u8Buf[8];
		for (int i = 0; i < configs_length; i++) {
			uint32_t type = configs[i].type;
			if (((type >> 16) & 0xFF) && ((type >> 16) & 0xFF) != cameraCfg.usbType)
				continue;
			switch (type & 0xFFFF) {
			case CONFIG_TYPE_REG:
				ArduCam_writeSensorReg(cameraHandle, configs[i].params[0], configs[i].params[1]);
				break;
			case CONFIG_TYPE_DELAY:
#ifdef linux
				usleep(1000 * configs[i].params[0]);
#endif
#ifdef _WIN32
				Sleep(configs[i].params[0]);
#endif	
				break;
			case CONFIG_TYPE_VRCMD:
				configBoard(cameraHandle, configs[i]);
				break;
			}
		}
		unsigned char u8TmpData[16];
		ArduCam_readUserData(cameraHandle, 0x400 - 16, 16, u8TmpData);
		printf("Serial: %c%c%c%c-%c%c%c%c-%c%c%c%c\n",
			u8TmpData[0], u8TmpData[1], u8TmpData[2], u8TmpData[3],
			u8TmpData[4], u8TmpData[5], u8TmpData[6], u8TmpData[7],
			u8TmpData[8], u8TmpData[9], u8TmpData[10], u8TmpData[11]);
	}
	else {
		std::cout << "Cannot open camera.rtn_val = " << ret_val << std::endl;
		return false;
	}

	return true;
}


cv::Mat ConvertImage(ArduCamOutData* frameData){
	cv::Mat rawImage ;
	Uint8* data = frameData->pu8ImageData;
	int height,width;
	width = cameraCfg.u32Width;
	height = cameraCfg.u32Height;

	switch(cameraCfg.emImageFmtMode){
	case FORMAT_MODE_RGB:
		rawImage = RGB565toMat(data,width,height);
		break;
	case FORMAT_MODE_RAW_D:
		rawImage = separationImage(data,width,height);
		switch(color_mode){
		case RAW_RG:cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerRG2BGR);
			break;
		case RAW_GR:cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerGR2BGR);
			break;
		case RAW_GB:cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerGB2BGR);
			break;
		case RAW_BG:cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerBG2BGR);
			break;
		default:
			cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerRG2BGR);
			break;
		}
		break;
	case FORMAT_MODE_MON_D:
		rawImage = separationImage(data,width,height);
		break;
	case FORMAT_MODE_JPG:
		rawImage = JPGToMat(data,frameData->stImagePara.u32Size);
		break;
	case FORMAT_MODE_RAW:
    	if(cameraCfg.u8PixelBytes == 2){
	        rawImage = dBytesToMat(data,frameData->stImagePara.u8PixelBits,width,height);
	    }else{
		    rawImage = BytestoMat(data, width, height);
		}
		switch(color_mode){
		case RAW_RG:cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerRG2BGR);
			break;
		case RAW_GR:cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerGR2BGR);
			break;
		case RAW_GB:cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerGB2BGR);
			break;
		case RAW_BG:cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerBG2BGR);
			break;
		default:
			cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerRG2BGR);
			break;
		}
		break;
	case FORMAT_MODE_YUV:
		rawImage = YUV422toMat(data,width,height);
		break;
	case FORMAT_MODE_MON:
		if(cameraCfg.u8PixelBytes == 2){
	        rawImage = dBytesToMat(data,frameData->stImagePara.u8PixelBits,width,height);
	    }else{
		    rawImage = BytestoMat(data, width, height);
		}
		break;
	default:
		if(cameraCfg.u8PixelBytes == 2){
	        rawImage = dBytesToMat(data,frameData->stImagePara.u8PixelBits,width,height);
	    }else{
		    rawImage = BytestoMat(data, width, height);
		}
		cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerRG2RGB);
		break;
	}

	return rawImage;

}



long total_frames[16];
void getAndDisplaySingleFrame(ArduCamHandle handle,int index){
	
    printf("Take picture.\n");
	char name[50];
	sprintf(name,"ArduCam%d",index);
    cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
	
	ArduCamOutData* frameData;
	
	cv::Mat rawImage ;

	Uint32 rtn_val = ArduCam_getSingleFrame(handle, frameData);

	if ( rtn_val == USB_CAMERA_NO_ERROR) {
		rawImage = ConvertImage(frameData);
		if (!rawImage.data)
		{
			std::cout << "Convert image fail,No image data \n";
			return;
		}

		total_frames[index]++;
		if(save_flag){
			char save_path[50];

			sprintf(save_path,"images%d",index);
#ifdef linux
			if(access(save_path, F_OK) != 0){  
				if(mkdir(save_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)  
					printf("mkdir error!\n");    
			}
#endif
#ifdef _WIN32
			if (_access(save_path, 0) != 0)
			{
				if (_mkdir(save_path))
					printf("mkdir error!\n");
			}
#endif
			printf("Camera%d,save image%ld.jpg.\n",index,total_frames[index]);
			char imageName[50];
			sprintf(imageName,"images%d/image%ld.jpg",index,total_frames[index]);

			if(save_raw){
				char rawName[50];
				sprintf(rawName,"images%d/image%ld.raw",index,total_frames[index]);
				FILE *file = fopen(rawName,"w");
				fwrite(frameData->pu8ImageData,1,cameraCfg.u32Width * cameraCfg.u32Height,file);
				fclose(file);
			}

			cv::imwrite(imageName,rawImage);
		}
		
		cv::resize(rawImage,rawImage,cv::Size(640, 480), (0, 0), (0, 0), cv::INTER_LINEAR);
	    cv::imshow(name, rawImage);
		cvWaitKey(50);
	    // cv::waitKey(50);
		printf("End display.\n");
	}else{
		printf("Take picture fail,ret_val = %d\n",rtn_val);
	}

}

void signal_handle(int signal){
	if(SIGINT == signal){
		_running = false;
	}
#ifdef linux
    usleep(1000 * 500);
#elif _WIN32
	Sleep(500);
#endif
    exit(0);
}


int main(int argc,char **argv)
{
	//receive Ctrl + C signal
	signal(SIGINT,signal_handle);
#ifdef linux
	static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif
	//get config file name
	const char * config_file_name;
	if(argc > 1){
		config_file_name = argv[1];
	}else{
		showHelp();
		return 0;
	}


	ArduCamIndexinfo pUsbIdxArray[16];
	int camera_num = ArduCam_scan(pUsbIdxArray);

    printf("device num:%d\n",camera_num);
	char serial[16];
	unsigned char *u8TmpData;
    for(int i = 0; i < camera_num ;i++){
		u8TmpData = pUsbIdxArray[i].u8SerialNum;
		sprintf(serial,"%c%c%c%c-%c%c%c%c-%c%c%c%c",
			u8TmpData[0], u8TmpData[1], u8TmpData[2], u8TmpData[3],
			u8TmpData[4], u8TmpData[5], u8TmpData[6], u8TmpData[7],
			u8TmpData[8], u8TmpData[9], u8TmpData[10], u8TmpData[11]);
        printf("index:%4d\tSerial:%s\n",pUsbIdxArray[i].u8UsbIndex,serial);
    }
	
#ifdef linux
    sleep(2);
#endif
	
	printf("Found %d devices.\n",camera_num);
	ArduCamHandle cameraHandles[16];
	long sTriggerSendTime[16];

	for(int i = 0 ; i < camera_num ;i++){
		//read config file and open the camera.
		sTriggerSendTime[i] = 0;
		if (!camera_initFromFile(config_file_name, cameraHandles[i], cameraCfg,i)) 
			 cameraHandles[i] = NULL;
		else{
			Uint32 ret_val =  ArduCam_setMode(cameraHandles[i],EXTERNAL_TRIGGER_MODE);
            if(ret_val == USB_BOARD_FW_VERSION_NOT_SUPPORT_ERROR){
                printf("Usb board firmware version not support single mode.\n");
                return 1;
            }
		}
	}

	while(_running && camera_num > 0){
			
		for(int i = 0 ; i < camera_num ;i++){
			ArduCamHandle tempHandle = cameraHandles[i];
			if(tempHandle == NULL){
				continue;
			}
			Uint32 rtn_val = ArduCam_isFrameReady(tempHandle);
			// printf("-----%d\n",rtn_val);
			if(rtn_val == 1){

#ifdef USE_SOFT_TRIGGER
				sTriggerSendTime[i] = 0;
#endif
				getAndDisplaySingleFrame(tempHandle,i);
			}
#ifdef USE_SOFT_TRIGGER
			else if(time(NULL) - sTriggerSendTime[i] > 3){
				ArduCam_softTrigger(tempHandle);
				sTriggerSendTime[i] = time(NULL);
			}
#endif
		}
		// usleep( 1000 * 50);
		cvWaitKey(10);
	}
	
	cv::destroyAllWindows();
	
	for(int i = 0 ;i < camera_num ;i++){
		if(cameraHandles[i] != NULL){
			ArduCam_close(cameraHandles[i]);
		}
	}
	std::cout << std::endl << "Press ENTER to exit..." << std::endl;
	std::string str_key;
	std::getline(std::cin,str_key);
#ifdef linux
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
	return 0;
}



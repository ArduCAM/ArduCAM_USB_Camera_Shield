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
#include <conio.h>
#endif
#include <opencv2/opencv.hpp>
#include <thread>
#include <time.h>
#include <iostream>
#include <istream>
#include <string>
#include <sys/types.h> 
#include <sys/stat.h> 
#include "arducam_config_parser.h"

using namespace std;
using namespace cv;

ArduCamCfg cameraCfg;
volatile bool _running = true;
bool save_raw = false;
bool save_flag = false;
int color_mode = 0;
void showHelp() {
	printf(" usage: sudo ./ArduCam_Demo <path/config-file-name>	\
			\n\n example: sudo ./ArduCam_Demo ../../../cpp_config/AR0134_960p_Color.yml	\
			\n\n While the program is running, you can press the following buttons in the terminal:	\
			\n\n 's':Save the image to the images folder.	\
			\n\n 'c':Stop saving images.	\
			\n\n 'q':Stop running the program.	\
			\n\n");
}


cv::Mat JPGToMat(Uint8* bytes, int length) {
	cv::Mat image = Mat(1, length, CV_8UC1, bytes);
	if (length <= 0) {
		image.data = NULL;
		return image;
	}

	image = imdecode(image, IMREAD_ANYCOLOR);
	return image;
}
cv::Mat YUV422toMat(Uint8* bytes, int width, int height)
{
	cv::Mat image = cv::Mat(height, width, CV_8UC2, bytes);
	cv::cvtColor(image, image, cv::COLOR_YUV2BGR_YUYV);
	return image;
}

cv::Mat separationImage(Uint8* bytes, int width, int height)
{
	int width_d = width << 1;
	unsigned char* temp1, *temp2;
	temp1 = (unsigned char*)malloc(width);
	temp2 = (unsigned char*)malloc(width);

	for (int k = 0; k < height; k++) {
		for (int i = 0, j = 0; i < width_d; i += 2) {
			temp1[j] = bytes[i + (k * width_d)];
			temp2[j++] = bytes[i + 1 + (k * width_d)];
		}
		memcpy(bytes + (k * width_d), temp1, width);
		memcpy(bytes + (k * width_d + width), temp2, width);
	}
	cv::Mat image = cv::Mat(height, width_d, CV_8UC1, bytes);
	free(temp1);
	free(temp2);
	return image;
}
#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f
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
		r = (temp & RGB565_RED) >> 8;
		g = (temp & RGB565_GREEN) >> 3;
		b = (temp & RGB565_BLUE) << 3;

		switch (color_mode) {
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


cv::Mat dBytesToMat(Uint8* bytes, int bit_width, int width, int height) {
	unsigned char* temp_data = (unsigned char*)malloc(width * height);
	int index = 0;
	for (int i = 0; i < width * height * 2; i += 2) {
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

cv::Mat ConvertImage(ArduCamOutData* frameData) {
	cv::Mat rawImage;
	Uint8* data = frameData->pu8ImageData;
	int height, width;
	width = cameraCfg.u32Width;
	height = cameraCfg.u32Height;

	switch (cameraCfg.emImageFmtMode) {
	case FORMAT_MODE_RGB:
		rawImage = RGB565toMat(data, width, height);
		break;
	case FORMAT_MODE_RAW_D:
		rawImage = separationImage(data, width, height);
		switch (color_mode) {
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
		rawImage = separationImage(data, width, height);
		break;
	case FORMAT_MODE_JPG:
		rawImage = JPGToMat(data, frameData->stImagePara.u32Size);
		break;
	case FORMAT_MODE_RAW:
		if (cameraCfg.u8PixelBytes == 2) {
			rawImage = dBytesToMat(data, frameData->stImagePara.u8PixelBits, width, height);
		}
		else {
			rawImage = BytestoMat(data, width, height);
		}
		switch (color_mode) {
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
		rawImage = YUV422toMat(data, width, height);
		break;
	case FORMAT_MODE_MON:
		if (cameraCfg.u8PixelBytes == 2) {
			rawImage = dBytesToMat(data, frameData->stImagePara.u8PixelBits, width, height);
		}
		else {
			rawImage = BytestoMat(data, width, height);
		}
		break;
	default:
		if (cameraCfg.u8PixelBytes == 2) {
			rawImage = dBytesToMat(data, frameData->stImagePara.u8PixelBits, width, height);
		}
		else {
			rawImage = BytestoMat(data, width, height);
		}
		cv::cvtColor(rawImage, rawImage, cv::COLOR_BayerRG2RGB);
		break;
	}

	return rawImage;

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
bool camera_initFromFile(std::string filename, ArduCamHandle &cameraHandle, ArduCamCfg &cameraCfg) {
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

	//int ret_val = ArduCam_open(cameraHandle, &cameraCfg,0);
	int ret_val = ArduCam_autoopen(cameraHandle, &cameraCfg);
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
		ArduCam_registerCtrls(cameraHandle, cam_cfgs.controls, cam_cfgs.controls_length);
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
	ArduCam_setCtrl(cameraHandle, "setFramerate", 5);

	return true;
}

void captureImage_thread(ArduCamHandle handle) {
	Uint32 rtn_val = ArduCam_beginCaptureImage(handle);
	if (rtn_val == USB_CAMERA_USB_TASK_ERROR) {
		std::cout << "Error beginning capture, rtn_val = " << rtn_val << std::endl;
		return;
	}
	else {
		std::cout << "Capture began, rtn_val = " << rtn_val << std::endl;
	}

	while (_running) {
		rtn_val = ArduCam_captureImage(handle);
		if (rtn_val == USB_CAMERA_USB_TASK_ERROR) {
			std::cout << "Error capture image, rtn_val = " << rtn_val << std::endl;
			break;
		}
		else if (rtn_val > 0xFF) {
			std::cout << "Error capture image, rtn_val = " << rtn_val << std::endl;
		}
	}
	_running = false;
	ArduCam_endCaptureImage(handle);
	std::cout << "Capture thread stopped." << std::endl;
}

void saveImage(cv::Mat img, int frameCount) {
	if (cameraCfg.emImageFmtMode == FORMAT_MODE_RAW_D) {
		char name1[50], name2[50];
		printf("save a_snapshot%d.png, b_snapshot%d.png.\n", frameCount, frameCount);
		sprintf(name1, "images/a_snapshot%d.jpg", frameCount);
		sprintf(name2, "images/b_snapshot%d.jpg", frameCount);
		Rect rect(0, 0, cameraCfg.u32Width, cameraCfg.u32Height);
		Rect rect2(cameraCfg.u32Width, 0, cameraCfg.u32Width, cameraCfg.u32Height);
		Mat img1 = img(rect);
		Mat img2 = img(rect2);
		cv::imwrite(name1, img1);
		cv::imwrite(name2, img2);
		return;
	}
	char imageName[50];
	printf("save snapshot%d.jpg.\n", frameCount);
	sprintf(imageName, "images/snapshot%d.jpg", frameCount);
	cv::imwrite(imageName, img);
}

void saveRaw(Uint8 *data, Uint32 size, int frameCount) {
	char imageName[50];
	printf("save snapshot%d.raw.\n", frameCount);
	sprintf(imageName, "images/snapshot%d.raw", frameCount);
	FILE *file = fopen(imageName, "wb");
	fwrite(data, 1, size, file);
	fclose(file);
}

void DisplayFps() {
	static int frame_count = 0;
	static time_t beginTime = time(NULL);
	frame_count++;
	if ((time(NULL) - beginTime) >= 1)
	{
		beginTime = time(NULL);
		printf("-------------------%d fps\n", frame_count);
		frame_count = 0;
	}
}

void readImage_thread(ArduCamHandle handle) {
	ArduCamOutData* frameData;
	cv::namedWindow("ArduCam", cv::WINDOW_AUTOSIZE);
	long beginTime = time(NULL);
	static int snapshot_count = 0;
	long total_frame = 0;
	int flag = 0;
	cv::Mat rawImage;
	const char* save_path = "images";
#ifdef linux
	if (access(save_path, F_OK) != 0) {
		if (mkdir(save_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
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

	while (_running) {
		if (ArduCam_availableImage(handle) > 0) {
			Uint32 rtn_val = ArduCam_readImage(handle, frameData);

			if (rtn_val == USB_CAMERA_NO_ERROR) {

				int begin_disp = clock();
				rawImage = ConvertImage(frameData);
				if (!rawImage.data)
				{
					std::cout << "No image data \n";
					ArduCam_del(handle);
					continue;
				}

				DisplayFps();

				if (save_flag) {
					char imageName[50];
					printf("save image%ld.jpg.\n", total_frame);
					sprintf(imageName, "images/image%ld.jpg", total_frame);
					cv::imwrite(imageName, rawImage);
					if (save_raw) {
						printf("save image%ld.raw.\n", total_frame);
						sprintf(imageName, "images/image%ld.raw", total_frame);
						FILE *file = fopen(imageName, "wb");
						fwrite(frameData->pu8ImageData, 1, frameData->stImagePara.u32Size, file);
						fclose(file);
					}
					total_frame++;
				}
				// double scale = 640.0 / rawImage.cols;
				// Mat showImage;
				// cv::resize(rawImage, showImage, cv::Size(640, (int)(rawImage.rows * scale)), (0, 0), (0, 0), cv::INTER_LINEAR);
				// cv::imshow("ArduCam", showImage);
				cv::imshow("ArduCam", rawImage);

				int key = -1;
				key = cv::waitKey(10);
				switch (key) {
				case 13:
					saveImage(rawImage, ++snapshot_count);
					saveRaw(frameData->pu8ImageData, frameData->stImagePara.u32Size, snapshot_count);
					break;
				case 's':
				case 'S':
					save_flag = true;
					break;
				case 'c':
				case 'C':
					save_flag = false;
					break;
				}
				ArduCam_del(handle);
			}
		}
	}
	std::cout << "Read thread stopped" << std::endl;
}


int main(int argc, char **argv)
{
#ifdef linux
	static struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif
	ArduCamHandle cameraHandle;

	const char * config_file_name;
	if (argc > 1) {
		config_file_name = argv[1];
	}
	else {
		showHelp();
		return 0;
	}

	ArduCamIndexinfo pUsbIdxArray[16];
	int camera_num = 0;
	camera_num = ArduCam_scan(pUsbIdxArray);
	printf("device num:%d\n", camera_num);
	char serial[16];
	unsigned char *u8TmpData;
	for (int i = 0; i < camera_num; i++) {
		u8TmpData = pUsbIdxArray[i].u8SerialNum;
		sprintf(serial, "%c%c%c%c-%c%c%c%c-%c%c%c%c",
			u8TmpData[0], u8TmpData[1], u8TmpData[2], u8TmpData[3],
			u8TmpData[4], u8TmpData[5], u8TmpData[6], u8TmpData[7],
			u8TmpData[8], u8TmpData[9], u8TmpData[10], u8TmpData[11]);
		printf("index:%4d\tSerial:%s\n", pUsbIdxArray[i].u8UsbIndex, serial);
	}
#ifdef linux
	sleep(2);
#endif
#ifdef _WIN32
	Sleep(2000);
#endif	

	//read config file and open the camera.
	if (camera_initFromFile(config_file_name, cameraHandle, cameraCfg)) {
		ArduCam_setMode(cameraHandle, CONTINUOUS_MODE);
		std::thread captureThread(captureImage_thread, cameraHandle);
		std::thread readThread(readImage_thread, cameraHandle);
		std::cout << "capture thread create successfully." << std::endl;
		std::cout << "read thread create successfully." << std::endl;
		int key = -1;
#ifdef linux
		while ((key = getchar()) != -1) {
#endif
#ifdef _WIN32
			while ((key = _getch()) != -1) {
#endif
				if (key == 'q' || key == 'Q') {
					break;
				}
				switch (key) {
				case 's':
				case 'S':
					save_flag = true;
					break;
				case 'c':
				case 'C':
					save_flag = false;
					break;
				}
			}

			_running = false;
			readThread.join();
			captureThread.join();
			cv::destroyAllWindows();
			ArduCam_close(cameraHandle);
		}


		std::cout << std::endl << "Press ENTER to exit..." << std::endl;
		std::string key;
		std::getline(std::cin, key);
#ifdef linux
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
		return 0;
	}


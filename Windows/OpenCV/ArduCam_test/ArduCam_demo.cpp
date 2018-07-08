// ArduCam_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ArduCamlib.h"


#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <thread>
#include <iostream>
#include <istream>
#include <string> 

//#define USB2
#define MONO

ArduCamCfg cameraCfg;
volatile bool _running = true;

cv::Mat BytestoMat(Uint8* bytes, int width, int height)
{
	cv::Mat image = cv::Mat(height, width, CV_8UC1, bytes);
	return image;
}

bool camera_initFromFile(std::string filename, ArduCamHandle &cameraHandle, ArduCamCfg &cameraCfg) {
	cv::FileStorage cfg;
	if (cfg.open(filename, cv::FileStorage::Mode::READ)) {
		cv::FileNode cp = cfg["camera_param"];
		int value;
		std::string hexStr;

		cp["emI2cMode"] >> value;
		switch (value) {
		case 0: cameraCfg.emI2cMode = I2C_MODE_8_8; break;
		case 1: cameraCfg.emI2cMode = I2C_MODE_8_16; break;
		case 2: cameraCfg.emI2cMode = I2C_MODE_16_8; break;
		case 3: cameraCfg.emI2cMode = I2C_MODE_16_16; break;
		default: break;
		}

		cp["emImageFmtMode"] >> value;
		switch (value) {
		case 0: cameraCfg.emImageFmtMode = FORMAT_MODE_RAW; break;
		case 1: cameraCfg.emImageFmtMode = FORMAT_MODE_RGB; break;
		case 2: cameraCfg.emImageFmtMode = FORMAT_MODE_YUV; break;
		case 3: cameraCfg.emImageFmtMode = FORMAT_MODE_JPG; break;
		case 4: cameraCfg.emImageFmtMode = FORMAT_MODE_MON; break;
		case 5: cameraCfg.emImageFmtMode = FORMAT_MODE_RAW_D; break;
		case 6: cameraCfg.emImageFmtMode = FORMAT_MODE_MON_D; break;
		default: break;
		}

		cp["u32Height"] >> value; cameraCfg.u32Height = value;
		cp["u32Width"] >> value; cameraCfg.u32Width = value;
		cp["u32I2cAddr"] >> hexStr; cameraCfg.u32I2cAddr = std::stoul(hexStr, nullptr, 16);
		cp["u8PixelBits"] >> value; cameraCfg.u8PixelBits = value;
		cp["u8PixelBytes"] >> value; cameraCfg.u8PixelBytes = value;
		cp["u32TransLvl"] >> value; cameraCfg.u32TransLvl = value;

		if (ArduCam_autoopen(cameraHandle, &cameraCfg) == USB_CAMERA_NO_ERROR) {
#ifdef USB2
			cv::FileNode bp = cfg["board_param_usb2"];
			for (int i = 0; i < bp.size(); i++) {
				uint8_t u8Buf[10];
				for (int j = 0; j < bp[i][4].size(); j++)
				{
					bp[i][4][j] >> hexStr;
					u8Buf[j] = std::stoul(hexStr, nullptr, 16);
					//bp[i][4][j] >> u8Buf[j];
				}
					

				bp[i][0] >> hexStr;
				Uint8 u8Command = std::stoul(hexStr, nullptr, 16);
				bp[i][1] >> hexStr;
				Uint16 u16Index = std::stoul(hexStr, nullptr, 16);
				bp[i][2] >> hexStr;
				Uint16 u16Value = std::stoul(hexStr, nullptr, 16);
				bp[i][3] >> hexStr;
				Uint32 u32BufSize = std::stoul(hexStr, nullptr, 16);
				ArduCam_boardConfig(cameraHandle, u8Command, u16Index, u16Value, u32BufSize, u8Buf);
			}
			cv::FileNode rp = cfg["register_param_usb2"];
			for (int i = 0; i < rp.size(); i++) {
				rp[i][0] >> hexStr;
				Uint32 addr = std::stoul(hexStr, nullptr, 16);
				rp[i][1] >> hexStr;
				Uint32 val = std::stoul(hexStr, nullptr, 16);
				ArduCam_writeSensorReg(cameraHandle, addr, val);
			}
#else
			cv::FileNode bp = cfg["board_param_usb3"];
			for (int i = 0; i < bp.size(); i++) {
				uint8_t u8Buf[10];
				for (int j = 0; j < bp[i][4].size(); j++)
				{
					bp[i][4][j] >> hexStr;
					u8Buf[j] = std::stoul(hexStr, nullptr, 16);
					//bp[i][4][j] >> u8Buf[j];
				}


				bp[i][0] >> hexStr;
				Uint8 u8Command = std::stoul(hexStr, nullptr, 16);
				bp[i][1] >> hexStr;
				Uint16 u16Index = std::stoul(hexStr, nullptr, 16);
				bp[i][2] >> hexStr;
				Uint16 u16Value = std::stoul(hexStr, nullptr, 16);
				bp[i][3] >> hexStr;
				Uint32 u32BufSize = std::stoul(hexStr, nullptr, 16);
				ArduCam_boardConfig(cameraHandle, u8Command, u16Index, u16Value, u32BufSize, u8Buf);
			}
			cv::FileNode rp = cfg["register_param_usb3"];
			for (int i = 0; i < rp.size(); i++) {
				rp[i][0] >> hexStr;
				Uint32 addr = std::stoul(hexStr, nullptr, 16);
				rp[i][1] >> hexStr;
				Uint32 val = std::stoul(hexStr, nullptr, 16);
				ArduCam_writeSensorReg(cameraHandle, addr, val);
			}
#endif
		}
		else {
			std::cout << "Cannot open camera" << std::endl;
			cfg.release();
			return false;
		}

		cfg.release();
		std::cout << "Camera is initlized successfully" << std::endl;
		return true;
	}
	else {
		std::cout << "Cannot find configuration file" << std::endl;
		return false;
	}
}

void captureImage(ArduCamHandle handle) {
	Uint32 rtn_val = ArduCam_beginCaptureImage(handle);
	if ( rtn_val == USB_CAMERA_USB_TASK_ERROR) {
		std::cout << "Error beginning capture, rtn_val = " << rtn_val << std::endl;
		return;
	}
	else {
		std::cout << "Capture thread started" << std::endl;
	}

	while (_running) {
		rtn_val = ArduCam_captureImage(handle);
		if ( rtn_val == USB_CAMERA_USB_TASK_ERROR) {
			std::cout << "Error capture image, rtn_val = " << rtn_val << std::endl;
			return;
		}
		else {
			//std::cout << "Image captured, rtn_val = " << rtn_val << std::endl;
		}
	}

	ArduCam_endCaptureImage(handle);
	std::cout << "Capture thread stopped" << std::endl;
}

void readImage(ArduCamHandle handle) {
	ArduCamOutData* frameData;
	std::cout << "Read thread started" << std::endl;
	while (_running) {
		if (ArduCam_availableImage(handle) > 0) {
			Uint32 rtn_val = ArduCam_readImage(handle, frameData);
			if ( rtn_val == USB_CAMERA_NO_ERROR) {
				cv::Mat rawImage = BytestoMat(frameData->pu8ImageData, cameraCfg.u32Width, cameraCfg.u32Height);

				#ifdef MONO
				//do nothing
				#else
				cv::Mat rgbImage;
				cv::cvtColor(rawImage, rgbImage, CV_BayerGR2RGB);
				#endif 

				ArduCam_del(handle);
				//std::cout << "Image read, rtn_val = " << rtn_val << std::endl;

				#ifdef MONO
				cv::imshow("ArduCam Test", rawImage);
				#else
				cv::imshow("ArduCam Test", rgbImage);
				#endif

				cv::waitKey(2);
			}
			else {
				std::cout << "Error reading image, rtn_val = " << rtn_val << std::endl;
			}
		}
		else {
			//std::cout << "No data in the stack" << std::endl;
		}
	}

	std::cout << "Read thread stopped" << std::endl;
}



int main()
{
	ArduCamHandle cameraHandle;

	
	/**
		Configure camera from file
	**/

	if (camera_initFromFile("ArduCamCfg.json", cameraHandle, cameraCfg)) {
		std::thread captureThread(captureImage, cameraHandle);
		std::thread readThread(readImage, cameraHandle);
		cv::namedWindow("ArduCam Test", cv::WINDOW_AUTOSIZE);
		while (cv::waitKey(1000) != 27) {
			// do nothing
		}
		_running = false;
		readThread.join();
		captureThread.join();
		cv::destroyAllWindows();
		ArduCam_close(cameraHandle);
	}

	std::cout << std::endl << "Press ENTER to exit..." << std::endl;
	std::string key;
	std::getline(std::cin,key);
    return 0;
}



#include "stdafx.h"
#include "USBTest.h"
#include "USBTestDlg.h"

UINT _FrameCaptureThread( LPVOID lParam );							// 图像采集线程
UINT _FrameReadThread( LPVOID lParam );								// 图像读取显示线程

UINT _FrameCaptureThread( LPVOID lParam )
{
	/* -- 1. 获取主线程指针 -- */
	CUSBTestDlg *pcTmpUSBTestDlg =( CUSBTestDlg* )lParam;

	/* -- 2. 执行原始视频网络接收 -- */
	pcTmpUSBTestDlg->CaptureFrame();

	/* -- 3. 返回0（必须有返回值） -- */
	return 0;
}


UINT _FrameReadThread( LPVOID lParam )
{
	/* -- 1. 获取主线程指针 -- */
	CUSBTestDlg *pcTmpUSBTestDlg =( CUSBTestDlg* )lParam;

	/* -- 2. 执行原始视频网络接收 -- */
	pcTmpUSBTestDlg->ReadFrame();

	/* -- 3. 返回0（必须有返回值） -- */
	return 0;
}
  

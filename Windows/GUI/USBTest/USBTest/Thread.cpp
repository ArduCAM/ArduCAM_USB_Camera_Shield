#include "stdafx.h"
#include "USBTest.h"
#include "USBTestDlg.h"

UINT _FrameCaptureThread( LPVOID lParam );							// ͼ��ɼ��߳�
UINT _FrameReadThread( LPVOID lParam );								// ͼ���ȡ��ʾ�߳�

UINT _FrameCaptureThread( LPVOID lParam )
{
	/* -- 1. ��ȡ���߳�ָ�� -- */
	CUSBTestDlg *pcTmpUSBTestDlg =( CUSBTestDlg* )lParam;

	/* -- 2. ִ��ԭʼ��Ƶ������� -- */
	pcTmpUSBTestDlg->CaptureFrame();

	/* -- 3. ����0�������з���ֵ�� -- */
	return 0;
}


UINT _FrameReadThread( LPVOID lParam )
{
	/* -- 1. ��ȡ���߳�ָ�� -- */
	CUSBTestDlg *pcTmpUSBTestDlg =( CUSBTestDlg* )lParam;

	/* -- 2. ִ��ԭʼ��Ƶ������� -- */
	pcTmpUSBTestDlg->ReadFrame();

	/* -- 3. ����0�������з���ֵ�� -- */
	return 0;
}
  

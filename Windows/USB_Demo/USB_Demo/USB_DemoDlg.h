// USBTestDlg.h : 头文件
//

#pragma once

/* ----- 调用库文件 ----- */
#pragma comment(lib,"ArduCamlib.lib")
#include "ArduCamlib.h"
#include "afxwin.h"
#include "afxcmn.h"
/* ----- 调用库文件 ----- */
#include "sensor.h"




#define B_OFFSET    0
#define G_OFFSET    1
#define R_OFFSET    2

#define ODD(i)      ((i)&1)
#define EVEN(i)     (!((i)&1))


#define MI_APTINA_VID               0x20FB
#define MI_MICRON_VID               0x0634

/* -- RichEdit输出颜色 -- */
#define OUTPUT_RED							RGB( 200,   0,   0 )			// 红色
#define OUTPUT_GREEN						RGB(   0, 200,   0 )			// 绿色
#define OUTPUT_BLUE							RGB(   0,   0, 200 )			// 蓝色
#define OUTPUT_YELLOW						RGB( 200, 200,   0 )			// 黄色
#define OUTPUT_CYAN							RGB(   0, 200, 200 )			// 青色
#define OUTPUT_PURPLE						RGB( 200,   0, 200 )			// 紫色

/* -- 线程 -- */
#define THREAD_ENABLE						0								// 线程使能
#define THREAD_DISABLE						1								// 线程不使能

/* -- 定时器ID -- */
#define DISP_INF_TIMER						1							
#define DISP_FRAME_TIMER					2

/* -- 播放状态 -- */
#define STATE_STOP							0								
#define STATE_PLAY							1
#define STATE_PAUSE							2	


/* -- ROW格式 -- */
#define RAW_RG								0
#define RAW_GR								1
#define RAW_GB								2
#define RAW_BG								3

// CUSBTestDlg 对话框
class CUSBTestDlg : public CDialog
{
// 构造
public:
	CUSBTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_USBTEST_DIALOG };

	void CaptureFrame( void );
	void ReadFrame( void );	

	void CaptureIMU( void );


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	ArduCamHandle m_cUsbCameraHd;

	CWinThread	*m_ptdFrameCaptureThread;					
	Uint32		m_u32FrameCaptureThreadEn;	

	CWinThread	*m_ptdFrameReadThread;					
	Uint32		m_u32FrameReadThreadEn;	

	CWinThread	*m_ptdIMUCaptureThread;					
	Uint32		m_u32IMUCaptureThreadEn;	

	Uint8		m_u8PlayState;

	Uint32		m_u32FrameCaptureCnt;
	Uint32		m_u32FrameReadCnt;

	Uint32		m_u32FrameCaptureCntBak;
	Uint32		m_u32FrameReadCntBak;

	Uint32		m_u32RawMode;


	LARGE_INTEGER	m_liLargeInterger;
	f64				m_f64Dff;
	__int64			m_s64ShotStart;
	__int64			m_s64ShotEnd;
	Uint32			m_u32ShotFlag;

	Uint32			m_u32CaptFlag;

	Uint8*			m_pu8OutRgb24;

	/* ----- 图像平移与缩放 ----- */
	Uint32	m_u32FrameLeftloc;
	Uint32	m_u32FrameRightloc; 
	Uint32	m_u32FrameUploc;
	Uint32	m_u32FrameDownloc;

	Int32	m_s32LocXMoved;
	Int32	m_s32LocYMoved;

	Int32	m_s32locStartX;
	Int32	m_s32locStartY;

	f64		m_pixScale;

	Int32	m_s32ScaleIndex;

	void InsertText( CString str, COLORREF rgb );
	void DispErrorLog( Uint32 u32ErrorCode );

	void DispInfo( void );
	
	void Raw2rgb24( Uint8* pu8DstBmpData, Uint8* pu16ResRawData, Uint32 u32Width, Uint32 u32Height, Uint8 u8RawMode, Uint8 u8PixelBits);

	void FrameDisplay( Uint8* pu8Rgb32, Uint32 u32Width, Uint32 u32Height );

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btInit;
public:
	afx_msg void OnBnClickedButtonInit();
public:
	CRichEditCtrl m_reInf;
public:
	CButton m_btPlay;
public:
	CButton m_btStop;
public:
	afx_msg void OnBnClickedButtonPlay();
public:
	CStatic m_sttFrameCaptureCnt;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	CStatic m_sttFrameReadCnt;
public:
	afx_msg void OnBnClickedButtonStop();
public:
	CStatic m_sttDisplay;
public:
	//CButton m_btClose;
public:
	afx_msg void OnBnClickedButtonClose();
public:
	CEdit m_edtRegAddr;
public:
	afx_msg void OnBnClickedButtonRegRead();
public:
	CEdit m_edtRegValue;
public:
	CButton m_btRegRead;
public:
	CButton m_btRegWrite;
public:
	afx_msg void OnBnClickedButtonRegWrite();
public:
	CComboBox m_cmbType;
public:
	CEdit m_edtWidth;
public:
	CEdit m_edtHeight;
public:
	CComboBox m_cmbBits;
public:
	CButton m_btScan;
public:
	CComboBox m_cmbUsbIndex;
public:
	afx_msg void OnBnClickedButtonScan();
public:
	CComboBox m_cmbVID;
public:
	CButton m_btOpen;
public:
	afx_msg void OnBnClickedButtonOpen();
public:
	afx_msg void OnDestroy();
public:
	CButton m_btClose;
	CButton m_btRawMode;
	afx_msg void OnBnClickedButtonByteConv();
	CComboBox m_cmbI2cMode;
	CEdit m_edtShipAddr;
	CButton m_btShot;
	afx_msg void OnBnClickedButtonShot();
	//CStatic m_sttShotTime;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	CButton m_chkForceDisp;
	afx_msg void OnBnClickedCheck1();
	/*CStatic m_sttIMUIdx;
	CStatic m_sttIMUTime;
	CStatic m_sttIMUAccX;
	CStatic m_sttIMUAccY;
	CStatic m_sttIMUAccZ;
	CStatic m_sttIMUGyroX;
	CStatic m_sttIMUGyroY;
	CStatic m_sttIMUGyroZ;
	CStatic m_sttIMUTemp;*/
	CComboBox m_cmbSensor;
	afx_msg void OnCbnSelchangeCombo6();

	struct sensor_reg *reg_setting; 
};

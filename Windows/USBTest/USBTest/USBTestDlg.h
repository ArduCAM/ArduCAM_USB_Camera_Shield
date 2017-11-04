// USBTestDlg.h : 头文件
//

#pragma once


#pragma comment(lib,"ArduCamlib.lib")
#include "ArduCamlib.h"
#pragma comment(lib,"ArduCamConfiglib.lib")
#include "ArduCamConfiglib.h"
#pragma comment(lib,"ArduCamDisplib.lib")
#include "ArduCamDisplib.h"

#include "afxwin.h"
#include "afxcmn.h"

#include "common.h"


/* -------------------- DlgFrame -------------------- */
#define FRAME_W_GAP					20
#define FRAME_H_GAP					20
/* -------------------- DlgFrame -------------------- */


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
#define READ_FRAME_TIMER					3

/* -- 播放状态 -- */
#define STATE_STOP							0								
#define STATE_PLAY							1
#define STATE_PAUSE							2	

/* -- RAW格式 -- */
#define RAW_RG								0
#define RAW_GR								1
#define RAW_GB								2
#define RAW_BG								3

/* -- YUV422格式 -- */
#define YUV422_YU							0
#define YUV422_YV							1
#define YUV422_UY							2
#define YUV422_VY							3

/* -- RGB565格式 -- */
#define RGB565_RB							0
#define RGB565_BR							1


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

	/* -- Common Tools -- */
	Int32 atoi_ex( CString csUseNum  ); 

	Int8  Asc2Num( Int8 s8Data );
	Int8  Num2Asc( Int8 s8Data );
	
	void  DlgFrameSet( void );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	ArduCamHandle		m_cUsbCameraHd;
	ArduCamCfgHandle	m_cUsbCameraCfgHd;
	ArduCamRecordHandle	m_cUsbCameraRecordHd;

	CWinThread	*m_ptdFrameCaptureThread;					
	Uint32		m_u32FrameCaptureThreadEn;	

	CWinThread	*m_ptdFrameReadThread;					
	Uint32		m_u32FrameReadThreadEn;	

	Uint8		m_u8PlayState;

	Uint32		m_u32FrameCaptureCnt;
	Uint32		m_u32FrameCaptureCntBak;
	Uint32		m_u32FrameReadCnt;
	Uint32		m_u32FrameReadCntBak;

	Uint32		m_u32RawMode;
	Uint32		m_u32YuvMode;
	Uint32		m_u32RgbMode;

	Uint32		m_u32TransLvl;

	Uint32		m_u32ShotTimeFlag;
	CString		m_csShotPath;
	CFile		m_pImageShotFile;

	Uint8		m_u8RecordEn;
	CString		m_csRecordPath;
	CString		m_csRecordSubPath;
	Uint32		m_u32RecordIndex;
	SYSTEMTIME	m_tiBackTime;

	Uint32		m_u32CaptFlag;

	Uint8*		m_pu8OutRgb24;
	Uint32		m_u32JpgSize;

	Uint8		m_u8FullScreenEn;

	/* ----- 图像平移与缩放 ----- */
	RECT	m_sttDisplay_rect;
	Uint32  m_u32DisplayWidth;
	Uint32  m_u32DisplayHeight;

	Int32	m_s32LocXMoved;
	Int32	m_s32LocYMoved;

	Int32	m_s32locStartX;
	Int32	m_s32locStartY;

	f64		m_pixScale;

	Int32	m_s32ScaleIndex;

	CString	m_csCfgFileName;
	CArray< WIN32_FIND_DATA, WIN32_FIND_DATA& > m_caCfgFileList;

	void InsertText( CString str, COLORREF rgb );
	void DispErrorLog( Uint32 u32ErrorCode );

	void DispInfo( void );

	void FrameDisplayBMP( Uint8* pu8RgbData, Uint32 u32Width, Uint32 u32Height );
	void FrameDisplayJPG( Uint8* pu8JpgData, Uint32 u32JpgSize );

	void FillBlackDisplay( void );
	
	CString CreateImageFileName( ArduCamCfg* pstUseCfg );
	CString CreateDataFileName( ArduCamCfg* pstUseCfg );

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
	CEdit m_edtI2cAddr;
	CButton m_btShot;
	afx_msg void OnBnClickedButtonShot();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	CButton m_chkForceDisp;
	afx_msg void OnBnClickedCheck1();

	CComboBox m_cmbImgFormat;
	afx_msg void OnCbnSelchangeComboType();
	CButton m_btCfgLoad;
	CComboBox m_cmbCfgFileName;
	afx_msg void OnBnClickedButtonLoad();
	CButton m_btCfgFrash;
	afx_msg void OnBnClickedButtonRefresh();
	CEdit m_edtVid;
	CEdit m_edtWidth;
	CButton m_chkFullScreen;
	afx_msg void OnBnClickedCheckFullScreen();
	afx_msg void OnBnClickedRadioDisplay();
	afx_msg void OnBnClickedRadioSaveData();
	afx_msg void OnBnClickedRadioSaveImage();
	afx_msg void OnBnClickedRadioSaveVideo();
	CEdit m_edtType;
	CButton m_chkIRcut;
	afx_msg void OnBnClickedCheckIrcut();
};

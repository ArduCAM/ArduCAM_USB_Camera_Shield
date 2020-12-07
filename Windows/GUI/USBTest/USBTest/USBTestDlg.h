// USBTestDlg.h : 头文件
//

#pragma once


#pragma comment(lib,"ArduCamlib.lib")
#include "ArduCamlib.h"
#pragma comment(lib,"ArduCamConfiglib.lib")
#include "ArduCamConfiglib.h"
#pragma comment(lib,"ArduCamDisplib.lib")
#include "ArduCamDisplib.h"
#include "ControlDlg.h"

#include "afxwin.h"
#include "afxcmn.h"
#include "TTComboBox.h"
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
#define SCAN_TIMER							4

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

/* -- Record -- */
#define RECORD_NULL					0
#define RECORD_DATA					1
#define RECORD_IMAGE				2
#define RECORD_VIDEO				3
/* --------------- Record --------------- */

/* -- Double Image display mode -- */
#define MODE_HORIZONTAL			0
#define MODE_VERTICAL			1
#define USB_CPLD_I2C_ADDRESS					0x46
typedef struct UTC_PARA_IN							// [ C0, U0 ]
{
	long long	s64C0;
	long long	s64U0;
}UTC_PARA_IN;

typedef struct UTC_PARA_OUT							// [ C0, a, b ]
{
	long long	s64C0;
	double		f64A;
	double		f64B;
}UTC_PARA_OUT;

#ifdef _WIN32
	#define int64 __int64
	#define uint64 unsigned long long
#else
	#include <inttypes.h>
	#define int64 int64_t
	#define uint64 uint64_t
#endif

extern UINT _FrameCaptureThread( LPVOID lParam );						// 图像采集线程
extern UINT _FrameReadThread( LPVOID lParam );							// 图像读取显示线程

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
	CToolTipCtrl  m_tooltip;
	CMenu m_Menu;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	ArduCamHandle		m_cUsbCameraHd;
	ArduCamCfgHandle	m_cUsbCameraCfgHd;
	ArduCamRecordHandle	m_cUsbCameraRecordHd;

	GUID		m_uuGuid;

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

	//GainSet		m_stGainGSet;
	//GainSet		m_stGainBSet;
	//GainSet		m_stGainRSet;
	//GainSet		m_stGainG2Set;
	//GainSet		m_stGainYSet;
	//GainSet		m_stGainUSet;
	//GainSet		m_stGainVSet;
	//GainSet		m_stGainGLSet;

	Uint32		m_u32ShotTimeFlag;
	CString		m_csShotPath;
	CFile		m_pImageShotFile;

	Uint8		m_u8RecordEn;
	Uint8		m_u8RecordEnd;
	CString		m_csRecordPath;
	CString		m_csRecordSubPath;
	Uint32		m_u32RecordIndex;
	SYSTEMTIME	m_tiBackTime;

	Uint32		m_u32CaptFlag;
	Uint32		m_u32ReadFlag;

	Uint8*		m_pu8OutRgb24;
	Uint32		m_u32JpgSize;

	Uint8		m_u8FullScreenEn;
	Uint8		m_u8DImageDispMode;
	Uint8		m_u8WhiteBalanceEn;
	//Uint8		m_u8WhiteBalanceCfgEn;

	//Uint32		m_u32TestR;
	//Uint32		m_u32TestG1;
	//Uint32		m_u32TestG2;

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

	int filetime_WIN32(int64 *filetime_int64);
	void DispInfo( void );

	void FrameDisplayBMP( Uint8* pu8RgbData, Uint32 u32Width, Uint32 u32Height );
	void FrameDisplayBMPMirror(Uint8* pu8RgbData, Uint32 u32Width, Uint32 u32Height);
	void FrameDisplayJPG( Uint8* pu8JpgData, Uint32 u32JpgSize );

	void FillBlackDisplay( void );
	
	CString CreateImageFileName( ArduCamCfg* pstUseCfg );
	CString CreateDataFileName( ArduCamCfg* pstUseCfg );

	void SetWhiteBalanceCfg( Int8 *pf64WBPara );

	UTC_PARA_IN stUtcParaIn;
	UTC_PARA_OUT stpstUtcParaOut;
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
	ControlDlg *m_ControlDlg;
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
	CTTComboBox m_cmbCfgFileName;
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
	CStatic m_sttSerial;
	CButton m_rdoHorizontal;
	int m_rdoVertical;
	afx_msg void OnBnClickedRadioModeHorizontal();
	afx_msg void OnBnClickedRadioModeVertical();
	CButton m_chkWhiteBalance;
	afx_msg void OnBnClickedCheckWhiteBalance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMenuAbout();
	void readCpldVersion(CString &str);
	void readUsbVersion(CString &str);
	virtual void OnOK();
	afx_msg void OnEnChangeEdit4();
	afx_msg void OnBnClickedOpenFile();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	LRESULT CUSBTestDlg::OnSelectAll(WPARAM wParam, LPARAM lParam);
	LRESULT CUSBTestDlg::OnCopy(WPARAM wParam, LPARAM lParam);
	LRESULT CUSBTestDlg::OnClearAll(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStnClickedStaticUuid();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnToolsControls();
	afx_msg void OnToolsDatainfo();
};

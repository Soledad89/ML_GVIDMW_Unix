#ifndef __cxIpCamApi_h__
#define __cxIpCamApi_h__

#ifdef IPCAMSDK_EXPORTS
#define IPCAMSDK_API __declspec(dllexport) 
#else
#define IPCAMSDK_API __declspec(dllimport)
#endif

#define CX_STDCALL		__stdcall

#ifdef __cplusplus
extern "C" {
#endif

#include "windows.h"

#ifndef UInt32
#define UInt32		unsigned long
#endif

#define AUDIO_CODEC_LINEPCM			0
#define AUDIO_CODEC_G726_32K		1

typedef struct tagMEDIA_UNIT
{
	char * pBuf;
	int nLength;
	DWORD dwTimeStamp;
	BOOL bIsIFrame;
	int nWidth;
	int nHeight;
	int nType;
}MEDIA_UNIT;

typedef struct tagIP_CAM_INFO
{
	DWORD dwIpAddr;
	DWORD dwMask;
	DWORD dwGw;

	BYTE aMac[6];
}IP_CAM_INFO;

typedef struct tagIP_CAM_INFO2
{
	int nLength;
	unsigned long nVersion;
	char szDateTime[20];
	unsigned int nDevType;	//0--encoder,1--decoder,2--both

	DWORD dwIpAddr;
	DWORD dwMask;
	DWORD dwGw;

	BYTE aMac[6];

	char szTcpServerAddr[32];	//TCP中心服务器的地址
	unsigned long	unTCPServerPort;
	unsigned long	unDNSServer;
}IP_CAM_INFO2;


typedef struct tagACODEC_PARAM
{
	UInt32 nFrameRate;
	UInt32 nBitrate;
	UInt32 nResolution;
	UInt32 nRcMode;
}ACODEC_PARAM;

typedef struct tagVIDEO_IN_PARAM
{
	UInt32 nBrightness;
	UInt32 nContrast;
	UInt32 nSaturation;
	UInt32 nHue;
}AVI_PARAM;

typedef struct tagCQ_WORKMODE
{
	DWORD	dwWorkMode;
	TCHAR	strServerAddr[32];
	DWORD	dwServerPort;
	DWORD	dwDnsIP;
	DWORD	dwUpCondition;		//上连条件
	DWORD	dwCloseCondition;	//断开条件
	char	strPPPOEUser[16];	//PPPOE用户名
	char	strPPPOEPass[16];	//PPPOE密码
}CQWORKMODE,*PCQWORKMODE;

enum
{
	CQ_OSDVALID_STATE = 0x1,
	CQ_OSDVALID_TIMEPOS = 0x2,
	CQ_OSDVALID_CHARDATA = 0x4,
	CQ_OSDVALID_OSDPOS = 0x8,
	CQ_OSDVALID_CHARMASK = 0x10,
	
	CQ_OSD_TIME = 1,	//叠加时间
	CQ_OSD_CHAR = 2,	//叠加汉字
};

typedef struct tagCQ_OSD_INFO
{
	unsigned long	uValidArea;	//有效字段
	unsigned long uOSDState;	//是否叠字
	short	sTimePosX;		//叠加时间的位置X
	short	sTimePosY;		//叠加时间的位置Y	
	char		szCharData[32];		//叠加的文字
	short	sOSDPosX;	//叠加汉字的位置X
	short	sOSDPosY;	//叠加汉字的位置Y
	char	pCharMask[512];//叠汉字的字模，除非16个都是汉字16x32 = 512
}CQ_OSD_INFO,*PCQ_OSD_INFO;

enum{
	CQ_AUDIO_RATE_8000 = 0x000,
	CQ_AUDIO_RATE_11025 = 0x100,
	CQ_AUDIO_RATE_16000 = 0x200,
	CQ_AUDIO_RATE_22050 = 0x300,
	CQ_AUDIO_RATE_32000 = 0x400,
	CQ_AUDIO_RATE_44100 = 0x500,
};

enum{
	CQ_IS_AUDIO_NOCOMPRESSED = 0x1,
	CQ_IS_AUDIO_COMPRESSED = 0x2,
};

typedef int CX_ERROR;
typedef int  ( CX_STDCALL * FN_IP_CAM_CALLBACK )( 
						LPVOID pContext,
						HANDLE hIpCam,
						int nAct,
						DWORD dwParam1,
						DWORD dwParam2 );

/**********************************************************************
 * 录音机回调函数
 *			hRec,录音机句柄
 *			pData:16位线性PCM音频
 *			nLength:采样点个数
 **********************************************************************/
//typedef (CX_STDCALL * FN_AUDIO_RECORDER)( HANDLE hRec,short * pData,int nLength );
typedef UINT (CALLBACK *FN_CXAUDINCALLBACK)(LPVOID pUser,HANDLE hCaller,LPBYTE pData,DWORD dwLength);
typedef VOID (CX_STDCALL * FN_DEAMON_CALLBACK)( DWORD dwUsrParam,DWORD dwReturnParam1,DWORD dwReturnParam2 );
typedef UINT (CALLBACK *FN_CQAP_NOTIFY_CALLBACK)( LPVOID pUser , LPVOID pCaller , DWORD dwCurTimeStamp , DWORD dwBufferedSize );	//用来通知上层播放到什么时间了，缓冲了多少数据
/**********************************************************************/
/*回调函数事件*/
#define ACT_CONNECT_OK					1
#define ACT_CONNECT_FAILED				2
#define ACT_CONNECT_TERMINATE			3
#define ACT_RECV_VIDEO_FRAME			4
#define ACT_RECV_AUDIO_FRAME			5
#define ACT_TRANSPARENT_DATA_232		6
#define ACT_VIDEO_LOSE					7
#define ACT_VI_PARAM					8
#define ACT_CODEC_PARAM					9
#define ACT_DEV_IP						10
#define ACT_RESOLUTION_CHANGED			11
#define ACT_INVALID_USER				12
#define ACT_ALARM_IN					13
#define ACT_MOTION_DETECT				14
#define ACT_OSD_INFO					15	//得到了OSD信息的
#define ACT_AUDIO_INFO					16	//得到了音频信息
#define ACT_GET_WORKMODE				17	//得到了设备工作模式
#define ACT_TRANSPARENT_DATA_485		18	
/*********************************************************************/
/*Errors*/
#define CX_ERR_NO_ERROR					0	//正常执行
#define CX_ERR_INVALID_HANDLE			1	//句柄无效
#define CX_ERR_INVALID_PARAM			2	//参数无效
#define CX_ERR_INIT_DIRECTDRAW			3	//初始化DirectDraw失败
#define CX_ERR_NOT_CONNECTED			4	//没有连接
#define CX_ERR_COMMAND_TIMEOUT			5	//命令超时
#define CX_ERR_SEND_FAIL				6	//发送失败
#define CX_ERR_BUSY						7	//设备正忙
#define CX_ERR_PENDING					8	//操作正在进行中			

#define CX_ERR_AUDIO_PLAYER_NO_MEMORY	64
/*********************************************************************/
#define CX_COLOR_SPACE_YV12				0
#define CX_COLOR_SPACE_RGB32			1
#define CX_COLOR_SPACE_RGB24			2
#define CX_COLOR_SPACE_RGB555			3
#define CX_COLOR_SPACE_RGB565			4
#define CX_COLOR_SPACE_YUYV				5
/*********************************************************************/
/*分辨率*/
#define CX_RESOLUTION_QCIF				1	//176X144
#define CX_RESOLUTION_CIF				2	//352X288
#define CX_RESOLUTION_HALF_D1			3	//704X576

/*********************************************************************/
/*帧率*/
#define CX_FRAME_RATE_25				0	//not lose
#define CX_FRAME_RATE_20				1	//lose 1 per 5 frames
#define CX_FRAME_RATE_18_5				2	//lose 1 per 4 frames
#define CX_FRAME_RATE_16_7				3	//lose 1 per 3 frames
#define CX_FRAME_RATE_12_5				4	//lose 1 per 2 frames
#define CX_FRAME_RATE_8_3				5	//pick 1 per 3 frames
#define CX_FRAME_RATE_6_25				6	//pick 1 per 4 frames
#define CX_FRAME_RATE_5					7	//pick 1 per 5 frames
#define CX_FRAME_RATE_4_1				8	//pick 1 per 6 frames
#define CX_FRAME_RATE_2_5				9	//pick 1 per 10 frames
#define CX_FRAME_RATE_1					10	//pick 1 per 25 frames

/**********************************************************************/
/*高质量解码*/
#define CX_ADV_DEC_NORMAL				0
#define CX_ADV_DEC_BETTER1				1
#define CX_ADV_DEC_BETTER2				2
#define	CX_ADV_DEC_BEST					3

/**********************************************************************/
/*数据通道编号*/
#define CX_DATA_CHN_485					0
#define CX_DATA_CHN_232					1

/**********************************************************************/
/*连接类型*/
#define CX_CONNECT_TYPE_UDP				0
#define CX_CONNECT_TYPE_TCP				1

/*设备工作模式*/
#define CX_DEVFUNC_TCP					0x1			//设备使用TCP主动上连的功能
#define CX_DEVFUNC_ENCODER				0x2			//设备具有视频编码的功能
#define CX_DEVFUNC_DECODER				0x4			//设备具有视频解码的功能
#define CX_DEVFUNC_PPPOE				0x8			//使用PPPOE拨号
#define CX_DEVFUNC_AUTOSEND				0x10		//控制连接断开后仍然持续发送媒体流
#define CX_DEVFUNC_TCPSERVER			0x20		//TCP服务模式
#define CX_DEVFUNC_MINIPACKET			0x40		//使用小型数据包
/**********************************************************************
 *返回标记：
 * CX_RETURN_EVENT 使用事件通知返回，参数为事件Handle，和返回设置参数的地址
 * CX_RETURN_WND   使用窗口返回，参数为窗口 和 消息
 * CX_RETURN_CALLBACK 返回调用函数，参数为回调函数和回调函数用户自定义参数
 **********************************************************************/
/*返回参数标记*/
#define CX_RETURN_EVENT					1
#define CX_RETURN_WND					2
#define CX_RETURN_CALLBACK				4

/**********************************************************************
 *	功能：打开设备
 *	dwIpAddr：设备地址,网络字节顺序
 *	fnCallback：回调函数
 *	pUsrName：用户名
 *	pPwd：口令
 *	hQuitEvent：设置退出事件，如果为NULL，由SDK在内部创建事件,manual reset
 *	pContext：调用回调函数的第一个参数
 **********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL
cxOpenIpCam( 
				DWORD dwIpAddr,
				FN_IP_CAM_CALLBACK fnCallback,
				char * pUsrName,
				char * pPwd,
				HANDLE hQuitEvent,
				LPVOID pContext );


/**********************************************************************
 *功能：关闭设备
 *hIpCam：设备句柄
 **********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxCloseIpCam( HANDLE hIpCam );


/**********************************************************************
 *功能：关闭所有系统中已经打开的设备
 **********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxCloseAllIpCam( void );

/**********************************************************************
 * 功能：从本地存储中取出设备相关配置，注意，有可能和设备实际配置不一致
 * 参数：
 * hDev 设备句柄
 * pInfo 设备信息
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetIpCamDevInfo( HANDLE hDev,IP_CAM_INFO2 * pInfo );

/**********************************************************************
 * 功能：从本地存储中取出ip相关配置，注意，有可能和设备实际配置不一致
 *		参见cxGetDevIp的说明。
 * 参数：
 * hDev 设备句柄
 * pdwIp IP地址
 * pdwMask 子网掩码
 * pdwDefGateway 默认网关
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL 
	cxGetLocalDevIp(
			HANDLE hDev,
			DWORD * pdwIp,
			DWORD * pdwMask,
			DWORD * pdwDefGateway );

/**********************************************************************
 * 功能：取得设备ip相关配置，这个函数向设备发送命令，取回配置后，返回
 *		 ACT_DEV_IP回调函数消息，应用程序通过GetLocalDevIp取得配置
 * 参数：
 * hDev 设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL 
	cxGetDevIp( HANDLE hDev );

/**********************************************************************
 * 功能：修改设备IP地址
 * 参数：
 * hDev 设备句柄
 * dwIp IP地址
 * dwMask 子网掩码
 * dwDefGateway 默认网关
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxModifyDevIp(
				HANDLE hDev,
				DWORD dwIp,
				DWORD dwMask,
				DWORD dwDefGateway );

/*********************************************************************
 * 功能：修改设备用户名和密码
 * 参数：
 * hDev 设备句柄
 * pUserName：用户名，小于8个ASCII字符
 * pPwd：密码，小于8个ASCII字符
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxModifyDevUserNamePwd( HANDLE hDev,char * pUserName,char * pPwd );

/*********************************************************************
 * 功能：设置视频分辨率
 * 参数：
 *   hDev：设备句柄
 *   nResolution：分辨率
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetVideoResolution( HANDLE hDev,int nResolution );

/*********************************************************************
 * 功能：设置帧率
 * 参数：
 *   hDev:设备句柄
 *   nFrameRate:帧率
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetVideoFrameRate( HANDLE hDev,int nFrameRate );

/*********************************************************************
 * 功能：存储配置数据
 * 参数：
 *   hDev：设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSaveIpCamConf( HANDLE hDev );

/*********************************************************************
 * 功能：设置视频编码码率
 * 参数：
 * hDev 设备句柄
 * nBitrate 码率，64~2048，单位Kpbs
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetVideoBitrate( HANDLE hDev,int nBitrate );

/*********************************************************************
 * 功能：返回当前实际视频编码码率(通过网络计算出来)
 * 参数：
 * hDev 设备句柄
 * pnBitrate 返回码率,单位Kbps
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetVideoCurrentBitrate( HANDLE hDev,int * pnBitrate );

/*********************************************************************
 * 功能:加载缺省编码参数
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxLoadDefCodecParam( HANDLE hDev );

/*********************************************************************
 * 功能:获得本地保存的编码参数
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetLocalCodecParam( HANDLE hDev,ACODEC_PARAM * pParam );

/*********************************************************************
 * 功能:设置亮度
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetViBrightness( HANDLE hDev,int nBrightness );

/*********************************************************************
 * 功能:设置对比度
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetViContrast( HANDLE hDev,int nContrast );

/*********************************************************************
 * 功能:设置饱和度
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetViSaturation( HANDLE hDev,int nSaturation );

/*********************************************************************
 * 功能:设置色调
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetViHue( HANDLE hDev,int nHue );

/*********************************************************************
 * 功能:一次性设置VI参数
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetViParam( HANDLE hDev,AVI_PARAM * pParam );

/*********************************************************************
 * 功能：一次性获取VI参数，参数不能马上返回，参数返回时，回调函数会
 * 产生ACT_VI_PARAM事件，然后调用cxGetLocalViParam可获得设备VI参数。
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetViParam( HANDLE hDev );

/*********************************************************************
 * 功能：获得本地主机保存的VI参数，设备连接成功后，会自动从设备获得
 * VI参数，保存在本地。
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetLocalViParam( HANDLE hDev,AVI_PARAM * pParam );

/*********************************************************************
 * 功能：恢复视频输入出厂默认参数
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxLoadViDefParam( HANDLE hDev );

/*********************************************************************
 * 功能：与设备建立连接
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxConnectIpCam( HANDLE hDev,DWORD dwIpAddr );

/*********************************************************************
 * 功能：获得当前状态
 * 参数：hDev 设备句柄
 * nStatus：
 *		bit0,bit1
 *			0 ---- idle
 *			1 ---- 正在连接
 *			2 ---- 已连接
 *		bit2 ---- 接收视频
 *		bit3 ---- 接收音频
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetStatus( HANDLE hDev,DWORD * nStatus);

/*********************************************************************
 * 功能：校时
 * 参数：hDev 设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxVerifyTime( HANDLE hDev );

/*********************************************************************
 * 功能：通知设备，开始发送视频，本函数通知设备将视频发送到dwIp指示的
 *		主机，端口为sPort，当dwIp为0时，表示将视频发送到本机，sPort不
 *		起作用。
 * 参数：
 *		hDev 设备句柄
 *		dwIp IP地址
 *		sPort 端口
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStartVideo( HANDLE hDev,DWORD dwIp,short sPort );

/*********************************************************************
 * 功能：通知设备，停止发送视频
 * 参数：hDev 设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStopVideo( HANDLE hDev );

/*********************************************************************
 * 功能：通知设备，开始发送音频，本函数通知设备将音频发送到dwIp指示的
 *		主机，端口为sPort，当dwIp为0时，表示将视频发送到本机，sPort
 *		不起作用。
 * 参数：
 *		hDev 设备句柄
 *		dwIp IP地址
 *		sPort 端口
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStartAudio( HANDLE hDev,DWORD dwIp,short sPort );

/*********************************************************************
 * 功能：通知设备，停止发送音频
 * 参数：hDev 设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStopAudio( HANDLE hDev );

/*********************************************************************
 * 功能：开始接收视频
 * 参数：hDev 设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStartRecvVideo( HANDLE hDev,
												   DWORD dwMultiIp,
												   short sPort );

/*********************************************************************
 * 功能：停止接收视频
 * 参数：hDev 设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStopRecvVideo( HANDLE hDev );

/*********************************************************************
 * 功能：开始接收音频
 * 参数：hDev 设备句柄
 *		 dwMcastIp 多播IP地址,如果为0,表示不使用多播
 *		 sPort	   端口,只有在多播时有效
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStartRecvAudio( HANDLE hDev,DWORD dwMcastIp,short sPort );

/*********************************************************************
 * 功能：停止接收音频
 * 参数：hDev 设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStopRecvAudio( HANDLE hDev );

/*********************************************************************
 * 功能：指定数据发送的IP和端口
 * 参数：hDev 设备句柄
 *		 dwHostIP 数据发送目的地址，为0时表示发送到本机
 *		 sPort 数据发送的端口,当dwHostIp为0时,SDK将自动设置这个参数
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStartData( HANDLE hDev,DWORD dwHostIp,short sPort );

/*********************************************************************
 * 功能：停止发送数据
 * 参数：hDev 设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStopData( HANDLE hDev );

/*********************************************************************
 * 功能：强制发送I帧
 * 参数：hDev 设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxForceIFrame( HANDLE hDev );

/*********************************************************************
 * 功能：取得报警输入
 * 参数：
 * hDev 设备句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetAlarmIn( HANDLE hDev );


/*********************************************************************
 * 功能：取得报警输入
 * 参数：
 * hDev 设备句柄
 * nSw 开关号，bit0--switch1 bit1--switch2		etc.
 * nStatus 开关状态 bit0--switch1,bit1--switch2 etc. 0--open,1--close
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetAlarmOut( HANDLE hDev,int nSwitchNo,int Status );

/*********************************************************************
 * 功能：发送透明数据
 * 参数：
 * hDev 设备句柄
 * pData 数据
 * nDataLen 数据长度
 * nChnNum ---- 0 is 485,1 is 232
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSendTransparentData( 
			HANDLE hDev,
			char * pData,
			int nDataLen,
			int nChnNum );

/*********************************************************************
 * 功能：设置232/485波特率
 * 参数：
 * hDev 设备句柄
 * nBaud 19200
 * nComNum ---- 0 is 485,1 is 232
 *       9600
 *       4800
 *       2400
 *       1200
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetDataBaudrate( HANDLE hDev,int nComNum,int nBaud );

/*********************************************************************
 * 功能：得到232/485波特率
 * 参数：
 * hDev 设备句柄
 * 结果从回调中返回
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetDataBaudrate( HANDLE hDev ,int nComNum );

/*********************************************************************
 * 功能：创建MPEG4解码器
 * 参数：
 *		nWide 宽度
 *		nHeight 高度
 * 返回：
 *		MPEG4解码器句柄
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateDecoder( int nWide,int nHeight );

/*********************************************************************
 * 功能：关闭MPEG4 Decoder
 * 参数：hDecoder MPEG4解码器句柄
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxCloseDecoder( HANDLE hDecoder );

/*********************************************************************
 * 功能：解码一帧
 * 参数：
 *		hDec：解码器句柄
 *		pData：MPEG4视频数据
 *		nLength：视频数据长度
 *		nAdvLevel：定义高质量解码级别
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxDecodeFrame( 
				HANDLE hDec,
				char * pData,
				int nLength,
				char * pDecBuf,
				int nStride,
				int nAdvLevel );

/*********************************************************************
 * 功能：初始化显示环境
 * 参数：pWnd 显示视频的窗口
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxInitVideoDisp( HWND hWnd );

/*********************************************************************
 * 功能：关闭显示环境
 * 参数：无
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxUninitVideoDisp( void );

/*********************************************************************
 * 功能：创建显示显示通道
 * 参数：
 *   rcx,rcy --- 实际图像大小
 *   x,y   ---- 位置
 *   cx,cy ---- 尺寸
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateDispChn( int rcx,
									 int rcy,
									 int x,
									 int y,
									 int cx,
									 int cy );

/*********************************************************************
 * 功能：创建显示显示通道
 * 参数：
 *	 rcx,rcy ----视频尺寸
 *   x,y   ---- 位置
 *   cx,cy ---- 尺寸
 *	 hWnd ----窗口句柄,调用此函数无需调用cxInitVideoDisp
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateDispChnEx( HWND hWnd,
									int rcx,
									int rcy,
									int x,
									int y,
									int cx,
									int cy );

/*********************************************************************
 * 功能：关闭显示通道
 * 参数：
 *   hDisp 通道句柄
 *	 hWnd 窗口句柄
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseDispChnEx( HWND hWnd,HANDLE hDisp );

/*********************************************************************
 * 功能：关闭与窗口相关的DDraw显示
 * 参数：
 *   hWnd 窗口句柄
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxClearDisp( HWND hWnd );

/*********************************************************************
 * 功能：关闭显示通道
 * 参数：
 *   hDisp 通道句柄
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseDispChn( HANDLE hDisp );

/*********************************************************************
 * 功能：显示视频
 * 参数：
 *   hDisp ---- 显示通道句柄
 *   pYUV  ---- 解码结果
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxDispVideo( HANDLE hDisp,char * pYUV );

/*********************************************************************
 * 功能：设置视频显示通道的位置
 * 参数：
 *   hDisp ---- 显示通道句柄
 *   x,y   ---- 位置
 *   cx,cy ---- 尺寸
 *   bFullWnd ---- 是否放大到整个窗口
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetDispLocate( HANDLE hDisp,int x,int y,int cx,int cy,BOOL bFullWnd );

/*********************************************************************
 * 功能：创建音频解码器
 * 参数：
 *		无
 * 返回：
 *		音频解码器句柄
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateAudioDecoder( void );

/*********************************************************************
 * 功能：创建音频解码器
 * 参数：
 *		hDec,音频解码器句柄
 * 返回：
 *		无
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseAudioDecoder( HANDLE hDec );

/*********************************************************************
 * 功能：音频解码
 * 参数：
 *		hDec：解码器句柄
 *		pData：音频数据
 *		nLength：音频数据长度
 *		pnDecLen：解码结果长度
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxDecodeAudio( 
				HANDLE hDec,
				char * pData,
				int nLength,
				short * pDecBuf,
				int * pnDecLen );

/*********************************************************************
 * 功能：创建音频编码器
 * 参数：
 *		无
 * 返回：
 *		音频编码器句柄
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateAudioEncoder( void );

/*********************************************************************
 * 功能：创建音频编码器
 * 参数：
 *		hDec,音频编码器句柄
 * 返回：
 *		无
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseAudioEncoder( HANDLE hEnc );

/*********************************************************************
 * 功能：音频编码
 * 参数：
 *		hEnc：解码器句柄
 *		pData：音频数据
 *		nLength：音频数据长度
 *		pnEncLen：编码结果长度
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxEncodeAudio( 
				HANDLE hEnc,
				short * pData,
				int nLength,
				char * pEncBuf,
				int * pnEncLen );

/*********************************************************************
 * 功能：创建音频播放器
 * 参数：
 *		nDev--音频设备编号
 * 返回：
 *		音频播放器句柄
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateAudioPlayer( int nDev ,int nRateIndex );

/*********************************************************************
 * 功能：关闭音频播放器
 * 参数：
 *		hPlayer: 音频播放器句柄
 * 返回：
 *		无
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseAudioPlayer( HANDLE hPlayer );

/*********************************************************************
 * 功能：播放音频
 * 参数：
 *		hPlayer: 音频播放器句柄
 *		pSamp:16位线形PCM音频
 *		nLength:音频长度(以采样点数计算)
 * 返回：
 *		CX_ERR_AUDIO_PLAYER_NO_MEMORY--播放缓冲区不足
 *********************************************************************/
IPCAMSDK_API  CX_ERROR CX_STDCALL cxPlayAudio( 
									HANDLE hPlayer,
									short * pSamp,
									int nLength, DWORD dwTimeStamp );


IPCAMSDK_API  CX_ERROR CX_STDCALL cxAudioPlayerRun( HANDLE hPlayer );
IPCAMSDK_API  CX_ERROR CX_STDCALL cxAudioPlayerStop( HANDLE hPlayer );

/************************************************************************/
/* 功能：设置音频播放回调，返回当前播放时间戳
/* 参数:
/* hPlayer 播放器句柄
/* fCallback 播放通知回调函数 
/* pUser	可在回调中返回的，用户设定变量
/* 返回：
/*		无
/************************************************************************/
IPCAMSDK_API  CX_ERROR CX_STDCALL cxAudioPlayerSetNotify( HANDLE hPlayer , FN_CQAP_NOTIFY_CALLBACK fCallback , LPVOID pUser);

IPCAMSDK_API  CX_ERROR CX_STDCALL cxAudioPlayerFreeBuf( HANDLE hPlayer , DWORD dwSizeToFree );
/*********************************************************************
 * 功能：创建音频录音机
 * 参数：
 *		nDev--录音设备编号
 * 返回：
 *		录音机句柄
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateAudioRecoder( int nDev ,int nRateIndex );

/*********************************************************************
 * 功能：关闭录音机
 * 参数：
 *		hRecorder: 录音机句柄
 * 返回：
 *		无
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseAudioRecorder( HANDLE hRecorder );

/*********************************************************************
 * 功能：开始录音
 * 参数：hRecoder:录音机句柄
 *	     fnCallback:回调函数,参见回调函数说明
 *********************************************************************/
IPCAMSDK_API  CX_ERROR CX_STDCALL cxStartRecordAudio( 
									HANDLE hRecorder,
									FN_CXAUDINCALLBACK fnCallback,
									LPVOID pContext);


/*********************************************************************
 * 功能：启动守护任务
 * 参数：无
 * 说明：守护任务完成整网络上视频服务器信息的收集工作
 *********************************************************************/
IPCAMSDK_API BOOL CX_STDCALL cxStartDeamonTask( void );
IPCAMSDK_API BOOL CX_STDCALL cxEnableDemonSearch( BOOL bSearch );

/*********************************************************************
 * 功能：结束守护任务
 * 参数：无
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxStopDeamonTask( void );

/*********************************************************************
 * 功能：取得所有已经找到的设备信息
 * 参数：
 *		pInfos 指向包含设备信息的内存地址
 * 返回:
 *      找到的设备数
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL cxGetDevFound(IP_CAM_INFO**pInfos);


/*********************************************************************
 * 功能：取得所有已经找到的设备信息(比上一个函数信息多)
 * 参数：
 *		pInfos 指向包含设备信息的内存地址
 * 返回:
 *      找到的设备数
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL cxGetDevFound2(IP_CAM_INFO2**pInfos );

/*********************************************************************
 * 功能：刷新设备信息
 * 参数：无
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxRefreshDeamon( void );

/*********************************************************************
 * 功能：直接修改设备IP地址，不需要与设备建立连接
 * 参数：
 *		 pMac	   MAC地址
 *		 dwNewIp   新IP
 *		 dwNewMask 新的掩码
 *		 dwNewGw   新的网关
 *		 pUserName 用户名
 *		 pPwd      口令
 *		 dwFlag	   参数标记
 *		 dwReturn  返回方式
 *		 dwReturnParam   设置返回参数
 * 返回:
 *		CX_ERR_NO_ERROR		成功
 *		CX_ERR_BUSY			上一次设置没有完成,需要等待
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxDirectSetIp( char * pMac,
												DWORD dwNewIp,
												DWORD dwNewMask,
												DWORD dwNewGw,
												char * pUserName,
												char * pPwd,
												DWORD dwFlag,
												DWORD dwReturn,
												DWORD dwReturnParam );
/*********************************************************************/

/*********************************************************************
 * 功能：设置设备主机的IP地址(TCP主机)
 * 参数：
 *		 pMac	   MAC地址
 *		 dwTcpHostIp 新IP
 *		 pUserName 用户名
 *		 pPwd      口令
 *		 dwFlag	   参数标记
 *		 dwReturn  返回方式
 *		 dwReturnParam   设置返回参数
 * 返回:
 *		CX_ERR_NO_ERROR		成功
 *		CX_ERR_BUSY			上一次设置没有完成,需要等待
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetWorkModeParams(char * pMac,
												DWORD	dwDnsServer,
												char * pUserName,
												char * pPwd,
												DWORD dwWorkMode,
												char * strServerAddr,
												unsigned short uServerPort,
												DWORD dwFlag,
												DWORD dwReturn,
												DWORD dwReturnParam);
/*********************************************************************/


/*********************************************************************
 *功能:创建AV文件,必须设置宽度和高度
 *参数:
 *    pFileName   文件名
 *    nWidth      视频宽度
 *    nHeight     视频高度
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateAVFile( char * pFileName,int nWidth,int nHeight , DWORD dwAudFlag );

/*********************************************************************
 *功能:创建AV文件,必须设置宽度和高度
 *参数:
 *    hAVFile   文件句柄
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseAVFile( HANDLE hAVFile );

/*********************************************************************
 *功能:写视频
 *参数:
 *		pUnit  ACT_RECV_VIDEO_FRAME 返回的参数,注意不要修改ACT_RECV_VIDEO_FRAME
 *				返回的值,将它直接传给这个函数
 *返回:
 *		-1   错误的句柄
 *		0    写文件错误
 *		>0	 写视频频长度
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL cxWriteVideo( HANDLE hFile,MEDIA_UNIT * pUnit );

/*********************************************************************
 *功能:写音频
 *参数:
 *		pUnit  ACT_RECV_AUDIO_FRAME 返回的参数,注意不要修改ACT_RECV_VIDEO_FRAME
 *				返回的值,将它直接传给这个函数
 *返回:
 *		-1   错误的句柄
 *		0    写文件错误
 *		>0	 写音频长度
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL cxWriteAudio( HANDLE hFile,MEDIA_UNIT * pUnit );

/*********************************************************************
 *功能:将YV12(4:2:0)格式转换成其他颜色空间
 *参数:
 *	  pYV12		   输出YV12格式的位图
 *	  nYV12Stride  YV12格式的Stride，本参数为Y的Stride,UV的stride为本参数的1/2
 *    nColorSpace  颜色空间,CX_COLOR_SPACE_ 打头的常量
 *    pOther	   其他颜色空间的位图
 *    nOtherStride 其他颜色空间的stride,即每一行最多有多少个像素
 *	  nWidth	   宽度
 *	  nHeight      高度
 *返回值:
 *		1		   成功
 *		0		   不能识别的颜色空间
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL YV12ToOtherColorSpace( char * pYV12,
									    int nYV12Stride,
										int nColorSpace,
									    char * pOther,
										int nOtherStride,
										int nWidth,
										int nHeight ); 

/*********************************************************************
 *功能:将其他颜色空间转换成YV12(4:2:0)格式  
 *参数:
 *    nColorSpace  颜色空间,CX_COLOR_SPACE_ 打头的常量
 *    pOther	   其他颜色空间的位图
 *    nOtherStride 其他颜色空间的stride,即每一行最多有多少个像素
 *	  pYV12		   输出YV12格式的位图
 *	  nYV12Stride  YV12格式的Stride，本参数为Y的Stride,UV的stride为本参数的1/2
 *	  nWidth	   宽度
 *	  nHeight      高度
 *返回值:
 *		1		   成功
 *		0		   不能识别的颜色空间
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL OtherColorSpaceToYV12( int nColorSpace,
									    char * pOther,
										int nOtherStride,
										char * pYV12,
									    int nYV12Stride,
										int nWidth,
										int nHeight );


//for tcp
/*********************************************************************
 *功能:启动TCP服务器
 *参数:
 *返回值:
 *		TRUE	   成功
 *		FALSE	   失败
 *********************************************************************/
IPCAMSDK_API BOOL CX_STDCALL cxStartTcpServer( void );

/*********************************************************************
 *功能:设置连接类型
 *参数:
 *		hDev 设备句柄
 *		nConnectType 连接类型
 *					#define CX_CONNECT_TYPE_UDP				0
 *					#define CX_CONNECT_TYPE_TCP				1
 *
 *		pMac 如果是TCP，传MAC地址
 *返回值:
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetConnectType( HANDLE hDev,int nConnectType,BYTE * pMac );

/*********************************************************************
 *功能:设置移动侦测参数
 *参数:
 *		hDev		 设备句柄
 *		pThredshold	 阈值，8级，共8个
 *		pLevels		 每个32*32块的级别，每个32x32块4bit,可表示0~15个级别,目前只用8个
 *		nLevelLength pLevels的字节长度
 *说明:
 *		实际计算方法为 pThredshold[pLevels[x]], X为32x32块的编号
 *返回值:
 *		无
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxSetMotionDetectParam( HANDLE hDev,
										  BOOL bStart,
										  short * pThredshold, 
										  unsigned char * pLevels,
										  int nLevelLength );


/*********************************************************************
 *功能:设置audio codec
 *参数:
 *		hDev		 设备句柄
 *	    nCodec       audio codec
 *说明:
 
 *返回值:
 *		
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxSetAudioCodec( HANDLE hDev,int nCodec );

/*********************************************************************
 *功能:释放SDK所占用的资源
 *参数: 无
 
 *说明: 在程序退出时调用
 
 *返回值:
 *		
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxCleanUp( void );

/*********************************************************************
 *功能:设置设备的叠字信息
 *参数: hDev 设备句柄
		pOsdInfo 叠字信息指针
		nLength 叠字信息结构中有效数据的长度
  *说明: 设置完信息后，只有调用cxSaveOsdInfo 才能把叠字信息保存在设备中
 
 *返回值:
 *		
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxSetOSDInfo( HANDLE hDev, LPBYTE pOsdInfo, int nLength );

/*****
得到设备的OSD信息，结果从回调中返回
******/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxGetOSDInfo( HANDLE hDev);

/*****
使设备保存已设定好的OSD信息，OSD信息永久保存在设备内部
对于经常需要变化的OSD,不必调用此函数
******/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxSaveOsdInfo( HANDLE hDev );

/*********************************************************************
 *功能:设置audio 采样率
 *参数:
 *		hDev		 设备句柄
 *	    nRateIndex 采样率索引
		CQ_AUDIO_RATE_8000 = 8K
		CQ_AUDIO_RATE_11025 = 11.025K
		CQ_AUDIO_RATE_16000 = 16K
		CQ_AUDIO_RATE_22050 = 22.05K
		CQ_AUDIO_RATE_32000 = 32K
		CQ_AUDIO_RATE_44100 = 44.1K
 *说明:
 
 *返回值:
 *		
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxSetAudioRate( HANDLE hDev ,int nRateIndex );
/*****************
得到音频采样率和压缩信息，结果从回调中返回
******************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxGetAudioInfo( HANDLE hDev);

/*********************************************************************
向设备发送音频的函数
*********************************************************************/
/*******************
建立一个媒体通道,默认情况下dwIP和pwPort可以为NULL,返回的是通道句柄
********************/
IPCAMSDK_API HANDLE		CX_STDCALL cxCreateMediaOutChn( HANDLE hDev,DWORD dwIP,WORD* pwPort );

/***********************
输出一个视频帧给设备，
hMediaOutChn	输出通道句柄
pData			需要输出的视频帧数据指针
dwLen			视频帧数据长度
dwTimeStamp		视频帧的时间戳
dwFlag			如果是关键帧则为0x10否则为0
*************************/
IPCAMSDK_API CX_ERROR	CX_STDCALL cxOutVidData( HANDLE hMediaOutChn,LPBYTE pData,DWORD dwLen,DWORD dwTimeStamp,DWORD dwFlag );

/***********************
输出一个音频帧给设备
音频帧的大小必须是640个采样，16位数据，如果没有压缩的话应该为1280字节
dwTimeStamp	为音频时间戳，如果是采集到的数据话，可以用GetTickCount()
dwFlag		CQ_AUDIO_RATE_8000 | CQ_IS_AUDIO_COMPRESSED 即采样率或上是否压缩
*************************/
IPCAMSDK_API CX_ERROR	CX_STDCALL cxOutAudData( HANDLE hMediaOutChn,LPBYTE pData,DWORD dwLen,DWORD dwTimeStamp,DWORD dwFlag );

/******************
关闭媒体输出通道
*********************/
IPCAMSDK_API CX_ERROR	CX_STDCALL cxCloseMediaOutChn( HANDLE hMediaOutChn );


//重启设备
IPCAMSDK_API CX_ERROR	CX_STDCALL cxResetDevice( HANDLE hDev );

//在与设备连接后，改变设备的工作模式
IPCAMSDK_API CX_ERROR	CX_STDCALL cxSetWorkMode( HANDLE hDev , PCQWORKMODE pstuWorkMode );
IPCAMSDK_API CX_ERROR	CX_STDCALL cxGetWorkMode( HANDLE hDev );//得到设备工作模式，回调中返回

/*********************************************************************
音频采集函数
*********************************************************************/
/*
IPCAMSDK_API CX_ERROR	CX_STDCALL cxGetAudInDev( LPTSTR strDevNames , PDWORD pdwCount );		
IPCAMSDK_API HANDLE		CX_STDCALL cxCreateAudInChn( LPTSTR strDevName );
IPCAMSDK_API CX_ERROR	CX_STDCALL cxSetAudInCallback( HANDLE hAUdIn );
IPCAMSDK_API CX_ERROR	CX_STDCALL cxAudInStartCapture( HANDLE hAudIn );
IPCAMSDK_API CX_ERROR	CX_STDCALL cxAudInStopCapture( HANDLE hAudIn );
IPCAMSDK_API CX_ERROR	CX_STDCALL cxSetAudinSampleRate( HANDLE hAudIn , DWORD dwSampleIndex );
IPCAMSDK_API DWORD		CX_STDCALL cxGetAudinSampleRate( HANDLE hAudIn );
*/
#ifdef __cplusplus
}
#endif

#endif

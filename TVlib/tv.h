#ifndef _TVPLAYER_H
#define _TVPLAYER_H

#include <stdio.h>
#include <Windows.h>


//定义显示的电视通道数
#define TVChannelNum 2

typedef struct  TV_CFG
{
	bool play_flag;
	bool Frost_flag;
	short startX;
	short startY;
	short Width;
	short Height;
}TVcfg,*ptvcfg;

//视频
typedef struct TV_VIDEO_CFG
{
	HWND    m_TVHwnd;         //视频显示的窗口句柄
	DWORD   m_dwAddress;      //摄像机的IP地址

	HANDLE	m_hCarama;        //摄像机句柄	
	HANDLE	m_hDisplay;	       //视频显示通道句柄
	HANDLE	m_hDecoder;	       //视频解码器句柄
	LPBYTE	m_pDecoderBuffer;	//视频解码缓冲区

}TvVideoCfg,*pTvVideoCfg;

typedef struct USER_INFO
{
	char* pUserName; // 用户名
	char* pPwd;  // 密码  
}UserInfo,pUserInfo;
 
class TVPlayer
{

private:
	TVcfg   tv_cfg[TVChannelNum];

	TvVideoCfg video_cfg[TVChannelNum];
	
	HANDLE AVRecordFile[TVChannelNum];

	bool IsAudioPlay[TVChannelNum];

	UserInfo  userinfo[TVChannelNum];

	//本地音频播放器
	HANDLE	m_LocalAudioPlayer;

public:
	DWORD DecodeBufferLen;
public:
	TVPlayer();
	~TVPlayer();

	void TV_Init(short VideoChannel,HWND VideoHwnd,char* pstrAddr,char* pUsrName="galileo",char* pPwd="ipcam");	
	//---------------------------------------电视对外接口-----------------------------------------------------------
	//打开电视
	int ML_GVID_OpenTv (short theDisplay,short VideoChannel ,short theX, short theY,short theWidth,short theHeight);
	//关闭电视
	int ML_GVID_CloseTv (short theDisplay,short VideoChannel );
	//设置窗口位置
	int ML_GVID_SetTvXY(short theDisplay,short VideoChannel ,short theX, short theY);
	//设置窗口大小
	int ML_GVID_SetTvWH(short theDisplay,short VideoChannel ,short theWidth, short theHeight);
	//冻结电视
	void ML_GVID_FrostTV(short theDisplay,short VideoChannel);
	//解冻电视
	void ML_GVID_UnFrostTV(short theDisplay,short VideoChannel);
	//改变电视亮度
	void ML_GVID_ChangTvBright(short theDisplay,short VideoChannel,int theBright);
	//改变电视对比度
	void ML_GVID_ChangTvContrast(short theDisplay,short VideoChannel,int theContrast);
	//调整电视色彩
	void ML_GVID_ChangTvColor(short theDisplay,short VideoChannel,int theColor);
	//---------------------------------------------结束---------------------------------------------------------------
	//记录音视频文件
	void tshStartRecordAVFile(short VideoChannel,char *FileName = NULL);
	void tshCloseRecordAVFile(short VideoChannel);
	void tshPlayAudio(short VideoChannel);
	void tshCloseAudio(short VideoChannel);
	bool tshSetOSDInfo(short VideoChannel,LPBYTE pOSDData,RECT OSD_Rect,DWORD dwCOlorSpcae);
	void tshSetOSDStr(short VideoChannel,char* pstr,short x,short y);
	void tshSetOSDTime(short VideoChannel,short x,short y);

	// 	* 功能：发送透明数据* pData 数据* nDataLen 数据长度 * nChnNum ---- 0 is 485,1 is 232
	int tshSendTransparentData(short VideoChannel,char* pData,int nLen,int nChnNum);
	int tshVerifyTime(short VideoChannel);// 功能：校时
private:
	BOOL tshStartWindowMoveHook();
	BOOL tshCloseWindowMoveHook();

protected:
// 	typedef int (*pFunc[TVChannelNum])(LPVOID pContext,HANDLE hIpCam,int nAct,
// 									   DWORD dwParam1,DWORD dwParam2 );
	//回调函数
	static int  __stdcall  CommonIpcamCallback( 
												LPVOID pContext,
												HANDLE hIpCam,
												int nAct,
												DWORD dwParam1,
												DWORD dwParam2 );
	int IpCamCallback(HANDLE hIpCam,
						int nAct,
						DWORD dwParam1,
						DWORD dwParam2 );


};

#endif
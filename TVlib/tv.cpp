//#include "StdAfx.h"
#include "tv.h"
#include <stddef.h>
#include <winsock.h>
#include "cxIpCamApi.h"
#include "HxPlayerSdk.h"
#include "GeVideoSDK.h"


//#pragma comment(lib,"ws2_32.lib")
//#pragma comment(lib,"IpCamSdk.lib")
//#pragma comment(lib,"HxPlayerSDK.lib")
//#pragma comment(lib,"GeVideoLib.lib")

#define VIDEO_WIDTH  704
#define VIDEO_HEIGHT 576


#define PIC_SIZE  704*576

//安装窗口移动钩子
static HHOOK WindowMoveHook=NULL; 

static HWND  hwnd_Window[TVChannelNum];
static HANDLE display_Window[TVChannelNum];
LRESULT CALLBACK  CallWndProc(int nCode,WPARAM wParam,LPARAM lParam);

TVPlayer::TVPlayer()
{		
	DecodeBufferLen = gevdGetSizeByFourCC(PIC_SIZE,MAKEFOURCC('I','4','2','0'));
	//设置视频
	for(int i=0;i < TVChannelNum;i++)
	{
		video_cfg[i].m_hCarama  =  0;
		video_cfg[i].m_hDecoder = 0;
		video_cfg[i].m_hDisplay = 0;	
		video_cfg[i].m_pDecoderBuffer = new BYTE [DecodeBufferLen];
		video_cfg[i].m_TVHwnd = NULL;
		video_cfg[i].m_dwAddress = INADDR_NONE; //使用IP地址连接，UDP设备
		//----------------------------------------------------------------------
		tv_cfg[i].play_flag  = false;
		tv_cfg[i].Frost_flag = false;

		//设置记录视频文件
		AVRecordFile[i] = NULL;

		IsAudioPlay[i] = false;
	}
	//创建本地音频播放器
	m_LocalAudioPlayer = cxCreateAudioPlayer(-1,0);

	//打开服务器
	cxStartTcpServer();
	//设置HOOK访问
	tshStartWindowMoveHook();
}
TVPlayer::~TVPlayer()
{
	cxCleanUp();
	//释放解码缓冲区
	for (int i=0;i < TVChannelNum;i++)
	{
		if(video_cfg[i].m_pDecoderBuffer != NULL)
		{
			delete video_cfg[i].m_pDecoderBuffer;
			video_cfg[i].m_pDecoderBuffer = NULL;
		}

		//关闭录像文件
		if(AVRecordFile[i] != NULL)
		{
			cxCloseAVFile(AVRecordFile[i]);
			AVRecordFile[i] = NULL;
		}
	}

	tshCloseWindowMoveHook();
	cxCloseAudioPlayer(m_LocalAudioPlayer);
    m_LocalAudioPlayer = NULL;

}
BOOL TVPlayer::tshStartWindowMoveHook()
{
	WindowMoveHook = ::SetWindowsHookEx(WH_CALLWNDPROC,CallWndProc,NULL,::GetCurrentThreadId());
	if(WindowMoveHook != NULL) return  TRUE;
	return FALSE;
}
//卸载钩子
BOOL TVPlayer::tshCloseWindowMoveHook() 
{
	if(WindowMoveHook) return ::UnhookWindowsHookEx(WindowMoveHook);
	else 
		return FALSE;
}

void TVPlayer::TV_Init(short VideoChannel,HWND VideoHwnd,char * pstrAddr,char * pUsrName,char * pPassword)
{
		video_cfg[VideoChannel].m_dwAddress = inet_addr(pstrAddr);//使用IP地址连接，UDP设备
		video_cfg[VideoChannel].m_TVHwnd = VideoHwnd;
		hwnd_Window[VideoChannel] = VideoHwnd;

		userinfo[VideoChannel].pUserName = pUsrName;
		userinfo[VideoChannel].pPwd = pPassword;

}
int TVPlayer::ML_GVID_OpenTv (short theDisplay,short VideoChannel ,short theX, short theY,short theWidth,short theHeight)
{
	RECT rect;
	if(!tv_cfg[VideoChannel].play_flag)
	{
		tv_cfg[VideoChannel].play_flag=TRUE;

		tv_cfg[VideoChannel].startX = theX;
		tv_cfg[VideoChannel].startY = theY;
		tv_cfg[VideoChannel].Width = theWidth;
		tv_cfg[VideoChannel].Height = theHeight;
		
		rect.left =  theX;
		rect.top = theY;
		rect.right = theWidth+rect.left;
		rect.bottom = theHeight+rect.top;
		//打开设备
		video_cfg[VideoChannel].m_hCarama  = cxOpenIpCam(video_cfg[VideoChannel].m_dwAddress,CommonIpcamCallback,userinfo[VideoChannel].pUserName,userinfo[VideoChannel].pPwd,NULL,this);
		//初始化视频解码器
		video_cfg[VideoChannel].m_hDecoder = cxCreateDecoder(VIDEO_WIDTH,VIDEO_HEIGHT);
		//创建视频显示通道
		video_cfg[VideoChannel].m_hDisplay = gevdCreateDisplay(video_cfg[VideoChannel].m_TVHwnd,VIDEO_WIDTH,VIDEO_HEIGHT,MAKEFOURCC('I','4','2','0'),GEVDS_OVERLAY);
		
		display_Window[VideoChannel] = video_cfg[VideoChannel].m_hDisplay;
		
		//连接设备
		DWORD dwError = cxConnectIpCam(video_cfg[VideoChannel].m_hCarama,video_cfg[VideoChannel].m_dwAddress);
		//gevdMoveVideo(video_cfg[VideoChannel].m_hDisplay,rect);
		return dwError;
	}
	return -1;
}
//关闭电视
int TVPlayer::ML_GVID_CloseTv (short theDisplay,short VideoChannel )
{
	if(tv_cfg[VideoChannel].play_flag == TRUE)
	{
		tv_cfg[VideoChannel].play_flag=FALSE;

		//本地停止接收视频
		cxStopRecvVideo(video_cfg[VideoChannel].m_hCarama);
		//远端停止发送视频
		cxStopVideo(video_cfg[VideoChannel].m_hCarama);
		//关闭视频解码器
		cxCloseDecoder(video_cfg[VideoChannel].m_hDecoder);

		//关闭视频显示通道
		if(video_cfg[VideoChannel].m_hDisplay != NULL)
		{
			gevdCloseDisplay(video_cfg[VideoChannel].m_hDisplay);
			video_cfg[VideoChannel].m_hDisplay = NULL;
		}
		//关闭摄像机连接
		cxCloseIpCam(video_cfg[VideoChannel].m_hCarama);

		return 0;
	}
	return -1;
}

//设置窗口位置
int TVPlayer::ML_GVID_SetTvXY(short theDisplay,short VideoChannel ,short theX, short theY)
{ 
	RECT rect;
		if(tv_cfg[VideoChannel].play_flag==TRUE)
			{
				tv_cfg[VideoChannel].startX = theX;
				tv_cfg[VideoChannel].startY = theY;

				//rect.left = theX;
				//rect.right = tv_cfg[VideoChannel].Width+theX;

				//rect.top = theY;
				//rect.bottom = tv_cfg[VideoChannel].Height+theY;
				//gevdMoveVideo(video_cfg[VideoChannel].m_hDisplay,rect);
				return 0;
			}
				return -1;
}
//设置窗口大小
int TVPlayer::ML_GVID_SetTvWH(short theDisplay,short VideoChannel ,short theWidth, short theHeight)
{
		RECT rect;
		if(tv_cfg[VideoChannel].play_flag==TRUE)
		{	
				tv_cfg[VideoChannel].Width = theWidth;
				tv_cfg[VideoChannel].Height = theHeight;

				//rect.left = tv_cfg[VideoChannel].startX ;
				//rect.right =theWidth+rect.left;
				//
				//rect.top = tv_cfg[VideoChannel].startY ;
				//rect.bottom = theHeight+rect.top;
				//gevdMoveVideo(video_cfg[VideoChannel].m_hDisplay,rect);
				return 0;
			}
			return -1;
}
//冻结电视
void TVPlayer::ML_GVID_FrostTV(short theDisplay,short VideoChannel)
{
	if((tv_cfg[VideoChannel].play_flag==TRUE)&&(tv_cfg[VideoChannel].Frost_flag==FALSE))			
	{
		tv_cfg[VideoChannel].Frost_flag=TRUE;
		cxStopRecvVideo(video_cfg[VideoChannel].m_hCarama);
	}
}
//解冻电视
void TVPlayer::ML_GVID_UnFrostTV(short theDisplay,short VideoChannel)
{
	if(tv_cfg[VideoChannel].play_flag&&tv_cfg[VideoChannel].Frost_flag==TRUE)				
	{
		tv_cfg[VideoChannel].Frost_flag=FALSE;
		cxStartRecvVideo(video_cfg[VideoChannel].m_hCarama,0,0);
	}
}
//改变电视亮度
void TVPlayer::ML_GVID_ChangTvBright(short theDisplay,short VideoChannel,int theBright)
{
	if((tv_cfg[VideoChannel].play_flag==TRUE)&&(tv_cfg[VideoChannel].Frost_flag==FALSE))			
	{
		cxSetViBrightness(video_cfg[VideoChannel].m_hCarama,theBright);
	}
	
}
//改变电视对比度
void TVPlayer::ML_GVID_ChangTvContrast(short theDisplay,short VideoChannel,int theContrast)
{	
	if((tv_cfg[VideoChannel].play_flag==TRUE)&&(tv_cfg[VideoChannel].Frost_flag==FALSE))			
	{
		cxSetViContrast(video_cfg[VideoChannel].m_hCarama,theContrast);
	}
	

}
//调整电视色彩
void TVPlayer::ML_GVID_ChangTvColor(short theDisplay,short VideoChannel,int theColor)
{	
	if((tv_cfg[VideoChannel].play_flag==TRUE)&&(tv_cfg[VideoChannel].Frost_flag==FALSE))			
	{
		cxSetViSaturation(video_cfg[VideoChannel].m_hCarama,theColor);
	}
	

}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
bool TVPlayer::tshSetOSDInfo(short VideoChannel,LPBYTE pOSDData,RECT OSD_Rect,DWORD dwCOlorSpcae)
{
	if((tv_cfg[VideoChannel].play_flag==TRUE)&&(tv_cfg[VideoChannel].Frost_flag==FALSE))
	{
		gevdSetOSDInfo(video_cfg[VideoChannel].m_hDisplay,pOSDData,OSD_Rect,dwCOlorSpcae);
		return true;
	}
	return false;
}
void TVPlayer::tshSetOSDStr(short VideoChannel,char* pstr,short x,short y)
{
	CQ_OSD_INFO stuOsdInfo;
	memset(&stuOsdInfo,0,sizeof(stuOsdInfo));

	stuOsdInfo.uValidArea = CQ_OSDVALID_CHARDATA|CQ_OSDVALID_OSDPOS|CQ_OSDVALID_STATE;

	stuOsdInfo.uOSDState = CQ_OSD_CHAR;
	strncpy(stuOsdInfo.szCharData,pstr,32);
	stuOsdInfo.sOSDPosX = x;
	stuOsdInfo.sOSDPosY = y;

	cxSetOSDInfo(video_cfg[VideoChannel].m_hCarama,(LPBYTE)&stuOsdInfo,offsetof(CQ_OSD_INFO,pCharMask));
}
void TVPlayer::tshSetOSDTime(short VideoChannel,short x,short y)
{
	CQ_OSD_INFO stuOsdInfo;
	memset(&stuOsdInfo,0,sizeof(stuOsdInfo));
	
	stuOsdInfo.uValidArea = CQ_OSDVALID_TIMEPOS|CQ_OSDVALID_STATE;

	stuOsdInfo.uOSDState = CQ_OSD_TIME;
	stuOsdInfo.sTimePosX = x;
	stuOsdInfo.sTimePosY = y;
	
	cxSetOSDInfo(video_cfg[VideoChannel].m_hCarama,(LPBYTE)&stuOsdInfo,offsetof(CQ_OSD_INFO,pCharMask));
}
int TVPlayer::tshVerifyTime(short VideoChannel)
{
	return cxVerifyTime(video_cfg[VideoChannel].m_hCarama);
}
void TVPlayer::tshStartRecordAVFile(short VideoChannel,char *FileName)
{
	if(!FileName) 
	{
		char str[25];
		sprintf(str,"AVRecordFile_%d.mpg",VideoChannel);
		FileName = str;
	}
	AVRecordFile[VideoChannel] = cxCreateAVFile(FileName,VIDEO_WIDTH,VIDEO_HEIGHT,0);
}

void TVPlayer::tshCloseRecordAVFile(short VideoChannel)
{
	//关闭录像文件
	if(AVRecordFile[VideoChannel] != NULL)
	{
		cxCloseAVFile(AVRecordFile[VideoChannel]);
		AVRecordFile[VideoChannel] = NULL;
	}
}
int  TVPlayer::tshSendTransparentData(short VideoChannel,char* pData,int nLen,int nChnNum)
{
	return cxSendTransparentData(video_cfg[VideoChannel].m_hCarama,pData,nLen,nChnNum);
}
void TVPlayer::tshPlayAudio(short VideoChannel)
{
	IsAudioPlay[VideoChannel] = true;
}
void TVPlayer::tshCloseAudio(short VideoChannel)
{
	IsAudioPlay[VideoChannel] = false;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int  TVPlayer::CommonIpcamCallback( LPVOID pContext,
						HANDLE hIpCam,
						int nAct,
						DWORD dwParam1,
						DWORD dwParam2 )
{
	return ((TVPlayer*)pContext)->IpCamCallback(hIpCam,nAct,dwParam1,dwParam2);
}

int TVPlayer::IpCamCallback(HANDLE hIpCam,
						int nAct,
						DWORD dwParam1,
						DWORD dwParam2 )
{
	int VideoChannel = (hIpCam == video_cfg[0].m_hCarama) ? 0 : 1;
	switch(nAct)
	{

	case ACT_CONNECT_FAILED:
		{
			//TRACE("Connect Fail\n");
		}
		break;

	case ACT_CONNECT_OK:
		{
			cxStartRecvVideo(hIpCam,0,0);//本地开始接收视频
			cxStartVideo(hIpCam,0,0);//远端开始发送视频
			
			cxStartRecvAudio(hIpCam,0,0);//本地开始接收音频
			cxStartAudio(hIpCam,0,0);//远端开始发送音频

			cxForceIFrame(hIpCam);
			//获得开关量信息
			//cxGetAlarmIn(hIpCam);
			//::MessageBox(NULL,"ok","OK!",NULL);
		}
		break;
	case ACT_RECV_VIDEO_FRAME:
		{
			MEDIA_UNIT* pVideoUnit = (MEDIA_UNIT*)dwParam1;
			//解码一帧图像
			cxDecodeFrame(video_cfg[VideoChannel].m_hDecoder,pVideoUnit->pBuf,pVideoUnit->nLength,(char*)video_cfg[VideoChannel].m_pDecoderBuffer,-1,0);		
			//显示一帧图像	
			gevdPushVideo(video_cfg[VideoChannel].m_hDisplay,video_cfg[VideoChannel].m_pDecoderBuffer,DecodeBufferLen,MAKEFOURCC('I','4','2','0'));
			//视频帧存盘
			if(AVRecordFile[VideoChannel]!= NULL)
			{
				cxWriteVideo(AVRecordFile[VideoChannel],pVideoUnit);
			}
		}
		break;
	case ACT_RECV_AUDIO_FRAME:
		{
			MEDIA_UNIT* pAudioUnit = (MEDIA_UNIT*)dwParam1;
			//音频帧存盘
			if(AVRecordFile[VideoChannel]!= NULL)
			{
				cxWriteAudio(AVRecordFile[VideoChannel],pAudioUnit);
			}
			if(IsAudioPlay[VideoChannel])//播放声音
			{
				cxPlayAudio(m_LocalAudioPlayer,(short*)pAudioUnit->pBuf,pAudioUnit->nLength/2,pAudioUnit->dwTimeStamp);
			}
		}
		break;
		case ACT_ALARM_IN: //告警信息处理
			break;
	}

	return 0;
}

LRESULT CALLBACK  CallWndProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	
	if (nCode < 0)  // do not process message 
		return CallNextHookEx(WindowMoveHook, nCode, wParam, lParam); 
	
	PCWPSTRUCT ptr = (PCWPSTRUCT)lParam;
	RECT rect;
	
	switch (nCode)
	{
	case HC_ACTION:			
		if(ptr->message == WM_MOVE||ptr->message == WM_SIZE)
		{
			if(ptr->hwnd == hwnd_Window[0])
			{	
				::GetClientRect(hwnd_Window[0],&rect);
				::ClientToScreen(hwnd_Window[0],(LPPOINT) &rect);
				::ClientToScreen(hwnd_Window[0],(LPPOINT)&rect+1);
				gevdMoveVideo(display_Window[0],rect);
			}
			else if(ptr->hwnd == hwnd_Window[1])
			{
				::GetClientRect(hwnd_Window[1],&rect);
				::ClientToScreen(hwnd_Window[1],(LPPOINT) &rect);
				::ClientToScreen(hwnd_Window[1],(LPPOINT) &rect+1);
				gevdMoveVideo(display_Window[1],rect);	
			}
		}
		break;
	default:
		break;
	}
	
    return CallNextHookEx(WindowMoveHook, nCode, wParam,lParam); 
}




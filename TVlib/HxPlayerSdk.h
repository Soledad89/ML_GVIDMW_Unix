///////////////////////////////////////////////////////////////////
//File:			HxPlayerSdk.h
//Date:			2004-07-16
//Description:	Interface for HxPlayerSdk.dll
//Copyright:	Beijing Co,.Ltd 2002-2008. All rights reserved.
///////////////////////////////////////////////////////////////////
#ifndef _HX_PLAYER_SDK
#define _HX_PLAYER_SDK

#if defined( _WINDLL)
	#define HX_PLAYER_API  extern "C"__declspec(dllexport) 
#else 
	#define HX_PLAYER_API  extern "C" __declspec(dllimport) 
#endif

#ifndef HX_STDCALL
#define HX_STDCALL	__stdcall
#endif	//HX_STDCALL
//Max channel numbers
#define HX_PLAYER_MAX_SUPPORTS 100
//Wave coef range;
#define MIN_WAVE_COEF -100
#define MAX_WAVE_COEF 100
//Timer type
#define TIMER_1 1 //Only 16 timers for every process.Default TIMER;
#define TIMER_2 2 //Not limit;But the precision less than TIMER_1; 
//BUFFER TYPE
#define BUF_VIDEO_SRC 1
#define BUF_AUDIO_SRC 2
#define BUF_VIDEO_RENDER 3
#define BUF_AUDIO_RENDER 4
//Error code
#define  HX_PLAYER_NOERROR					0	//no error
#define	 HX_PLAYER_PARA_OVER				1	//input parameter is invalid;
#define  HX_PLAYER_ORDER_ERROR				2	//The order of the function to be called is error.
#define	 HX_PLAYER_TIMER_ERROR				3	//Create multimedia clock failed;
#define  HX_PLAYER_DEC_VIDEO_ERROR			4	//Decode video data failed.
#define  HX_PLAYER_DEC_AUDIO_ERROR			5	//Decode audio data failed.
#define	 HX_PLAYER_ALLOC_MEMORY_ERROR		6	//Allocate memory failed.
#define  HX_PLAYER_OPEN_FILE_ERROR			7	//Open the file failed.
#define  HX_PLAYER_CREATE_OBJ_ERROR		8	//Create thread or event failed
#define  HX_PLAYER_CREATE_DDRAW_ERROR		9	//Create DirectDraw object failed.
#define  HX_PLAYER_CREATE_OFFSCREEN_ERROR 10	//failed when creating off-screen surface.
#define  HX_PLAYER_BUF_OVER			   11	//buffer is overflow
#define  HX_PLAYER_CREATE_SOUND_ERROR	   12	//failed when creating audio device.	
#define	 HX_PLAYER_SET_VOLUME_ERROR	   13	//Set volume failed
#define  HX_PLAYER_SUPPORT_FILE_ONLY	   14	//The function only support play file.
#define  HX_PLAYER_SUPPORT_STREAM_ONLY	   15	//The function only support play stream.
#define  HX_PLAYER_SYS_NOT_SUPPORT		   16	//System not support.
#define  HX_PLAYER_FILEHEADER_UNKNOWN     17	//No file header.
#define  HX_PLAYER_VERSION_INCORRECT	   18	//The version of decoder and encoder is not adapted.  
#define  HX_PLAYER_INIT_DECODER_ERROR     19	//Initialize decoder failed.
#define  HX_PLAYER_CHECK_FILE_ERROR	   20	//The file data is unknown.
#define  HX_PLAYER_INIT_TIMER_ERROR	   21	//Initialize multimedia clock failed.
#define	 HX_PLAYER_BLT_ERROR			   22	//Blt failed.
#define  HX_PLAYER_UPDATE_ERROR		   23	//Update failed.

//Max display regions.
#define MAX_DISPLAY_WND 4

//Display type
#define DISPLAY_NORMAL  1   
#define DISPLAY_QUARTER 2	

//Display buffers
#define MAX_DIS_FRAMES 50
#define MIN_DIS_FRAMES 6

//Locate by
#define BY_FRAMENUM  1
#define BY_FRAMETIME 2

//Source buffer
#define SOURCE_BUF_MAX	1024*100000
#define SOURCE_BUF_MIN	1024*50

//Stream type
#define STREAME_REALTIME 0
#define STREAME_FILE	 1

//frame type
#define T_AUDIO16	101
#define T_UYVY		1
#define T_YV12		3
#define T_RGB32		7
//capability
#define SUPPORT_DDRAW		1 
#define SUPPORT_BLT         2 
#define SUPPORT_BLTFOURCC   4 
#define SUPPORT_BLTSHRINKX  8 
#define SUPPORT_BLTSHRINKY  16
#define SUPPORT_BLTSTRETCHX 32
#define SUPPORT_BLTSTRETCHY 64
#define SUPPORT_SSE         128
#define SUPPORT_MMX			256 

//Frame position
typedef struct{
	long nFilePos;
	long nFrameNum;
	long nFrameTime;
}FRAME_POS,*PFRAME_POS;

//Frame Info
typedef struct{
	long nWidth;
	long nHeight;
	long nStamp;
	long nType;
	long nFrameRate;
}FRAME_INFO;

#define HX_PLAYER_MODE_PARSE		0x1	//播放时，解析文件
#define HX_PLAYER_MODE_DECODEVIDEO	0x2	//播放时解码视频
#define HX_PLAYER_MODE_DECODEAUDIO	0x4	//播放时解码音频 
#define HX_PLAYER_MODE_DISPLAY		0x8	//播放时显示
#define HX_PLAYER_MODE_PLAYSOUND	0x10	//播放时播放声音	

//////////////////////////////////////////////////////////////////////////////
//API
//////////////////////////////////////////////////////////////////////////////

////////////////ver 1.0///////////////////////////////////////
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_OpenFile(LONG nPort,LPCTSTR sFileName);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_CloseFile(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_Play(LONG nPort, HWND hWnd);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_Stop(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_Pause(LONG nPort,DWORD nPause);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_Fast(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_Slow(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_OneByOne(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetPlayPos(LONG nPort,float fRelativePos);
HX_PLAYER_API float HX_STDCALL HX_PLAYER_GetPlayPos(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetFileEndMsg(LONG nPort,HWND hWnd,UINT nMsg);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetVolume(LONG nPort,WORD nVolume);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_StopSound(LONG nPort);//停止播放声音
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_PlaySound(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_OpenStream(LONG nPort,PBYTE pFileHeadBuf,DWORD nSize,DWORD nBufPoolSize);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_InputData(LONG nPort,PBYTE pBuf,DWORD nSize);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_CloseStream(LONG nPort);
HX_PLAYER_API int  HX_STDCALL  HX_PLAYER_GetCaps(void);
HX_PLAYER_API DWORD HX_STDCALL HX_PLAYER_GetFileTime(LONG nPort);
HX_PLAYER_API DWORD HX_STDCALL HX_PLAYER_GetPlayedTime(LONG nPort);
HX_PLAYER_API DWORD HX_STDCALL HX_PLAYER_GetPlayedFrames(LONG nPort);

////////////////ver 2.0 added///////////////////////////////////////
typedef void (CALLBACK* HX_PLAYER_DECCBFUN)(long nPort,char * pBuf,long nSize,FRAME_INFO * pFrameInfo, long nReserved1,long nReserved2);
HX_PLAYER_API BOOL HX_STDCALL	HX_PLAYER_SetDecCallBack(LONG nPort,HX_PLAYER_DECCBFUN fDecCallback);
typedef void (CALLBACK* HX_PLAYER_DISPLAYCBFUN)(long nPort,char * pBuf,long nSize,long nWidth,long nHeight,long nStamp,long nType,long nReserved);
HX_PLAYER_API BOOL HX_STDCALL	HX_PLAYER_SetDisplayCallBack(LONG nPort,HX_PLAYER_DISPLAYCBFUN fDispCallback);
HX_PLAYER_API BOOL HX_STDCALL	HX_PLAYER_ConvertToBmpFile(char * pBuf,long nSize,long nWidth,long nHeight,long nType,char *sFileName);
HX_PLAYER_API DWORD HX_STDCALL	HX_PLAYER_GetFileTotalFrames(LONG nPort);
HX_PLAYER_API DWORD HX_STDCALL	HX_PLAYER_GetCurrentFrameRate(LONG nPort);
HX_PLAYER_API DWORD HX_STDCALL	HX_PLAYER_GetPlayedTimeEx(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL	HX_PLAYER_SetPlayedTimeEx(LONG nPort,DWORD nTime);
HX_PLAYER_API DWORD HX_STDCALL	HX_PLAYER_GetCurrentFrameNum(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL	HX_PLAYER_SetStreamOpenMode(LONG nPort,DWORD nMode);
HX_PLAYER_API DWORD HX_STDCALL	HX_PLAYER_GetFileHeadLength(void);
HX_PLAYER_API DWORD HX_STDCALL	HX_PLAYER_GetSdkVersion(void);

////////////////ver 2.2 added///////////////////////////////////////
HX_PLAYER_API DWORD HX_STDCALL  HX_PLAYER_GetLastError(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_RefreshPlay(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetOverlayMode(LONG nPort,BOOL bOverlay,COLORREF colorKey);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_GetPictureSize(LONG nPort,LONG *pWidth,LONG *pHeight);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetPicQuality(LONG nPort,BOOL bHighQuality);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_PlaySoundShare(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_StopSoundShare(LONG nPort);

////////////////ver 2.4 added///////////////////////////////////////
HX_PLAYER_API LONG HX_STDCALL HX_PLAYER_GetStreamOpenMode(LONG nPort);
HX_PLAYER_API LONG HX_STDCALL HX_PLAYER_GetOverlayMode(LONG nPort);
HX_PLAYER_API COLORREF HX_STDCALL HX_PLAYER_GetColorKey(LONG nPort);
HX_PLAYER_API WORD HX_STDCALL HX_PLAYER_GetVolume(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_GetPictureQuality(LONG nPort,BOOL *bHighQuality);
HX_PLAYER_API DWORD HX_STDCALL HX_PLAYER_GetSourceBufferRemain(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_ResetSourceBuffer(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetSourceBufCallBack(LONG nPort,DWORD nThreShold,void (CALLBACK * SourceBufCallBack)(long nPort,DWORD nBufSize,DWORD dwUser,void*pResvered),DWORD dwUser,void *pReserved);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_ResetSourceBufFlag(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetDisplayBuf(LONG nPort,DWORD nNum);
HX_PLAYER_API DWORD HX_STDCALL HX_PLAYER_GetDisplayBuf(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_OneByOneBack(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetFileRefCallBack(LONG nPort, void (CALLBACK *pFileRefDone)(DWORD nPort,DWORD nUser),DWORD nUser);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetCurrentFrameNum(LONG nPort,DWORD nFrameNum);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_GetKeyFramePos(LONG nPort,DWORD nValue, DWORD nType, PFRAME_POS pFramePos);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_GetNextKeyFramePos(LONG nPort,DWORD nValue, DWORD nType, PFRAME_POS pFramePos);
#if (WINVER > 0x0400)
//Note: These funtion must be builded under win2000 or above with Microsoft Platform sdk.
//	    You can download the sdk from "http://www.microsoft.com/msdownload/platformsdk/sdkupdate/";
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_InitDDrawDevice(void);
HX_PLAYER_API void HX_STDCALL HX_PLAYER_ReleaseDDrawDevice(void);
HX_PLAYER_API DWORD HX_STDCALL HX_PLAYER_GetDDrawDeviceTotalNums(void);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetDDrawDevice(LONG nPort,DWORD nDeviceNum);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_GetDDrawDeviceInfo(DWORD nDeviceNum,LPSTR  lpDriverDescription,DWORD nDespLen,LPSTR lpDriverName ,DWORD nNameLen,HMONITOR *hhMonitor);
HX_PLAYER_API int  HX_STDCALL  HX_PLAYER_GetCapsEx(DWORD nDDrawDeviceNum);
#endif
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_ThrowBFrameNum(LONG nPort,DWORD nNum);

////////////////ver 2.5 added///////////////////////////////////////
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetDisplayType(LONG nPort,LONG nType);
HX_PLAYER_API long HX_STDCALL HX_PLAYER_GetDisplayType(LONG nPort);

////////////////ver 3.0 added///////////////////////////////////////
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetDecCBStream(LONG nPort,DWORD nStream);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetDisplayRegion(LONG nPort,DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_RefreshPlayEx(LONG nPort,DWORD nRegionNum);
#if (WINVER > 0x0400)
//Note: The funtion must be builded under win2000 or above with Microsoft Platform sdk.
//	    You can download the sdk from http://www.microsoft.com/msdownload/platformsdk/sdkupdate/;
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetDDrawDeviceEx(LONG nPort,DWORD nRegionNum,DWORD nDeviceNum);
#endif
/////////////////v3.2 added/////////////////////////////////////////

HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_GetRefValue(LONG nPort,BYTE *pBuffer, DWORD *pSize);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetRefValue(LONG nPort,BYTE *pBuffer, DWORD nSize);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_OpenStreamEx(LONG nPort,PBYTE pFileHeadBuf,DWORD nSize,DWORD nBufPoolSize);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_CloseStreamEx(LONG nPort);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_InputVideoData(LONG nPort,PBYTE pBuf,DWORD nSize);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_InputAudioData(LONG nPort,PBYTE pBuf,DWORD nSize);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_RigisterDrawFun(LONG nPort,void (CALLBACK* DrawFun)(long nPort,HDC hDc,LONG nUser),LONG nUser);
//////////////////v3.4/////////////////////////////////////////////////////
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetTimerType(LONG nPort,DWORD nTimerType,DWORD nReserved);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_GetTimerType(LONG nPort,DWORD *pTimerType,DWORD *pReserved);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_ResetBuffer(LONG nPort,DWORD nBufType);
HX_PLAYER_API DWORD HX_STDCALL HX_PLAYER_GetBufferValue(LONG nPort,DWORD nBufType);
//////////////////V3.6/////////////////////////////////////////////////////////
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_AdjustWaveAudio(LONG nPort,LONG nCoefficient);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetVerifyCallBack(LONG nPort, DWORD nBeginTime, DWORD nEndTime, void (CALLBACK * funVerify)(long nPort, FRAME_POS * pFilePos, DWORD bIsVideo, DWORD nUser),  DWORD  nUser);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_SetAudioCallBack(LONG nPort, void (CALLBACK * funAudio)(long nPort, char * pAudioBuf, long nSize, long nStamp, long nType, long nUser), long nUser);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_GrabJPEGPic(LONG nPort,LPTSTR lpFileName,LONG lQuality);//抓拍图片
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_FinalRelease(void);
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_ReversePlay(LONG nPort);	//以与当前播放方向相反的方向播放
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_FillValidStick(LONG nPort,LPBYTE pValidBuf,LONG lValidLen);//填充以时间轴上视频有效的区域
HX_PLAYER_API BOOL HX_STDCALL HX_PLAYER_GetLastDispFrame(LONG nPort,LPBYTE pData,PDWORD pdwSize,PDWORD pdwWidth,PDWORD pdwHeight,PLONG plType);
HX_PLAYER_API DWORD HX_STDCALL HX_PLAYER_SetPlayMode(LONG nPort,DWORD dwNewMode);	//设定新的播放模式，返回实际设定的
HX_PLAYER_API DWORD HX_STDCALL HX_PLAYER_GetPlayMode(LONG nPort);					//取得播放模式
#endif
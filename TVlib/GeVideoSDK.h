#ifndef _GEVIDEOSDK_H_
#define _GEVIDEOSDK_H_
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GEVIDEOLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GEVIDEOLIB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef GEVIDEOLIB_EXPORTS
#define GEVIDEOLIB_API __declspec(dllexport)
#else
#define GEVIDEOLIB_API __declspec(dllimport)
#endif

#ifndef GE_STDCALL
#define GE_STDCALL	__stdcall
#endif //GE_STDCALL

#include <windows.h>
#include <mmsystem.h>
#define GEV_ERR_NO_ERROR					0	//正常执行
#define GEV_ERR_INVALID_HANDLE				1	//句柄无效
#define GEV_ERR_INVALID_PARAM				2	//参数无效
#define GEV_ERR_BUFFER_NOT_ENOUGH			3	//缓冲区大小不够
#define GEV_ERR_TIMEOUT						4	//超时
#define GEV_ERR_NO_STREAM					5	//没有此数据流
#define GEV_ERR_NOT_IMPL					6	//还没有实现


#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

GEVIDEOLIB_API DWORD	GE_STDCALL	geviEnumDevNames( LPTSTR strDevNames , DWORD* pdwSize );	///枚举所有的音频输入设备名称，中间用\0格开
GEVIDEOLIB_API HANDLE	GE_STDCALL 	geviCreateDev( LPTSTR strDevName );
GEVIDEOLIB_API DWORD	GE_STDCALL	geviSetFormat( HANDLE hVideoIn , DWORD dwWidth , DWORD dwHeight , DWORD dwFrameRate , DWORD dwForCC );
GEVIDEOLIB_API DWORD	GE_STDCALL	geviGetFormat( HANDLE hVideoIn , BITMAPINFOHEADER* pInfoHeader ,DWORD* pdwFrameRate );
GEVIDEOLIB_API DWORD	GE_STDCALL	geviStartCapture( HANDLE hVideoIn );//开始捕获设置回调
GEVIDEOLIB_API DWORD	GE_STDCALL	geviStopCapture( HANDLE hVideoIn );//停止捕获
GEVIDEOLIB_API DWORD	GE_STDCALL	geviCloseDev( HANDLE hVideoIn );//关闭设备
GEVIDEOLIB_API DWORD	GE_STDCALL	geviCloseAllDevs();//关闭所有设备

typedef enum{
	GEVP_TYPE_VIDEO = 1,
	GEVP_TYPE_AUDIO = 2,
}GEVP_MEDIA_TYPE;
typedef UINT (CALLBACK *FGEVPSAMPLECALLBACK)(LPVOID pUser,LPVOID pCaller , LPBYTE pData , DWORD dwLength , GEVP_MEDIA_TYPE eType , DWORD dwTimeStamp);
GEVIDEOLIB_API HANDLE	GE_STDCALL	gevpCreatePlayer( LPTSTR strFileName );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpGetVidFormat( HANDLE hPlayer , BITMAPINFOHEADER* pInfoHeader , DWORD* pdwFrameRate );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpGetAudFormat( HANDLE hPlayer , WAVEFORMATEX* pWaveFormat );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpSetVidCallback( HANDLE hPlayer , FGEVPSAMPLECALLBACK fCallback , LPVOID pUser  );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpSetAudCallback( HANDLE hPlayer , FGEVPSAMPLECALLBACK fCallback , LPVOID pUser  );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpWaitForComplete( HANDLE hPlayer , DWORD dwTimeout );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpGetTimeLen( HANDLE hPlayer );	//得到播放文件的时间长度
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpPlay( HANDLE hPlayer );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpStop( HANDLE hPlayer );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpPause( HANDLE hPlayer );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpSeekPercent( HANDLE hPlayer , float fPercent );
GEVIDEOLIB_API float	GE_STDCALL	gevpGetPlayedPercent( HANDLE hPlayer );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpClosePlayer( HANDLE hPlayer );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpCloseAllPlayers();

/**
显示模块使用方法
1、调用 gevdCreateDisplay 创建显示对象 
2、在窗口发生移动时，调用 gevdMoveVideo 移动显示对象
3、使用 gevdPushVideo显示图像
4、使用完，用 gevdCloseDisplay 关闭
**/

//直接使用DirectDraw构建的视频显示
typedef enum _tagGEVDS_VBUFFER_STYLE
{
	GEVDS_NONE = 0,
	GEVDS_DB_BUFFER = 0x1,	//使用双缓冲显示
	GEVDS_OVERLAY = 0x2,	//优先使用 overlay 做显示	
	GEVDS_AUTO_CLRSPACE = 0x4,	//自动选择颜色空间
}GEVDS_VBUFFER_STYLE;
/**
功能:创建一个显示对象
参数:
	HWND hWnd	显示依赖的窗口
	DWORD dwWidth 视频的实际宽度
	DWORD dwHeight	视频的实际高度
	DWORD dwSuggestFourCC 创建平面时建议的颜色空间 一般为 MAKEFOURCC('I','4','2','0') 或者 MAKEFOURCC('Y','V','1','2'),
						注:根据实际情况系统会自动分配，（老的显卡不支持这两个颜色空间，所以只是建议值）
	GEVDS_VBUFFER_STYLE dwDisableStyle	需要关闭的硬件加速选项 参见 GEVDS_VBUFFER_STYLE 定义,注意是需要关闭的，默认是GEVDS_NONE
返回值:
	创建好的 显示对象句柄，错误返回NULL
**/
GEVIDEOLIB_API HANDLE	GE_STDCALL	gevdCreateDisplay( HWND	hWnd , DWORD dwWidth , DWORD dwHeight , DWORD dwSuggestFourCC , GEVDS_VBUFFER_STYLE dwDisableStyle = GEVDS_NONE );
/**
功能:显示一帧视频
参数:
	HANDLE hDisplay	显示对象句柄,由gevdCreateDisplay返回
	LPBYTE pBuffer	要显示的数据
	DWORD dwSize	要显示的数据长度 可根据 gevdGetSizeByFourCC 得到
	DWORD dwFourCC	显示数据的颜色空间,一般解压之后的数据为MAKEFOURCC('I','4','2','0') 或者 MAKEFOURCC('Y','V','1','2')
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功	
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdPushVideo( HANDLE hDisplay , LPBYTE pBuffer , DWORD dwSize , DWORD dwFourCC );

/**
功能:刷新显示对象，在显示窗口需要刷新时调用
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdRefreshVideo( HANDLE hDisplay );

/**
功能:关闭显示对象
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdCloseDisplay( HANDLE hDisplay );

/**
功能:通过图像面积和颜色空间 获取图像实际需要的缓冲区大小
参数:
	DWORD dwPicSize 显示图像面积，width * height
	DWORD dwFourCC	显示图像的颜色空间
返回值:
	返回 图像需要的缓冲区大小
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdGetSizeByFourCC( DWORD dwPicSize , DWORD dwFourCC );

/**
功能: 移动显示对象，在显示窗口移动时调用
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
	RECT rcVideo	图像的显示区域，屏幕坐标！！，注意是屏幕坐标
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdMoveVideo( HANDLE hDisplay , RECT rcVideo );

/**
功能: 判断显示对象是否overlay平面
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
返回值:
	返回 TRUE 表示是overlay平面
**/
GEVIDEOLIB_API BOOL		GE_STDCALL	gevdIsOverlay( HANDLE hDisplay );

/**
功能:如果显示对象是overlay平面，通过此接口获得关键色
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
返回值:
	返回 关键色 一般为 RGB(16,0,16)
注：
	显示overlay平面的窗口，必须涂成这个颜色才能显示图像，一般一个显卡只能有一个overlay平面
	overlay平面的显示效果比一般的平面效果要好
**/
GEVIDEOLIB_API COLORREF	GE_STDCALL	gevdGetKeyColor( HANDLE hDisplay );

/**
功能:改变显示对象依赖的窗口
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
	HWND hParentWnd	显示对象依赖的窗口句柄
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功
**/
GEVIDEOLIB_API DWORD	GE_STDCALL  gevdSetParentWnd( HANDLE hDisplay , HWND hParentWnd );

/**
功能:关闭所有的显示对象
参数:
	无
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功
注：
	偷懒的做法，一次关闭所有显示对象
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdCloseAll(); 

/**
功能:设置显示对象上叠加的鼠标光标
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
	HCURSOR hCursor	鼠标光标句柄
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdSetCursor( HANDLE hDisplay , HCURSOR hCursor );  

/**
功能:直接得到显示用的平面缓冲区指针，这样可直接将数据解压缩到这个缓冲区，直接显示，比gevdPushVideo少一次拷贝
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功
**/
GEVIDEOLIB_API LPVOID	GE_STDCALL	gevdGetVidBuf( HANDLE hDisplay , DWORD dwPicSize , DWORD dwFourCC , DWORD dwBufSize );	//另外一种显示方式 第一步

/**
功能:gevdGetVidBuf之后，使用此函数显示缓冲区图像
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdShowVidBuf( HANDLE hDisplay );	//另外一种显示方式 第二步

/**
功能:显示的时候叠加一个OSD图片,可设置透明度
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdSetOSDInfo( HANDLE hDisplay , LPBYTE pOSDData , RECT rcOSD , DWORD dwCOlorSpcae );

/**
功能:设置叠加OSD图片的透明度
参数:
	HANDLE hDisplay 显示对象句柄,由gevdCreateDisplay返回
	unsigned char cTrans 透明度0~255
返回值:
	返回 GEV_ERR_NO_ERROR 表示成功
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdSetOSDTrans( HANDLE hDisplay , unsigned char cTrans );	//设置OSD 透明度 0~255


//使用DirectShow构建的视频显示
enum
{
	GEVS_OUTPUT_VIDEO = 1,
	GEVS_OUTPUT_AUDIO = 2,
	GEVS_OUTPUT_BOTH = 3,
};
GEVIDEOLIB_API HANDLE	GE_STDCALL	gevsCreateDisplay( HWND	hWnd , DWORD dwWidth , DWORD dwHeight , DWORD dwFourCC );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevsSetVideoFmt( HANDLE hDisplay , BITMAPINFOHEADER* pInfoHeader , DWORD dwFrameRate );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevsSetAudioFmt( HANDLE hDisplay , WAVEFORMATEX* pWaveFmt );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevsSetParentWnd( HANDLE hDisplay , HWND hParent , LPRECT prcRect );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevsPushVideo( HANDLE hDisplay , LPBYTE pData , DWORD dwSize , DWORD dwTimeStamp );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevsPushAudio( HANDLE hDisplay , LPBYTE pData , DWORD dwSize , DWORD dwTimeStamp );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevsSetFullScreen( HANDLE hDisplay , BOOL bFullScreen );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevsGetFullScreen( HANDLE hDisplay , BOOL* pbFullScreen );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevsCloseDisplay( HANDLE hDisplay );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevsRefreshVideo( HANDLE hDisplay );
//视频播放缓冲对象接口VideoBuffer
typedef UINT (CALLBACK *FPLAYCHNCALLBACK)( LPVOID pUser,LPVOID pCaller,LPBYTE pData,DWORD dwLength , DWORD dwTimeStamp,DWORD dwFlags);
GEVIDEOLIB_API HANDLE	GE_STDCALL	gevbCreateChannel();			//建立一个播放通道
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbPlay( HANDLE hChannel );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbStop( HANDLE hChannel );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbPushVideo( HANDLE hChannel , LPBYTE pData , DWORD dwLength , DWORD dwTimeStamp , DWORD dwFlags );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbAddjuestStamp( HANDLE hChannel , DWORD dwTimeStamp );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbSetRenderCallback( HANDLE hChannel , FPLAYCHNCALLBACK fCallback , LPVOID pUser );	//设置播放回调
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbCloseChannel( HANDLE hChannel );	//销毁播放通道
GEVIDEOLIB_API DWORD	GE_STDCALL	gevbCloseAllChannels();
GEVIDEOLIB_API DWORD	GE_STDCALL	gevbSetSpeed( HANDLE hChannel , float fSpeed );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevbGetSpeed( HANDLE hChannel , float* pfSpeed );	

//写AVI文件
GEVIDEOLIB_API HANDLE	GE_STDCALL	geaviCreate( LPCTSTR strFileName );
GEVIDEOLIB_API DWORD	GE_STDCALL	geaviSetAudFmt( HANDLE hAviFile , WAVEFORMATEX* pWaveFmt );
GEVIDEOLIB_API DWORD	GE_STDCALL	geaviSetVidFmt( HANDLE hAviFile , DWORD dwWidth , DWORD dwHeight , DWORD dwFourCC , DWORD dwFramerate );
GEVIDEOLIB_API DWORD	GE_STDCALL	geaviWriteVideo( HANDLE hAviFile , LPBYTE pData , DWORD dwSize , DWORD dwTimeStamp ,DWORD dwFlags );
GEVIDEOLIB_API DWORD	GE_STDCALL	geaviWriteAudio( HANDLE hAviFile , LPBYTE pData , DWORD dwSize , DWORD dwTimeStamp );
GEVIDEOLIB_API DWORD	GE_STDCALL	geaviClose( HANDLE hAviFile );

typedef  GEVIDEOLIB_API void (color_inputFunc) (BYTE * y_out,
								BYTE * u_out,
								BYTE * v_out,
								BYTE * src,
								int width,
								int height,
								int stride);
typedef GEVIDEOLIB_API color_inputFunc *color_inputFuncPtr;

typedef void (color_outputFunc) (BYTE * dst,
								 int dst_stride,
								 BYTE * y_src,
								 BYTE * v_src,
								 BYTE * u_src,
								 int y_stride,
								 int uv_stride,
								 int width,
								 int height);
typedef color_outputFunc *color_outputFuncPtr;

color_inputFunc rgb24_to_yv12;
color_inputFunc rgb32_to_yv12;
color_inputFunc yuv_to_yv12;
color_inputFunc yuyv_to_yv12;
color_inputFunc uyvy_to_yv12;

color_outputFunc yv12_to_rgb24;
color_outputFunc yv12_to_rgb32;
color_outputFunc yv12_to_yuyv;
color_outputFunc yv12_to_uyvy;

GEVIDEOLIB_API	void GE_STDCALL	Transpose720p_yv12( LPBYTE pSrcData , LPBYTE pDestData );

#ifdef __cplusplus
}
#endif //__cplusplus


#endif //_GEVIDEOSDK_H_



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
#define GEV_ERR_NO_ERROR					0	//����ִ��
#define GEV_ERR_INVALID_HANDLE				1	//�����Ч
#define GEV_ERR_INVALID_PARAM				2	//������Ч
#define GEV_ERR_BUFFER_NOT_ENOUGH			3	//��������С����
#define GEV_ERR_TIMEOUT						4	//��ʱ
#define GEV_ERR_NO_STREAM					5	//û�д�������
#define GEV_ERR_NOT_IMPL					6	//��û��ʵ��


#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

GEVIDEOLIB_API DWORD	GE_STDCALL	geviEnumDevNames( LPTSTR strDevNames , DWORD* pdwSize );	///ö�����е���Ƶ�����豸���ƣ��м���\0��
GEVIDEOLIB_API HANDLE	GE_STDCALL 	geviCreateDev( LPTSTR strDevName );
GEVIDEOLIB_API DWORD	GE_STDCALL	geviSetFormat( HANDLE hVideoIn , DWORD dwWidth , DWORD dwHeight , DWORD dwFrameRate , DWORD dwForCC );
GEVIDEOLIB_API DWORD	GE_STDCALL	geviGetFormat( HANDLE hVideoIn , BITMAPINFOHEADER* pInfoHeader ,DWORD* pdwFrameRate );
GEVIDEOLIB_API DWORD	GE_STDCALL	geviStartCapture( HANDLE hVideoIn );//��ʼ�������ûص�
GEVIDEOLIB_API DWORD	GE_STDCALL	geviStopCapture( HANDLE hVideoIn );//ֹͣ����
GEVIDEOLIB_API DWORD	GE_STDCALL	geviCloseDev( HANDLE hVideoIn );//�ر��豸
GEVIDEOLIB_API DWORD	GE_STDCALL	geviCloseAllDevs();//�ر������豸

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
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpGetTimeLen( HANDLE hPlayer );	//�õ������ļ���ʱ�䳤��
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpPlay( HANDLE hPlayer );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpStop( HANDLE hPlayer );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpPause( HANDLE hPlayer );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpSeekPercent( HANDLE hPlayer , float fPercent );
GEVIDEOLIB_API float	GE_STDCALL	gevpGetPlayedPercent( HANDLE hPlayer );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpClosePlayer( HANDLE hPlayer );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevpCloseAllPlayers();

/**
��ʾģ��ʹ�÷���
1������ gevdCreateDisplay ������ʾ���� 
2���ڴ��ڷ����ƶ�ʱ������ gevdMoveVideo �ƶ���ʾ����
3��ʹ�� gevdPushVideo��ʾͼ��
4��ʹ���꣬�� gevdCloseDisplay �ر�
**/

//ֱ��ʹ��DirectDraw��������Ƶ��ʾ
typedef enum _tagGEVDS_VBUFFER_STYLE
{
	GEVDS_NONE = 0,
	GEVDS_DB_BUFFER = 0x1,	//ʹ��˫������ʾ
	GEVDS_OVERLAY = 0x2,	//����ʹ�� overlay ����ʾ	
	GEVDS_AUTO_CLRSPACE = 0x4,	//�Զ�ѡ����ɫ�ռ�
}GEVDS_VBUFFER_STYLE;
/**
����:����һ����ʾ����
����:
	HWND hWnd	��ʾ�����Ĵ���
	DWORD dwWidth ��Ƶ��ʵ�ʿ��
	DWORD dwHeight	��Ƶ��ʵ�ʸ߶�
	DWORD dwSuggestFourCC ����ƽ��ʱ�������ɫ�ռ� һ��Ϊ MAKEFOURCC('I','4','2','0') ���� MAKEFOURCC('Y','V','1','2'),
						ע:����ʵ�����ϵͳ���Զ����䣬���ϵ��Կ���֧����������ɫ�ռ䣬����ֻ�ǽ���ֵ��
	GEVDS_VBUFFER_STYLE dwDisableStyle	��Ҫ�رյ�Ӳ������ѡ�� �μ� GEVDS_VBUFFER_STYLE ����,ע������Ҫ�رյģ�Ĭ����GEVDS_NONE
����ֵ:
	�����õ� ��ʾ�����������󷵻�NULL
**/
GEVIDEOLIB_API HANDLE	GE_STDCALL	gevdCreateDisplay( HWND	hWnd , DWORD dwWidth , DWORD dwHeight , DWORD dwSuggestFourCC , GEVDS_VBUFFER_STYLE dwDisableStyle = GEVDS_NONE );
/**
����:��ʾһ֡��Ƶ
����:
	HANDLE hDisplay	��ʾ������,��gevdCreateDisplay����
	LPBYTE pBuffer	Ҫ��ʾ������
	DWORD dwSize	Ҫ��ʾ�����ݳ��� �ɸ��� gevdGetSizeByFourCC �õ�
	DWORD dwFourCC	��ʾ���ݵ���ɫ�ռ�,һ���ѹ֮�������ΪMAKEFOURCC('I','4','2','0') ���� MAKEFOURCC('Y','V','1','2')
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�	
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdPushVideo( HANDLE hDisplay , LPBYTE pBuffer , DWORD dwSize , DWORD dwFourCC );

/**
����:ˢ����ʾ��������ʾ������Ҫˢ��ʱ����
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdRefreshVideo( HANDLE hDisplay );

/**
����:�ر���ʾ����
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdCloseDisplay( HANDLE hDisplay );

/**
����:ͨ��ͼ���������ɫ�ռ� ��ȡͼ��ʵ����Ҫ�Ļ�������С
����:
	DWORD dwPicSize ��ʾͼ�������width * height
	DWORD dwFourCC	��ʾͼ�����ɫ�ռ�
����ֵ:
	���� ͼ����Ҫ�Ļ�������С
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdGetSizeByFourCC( DWORD dwPicSize , DWORD dwFourCC );

/**
����: �ƶ���ʾ��������ʾ�����ƶ�ʱ����
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
	RECT rcVideo	ͼ�����ʾ������Ļ���꣡����ע������Ļ����
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdMoveVideo( HANDLE hDisplay , RECT rcVideo );

/**
����: �ж���ʾ�����Ƿ�overlayƽ��
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
����ֵ:
	���� TRUE ��ʾ��overlayƽ��
**/
GEVIDEOLIB_API BOOL		GE_STDCALL	gevdIsOverlay( HANDLE hDisplay );

/**
����:�����ʾ������overlayƽ�棬ͨ���˽ӿڻ�ùؼ�ɫ
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
����ֵ:
	���� �ؼ�ɫ һ��Ϊ RGB(16,0,16)
ע��
	��ʾoverlayƽ��Ĵ��ڣ�����Ϳ�������ɫ������ʾͼ��һ��һ���Կ�ֻ����һ��overlayƽ��
	overlayƽ�����ʾЧ����һ���ƽ��Ч��Ҫ��
**/
GEVIDEOLIB_API COLORREF	GE_STDCALL	gevdGetKeyColor( HANDLE hDisplay );

/**
����:�ı���ʾ���������Ĵ���
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
	HWND hParentWnd	��ʾ���������Ĵ��ھ��
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�
**/
GEVIDEOLIB_API DWORD	GE_STDCALL  gevdSetParentWnd( HANDLE hDisplay , HWND hParentWnd );

/**
����:�ر����е���ʾ����
����:
	��
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�
ע��
	͵����������һ�ιر�������ʾ����
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdCloseAll(); 

/**
����:������ʾ�����ϵ��ӵ������
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
	HCURSOR hCursor	�������
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdSetCursor( HANDLE hDisplay , HCURSOR hCursor );  

/**
����:ֱ�ӵõ���ʾ�õ�ƽ�滺����ָ�룬������ֱ�ӽ����ݽ�ѹ���������������ֱ����ʾ����gevdPushVideo��һ�ο���
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�
**/
GEVIDEOLIB_API LPVOID	GE_STDCALL	gevdGetVidBuf( HANDLE hDisplay , DWORD dwPicSize , DWORD dwFourCC , DWORD dwBufSize );	//����һ����ʾ��ʽ ��һ��

/**
����:gevdGetVidBuf֮��ʹ�ô˺�����ʾ������ͼ��
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdShowVidBuf( HANDLE hDisplay );	//����һ����ʾ��ʽ �ڶ���

/**
����:��ʾ��ʱ�����һ��OSDͼƬ,������͸����
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdSetOSDInfo( HANDLE hDisplay , LPBYTE pOSDData , RECT rcOSD , DWORD dwCOlorSpcae );

/**
����:���õ���OSDͼƬ��͸����
����:
	HANDLE hDisplay ��ʾ������,��gevdCreateDisplay����
	unsigned char cTrans ͸����0~255
����ֵ:
	���� GEV_ERR_NO_ERROR ��ʾ�ɹ�
**/
GEVIDEOLIB_API DWORD	GE_STDCALL	gevdSetOSDTrans( HANDLE hDisplay , unsigned char cTrans );	//����OSD ͸���� 0~255


//ʹ��DirectShow��������Ƶ��ʾ
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
//��Ƶ���Ż������ӿ�VideoBuffer
typedef UINT (CALLBACK *FPLAYCHNCALLBACK)( LPVOID pUser,LPVOID pCaller,LPBYTE pData,DWORD dwLength , DWORD dwTimeStamp,DWORD dwFlags);
GEVIDEOLIB_API HANDLE	GE_STDCALL	gevbCreateChannel();			//����һ������ͨ��
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbPlay( HANDLE hChannel );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbStop( HANDLE hChannel );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbPushVideo( HANDLE hChannel , LPBYTE pData , DWORD dwLength , DWORD dwTimeStamp , DWORD dwFlags );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbAddjuestStamp( HANDLE hChannel , DWORD dwTimeStamp );
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbSetRenderCallback( HANDLE hChannel , FPLAYCHNCALLBACK fCallback , LPVOID pUser );	//���ò��Żص�
GEVIDEOLIB_API DWORD	GE_STDCALL  gevbCloseChannel( HANDLE hChannel );	//���ٲ���ͨ��
GEVIDEOLIB_API DWORD	GE_STDCALL	gevbCloseAllChannels();
GEVIDEOLIB_API DWORD	GE_STDCALL	gevbSetSpeed( HANDLE hChannel , float fSpeed );
GEVIDEOLIB_API DWORD	GE_STDCALL	gevbGetSpeed( HANDLE hChannel , float* pfSpeed );	

//дAVI�ļ�
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



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

	char szTcpServerAddr[32];	//TCP���ķ������ĵ�ַ
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
	DWORD	dwUpCondition;		//��������
	DWORD	dwCloseCondition;	//�Ͽ�����
	char	strPPPOEUser[16];	//PPPOE�û���
	char	strPPPOEPass[16];	//PPPOE����
}CQWORKMODE,*PCQWORKMODE;

enum
{
	CQ_OSDVALID_STATE = 0x1,
	CQ_OSDVALID_TIMEPOS = 0x2,
	CQ_OSDVALID_CHARDATA = 0x4,
	CQ_OSDVALID_OSDPOS = 0x8,
	CQ_OSDVALID_CHARMASK = 0x10,
	
	CQ_OSD_TIME = 1,	//����ʱ��
	CQ_OSD_CHAR = 2,	//���Ӻ���
};

typedef struct tagCQ_OSD_INFO
{
	unsigned long	uValidArea;	//��Ч�ֶ�
	unsigned long uOSDState;	//�Ƿ����
	short	sTimePosX;		//����ʱ���λ��X
	short	sTimePosY;		//����ʱ���λ��Y	
	char		szCharData[32];		//���ӵ�����
	short	sOSDPosX;	//���Ӻ��ֵ�λ��X
	short	sOSDPosY;	//���Ӻ��ֵ�λ��Y
	char	pCharMask[512];//�����ֵ���ģ������16�����Ǻ���16x32 = 512
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
 * ¼�����ص�����
 *			hRec,¼�������
 *			pData:16λ����PCM��Ƶ
 *			nLength:���������
 **********************************************************************/
//typedef (CX_STDCALL * FN_AUDIO_RECORDER)( HANDLE hRec,short * pData,int nLength );
typedef UINT (CALLBACK *FN_CXAUDINCALLBACK)(LPVOID pUser,HANDLE hCaller,LPBYTE pData,DWORD dwLength);
typedef VOID (CX_STDCALL * FN_DEAMON_CALLBACK)( DWORD dwUsrParam,DWORD dwReturnParam1,DWORD dwReturnParam2 );
typedef UINT (CALLBACK *FN_CQAP_NOTIFY_CALLBACK)( LPVOID pUser , LPVOID pCaller , DWORD dwCurTimeStamp , DWORD dwBufferedSize );	//����֪ͨ�ϲ㲥�ŵ�ʲôʱ���ˣ������˶�������
/**********************************************************************/
/*�ص������¼�*/
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
#define ACT_OSD_INFO					15	//�õ���OSD��Ϣ��
#define ACT_AUDIO_INFO					16	//�õ�����Ƶ��Ϣ
#define ACT_GET_WORKMODE				17	//�õ����豸����ģʽ
#define ACT_TRANSPARENT_DATA_485		18	
/*********************************************************************/
/*Errors*/
#define CX_ERR_NO_ERROR					0	//����ִ��
#define CX_ERR_INVALID_HANDLE			1	//�����Ч
#define CX_ERR_INVALID_PARAM			2	//������Ч
#define CX_ERR_INIT_DIRECTDRAW			3	//��ʼ��DirectDrawʧ��
#define CX_ERR_NOT_CONNECTED			4	//û������
#define CX_ERR_COMMAND_TIMEOUT			5	//���ʱ
#define CX_ERR_SEND_FAIL				6	//����ʧ��
#define CX_ERR_BUSY						7	//�豸��æ
#define CX_ERR_PENDING					8	//�������ڽ�����			

#define CX_ERR_AUDIO_PLAYER_NO_MEMORY	64
/*********************************************************************/
#define CX_COLOR_SPACE_YV12				0
#define CX_COLOR_SPACE_RGB32			1
#define CX_COLOR_SPACE_RGB24			2
#define CX_COLOR_SPACE_RGB555			3
#define CX_COLOR_SPACE_RGB565			4
#define CX_COLOR_SPACE_YUYV				5
/*********************************************************************/
/*�ֱ���*/
#define CX_RESOLUTION_QCIF				1	//176X144
#define CX_RESOLUTION_CIF				2	//352X288
#define CX_RESOLUTION_HALF_D1			3	//704X576

/*********************************************************************/
/*֡��*/
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
/*����������*/
#define CX_ADV_DEC_NORMAL				0
#define CX_ADV_DEC_BETTER1				1
#define CX_ADV_DEC_BETTER2				2
#define	CX_ADV_DEC_BEST					3

/**********************************************************************/
/*����ͨ�����*/
#define CX_DATA_CHN_485					0
#define CX_DATA_CHN_232					1

/**********************************************************************/
/*��������*/
#define CX_CONNECT_TYPE_UDP				0
#define CX_CONNECT_TYPE_TCP				1

/*�豸����ģʽ*/
#define CX_DEVFUNC_TCP					0x1			//�豸ʹ��TCP���������Ĺ���
#define CX_DEVFUNC_ENCODER				0x2			//�豸������Ƶ����Ĺ���
#define CX_DEVFUNC_DECODER				0x4			//�豸������Ƶ����Ĺ���
#define CX_DEVFUNC_PPPOE				0x8			//ʹ��PPPOE����
#define CX_DEVFUNC_AUTOSEND				0x10		//�������ӶϿ�����Ȼ��������ý����
#define CX_DEVFUNC_TCPSERVER			0x20		//TCP����ģʽ
#define CX_DEVFUNC_MINIPACKET			0x40		//ʹ��С�����ݰ�
/**********************************************************************
 *���ر�ǣ�
 * CX_RETURN_EVENT ʹ���¼�֪ͨ���أ�����Ϊ�¼�Handle���ͷ������ò����ĵ�ַ
 * CX_RETURN_WND   ʹ�ô��ڷ��أ�����Ϊ���� �� ��Ϣ
 * CX_RETURN_CALLBACK ���ص��ú���������Ϊ�ص������ͻص������û��Զ������
 **********************************************************************/
/*���ز������*/
#define CX_RETURN_EVENT					1
#define CX_RETURN_WND					2
#define CX_RETURN_CALLBACK				4

/**********************************************************************
 *	���ܣ����豸
 *	dwIpAddr���豸��ַ,�����ֽ�˳��
 *	fnCallback���ص�����
 *	pUsrName���û���
 *	pPwd������
 *	hQuitEvent�������˳��¼������ΪNULL����SDK���ڲ������¼�,manual reset
 *	pContext�����ûص������ĵ�һ������
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
 *���ܣ��ر��豸
 *hIpCam���豸���
 **********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxCloseIpCam( HANDLE hIpCam );


/**********************************************************************
 *���ܣ��ر�����ϵͳ���Ѿ��򿪵��豸
 **********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxCloseAllIpCam( void );

/**********************************************************************
 * ���ܣ��ӱ��ش洢��ȡ���豸������ã�ע�⣬�п��ܺ��豸ʵ�����ò�һ��
 * ������
 * hDev �豸���
 * pInfo �豸��Ϣ
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetIpCamDevInfo( HANDLE hDev,IP_CAM_INFO2 * pInfo );

/**********************************************************************
 * ���ܣ��ӱ��ش洢��ȡ��ip������ã�ע�⣬�п��ܺ��豸ʵ�����ò�һ��
 *		�μ�cxGetDevIp��˵����
 * ������
 * hDev �豸���
 * pdwIp IP��ַ
 * pdwMask ��������
 * pdwDefGateway Ĭ������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL 
	cxGetLocalDevIp(
			HANDLE hDev,
			DWORD * pdwIp,
			DWORD * pdwMask,
			DWORD * pdwDefGateway );

/**********************************************************************
 * ���ܣ�ȡ���豸ip������ã�����������豸�������ȡ�����ú󣬷���
 *		 ACT_DEV_IP�ص�������Ϣ��Ӧ�ó���ͨ��GetLocalDevIpȡ������
 * ������
 * hDev �豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL 
	cxGetDevIp( HANDLE hDev );

/**********************************************************************
 * ���ܣ��޸��豸IP��ַ
 * ������
 * hDev �豸���
 * dwIp IP��ַ
 * dwMask ��������
 * dwDefGateway Ĭ������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxModifyDevIp(
				HANDLE hDev,
				DWORD dwIp,
				DWORD dwMask,
				DWORD dwDefGateway );

/*********************************************************************
 * ���ܣ��޸��豸�û���������
 * ������
 * hDev �豸���
 * pUserName���û�����С��8��ASCII�ַ�
 * pPwd�����룬С��8��ASCII�ַ�
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxModifyDevUserNamePwd( HANDLE hDev,char * pUserName,char * pPwd );

/*********************************************************************
 * ���ܣ�������Ƶ�ֱ���
 * ������
 *   hDev���豸���
 *   nResolution���ֱ���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetVideoResolution( HANDLE hDev,int nResolution );

/*********************************************************************
 * ���ܣ�����֡��
 * ������
 *   hDev:�豸���
 *   nFrameRate:֡��
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetVideoFrameRate( HANDLE hDev,int nFrameRate );

/*********************************************************************
 * ���ܣ��洢��������
 * ������
 *   hDev���豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSaveIpCamConf( HANDLE hDev );

/*********************************************************************
 * ���ܣ�������Ƶ��������
 * ������
 * hDev �豸���
 * nBitrate ���ʣ�64~2048����λKpbs
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetVideoBitrate( HANDLE hDev,int nBitrate );

/*********************************************************************
 * ���ܣ����ص�ǰʵ����Ƶ��������(ͨ������������)
 * ������
 * hDev �豸���
 * pnBitrate ��������,��λKbps
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetVideoCurrentBitrate( HANDLE hDev,int * pnBitrate );

/*********************************************************************
 * ����:����ȱʡ�������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxLoadDefCodecParam( HANDLE hDev );

/*********************************************************************
 * ����:��ñ��ر���ı������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetLocalCodecParam( HANDLE hDev,ACODEC_PARAM * pParam );

/*********************************************************************
 * ����:��������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetViBrightness( HANDLE hDev,int nBrightness );

/*********************************************************************
 * ����:���öԱȶ�
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetViContrast( HANDLE hDev,int nContrast );

/*********************************************************************
 * ����:���ñ��Ͷ�
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetViSaturation( HANDLE hDev,int nSaturation );

/*********************************************************************
 * ����:����ɫ��
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetViHue( HANDLE hDev,int nHue );

/*********************************************************************
 * ����:һ��������VI����
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetViParam( HANDLE hDev,AVI_PARAM * pParam );

/*********************************************************************
 * ���ܣ�һ���Ի�ȡVI�����������������Ϸ��أ���������ʱ���ص�������
 * ����ACT_VI_PARAM�¼���Ȼ�����cxGetLocalViParam�ɻ���豸VI������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetViParam( HANDLE hDev );

/*********************************************************************
 * ���ܣ���ñ������������VI�������豸���ӳɹ��󣬻��Զ����豸���
 * VI�����������ڱ��ء�
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetLocalViParam( HANDLE hDev,AVI_PARAM * pParam );

/*********************************************************************
 * ���ܣ��ָ���Ƶ�������Ĭ�ϲ���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxLoadViDefParam( HANDLE hDev );

/*********************************************************************
 * ���ܣ����豸��������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxConnectIpCam( HANDLE hDev,DWORD dwIpAddr );

/*********************************************************************
 * ���ܣ���õ�ǰ״̬
 * ������hDev �豸���
 * nStatus��
 *		bit0,bit1
 *			0 ---- idle
 *			1 ---- ��������
 *			2 ---- ������
 *		bit2 ---- ������Ƶ
 *		bit3 ---- ������Ƶ
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetStatus( HANDLE hDev,DWORD * nStatus);

/*********************************************************************
 * ���ܣ�Уʱ
 * ������hDev �豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxVerifyTime( HANDLE hDev );

/*********************************************************************
 * ���ܣ�֪ͨ�豸����ʼ������Ƶ��������֪ͨ�豸����Ƶ���͵�dwIpָʾ��
 *		�������˿�ΪsPort����dwIpΪ0ʱ����ʾ����Ƶ���͵�������sPort��
 *		�����á�
 * ������
 *		hDev �豸���
 *		dwIp IP��ַ
 *		sPort �˿�
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStartVideo( HANDLE hDev,DWORD dwIp,short sPort );

/*********************************************************************
 * ���ܣ�֪ͨ�豸��ֹͣ������Ƶ
 * ������hDev �豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStopVideo( HANDLE hDev );

/*********************************************************************
 * ���ܣ�֪ͨ�豸����ʼ������Ƶ��������֪ͨ�豸����Ƶ���͵�dwIpָʾ��
 *		�������˿�ΪsPort����dwIpΪ0ʱ����ʾ����Ƶ���͵�������sPort
 *		�������á�
 * ������
 *		hDev �豸���
 *		dwIp IP��ַ
 *		sPort �˿�
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStartAudio( HANDLE hDev,DWORD dwIp,short sPort );

/*********************************************************************
 * ���ܣ�֪ͨ�豸��ֹͣ������Ƶ
 * ������hDev �豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStopAudio( HANDLE hDev );

/*********************************************************************
 * ���ܣ���ʼ������Ƶ
 * ������hDev �豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStartRecvVideo( HANDLE hDev,
												   DWORD dwMultiIp,
												   short sPort );

/*********************************************************************
 * ���ܣ�ֹͣ������Ƶ
 * ������hDev �豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStopRecvVideo( HANDLE hDev );

/*********************************************************************
 * ���ܣ���ʼ������Ƶ
 * ������hDev �豸���
 *		 dwMcastIp �ಥIP��ַ,���Ϊ0,��ʾ��ʹ�öಥ
 *		 sPort	   �˿�,ֻ���ڶಥʱ��Ч
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStartRecvAudio( HANDLE hDev,DWORD dwMcastIp,short sPort );

/*********************************************************************
 * ���ܣ�ֹͣ������Ƶ
 * ������hDev �豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStopRecvAudio( HANDLE hDev );

/*********************************************************************
 * ���ܣ�ָ�����ݷ��͵�IP�Ͷ˿�
 * ������hDev �豸���
 *		 dwHostIP ���ݷ���Ŀ�ĵ�ַ��Ϊ0ʱ��ʾ���͵�����
 *		 sPort ���ݷ��͵Ķ˿�,��dwHostIpΪ0ʱ,SDK���Զ������������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStartData( HANDLE hDev,DWORD dwHostIp,short sPort );

/*********************************************************************
 * ���ܣ�ֹͣ��������
 * ������hDev �豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxStopData( HANDLE hDev );

/*********************************************************************
 * ���ܣ�ǿ�Ʒ���I֡
 * ������hDev �豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxForceIFrame( HANDLE hDev );

/*********************************************************************
 * ���ܣ�ȡ�ñ�������
 * ������
 * hDev �豸���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetAlarmIn( HANDLE hDev );


/*********************************************************************
 * ���ܣ�ȡ�ñ�������
 * ������
 * hDev �豸���
 * nSw ���غţ�bit0--switch1 bit1--switch2		etc.
 * nStatus ����״̬ bit0--switch1,bit1--switch2 etc. 0--open,1--close
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetAlarmOut( HANDLE hDev,int nSwitchNo,int Status );

/*********************************************************************
 * ���ܣ�����͸������
 * ������
 * hDev �豸���
 * pData ����
 * nDataLen ���ݳ���
 * nChnNum ---- 0 is 485,1 is 232
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSendTransparentData( 
			HANDLE hDev,
			char * pData,
			int nDataLen,
			int nChnNum );

/*********************************************************************
 * ���ܣ�����232/485������
 * ������
 * hDev �豸���
 * nBaud 19200
 * nComNum ---- 0 is 485,1 is 232
 *       9600
 *       4800
 *       2400
 *       1200
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetDataBaudrate( HANDLE hDev,int nComNum,int nBaud );

/*********************************************************************
 * ���ܣ��õ�232/485������
 * ������
 * hDev �豸���
 * ����ӻص��з���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxGetDataBaudrate( HANDLE hDev ,int nComNum );

/*********************************************************************
 * ���ܣ�����MPEG4������
 * ������
 *		nWide ���
 *		nHeight �߶�
 * ���أ�
 *		MPEG4���������
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateDecoder( int nWide,int nHeight );

/*********************************************************************
 * ���ܣ��ر�MPEG4 Decoder
 * ������hDecoder MPEG4���������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxCloseDecoder( HANDLE hDecoder );

/*********************************************************************
 * ���ܣ�����һ֡
 * ������
 *		hDec�����������
 *		pData��MPEG4��Ƶ����
 *		nLength����Ƶ���ݳ���
 *		nAdvLevel��������������뼶��
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxDecodeFrame( 
				HANDLE hDec,
				char * pData,
				int nLength,
				char * pDecBuf,
				int nStride,
				int nAdvLevel );

/*********************************************************************
 * ���ܣ���ʼ����ʾ����
 * ������pWnd ��ʾ��Ƶ�Ĵ���
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxInitVideoDisp( HWND hWnd );

/*********************************************************************
 * ���ܣ��ر���ʾ����
 * ��������
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxUninitVideoDisp( void );

/*********************************************************************
 * ���ܣ�������ʾ��ʾͨ��
 * ������
 *   rcx,rcy --- ʵ��ͼ���С
 *   x,y   ---- λ��
 *   cx,cy ---- �ߴ�
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateDispChn( int rcx,
									 int rcy,
									 int x,
									 int y,
									 int cx,
									 int cy );

/*********************************************************************
 * ���ܣ�������ʾ��ʾͨ��
 * ������
 *	 rcx,rcy ----��Ƶ�ߴ�
 *   x,y   ---- λ��
 *   cx,cy ---- �ߴ�
 *	 hWnd ----���ھ��,���ô˺����������cxInitVideoDisp
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateDispChnEx( HWND hWnd,
									int rcx,
									int rcy,
									int x,
									int y,
									int cx,
									int cy );

/*********************************************************************
 * ���ܣ��ر���ʾͨ��
 * ������
 *   hDisp ͨ�����
 *	 hWnd ���ھ��
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseDispChnEx( HWND hWnd,HANDLE hDisp );

/*********************************************************************
 * ���ܣ��ر��봰����ص�DDraw��ʾ
 * ������
 *   hWnd ���ھ��
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxClearDisp( HWND hWnd );

/*********************************************************************
 * ���ܣ��ر���ʾͨ��
 * ������
 *   hDisp ͨ�����
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseDispChn( HANDLE hDisp );

/*********************************************************************
 * ���ܣ���ʾ��Ƶ
 * ������
 *   hDisp ---- ��ʾͨ�����
 *   pYUV  ---- ������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxDispVideo( HANDLE hDisp,char * pYUV );

/*********************************************************************
 * ���ܣ�������Ƶ��ʾͨ����λ��
 * ������
 *   hDisp ---- ��ʾͨ�����
 *   x,y   ---- λ��
 *   cx,cy ---- �ߴ�
 *   bFullWnd ---- �Ƿ�Ŵ���������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetDispLocate( HANDLE hDisp,int x,int y,int cx,int cy,BOOL bFullWnd );

/*********************************************************************
 * ���ܣ�������Ƶ������
 * ������
 *		��
 * ���أ�
 *		��Ƶ���������
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateAudioDecoder( void );

/*********************************************************************
 * ���ܣ�������Ƶ������
 * ������
 *		hDec,��Ƶ���������
 * ���أ�
 *		��
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseAudioDecoder( HANDLE hDec );

/*********************************************************************
 * ���ܣ���Ƶ����
 * ������
 *		hDec�����������
 *		pData����Ƶ����
 *		nLength����Ƶ���ݳ���
 *		pnDecLen������������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxDecodeAudio( 
				HANDLE hDec,
				char * pData,
				int nLength,
				short * pDecBuf,
				int * pnDecLen );

/*********************************************************************
 * ���ܣ�������Ƶ������
 * ������
 *		��
 * ���أ�
 *		��Ƶ���������
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateAudioEncoder( void );

/*********************************************************************
 * ���ܣ�������Ƶ������
 * ������
 *		hDec,��Ƶ���������
 * ���أ�
 *		��
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseAudioEncoder( HANDLE hEnc );

/*********************************************************************
 * ���ܣ���Ƶ����
 * ������
 *		hEnc�����������
 *		pData����Ƶ����
 *		nLength����Ƶ���ݳ���
 *		pnEncLen������������
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxEncodeAudio( 
				HANDLE hEnc,
				short * pData,
				int nLength,
				char * pEncBuf,
				int * pnEncLen );

/*********************************************************************
 * ���ܣ�������Ƶ������
 * ������
 *		nDev--��Ƶ�豸���
 * ���أ�
 *		��Ƶ���������
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateAudioPlayer( int nDev ,int nRateIndex );

/*********************************************************************
 * ���ܣ��ر���Ƶ������
 * ������
 *		hPlayer: ��Ƶ���������
 * ���أ�
 *		��
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseAudioPlayer( HANDLE hPlayer );

/*********************************************************************
 * ���ܣ�������Ƶ
 * ������
 *		hPlayer: ��Ƶ���������
 *		pSamp:16λ����PCM��Ƶ
 *		nLength:��Ƶ����(�Բ�����������)
 * ���أ�
 *		CX_ERR_AUDIO_PLAYER_NO_MEMORY--���Ż���������
 *********************************************************************/
IPCAMSDK_API  CX_ERROR CX_STDCALL cxPlayAudio( 
									HANDLE hPlayer,
									short * pSamp,
									int nLength, DWORD dwTimeStamp );


IPCAMSDK_API  CX_ERROR CX_STDCALL cxAudioPlayerRun( HANDLE hPlayer );
IPCAMSDK_API  CX_ERROR CX_STDCALL cxAudioPlayerStop( HANDLE hPlayer );

/************************************************************************/
/* ���ܣ�������Ƶ���Żص������ص�ǰ����ʱ���
/* ����:
/* hPlayer ���������
/* fCallback ����֪ͨ�ص����� 
/* pUser	���ڻص��з��صģ��û��趨����
/* ���أ�
/*		��
/************************************************************************/
IPCAMSDK_API  CX_ERROR CX_STDCALL cxAudioPlayerSetNotify( HANDLE hPlayer , FN_CQAP_NOTIFY_CALLBACK fCallback , LPVOID pUser);

IPCAMSDK_API  CX_ERROR CX_STDCALL cxAudioPlayerFreeBuf( HANDLE hPlayer , DWORD dwSizeToFree );
/*********************************************************************
 * ���ܣ�������Ƶ¼����
 * ������
 *		nDev--¼���豸���
 * ���أ�
 *		¼�������
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateAudioRecoder( int nDev ,int nRateIndex );

/*********************************************************************
 * ���ܣ��ر�¼����
 * ������
 *		hRecorder: ¼�������
 * ���أ�
 *		��
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseAudioRecorder( HANDLE hRecorder );

/*********************************************************************
 * ���ܣ���ʼ¼��
 * ������hRecoder:¼�������
 *	     fnCallback:�ص�����,�μ��ص�����˵��
 *********************************************************************/
IPCAMSDK_API  CX_ERROR CX_STDCALL cxStartRecordAudio( 
									HANDLE hRecorder,
									FN_CXAUDINCALLBACK fnCallback,
									LPVOID pContext);


/*********************************************************************
 * ���ܣ������ػ�����
 * ��������
 * ˵�����ػ������������������Ƶ��������Ϣ���ռ�����
 *********************************************************************/
IPCAMSDK_API BOOL CX_STDCALL cxStartDeamonTask( void );
IPCAMSDK_API BOOL CX_STDCALL cxEnableDemonSearch( BOOL bSearch );

/*********************************************************************
 * ���ܣ������ػ�����
 * ��������
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxStopDeamonTask( void );

/*********************************************************************
 * ���ܣ�ȡ�������Ѿ��ҵ����豸��Ϣ
 * ������
 *		pInfos ָ������豸��Ϣ���ڴ��ַ
 * ����:
 *      �ҵ����豸��
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL cxGetDevFound(IP_CAM_INFO**pInfos);


/*********************************************************************
 * ���ܣ�ȡ�������Ѿ��ҵ����豸��Ϣ(����һ��������Ϣ��)
 * ������
 *		pInfos ָ������豸��Ϣ���ڴ��ַ
 * ����:
 *      �ҵ����豸��
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL cxGetDevFound2(IP_CAM_INFO2**pInfos );

/*********************************************************************
 * ���ܣ�ˢ���豸��Ϣ
 * ��������
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxRefreshDeamon( void );

/*********************************************************************
 * ���ܣ�ֱ���޸��豸IP��ַ������Ҫ���豸��������
 * ������
 *		 pMac	   MAC��ַ
 *		 dwNewIp   ��IP
 *		 dwNewMask �µ�����
 *		 dwNewGw   �µ�����
 *		 pUserName �û���
 *		 pPwd      ����
 *		 dwFlag	   �������
 *		 dwReturn  ���ط�ʽ
 *		 dwReturnParam   ���÷��ز���
 * ����:
 *		CX_ERR_NO_ERROR		�ɹ�
 *		CX_ERR_BUSY			��һ������û�����,��Ҫ�ȴ�
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
 * ���ܣ������豸������IP��ַ(TCP����)
 * ������
 *		 pMac	   MAC��ַ
 *		 dwTcpHostIp ��IP
 *		 pUserName �û���
 *		 pPwd      ����
 *		 dwFlag	   �������
 *		 dwReturn  ���ط�ʽ
 *		 dwReturnParam   ���÷��ز���
 * ����:
 *		CX_ERR_NO_ERROR		�ɹ�
 *		CX_ERR_BUSY			��һ������û�����,��Ҫ�ȴ�
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
 *����:����AV�ļ�,�������ÿ�Ⱥ͸߶�
 *����:
 *    pFileName   �ļ���
 *    nWidth      ��Ƶ���
 *    nHeight     ��Ƶ�߶�
 *********************************************************************/
IPCAMSDK_API HANDLE CX_STDCALL cxCreateAVFile( char * pFileName,int nWidth,int nHeight , DWORD dwAudFlag );

/*********************************************************************
 *����:����AV�ļ�,�������ÿ�Ⱥ͸߶�
 *����:
 *    hAVFile   �ļ����
 *********************************************************************/
IPCAMSDK_API void CX_STDCALL cxCloseAVFile( HANDLE hAVFile );

/*********************************************************************
 *����:д��Ƶ
 *����:
 *		pUnit  ACT_RECV_VIDEO_FRAME ���صĲ���,ע�ⲻҪ�޸�ACT_RECV_VIDEO_FRAME
 *				���ص�ֵ,����ֱ�Ӵ����������
 *����:
 *		-1   ����ľ��
 *		0    д�ļ�����
 *		>0	 д��ƵƵ����
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL cxWriteVideo( HANDLE hFile,MEDIA_UNIT * pUnit );

/*********************************************************************
 *����:д��Ƶ
 *����:
 *		pUnit  ACT_RECV_AUDIO_FRAME ���صĲ���,ע�ⲻҪ�޸�ACT_RECV_VIDEO_FRAME
 *				���ص�ֵ,����ֱ�Ӵ����������
 *����:
 *		-1   ����ľ��
 *		0    д�ļ�����
 *		>0	 д��Ƶ����
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL cxWriteAudio( HANDLE hFile,MEDIA_UNIT * pUnit );

/*********************************************************************
 *����:��YV12(4:2:0)��ʽת����������ɫ�ռ�
 *����:
 *	  pYV12		   ���YV12��ʽ��λͼ
 *	  nYV12Stride  YV12��ʽ��Stride��������ΪY��Stride,UV��strideΪ��������1/2
 *    nColorSpace  ��ɫ�ռ�,CX_COLOR_SPACE_ ��ͷ�ĳ���
 *    pOther	   ������ɫ�ռ��λͼ
 *    nOtherStride ������ɫ�ռ��stride,��ÿһ������ж��ٸ�����
 *	  nWidth	   ���
 *	  nHeight      �߶�
 *����ֵ:
 *		1		   �ɹ�
 *		0		   ����ʶ�����ɫ�ռ�
 *********************************************************************/
IPCAMSDK_API int CX_STDCALL YV12ToOtherColorSpace( char * pYV12,
									    int nYV12Stride,
										int nColorSpace,
									    char * pOther,
										int nOtherStride,
										int nWidth,
										int nHeight ); 

/*********************************************************************
 *����:��������ɫ�ռ�ת����YV12(4:2:0)��ʽ  
 *����:
 *    nColorSpace  ��ɫ�ռ�,CX_COLOR_SPACE_ ��ͷ�ĳ���
 *    pOther	   ������ɫ�ռ��λͼ
 *    nOtherStride ������ɫ�ռ��stride,��ÿһ������ж��ٸ�����
 *	  pYV12		   ���YV12��ʽ��λͼ
 *	  nYV12Stride  YV12��ʽ��Stride��������ΪY��Stride,UV��strideΪ��������1/2
 *	  nWidth	   ���
 *	  nHeight      �߶�
 *����ֵ:
 *		1		   �ɹ�
 *		0		   ����ʶ�����ɫ�ռ�
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
 *����:����TCP������
 *����:
 *����ֵ:
 *		TRUE	   �ɹ�
 *		FALSE	   ʧ��
 *********************************************************************/
IPCAMSDK_API BOOL CX_STDCALL cxStartTcpServer( void );

/*********************************************************************
 *����:������������
 *����:
 *		hDev �豸���
 *		nConnectType ��������
 *					#define CX_CONNECT_TYPE_UDP				0
 *					#define CX_CONNECT_TYPE_TCP				1
 *
 *		pMac �����TCP����MAC��ַ
 *����ֵ:
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL cxSetConnectType( HANDLE hDev,int nConnectType,BYTE * pMac );

/*********************************************************************
 *����:�����ƶ�������
 *����:
 *		hDev		 �豸���
 *		pThredshold	 ��ֵ��8������8��
 *		pLevels		 ÿ��32*32��ļ���ÿ��32x32��4bit,�ɱ�ʾ0~15������,Ŀǰֻ��8��
 *		nLevelLength pLevels���ֽڳ���
 *˵��:
 *		ʵ�ʼ��㷽��Ϊ pThredshold[pLevels[x]], XΪ32x32��ı��
 *����ֵ:
 *		��
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxSetMotionDetectParam( HANDLE hDev,
										  BOOL bStart,
										  short * pThredshold, 
										  unsigned char * pLevels,
										  int nLevelLength );


/*********************************************************************
 *����:����audio codec
 *����:
 *		hDev		 �豸���
 *	    nCodec       audio codec
 *˵��:
 
 *����ֵ:
 *		
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxSetAudioCodec( HANDLE hDev,int nCodec );

/*********************************************************************
 *����:�ͷ�SDK��ռ�õ���Դ
 *����: ��
 
 *˵��: �ڳ����˳�ʱ����
 
 *����ֵ:
 *		
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxCleanUp( void );

/*********************************************************************
 *����:�����豸�ĵ�����Ϣ
 *����: hDev �豸���
		pOsdInfo ������Ϣָ��
		nLength ������Ϣ�ṹ����Ч���ݵĳ���
  *˵��: ��������Ϣ��ֻ�е���cxSaveOsdInfo ���ܰѵ�����Ϣ�������豸��
 
 *����ֵ:
 *		
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxSetOSDInfo( HANDLE hDev, LPBYTE pOsdInfo, int nLength );

/*****
�õ��豸��OSD��Ϣ������ӻص��з���
******/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxGetOSDInfo( HANDLE hDev);

/*****
ʹ�豸�������趨�õ�OSD��Ϣ��OSD��Ϣ���ñ������豸�ڲ�
���ھ�����Ҫ�仯��OSD,���ص��ô˺���
******/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxSaveOsdInfo( HANDLE hDev );

/*********************************************************************
 *����:����audio ������
 *����:
 *		hDev		 �豸���
 *	    nRateIndex ����������
		CQ_AUDIO_RATE_8000 = 8K
		CQ_AUDIO_RATE_11025 = 11.025K
		CQ_AUDIO_RATE_16000 = 16K
		CQ_AUDIO_RATE_22050 = 22.05K
		CQ_AUDIO_RATE_32000 = 32K
		CQ_AUDIO_RATE_44100 = 44.1K
 *˵��:
 
 *����ֵ:
 *		
 *********************************************************************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxSetAudioRate( HANDLE hDev ,int nRateIndex );
/*****************
�õ���Ƶ�����ʺ�ѹ����Ϣ������ӻص��з���
******************/
IPCAMSDK_API CX_ERROR CX_STDCALL  cxGetAudioInfo( HANDLE hDev);

/*********************************************************************
���豸������Ƶ�ĺ���
*********************************************************************/
/*******************
����һ��ý��ͨ��,Ĭ�������dwIP��pwPort����ΪNULL,���ص���ͨ�����
********************/
IPCAMSDK_API HANDLE		CX_STDCALL cxCreateMediaOutChn( HANDLE hDev,DWORD dwIP,WORD* pwPort );

/***********************
���һ����Ƶ֡���豸��
hMediaOutChn	���ͨ�����
pData			��Ҫ�������Ƶ֡����ָ��
dwLen			��Ƶ֡���ݳ���
dwTimeStamp		��Ƶ֡��ʱ���
dwFlag			����ǹؼ�֡��Ϊ0x10����Ϊ0
*************************/
IPCAMSDK_API CX_ERROR	CX_STDCALL cxOutVidData( HANDLE hMediaOutChn,LPBYTE pData,DWORD dwLen,DWORD dwTimeStamp,DWORD dwFlag );

/***********************
���һ����Ƶ֡���豸
��Ƶ֡�Ĵ�С������640��������16λ���ݣ����û��ѹ���Ļ�Ӧ��Ϊ1280�ֽ�
dwTimeStamp	Ϊ��Ƶʱ���������ǲɼ��������ݻ���������GetTickCount()
dwFlag		CQ_AUDIO_RATE_8000 | CQ_IS_AUDIO_COMPRESSED �������ʻ����Ƿ�ѹ��
*************************/
IPCAMSDK_API CX_ERROR	CX_STDCALL cxOutAudData( HANDLE hMediaOutChn,LPBYTE pData,DWORD dwLen,DWORD dwTimeStamp,DWORD dwFlag );

/******************
�ر�ý�����ͨ��
*********************/
IPCAMSDK_API CX_ERROR	CX_STDCALL cxCloseMediaOutChn( HANDLE hMediaOutChn );


//�����豸
IPCAMSDK_API CX_ERROR	CX_STDCALL cxResetDevice( HANDLE hDev );

//�����豸���Ӻ󣬸ı��豸�Ĺ���ģʽ
IPCAMSDK_API CX_ERROR	CX_STDCALL cxSetWorkMode( HANDLE hDev , PCQWORKMODE pstuWorkMode );
IPCAMSDK_API CX_ERROR	CX_STDCALL cxGetWorkMode( HANDLE hDev );//�õ��豸����ģʽ���ص��з���

/*********************************************************************
��Ƶ�ɼ�����
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

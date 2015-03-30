#ifndef _TVPLAYER_H
#define _TVPLAYER_H

#include <stdio.h>
#include <Windows.h>


//������ʾ�ĵ���ͨ����
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

//��Ƶ
typedef struct TV_VIDEO_CFG
{
	HWND    m_TVHwnd;         //��Ƶ��ʾ�Ĵ��ھ��
	DWORD   m_dwAddress;      //�������IP��ַ

	HANDLE	m_hCarama;        //��������	
	HANDLE	m_hDisplay;	       //��Ƶ��ʾͨ�����
	HANDLE	m_hDecoder;	       //��Ƶ���������
	LPBYTE	m_pDecoderBuffer;	//��Ƶ���뻺����

}TvVideoCfg,*pTvVideoCfg;

typedef struct USER_INFO
{
	char* pUserName; // �û���
	char* pPwd;  // ����  
}UserInfo,pUserInfo;
 
class TVPlayer
{

private:
	TVcfg   tv_cfg[TVChannelNum];

	TvVideoCfg video_cfg[TVChannelNum];
	
	HANDLE AVRecordFile[TVChannelNum];

	bool IsAudioPlay[TVChannelNum];

	UserInfo  userinfo[TVChannelNum];

	//������Ƶ������
	HANDLE	m_LocalAudioPlayer;

public:
	DWORD DecodeBufferLen;
public:
	TVPlayer();
	~TVPlayer();

	void TV_Init(short VideoChannel,HWND VideoHwnd,char* pstrAddr,char* pUsrName="galileo",char* pPwd="ipcam");	
	//---------------------------------------���Ӷ���ӿ�-----------------------------------------------------------
	//�򿪵���
	int ML_GVID_OpenTv (short theDisplay,short VideoChannel ,short theX, short theY,short theWidth,short theHeight);
	//�رյ���
	int ML_GVID_CloseTv (short theDisplay,short VideoChannel );
	//���ô���λ��
	int ML_GVID_SetTvXY(short theDisplay,short VideoChannel ,short theX, short theY);
	//���ô��ڴ�С
	int ML_GVID_SetTvWH(short theDisplay,short VideoChannel ,short theWidth, short theHeight);
	//�������
	void ML_GVID_FrostTV(short theDisplay,short VideoChannel);
	//�ⶳ����
	void ML_GVID_UnFrostTV(short theDisplay,short VideoChannel);
	//�ı��������
	void ML_GVID_ChangTvBright(short theDisplay,short VideoChannel,int theBright);
	//�ı���ӶԱȶ�
	void ML_GVID_ChangTvContrast(short theDisplay,short VideoChannel,int theContrast);
	//��������ɫ��
	void ML_GVID_ChangTvColor(short theDisplay,short VideoChannel,int theColor);
	//---------------------------------------------����---------------------------------------------------------------
	//��¼����Ƶ�ļ�
	void tshStartRecordAVFile(short VideoChannel,char *FileName = NULL);
	void tshCloseRecordAVFile(short VideoChannel);
	void tshPlayAudio(short VideoChannel);
	void tshCloseAudio(short VideoChannel);
	bool tshSetOSDInfo(short VideoChannel,LPBYTE pOSDData,RECT OSD_Rect,DWORD dwCOlorSpcae);
	void tshSetOSDStr(short VideoChannel,char* pstr,short x,short y);
	void tshSetOSDTime(short VideoChannel,short x,short y);

	// 	* ���ܣ�����͸������* pData ����* nDataLen ���ݳ��� * nChnNum ---- 0 is 485,1 is 232
	int tshSendTransparentData(short VideoChannel,char* pData,int nLen,int nChnNum);
	int tshVerifyTime(short VideoChannel);// ���ܣ�Уʱ
private:
	BOOL tshStartWindowMoveHook();
	BOOL tshCloseWindowMoveHook();

protected:
// 	typedef int (*pFunc[TVChannelNum])(LPVOID pContext,HANDLE hIpCam,int nAct,
// 									   DWORD dwParam1,DWORD dwParam2 );
	//�ص�����
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
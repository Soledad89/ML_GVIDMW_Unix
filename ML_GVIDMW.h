
/****************************************************
�м�����ƣ��ۺ�ͼ��ͼ����ʾ�м��������棩
�汾��3.1������汾��1.0��
��Ʒ��λ���廪��ѧ���ӹ���ϵ���źŲɼ����������Կ�ʵ����
�ļ��������м������ӿڶ���
****************************************************/


#ifndef __THU_6BE7D83F_50C1_4007_804D_525118CDCC34__
#define __THU_6BE7D83F_50C1_4007_804D_525118CDCC34__


#ifdef _WINDLL
	#define GVIDMW_API  extern "C" __declspec(dllexport) 
#else 
	#define GVIDMW_API  extern "C" __declspec(dllimport) 
#endif

typedef int Status;


//======��׼�й涨�Ķ���ӿ�======
// VideoChannel ȡֵ1��2
// theColor�ĸ�ʽΪ���ɸߵ��ͣ�R��G��B������  ( 0xRRGGBB00 )

//------init------
GVIDMW_API Status ML_GVID_Init();
//��ʼ����������ֵ������Ϣ��
//  -1  --  �ظ����ó�ʼ������
//  -2  --  ��־�ļ�������������û��save�ļ���
//  -3  --  �����ļ�����
//  -4  --  ͼ��ϵͳ������������û����DirectDraw����
//  -5  --  ϵͳ���ݳ������dat�ļ������ļ�
//  -6  --  �״���ϢԴ��Ϊlocal��ʽ��save�ļ�����ȱ����Ϊradar1��radar2����Ƶ�ļ�

//------PPI------
GVIDMW_API Status ML_GVID_OpenPPI(char VideoChannel,short theX, short theY , short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_ClosePPI(char VideoChannel);
GVIDMW_API Status ML_GVID_SetPPIXY(char VideoChannel ,short theX ,short theY);
GVIDMW_API Status ML_GVID_SetPPIWH(char VideoChannel ,short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_SetPPIMode(char VideoChannel, short theMode);
GVIDMW_API Status ML_GVID_SetPPICenter(char VideoChannel ,short theX ,short theY);
GVIDMW_API Status ML_GVID_SetPPIShipDir(char VideoChannel,float theShipDir);
GVIDMW_API Status ML_GVID_SetPPIColor(char VideoChannel,unsigned int theColor);
GVIDMW_API Status ML_GVID_SetPPIScan(char VideoChannel,int theScan);
GVIDMW_API Status ML_GVID_SetPPILineColor(char VideoChannel,unsigned int theColor);
GVIDMW_API Status ML_GVID_FrostPPI(char VideoChannel,char theFrost);
GVIDMW_API Status ML_GVID_SetPPIDist(char VideoChannel,short theDist);
GVIDMW_API Status ML_GVID_SetPPISwitch(char VideoChannel,short theSwitch);
GVIDMW_API Status ML_GVID_SetPPIGate(char VideoChannel,char theGate);
GVIDMW_API Status ML_GVID_SetPPITail(char VideoChannel,int theTail);

//------AR------
GVIDMW_API Status ML_GVID_OpenAr(char VideoChannel,short theX, short theY , short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_CloseAr(char VideoChannel);
GVIDMW_API Status ML_GVID_SetArXY(char VideoChannel ,short theX ,short theY);
GVIDMW_API Status ML_GVID_SetArWH(char VideoChannel ,short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_SetArGate (char VideoChannel ,char theGate);
GVIDMW_API Status ML_GVID_SetArDist (char VideoChannel ,short theDist);

//------MiniWin------
GVIDMW_API Status ML_GVID_OpenMiniWin(char VideoChannel,short theX, short theY , short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_CloseMiniWin(char VideoChannel);
GVIDMW_API Status ML_GVID_SetMiniWinXY(char VideoChannel ,short theX ,short theY);
GVIDMW_API Status ML_GVID_SetMiniWinWH(char VideoChannel ,short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_SetMiniWinMode(char VideoChannel, short theMode);
GVIDMW_API Status ML_GVID_SetMiniWinColor(char VideoChannel,unsigned int theColor);
GVIDMW_API Status ML_GVID_SetMiniWinDistDir(char VideoChannel ,short theDistance ,float theDirection);
GVIDMW_API Status ML_GVID_SetMiniWinScale(char VideoChannel,int theScale);

//------TV------
GVIDMW_API Status ML_GVID_OpenTV (char VideoChannel ,short theX, short theY,short theWidth,short theHeight);
GVIDMW_API Status ML_GVID_CloseTV (char VideoChannel );
GVIDMW_API Status ML_GVID_SetTVXY(char VideoChannel ,short theX, short theY);
GVIDMW_API Status ML_GVID_SetTVWH(char VideoChannel ,short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_FrostTV(char VideoChannel,char theFrost);
GVIDMW_API Status ML_GVID_SetTVBright(char VideoChannel,unsigned char theBright);
GVIDMW_API Status ML_GVID_SetTVContrast(char VideoChannel,unsigned char theContrast);
GVIDMW_API Status ML_GVID_SetTVColor(char VideoChannel,unsigned char theColor);


//========== ��׼�����չ�ӿ�==========
// VideoChannel ȡֵ1��2

// ����PPI��AR�ı����Ƿ���ʾ��disp��true��ʾ��false����ʾ
GVIDMW_API int tshSetArMarkDisplay(char VideoChannel,bool disp);
GVIDMW_API int tshSetRadarMarkDisplay(char VideoChannel,bool disp);

// ========== ��ͼ��չ�ӿ� ==========

 // ��ͼ�����Ͷ���(surfType)
#define SURF_PPI_BOTTOM 0
#define SURF_PPI_TOP 1
#define SURF_AR_TOP 2
#define SURF_MINI_TOP 3

GVIDMW_API int tshGetSurfaceDC(HDC* dc, char VideoChannel,int surfType);
GVIDMW_API int tshReleaseSurfaceDC(HDC dc, char VideoChannel,int surfType);

// !! ע�⣬colorKey��������COLORREF����ͬ�ڱ�׼�ӿ��е���ɫ��ʽ��
// ʹ�ú�RGB(r,g,b)���ɣ���ʽΪ���ɸߵ��ͣ�������B��G��R   ( 0x00BBGGRR )
GVIDMW_API int tshSetSurfaceColorKey(char VideoChannel,int surfType,COLORREF colorkey);
GVIDMW_API int tshSetSurfaceVisible(char VideoChannel,int surfType,BOOL visible);
GVIDMW_API int tshEraseSurface(char VideoChannel,int surfType);

// ========== ���������չ�ӿ� ==========

// ����OSD����ʾ�ַ���ʱ��
// XY����Ϊ�ַ��������Ͻǡ������Ƶ�����Ͻǡ�������,�ַ���󳤶�32
GVIDMW_API int tshSetOSDStr(char VideoChannel,char* pstr,short x,short y);
GVIDMW_API int tshSetOSDTime(char VideoChannel,short x,short y);

// ����͸���������豸�˴��ڣ�pData ����; nLen ���ݳ���; nChnNum ---- 0Ϊ485�ӿ�,1Ϊ232�ӿ�
GVIDMW_API int tshSendTransparentData(char VideoChannel,char* pData,int nLen,int nChnNum);
// Уʱ,��ϵͳʱ�䷢�͵��豸��
GVIDMW_API int tshVerifyTime(char VideoChannel);


// ========== ��¼�طŽӿ� ==========
//typedef UINT (__stdcall *FN_RADAR_REC_CALLBACK)(LPBYTE pData,DWORD pkgLen);
//typedef UINT (__stdcall *FN_RADAR_PLY_CALLBACK)(LPBYTE pBuff,DWORD bufLen,DWORD PkgNum);
//
//GVIDMW_API int tshRadarStartRecord( char VideoChannel );
//GVIDMW_API int tshRadarStopRecord( char VideoChannel );
//GVIDMW_API int tshRadarSetRecordCallback( char VideoChannel,  FN_RADAR_REC_CALLBACK lpFun);
//
//GVIDMW_API int tshRadarStartReplay( char VideoChannel, int timeStep );
//GVIDMW_API int tshRadarStopReplay( char VideoChannel );
//GVIDMW_API int tshRadarSetReplayCallback( char VideoChannel,  FN_RADAR_PLY_CALLBACK lpFun);
//
//
//GVIDMW_API int tshTVStartRecord( char VideoChannel ,char * filename);
//GVIDMW_API int tshTVStopRecord( char VideoChannel );
//
//GVIDMW_API int tshTVStartReplay( char VideoChannel,char * filename, int timeStep );
//GVIDMW_API int tshTVStopReplay( char VideoChannel );

// !!!!!!   **** ������չ�ӿڣ����հ治�ṩ ****

GVIDMW_API Status tshPluginInterface1(void *param1,void *param2,void *param3,void *param4);
GVIDMW_API Status tshPluginInterface2(void *param1,void *param2,void *param3,void *param4);
GVIDMW_API Status tshPluginInterface3(void *param1,void *param2,void *param3,void *param4);
GVIDMW_API Status tshPluginInterface4(void *param1,void *param2,void *param3,void *param4);


#endif
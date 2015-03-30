
/****************************************************
�м�����ƣ��ۺ�ͼ��ͼ����ʾ�м��
�汾��3.0
��Ʒ��λ���廪��ѧ���ӹ���ϵ���źŲɼ����������Կ�ʵ����
�ļ��������м����չ�ӿ�ʵ�ֵ����ļ�
****************************************************/


#include "StdAfx.h"

#include "ML_GVIDMW.h"
#include "ML_GVIDMW_sys.h"
#include "C_THU_ML_GVID.h"

extern C_THU_ML_GVID * mywidget;

extern CDrawingManager* p_dm[2];

extern bool bInit;
extern bool bInitP;

extern bool bInitPPI[2];
extern bool bInitAR[2];
extern bool bInitMiniWin[2];
extern bool bInitTV[2];

extern s_ppi_video_for_disp *dispbuf;// Ϊ������PPI����ӿ�


//=========== ϵͳ�ṩ�ĸ���ӿ� =============
//    VideoChannel ��Ϊ 1,2
//    VideoChannel��ÿ�������ж������ȡֵ��飬
//    !!! ���Ҽ�������1���Ա�ֱ�������������� !!!

// ---- ��ͼ��չ�ӿ� ----
#define SURF_FOR_USER_NUM 4
const e_surface_type _surfTypeTrans[SURF_FOR_USER_NUM]={E_PPI_BOTTOM,E_PPI_TOP,E_AR_TOP,E_MINI_TOP};

 int tshLockSurface(char VideoChannel,int surfType)
{
	INIT_CHECK
		VC_CHECK
		VR_CHECK(surfType,0,SURF_FOR_USER_NUM-1)

		return p_dm[VideoChannel]->LockSurface(_surfTypeTrans[surfType]);
}

 int tshUnLockSurface(char VideoChannel,int surfType)
{
	INIT_CHECK
		VC_CHECK
		VR_CHECK(surfType,0,SURF_FOR_USER_NUM-1)

		return p_dm[VideoChannel]->UnLockSurface(_surfTypeTrans[surfType]);
}


GVIDMW_API int tshGetSurfaceDC(HDC* dc, char VideoChannel,int surfType)
{
	INIT_CHECK
		VC_CHECK
		VR_CHECK(surfType,0,SURF_FOR_USER_NUM-1)

		return p_dm[VideoChannel]->GetSurfaceDC(dc, _surfTypeTrans[surfType]);
}

GVIDMW_API int tshReleaseSurfaceDC(HDC dc, char VideoChannel,int surfType)
{
	INIT_CHECK
		VC_CHECK
		VR_CHECK(surfType,0,SURF_FOR_USER_NUM-1)

		return p_dm[VideoChannel]->ReleaseSurfaceDC(dc,_surfTypeTrans[surfType]);
}


GVIDMW_API int tshSetSurfaceColorKey(char VideoChannel,int surfType,COLORREF colorkey)
{
	INIT_CHECK
		VC_CHECK
		VR_CHECK(surfType,0,SURF_FOR_USER_NUM-1)

		return p_dm[VideoChannel]->SetSurfaceColorKey(_surfTypeTrans[surfType],colorkey);
}

GVIDMW_API int tshSetSurfaceVisible(char VideoChannel,int surfType,BOOL visible)
{
	INIT_CHECK
		VC_CHECK
		VR_CHECK(surfType,0,SURF_FOR_USER_NUM-1)

		return p_dm[VideoChannel]->SetSurfaceVisible(_surfTypeTrans[surfType],visible);
}

GVIDMW_API int tshEraseSurface(char VideoChannel,int surfType)
{
	INIT_CHECK
		VC_CHECK
		VR_CHECK(surfType,0,SURF_FOR_USER_NUM-1)

		return p_dm[VideoChannel]->EraseSurface(_surfTypeTrans[surfType]);
}

//----------------------- ���������ʾ�ӿ� -------------------------
GVIDMW_API int tshSetArMarkDisplay(char VideoChannel,bool disp)
{
	INIT_CHECK
		INIT_AR_CHECK
		VC_CHECK

		p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->setArMarkDisplay(disp);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();

	return 1;
}
GVIDMW_API int tshSetRadarMarkDisplay(char VideoChannel,bool disp)
{
	INIT_CHECK
		INIT_PPI_CHECK
		VC_CHECK

	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->setRadarMarkDisplay(disp);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();

	return 1;
}


//--------------------------- �״���ӿ� --------------------------
//-------------------------- �ð汾�ݲ��ṩ -------------------------
#define TMP_RET int

TMP_RET tshSetLayerDisplay(char VideoChannel,int theLayer,int flag)//layer is 1~5, flag:not 0-display;0-dont show.
{
	if(theLayer<1 || theLayer>5)return -1;
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setLayerDisplay(theLayer,flag);
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshSetHollow(char VideoChannel,int r)//unit is pixel
{
	if(r<0 || r>60)return -1;
	VC_CHECK

	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ppi->setHollowNum(r);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();

	return 1;
}

TMP_RET tshSetYhTime(char VideoChannel,int d)//20 steps
{
	if(d<1 || d>20)return -1;
	VC_CHECK

	mywidget->widgets[VideoChannel]->yh->setYhWeight(d*5);
	
	return 1;
}


TMP_RET tshSetBackColor(char VideoChannel,unsigned int color)//color Ϊ���ֽڣ��Ӹߵ��ͷֱ�Ϊ��R��G��B��������
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setBackColor(color);	
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshSetForeColor(char VideoChannel,unsigned int color)
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setForeColor(color);	
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshSetEchoColor(char VideoChannel,unsigned int color)
{
	ML_GVID_SetPPIColor(VideoChannel,color);
	return 1;
}

TMP_RET tshSetPPICenter(char VideoChannel,int theX,int theY)
{

	ML_GVID_SetPPICenter(VideoChannel,theX,theY);
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setOrigin(theX,theY);
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshResetPPICenter(char VideoChannel)
{
	ML_GVID_SetPPICenter(VideoChannel,600,600);
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setOrigin(600,600);
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}


TMP_RET tshSetRange(char VideoChannel,float range,float bstep,float tstep)// units are all kilometer
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setRange(range,bstep,tstep);	
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshSetAziLine(char VideoChannel,float bstep,float tstep)// units are all degree
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setAziLine(bstep,tstep);
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshSetBoldAziLineBright(char VideoChannel,unsigned char bright)
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setBoldAziLineBright(bright);
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshSetThinAziLineBright(char VideoChannel,unsigned char bright)
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setThinAziLineBright(bright);	
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshSetBoldDisLineBright(char VideoChannel,unsigned char bright)
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setBoldDisLineBright(bright);	
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshSetThinDisLineBright(char VideoChannel,unsigned char bright)
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setThinDisLineBright(bright);
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshSetBackBright(char VideoChannel,unsigned char bright)
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setBackBright(bright);	
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

TMP_RET tshSetFixCircleLabel(char VideoChannel,int type,float*pLabel,int lNum,int pn)
//type 0�����̾��ȱ�ע����ʱ������������Ч����
//       1��ָ�����ֱ�ע��pLabel�����ֱ�ע��ָ�룻
//lNum�����ֱ�ע��Ŀ��pn��С��������ʾ��λ��
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_PPI_RAW);
	mywidget->widgets[VideoChannel]->map->setFixCircleLabel(type,pLabel,lNum,pn);
	p_dm[VideoChannel]->UnLockSurface(E_PPI_RAW);

	return 1;
}

//------------- AR display -----------------
TMP_RET tshSetARDisplay(char VideoChannel)
{
	VC_CHECK

	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->setAROnlyDisplay();	
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();

	return 1;

}
TMP_RET tshSetARForeColor(char VideoChannel,unsigned int c)
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_AR_RAW);
	mywidget->widgets[VideoChannel]->ar->setForeColor(c);
	p_dm[VideoChannel]->UnLockSurface(E_AR_RAW);

	return 1;
}
TMP_RET tshSetARMeshBright(char VideoChannel,unsigned char b)
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_AR_RAW);
	mywidget->widgets[VideoChannel]->ar->setMeshBright(b);	
	p_dm[VideoChannel]->UnLockSurface(E_AR_RAW);

	return 1;
}
TMP_RET tshARUpdate(char VideoChannel,unsigned char *d)
{
	int len=1200;

	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_AR_RAW);
	mywidget->widgets[VideoChannel]->ar->ARDraw(d,len);
	p_dm[VideoChannel]->UnLockSurface(E_AR_RAW);

	return 1;

}

TMP_RET tshSetARRange(char VideoChannel,int r)//����AR���̣����ڱ�ע����λ��km
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_AR_RAW);
	mywidget->widgets[VideoChannel]->ar->setARRange(r);	
	p_dm[VideoChannel]->UnLockSurface(E_AR_RAW);

	return 1;
}
TMP_RET tshSetAREnlargeRegion(char VideoChannel,int st,int ed)//���÷Ŵ�������ʼ����(st)����ֹ����(ed)����λ��km
{
	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_AR_RAW);
	mywidget->widgets[VideoChannel]->ar->setEnlargeRegion(st,ed);	
	p_dm[VideoChannel]->UnLockSurface(E_AR_RAW);

	return 1;
}


//-------------- ��Ϣ����ʾ ----------------
typedef struct  interface_pkg
{
	unsigned short	 pkgNum;//�����,��ʾ���ݰ�֮���ǰ���ϵ��ȡֵ��Χ 0~65535
	unsigned short	 azi;//��λ,��������ķ�λֵ��ȡֵ��Χ0~4095����Ӧ0~360��
	char video[1200];//��Ƶ,�ϲ���������ʾ���״���Ƶ�����������������ٶ���Ϊ�̶�ֵ
}interface_pkg;

Status tshInputRadarPkg(char VideoChannel,interface_pkg *dat)
{
	VC_CHECK

	static int lastAzi1=0;
	static int lastAzi2=0;

	int lastAzi;
	int curAzi;
	int i;

	curAzi = dat->azi;

	if(VideoChannel==0)
		lastAzi=lastAzi1;
	else if(VideoChannel==1)
		lastAzi=lastAzi2;
	else
		return -1;

	if(curAzi>lastAzi && curAzi<lastAzi+20)
	{
		dispbuf->znum=curAzi-lastAzi;
		for(i=0;i<dispbuf->znum && i<max_azi_num_per_video;i++)
			dispbuf->azicnt[i]=lastAzi+i;
	}
	else if(curAzi<lastAzi && curAzi<20 && lastAzi > 4096-20)
	{
		dispbuf->znum=4096+curAzi-lastAzi;
		for(i=0;i<dispbuf->znum && i<max_azi_num_per_video;i++)
			dispbuf->azicnt[i]=(lastAzi+i)%4096;
	}
	else
	{
		dispbuf->znum=1;
		dispbuf->azicnt[0]=curAzi;
		//return;
	}

	lastAzi = curAzi;

	if(VideoChannel==0)
		lastAzi1=lastAzi;
	else
		lastAzi2=lastAzi;

	memcpy(dispbuf->videodata,dat->video,1200);//pData��ǰ�ĸ��ֽ�Ϊ������Ϣ

	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->dispUpdate(VideoChannel,dispbuf);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();

	return 0;

}


//  ��ʼ�����漰�������

int tshInitSurface(char VideoChannel)
{
	VC_CHECK

	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->initSurface(VideoChannel);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();

	return 1;
}


int tshClearScreen(char VideoChannel)
{
	VC_CHECK

		p_dm[VideoChannel]->LockAllRadarRawSurfaces();

	mywidget->clearScreen(VideoChannel);

	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();

	return 1;
}

// ==============  ���������չ�ӿ�  =================
GVIDMW_API int tshSetOSDStr(char VideoChannel,char* pstr,short x,short y)
{
	VC_CHECK
	mywidget->player->tshSetOSDStr(VideoChannel,pstr,x,y);
	return 1;
}
GVIDMW_API int tshSetOSDTime(char VideoChannel,short x,short y)
{
	VC_CHECK
		mywidget->player->tshSetOSDTime(VideoChannel,x,y);
	return 1;
}

// 	* ���ܣ�����͸������* pData ����* nDataLen ���ݳ��� * nChnNum ---- 0 is 485,1 is 232
GVIDMW_API int tshSendTransparentData(char VideoChannel,char* pData,int nLen,int nChnNum)
{
	VC_CHECK
		mywidget->player->tshSendTransparentData(VideoChannel,pData,nLen,nChnNum);
	return 1;
}
GVIDMW_API int tshVerifyTime(char VideoChannel)// ���ܣ�Уʱ
{
	VC_CHECK
		mywidget->player->tshVerifyTime(VideoChannel);
	return 1;
}

//============ ������չ�ӿ� =============

#define BASE_PLUGIN_CODE 6335

//------���ݰ�ֱ������------
GVIDMW_API Status tshPluginInterface2(void *param1,void *param2,void *param3,void *param4)
{
	if(*(int*)param4 != BASE_PLUGIN_CODE)return 1;

	return tshInputRadarPkg(*(char*)param2,(interface_pkg*)param1);
}
//-----����AR��------
GVIDMW_API Status tshPluginInterface3(void *param1,void *param2,void *param3,void *param4)
{
	if(*(int*)param4 != BASE_PLUGIN_CODE)return 1;

	int len=*(int*)param3;
	char VideoChannel = *(char*)param2;

	VC_CHECK

	p_dm[VideoChannel]->LockSurface(E_AR_RAW);
	mywidget->widgets[VideoChannel]->ar->ARDraw( (unsigned char*)param1,len);
	p_dm[VideoChannel]->UnLockSurface(E_AR_RAW);

	return 1;

}

//-------��ʱ������--------
extern int timersID[4];

void CALLBACK yhTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK radarTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK FlipFrontSurfaceTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK MapTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime);

GVIDMW_API Status tshPluginInterface4(void *param1,void *param2,void *param3,void *param4)
{
	int code = *(int*)param4;
	int t ;
	if( code< BASE_PLUGIN_CODE)return 1;

	if(code-BASE_PLUGIN_CODE<4) t = *(int*)param1;

	switch(code-BASE_PLUGIN_CODE)
	{
	case 0:
		timersID[0]=::SetTimer(NULL,100,t,radarTimerProc);//radar display hMyWnd
		break;
	case 1:
		timersID[1]=::SetTimer(NULL,101,t,FlipFrontSurfaceTimerProc);//flip to front surface
		break;
	case 2:
		timersID[2]=::SetTimer(NULL,103,t,yhTimerProc);//yh
		break;
	case 3:
		timersID[3]=::SetTimer(NULL,104,t,MapTimerProc);//�۱껷,�̶���,ʮ���ߵ�
		break;

	case 4:
		if(timersID[0])::KillTimer(NULL,timersID[0]);//radar display hMyWnd
		break;
	case 5:
		if(timersID[1])::KillTimer(NULL,timersID[1]);//flip to front surface
		break;
	case 6:
		if(timersID[2])::KillTimer(NULL,timersID[2]);//yh
		break;
	case 7:
		if(timersID[3])::KillTimer(NULL,timersID[3]);//�۱껷,�̶���,ʮ���ߵ�
		break;
	}

	return 1;

}

//--------�������û��ӿڣ�����Ժ��ʾ�õĽӿڣ�---------
GVIDMW_API Status tshPluginInterface1(void *param1,void *param2,void *param3,void *param4)
{
	int code = *(int*)param4;
	if( code< BASE_PLUGIN_CODE)return 1;

	switch(code-BASE_PLUGIN_CODE)
	{
	//case 0:
	//	return tshSetLayerDisplay(*(char*)param1,*(char*)param2,*(char*)param3);//layer is 1~5, flag:not 0-display;0-dont show.

	case 1:
		return tshSetHollow(*(char*)param1,*(int*)param2);//unit is pixel
	
	case 2:
		return tshSetYhTime(*(char*)param1,*(int*)param2);//20 steps

	case 3:
		return tshSetBackColor(*(char*)param1,*(unsigned int*)param2);//color Ϊ���ֽڣ��Ӹߵ��ͷֱ�Ϊ��R��G��B��������

	case 4:
		return tshSetForeColor(*(char*)param1,*(unsigned int*)param2);

	case 5:
		return tshSetEchoColor(*(char*)param1,*(unsigned int*)param2);

	case 6:
		return tshSetPPICenter(*(char*)param1,*(int*)param2,*(int*)param3);

	case 7:
		return tshResetPPICenter(*(char*)param1);

	case 8:
		return tshSetRange(*(char*)param1,*(float*)param2,*((float*)param2+1),*((float*)param2+2));// units are all kilometer

	case 9:
		return tshSetAziLine(*(char*)param1,*(float*)param2,*(float*)param3);// units are all degree

	case 10:
		return tshSetBoldAziLineBright(*(char*)param1,*(unsigned char*)param2);

	case 11:
		return tshSetThinAziLineBright(*(char*)param1,*(unsigned char*)param2);

	case 12:
		return tshSetBoldDisLineBright(*(char*)param1,*(unsigned char*)param2);


	case 13:
		return tshSetThinDisLineBright(*(char*)param1,*(unsigned char*)param2);


	case 14:
		return tshSetBackBright(*(char*)param1,*(unsigned char*)param2);

	case 15:
		return tshSetFixCircleLabel(*(char*)param1,*(int*)param3,(float*)param2,*((int*)param3+1),*((int*)param3+2));

	//------------- AR display -----------------
	//case 16:
	//	return tshSetARDisplay(*(char*)param1);

	case 17:
		return tshSetARForeColor(*(char*)param1,*(unsigned int*)param2);

	case 18:
		return tshSetARMeshBright(*(char*)param1,*(unsigned char*)param2);

	//case 19:
	//	return tshARUpdate(*(char*)param1,*(unsigned char*)param2);

	//case 20:
	//	return tshSetARRange(*(char*)param1,*(int*)param2);//����AR���̣����ڱ�ע����λ��km

	//case 21:
	//	return tshSetAREnlargeRegion(*(char*)param1,*(int*)param2,*(int*)param3);//���÷Ŵ�������ʼ����(st)����ֹ����(ed)����λ��km

	}

	return 1;
}
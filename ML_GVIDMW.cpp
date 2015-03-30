
/****************************************************
中间件名称：综合图形图像显示中间件
版本：3.1
出品单位：清华大学电子工程系，信号采集、处理与显控实验室
文件描述：中间件标准接口实现的主文件
****************************************************/


#include "StdAfx.h"

#include <stdio.h>
#include <ddraw.h>
#include <Windows.h>

#include "ML_GVIDMW.h"
#include "ML_GVIDMW_sys.h"

#include "DrawingManager_DD.h"

#include "C_THU_ML_GVID.h"

#include "Communication.h"
#include "deqtrace.h"
#include "ppidef.h"
#include "layers.h"
#include "secSN.h"
#include "track.h"

#include "radarShow.h"


/////////////////////////// 全局变量定义 ///////////////////////////////////

//==== 加密相关 ====
#define __SN_ST_POS 75
#define __SN_JMP_NUM 7
#define __SN_CODE_LEN 9

#define __AU_ST_POS 963
#define __AU_JMP_NUM 16
#define __AU_CODE_LEN 15

char snfilename[]="./sn.dat";
char aufilename[]="./dat/au.dat";
char addr[]="192.168.1.101";
//----------------------------

#define NAME    "THU"
#define TITLE   "radar1"

CRITICAL_SECTION cs_callback;

C_THU_ML_GVID * mywidget=NULL;
CDrawingManager* p_dm[2];

//关于雷达PPI视频窗口，小窗口，AR窗口，电视窗口的定义
HWND radar_ppi_hwnd[2];
RECT radar_ppi_rect,screen_ppi_rect[2];

HWND radar_ar_hwnd[2];
RECT radar_ar_rect,screen_ar_rect[2];

HWND radar_miniwin_hwnd[2];
RECT radar_miniwin_rect,screen_miniwin_rect[2];

HWND tv_hwnd[2];
RECT tv_rect,screen_tv_rect[2];

typedef struct _tshrect
{
	LONG    start_x;
	LONG    start_y;
	LONG    width;
	LONG    height;
} TSH_RECT, *PTSH_RECT;//用来记录当前窗口的位置

TSH_RECT  ppi_tsh_rect[2];
TSH_RECT  ar_tsh_rect[2];
TSH_RECT  miniwin_tsh_rect[2];
TSH_RECT  tv_tsh_rect[2];

RECT miniwin_src_rect[2];
//---------------------------------------
HWND hMyWnd = NULL;

FILE * debugfp;

int rc;
//int mode;
bool isOK=false;

bool bInit=false;
bool bInitP=false;

bool bInitPPI[2] = {false,false};
bool bInitAR[2] = {false,false};
bool bInitMiniWin[2] = {false,false};
bool bInitTV[2] = {false,false};

s_in_param initP;
int initFlag; // 初始化标志，0--窗口外建，1--窗口内建 

int screenMode;// 0--双屏，横排； 1--双屏，竖排； 2--单屏

s_ppi_video_for_disp *dispbuf;// 为了网络PPI对外接口

int timersID[4]={0,0,0,0};
//////////////////////////////定时器回调函数//////////////////
void CALLBACK yhTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK radarTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK FlipFrontSurfaceTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime);
void CALLBACK MapTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime);
DWORD WINAPI FlipThread(void* pParam);

//////////////////////////////暂时使用//////////////////
LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND tshCreateWindow(char * name,char VideoChannel,short theX, short theY , short theWidth, short theHeight);

bool initPpiThread(void);
void freePpiThread(void);
void pkgTimerHandler(void);
void yhTimerHandler(void);
//---------------------------------------------------------
void tshInitRect();//各绘图区在后页面上的位置
RECT getScrRect(float b);
bool checkXY(int x,int y);

void tsgGetClient(char VideoChannel,HWND hwnd,RECT *screen_rect)
{	//获取窗口客户区的坐标,把客户区的大小写进第二个参数所指的Rect结构当中。
	::GetClientRect(hwnd,screen_rect);
	//fprintf(debugfp,"1screen_rect->right = %d\n",screen_rect->right);

	//将你打开的APP中客户区的坐标点信息转换为整个屏幕的坐标
	ClientToScreen(hwnd,(POINT *)screen_rect);
	//fprintf(debugfp,"2screen_rect->left = %d\n",screen_rect->left);

	ClientToScreen(hwnd,(POINT *)screen_rect+1);
	//fprintf(debugfp,"3screen_rect->left = %d\n",screen_rect->right);

	screen_rect->left%=1600;
	screen_rect->right%=1600;
	//fprintf(debugfp,"2.screen_rect->left = %d\n",screen_rect->left);
	//fprintf(debugfp,"2.screen_rect->left = %d\n",screen_rect->right);
}

/////////////////////////////////////////////////////////////////////////////

bool WINAPI DllEntryPoint(HINSTANCE hDLL,DWORD dwReason,LPVOID Reserved)
{
	
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		isOK=false;
//		mode=0;
		break;
	case DLL_PROCESS_DETACH:
		if(timersID[0])KillTimer(NULL,timersID[0]);
		if(timersID[1])KillTimer(NULL,timersID[1]);
		if(timersID[2])KillTimer(NULL,timersID[2]);
		if(timersID[3])KillTimer(NULL,timersID[3]);

		ML_GVID_CloseTV(1);
		ML_GVID_CloseTV(2);

		if(mywidget)delete mywidget;
		if(debugfp)fclose(debugfp);
	
		break;
	}
	
	return true;
}

Status tshWindowInit()
{
	WNDCLASS                    wc;											//由系统支持的结构，用来储存某一类窗口的信息：包含窗口的全部信息
	wc.lpszClassName = NAME;												//窗口类名
	wc.lpfnWndProc = MyWndProc;												//窗口处理函数
	wc.style = CS_VREDRAW | CS_HREDRAW|CS_NOCLOSE;							//窗口类型
	wc.hInstance = NULL;													//实例句柄
	wc.hIcon = 0;//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON))		//窗口的最小化图标
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);								//窗口的鼠标光标
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);					//(HBRUSH) (COLOR_WINDOW + 1);mybrush;//窗口背景色
	wc.lpszMenuName = NULL;													//窗口菜单
	wc.cbClsExtra = 0;														//窗口扩展
	wc.cbWndExtra = 0;														//窗口实例扩展

	if (!RegisterClass(&wc))return -1;

	//----------------------------开始生成窗口-----------------------------------------
	for (int i = 0;i<2;i++)
	{
		if(initP._param[i].radarChannel.hPPIWnd)
		{
			radar_ppi_hwnd[i] = tshCreateWindow("PPI_",i+1,0,0,0,0);
			initP._param[i].radarChannel.hPPIWnd = radar_ppi_hwnd[i];
		}

		if(initP._param[i].radarChannel.hMiniWnd)
		{
			radar_miniwin_hwnd[i] = tshCreateWindow("MiniWin_",i+1,0,0,0,0);
			initP._param[i].radarChannel.hMiniWnd = radar_miniwin_hwnd[i];
		}

		if(initP._param[i].radarChannel.hARWnd)
		{
			radar_ar_hwnd[i] = tshCreateWindow("AR_",i+1,0,0,0,0);
			initP._param[i].radarChannel.hARWnd = radar_ar_hwnd[i];
		}

		if(initP._param[i].tvChannel.hTVWnd)
		{
			tv_hwnd[i] = tshCreateWindow("TV_",i+1,0,0,0,0);
			initP._param[i].tvChannel.hTVWnd = tv_hwnd[i];
		}

		//ShowWindow(radar_ppi_hwnd[i], SW_HIDE);
		//ShowWindow(radar_miniwin_hwnd[i], SW_HIDE);
		//ShowWindow(radar_ar_hwnd[i], SW_HIDE);
		//ShowWindow(tv_hwnd[i], SW_HIDE);
	}

	return 1;
}

int selfInitParam()
{
	//---- init param ----

	char str[20];
	int port;
	char filename[]="./dat/sysconfig.ini";
	int rt;

	char sorStr[20];
	e_source_type source;
	rt = GetPrivateProfileString("radar","source",NULL,sorStr,20,filename);

	if( rt>0 && (strcmp(sorStr,"local")==0) )
		source = E_CARD;
	else
		source = E_NET;

	//       channel 1
	initP._param[0].ID=1;
	initP._param[0].refreshRate=30;
	initP._param[0].radarChannel.ID=1;

	initP._param[0].radarChannel.hPPIWnd= (HWND)1;
	initP._param[0].radarChannel.hARWnd= (HWND)1;
	initP._param[0].radarChannel.hMiniWnd= (HWND)1;

	initP._param[0].radarChannel.PPIWidth=1200;
	initP._param[0].radarChannel.PPIHeight=1200;
	initP._param[0].radarChannel.hasTop=true;
	initP._param[0].radarChannel.hasBottom=true;
	initP._param[0].radarChannel.source=source;
	initP._param[0].radarChannel.hwCode=0x01;

	initP._param[0].tvChannel.ID=1;
	initP._param[0].tvChannel.hTVWnd=(HWND)1;

	rt = GetPrivateProfileString("tv1","address",NULL,initP._param[0].tvChannel.netConfig.address,20,filename);
	if(rt<=0)return -1;
	rt = GetPrivateProfileString("tv1","user",NULL,initP._param[0].tvChannel.netConfig.user,20,filename);
	if(rt<=0)return -1;
	rt = GetPrivateProfileString("tv1","pwd",NULL,initP._param[0].tvChannel.netConfig.pwd,20,filename);
	if(rt<=0)return -1;

	//       channel 2
	initP._param[1].ID=2;
	initP._param[1].refreshRate=30;
	initP._param[1].radarChannel.ID=2;

	initP._param[1].radarChannel.hPPIWnd= (HWND)1;
	initP._param[1].radarChannel.hARWnd= (HWND)1;
	initP._param[1].radarChannel.hMiniWnd= (HWND)1;

	initP._param[1].radarChannel.PPIWidth=1200;
	initP._param[1].radarChannel.PPIHeight=1200;
	initP._param[1].radarChannel.hasTop=true;
	initP._param[1].radarChannel.hasBottom=true;
	initP._param[1].radarChannel.source=source;
	initP._param[1].radarChannel.hwCode=0x02;

	initP._param[1].tvChannel.ID=2;
	initP._param[1].tvChannel.hTVWnd=(HWND)1;

	rt = GetPrivateProfileString("tv2","address",NULL,initP._param[1].tvChannel.netConfig.address,20,filename);
	if(rt<=0)return -1;
	rt = GetPrivateProfileString("tv2","user",NULL,initP._param[1].tvChannel.netConfig.user,20,filename);
	if(rt<=0)return -1;
	rt = GetPrivateProfileString("tv2","pwd",NULL,initP._param[1].tvChannel.netConfig.pwd,20,filename);
	if(rt<=0)return -1;

	return 1;
}

//---------- 用户自定义初始化参数的接口 ----------
//通过该函数将初始化参数传进来，然后再调用ML_GVID_Init()初始化中间件
//否则，中间件将采用默认方式初始化，此时无显示，只为接口测试用
int tshGetInitParam(s_init_param* param1,s_init_param* param2)
{
	if(param1!=NULL)memcpy(param1,&initP._param[0],sizeof(s_init_param));
	if(param2!=NULL)memcpy(param2,&initP._param[1],sizeof(s_init_param));
	return 1;
}

int tshSetInitParam(s_init_param* param1,s_init_param* param2,int flag)
{
	if(bInit)
		return -1;
	else
	{
		if(param1!=NULL)memcpy(&initP._param[0],param1,sizeof(s_init_param));
		if(param2!=NULL)memcpy(&initP._param[1],param2,sizeof(s_init_param));
		initFlag=flag;

		bInitP=true;
		return 1;
	}
}

//======标准中规定的对外接口======
//------init------
GVIDMW_API int ML_GVID_Init()
{
	if(bInit == true) return -1;

	int ret;

	//DWORD tickcount[8];
	//tickcount[0]=GetTickCount();
	InitializeCriticalSection(&cs_callback);					//初始化一个临界资源对象

	//---- create debug file ----
	if( (debugfp=fopen("./save/dlldebug.txt","w"))==NULL ){
		return -2;
	}

	if( selfInitParam()<0 )return -3;//使用内建初始化参数
	tshInitRect();

	//if( bInitP && initFlag==0)//使用外建窗口
	//{
	//	for(int i=0;i<2;++i)
	//	{
	//		radar_miniwin_hwnd[i]=initP._param[i].radarChannel.hMiniWnd;
	//		radar_ar_hwnd[i]=initP._param[i].radarChannel.hARWnd;
	//		radar_ppi_hwnd[i]=initP._param[i].radarChannel.hPPIWnd;

	//		tv_hwnd[i]=initP._param[i].tvChannel.hTVWnd;
	//	}
	//}
	//else
		tshWindowInit();//内建窗口

	hMyWnd = radar_ppi_hwnd[0];

	//---- 横竖屏判断 ----
	int sx=GetSystemMetrics(78);									//用于得到被定义的系统数据或者系统配置信息.
	int sy=GetSystemMetrics(79);

	// screenMode : 0--双屏，横排； 1--双屏，竖排； 2--单屏
	if(sx<=1600 && sy<=1200)screenMode=2;
	else if(sx>1600 && sy<=1200)screenMode=0;
	else if(sx<=1600 && sy>1200)screenMode=1;
	else screenMode=0;

	errno_t err;
	int i;

	//---- create drawing context ----

	p_dm[0]=new CDrawingManager;
	p_dm[1]=new CDrawingManager;

	ret=p_dm[0]->init(initP._param[0]);
	if(ret!=DM_OK)
	{
		fprintf(debugfp,"error init DrawingManager 1! err no = %d\n",ret);
		return -4;
	}
	ret=p_dm[1]->init(initP._param[1]);
	if(ret!=DM_OK)
	{
		fprintf(debugfp,"error init DrawingManager 2! err no = %d\n",ret);
		return -4;
	}

	p_dm[0]->SetSurfaceVisible(E_PPI_BOTTOM,false);
	p_dm[0]->SetSurfaceVisible(E_PPI_TOP,false);
	p_dm[0]->SetSurfaceVisible(E_AR_TOP,false);
	p_dm[0]->SetSurfaceVisible(E_MINI_TOP,false);

	p_dm[1]->SetSurfaceVisible(E_PPI_BOTTOM,false);
	p_dm[1]->SetSurfaceVisible(E_PPI_TOP,false);
	p_dm[1]->SetSurfaceVisible(E_AR_TOP,false);
	p_dm[1]->SetSurfaceVisible(E_MINI_TOP,false);

	//---- 输出初始化参数 ----
	//fprintf(debugfp,"init param:\n");
	s_net_config* p;
	for(int i=0;i<2;++i)
	{
		fprintf(debugfp,"Channel %d :\n",i);
		if(initP._param[i].radarChannel.ID>=0)
		{
			if(initP._param[i].radarChannel.source == E_NET || initP._param[i].radarChannel.source == E_SWITCH)
			{
				p=&initP._param[i].radarChannel.netConfig;
				fprintf(debugfp,"use net Radar --- IP=%s\tPORT=%d\tTYPE=%d\n",p->address,p->port,p->accessType);
			}
			else if(initP._param[i].radarChannel.source == E_CARD || initP._param[i].radarChannel.source == E_SWITCH)
			{
				fprintf(debugfp,"use card Radar ---- HW code=%d\n",initP._param[i].radarChannel.hwCode);
			}
		}

		fprintf(debugfp,"--------------------------------\n");
		if(initP._param[i].tvChannel.ID>=0)
		{
			if(initP._param[i].tvChannel.source == E_NET || initP._param[i].tvChannel.source == E_SWITCH)
			{
				p=&initP._param[i].tvChannel.netConfig;
				fprintf(debugfp,"use net TV --- IP=%s\tPORT=%d\tTYPE=%d\n",p->address,p->port,p->accessType);
			}
			else if(initP._param[i].tvChannel.source == E_CARD || initP._param[i].tvChannel.source == E_SWITCH)
			{
				fprintf(debugfp,"use card TV ---- HW code=%d\n",initP._param[i].tvChannel.hwCode);
			}
		}

		fprintf(debugfp,"--------------------------------\n");
	}

	fprintf(debugfp,"--------------------------------\n");

	// create main class
	mywidget = new C_THU_ML_GVID(&initP,p_dm[0],p_dm[1]);
	fprintf(debugfp,"--------the outer main class created!--------\n");

	if(mywidget==NULL)return -5;

	dispbuf = new s_ppi_video_for_disp;								//PPI显示数据接口

	isOK=true;

	tshClearScreen(1);//清内部绘图层数据结构（Layer）
	tshClearScreen(2);
	
	radarOpenState[0]=false;
	radarOpenState[1]=false;

	if(initP._param[0].radarChannel.source == E_CARD)
	{
		if(!initPpiThread()) 
		{
			fprintf(debugfp,"init ppi error! file may not exist.\n");
			return -6;
		}
		else 
			timersID[0]=::SetTimer(NULL,100,20,radarTimerProc);//radar display hMyWnd
	}

	timersID[1]=::SetTimer(NULL,101,50,FlipFrontSurfaceTimerProc);//flip to front surface
	timersID[2]=::SetTimer(NULL,103,50,yhTimerProc);//yh
	timersID[3]=::SetTimer(NULL,104,3000,MapTimerProc);//聚标环,刻度线,十字线等

	bInit=true;

	//DWORD threadID;
	//CreateThread(NULL,0,FlipThread,0,0,&threadID);

	return 1;
}

//  利用垂直同步翻屏跑不动？？
DWORD WINAPI FlipThread(void* pParam)
{
	LPDIRECTDRAW7 ddo = p_dm[0]->getDirectDrawObject();
	int c = 0;
	while(true)
	{
		HRESULT r = IDirectDraw_WaitForVerticalBlank(ddo,DDWAITVB_BLOCKBEGIN,NULL);

		if(r==DD_OK)
		{
				FlipFrontSurfaceTimerProc(NULL,NULL, NULL, NULL);
		}
		else
			break;
	}

	return 1;

}

inline void transPos(short &x,short &y)
{

}

bool checkXY(int x,int y)
{
	switch(screenMode)
	{
	case 0:
		if(x<0 || x>=3200 || y<0 || y>=1200)
			return false;
		break;
	case 1:
		if(x<0 || x>=1600 || y<0 || y>=2400)
			return false;
		break;
	case 2:
		if(x<0 || x>=1600 || y<0 || y>=1200)
			return false;
		break;
	}

	return true;
}

void CALLBACK yhTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime)
{

	if(radarOpenState[0]) // add lsd 100326
	{
		for(int i=0;i<4;i++){
			tshTestYh(0);
		}
	}

	if(radarOpenState[1])
	{
		for(int i=0;i<4;i++){
			tshTestYh(1);

		}
	}
}
void CALLBACK radarTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime)
{
	//EnterCriticalSection(&cs_callback);
		pkgTimerHandler();
		//MessageBox(radar_ppi_hwnd[0],"ads","asd",NULL);
	//LeaveCriticalSection(&cs_callback);

}
void CALLBACK FlipFrontSurfaceTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime)
{
	int i =0;
	HDC dc;
	int n;

	//   垂直同步
	LPDIRECTDRAW7 ddo = p_dm[0]->getDirectDrawObject();
	HRESULT r = IDirectDraw_WaitForVerticalBlank(ddo,DDWAITVB_BLOCKBEGIN,NULL);

	//EnterCriticalSection(&cs_callback);

		for (i = 0;i<2;i++)
		{
			if (bInitPPI[i])
			{			
				::GetClientRect(radar_ppi_hwnd[i],&screen_ppi_rect[i]);
				dc=::GetDC(radar_ppi_hwnd[i]);
				p_dm[i]->WindowDisplay_DC(dc,E_PPI,screen_ppi_rect[i]);
				::ReleaseDC(radar_ppi_hwnd[i],dc);

				if (bInitAR[i])
				{			
					::GetClientRect(radar_ar_hwnd[i],&screen_ar_rect[i]);
					dc=::GetDC(radar_ar_hwnd[i]);
					p_dm[i]->WindowDisplay_DC(dc,E_AR,screen_ar_rect[i]);
					::ReleaseDC(radar_ar_hwnd[i],dc);
				}	

				if (bInitMiniWin[i])
				{
					::GetClientRect(radar_miniwin_hwnd[i],&screen_miniwin_rect[i]);
					dc=::GetDC(radar_miniwin_hwnd[i]);
					p_dm[i]->WindowDisplay_DC(dc,E_MINI,screen_miniwin_rect[i],miniwin_src_rect[i]);
					::ReleaseDC(radar_miniwin_hwnd[i],dc);
				}
			
			}

			//if (bInitTV[i])
			//{
			//	::GetClientRect(tv_hwnd[i],&screen_tv_rect[i]);
			//	dc=::GetDC(tv_hwnd[i]);
			//	p_dm[i]->WindowDisplay_DC(dc,E_TV,screen_tv_rect[i]);
			//	::ReleaseDC(tv_hwnd[i],dc);
			//}

		}

		//LeaveCriticalSection(&cs_callback);


}

void CALLBACK MapTimerProc(HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if(bInit == false) return;
	if(mywidget->widget1->dp->dispState.scopeMode ==1)tshInitSurface(1);
	
	if(mywidget->widget2->dp->dispState.scopeMode ==1)tshInitSurface(2);

}

//------PPI------
GVIDMW_API Status ML_GVID_OpenPPI(char VideoChannel,short theX, short theY , short theWidth, short theHeight)
{
	INIT_CHECK
	VC_CHECK
	XY_CHECK
	VR_CHECK(theWidth,1,1200)
	VR_CHECK(theHeight,1,1200)

	transPos(theX,theY);	

	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	rc=mywidget->widgets[VideoChannel]->ML_GVID_OpenPPI(-1,VideoChannel,0,0,1200,1200);//PPI在后页的作图区域
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();	
	
	ppi_tsh_rect[VideoChannel].start_x = theX;
	ppi_tsh_rect[VideoChannel].start_y =theY;
	ppi_tsh_rect[VideoChannel].width   =theWidth;
	ppi_tsh_rect[VideoChannel].height  =theHeight;
	SetWindowPos(radar_ppi_hwnd[VideoChannel],NULL,theX,theY,theWidth,theHeight,SWP_SHOWWINDOW);
	ShowWindow(radar_ppi_hwnd[VideoChannel], SW_SHOW);

	radarOpenState[VideoChannel]=true;
	bInitPPI[VideoChannel]= true;
	return (rc==0 ? 1 : -1);
}
GVIDMW_API Status ML_GVID_ClosePPI(char VideoChannel)
{
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	rc=mywidget->widgets[VideoChannel]->ML_GVID_ClosePPI(-1,VideoChannel);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	bInitPPI[VideoChannel] = false;
	ShowWindow(radar_ppi_hwnd[VideoChannel], SW_HIDE);
	return (rc==0 ? 1 : -1);
}
GVIDMW_API Status ML_GVID_SetPPIXY(char VideoChannel ,short theX ,short theY)
{	
	bool flag = false;
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	XY_CHECK
	transPos(theX,theY);
	
	ppi_tsh_rect[VideoChannel].start_x = theX;
	ppi_tsh_rect[VideoChannel].start_y = theY;  
	
	flag=SetWindowPos(radar_ppi_hwnd[VideoChannel],NULL,theX,theY,ppi_tsh_rect[VideoChannel].width,ppi_tsh_rect[VideoChannel].height,SWP_SHOWWINDOW);
	return (flag==true ? 1 : -1);
}
GVIDMW_API Status ML_GVID_SetPPIWH(char VideoChannel ,short theWidth, short theHeight)
{
	bool flag = false;
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	VR_CHECK(theWidth,1,1200)
	VR_CHECK(theHeight,1,1200)

	ppi_tsh_rect[VideoChannel].width   =theWidth;
	ppi_tsh_rect[VideoChannel].height  =theHeight;
	//rc=mywidget->ML_GVID_SetPPIWH(-1,VideoChannel,theWidth,theHeight);
	flag=SetWindowPos(radar_ppi_hwnd[VideoChannel],NULL,ppi_tsh_rect[VideoChannel].start_x,ppi_tsh_rect[VideoChannel].start_y,theWidth,theHeight,SWP_SHOWWINDOW);
	return (flag==true ? 1 : -1);
}
GVIDMW_API Status ML_GVID_SetPPIMode(char VideoChannel, short theMode)
{
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	VR_CHECK(theMode,0,2)
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPIMode(-1,VideoChannel,theMode);	
	p_dm[VideoChannel]->EraseSurface(E_PPI_RAW);
	p_dm[VideoChannel]->EraseSurface(E_MINI_RAW);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}
GVIDMW_API Status ML_GVID_SetPPICenter(char VideoChannel ,short theX ,short theY)
{
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	VR_CHECK(theX,0,1199)
	VR_CHECK(theY,0,1199)
	transPos(theX,theY);	
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPICenter(-1,VideoChannel,theX,theY);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}
GVIDMW_API Status ML_GVID_SetPPIShipDir(char VideoChannel,float theShipDir)
{
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	while(theShipDir<0)
			theShipDir+=360.0;
	while(theShipDir>=360)
		theShipDir -= 360.0;
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPIShipDir(-1,VideoChannel,theShipDir);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}
GVIDMW_API Status ML_GVID_SetPPIColor(char VideoChannel,unsigned int theColor)
{		
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPIColor(-1,VideoChannel,theColor);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}
GVIDMW_API Status ML_GVID_SetPPIScan(char VideoChannel,int theScan)
{			
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	VR_CHECK(theScan,0,1)
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPIScan(-1,VideoChannel,theScan);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}
GVIDMW_API Status ML_GVID_SetPPILineColor(char VideoChannel,unsigned int theColor)
{		
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPILineColor(-1,VideoChannel,theColor);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}
GVIDMW_API Status ML_GVID_FrostPPI(char VideoChannel,char theFrost)
{		
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	VR_CHECK(theFrost,0,1)
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPIFrost(-1,VideoChannel,theFrost);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}
GVIDMW_API Status ML_GVID_SetPPIDist(char VideoChannel,short theDist)
{		
		INIT_CHECK
		INIT_PPI_CHECK
		VC_CHECK
		VR_CHECK(theDist,1,32767)
		p_dm[VideoChannel]->LockAllRadarRawSurfaces();
		mywidget->widgets[VideoChannel]->ML_GVID_SetPPIDist(-1,VideoChannel,theDist);
		p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
		return 1;

	
}
GVIDMW_API Status ML_GVID_SetPPISwitch(char VideoChannel,short theSwitch)
{	
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	VR_CHECK(theSwitch,0,1)
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPISwitch(-1,VideoChannel,theSwitch);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}
GVIDMW_API Status ML_GVID_SetPPIGate(char VideoChannel,char theGate)
{		
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	if(theGate>=1 && theGate <=8)
	{
		theGate -= 4;
		theGate *= 25;//分档
		p_dm[VideoChannel]->LockAllRadarRawSurfaces();
		mywidget->widgets[VideoChannel]->ML_GVID_SetPPIGate(-1,VideoChannel,theGate);
		p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
		return 1;
	}
	else
		return -1;
	
}
GVIDMW_API Status ML_GVID_SetPPITail(char VideoChannel,int theTail)
{
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK

	if (theTail < 0 || theTail >65535 ) return -1;

	theTail = (double)theTail/256;

	theTail = 255 - theTail;
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPITail(-1,VideoChannel,theTail);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}

//------AR------
GVIDMW_API Status ML_GVID_OpenAr(char VideoChannel,short theX, short theY , short theWidth, short theHeight)
{
	//int screen_num = 1;
	INIT_CHECK
	INIT_PPI_CHECK
	VC_CHECK
	XY_CHECK
	VR_CHECK(theWidth,1,512)
	VR_CHECK(theHeight,1,512)
	//if(theY > 1199) screen_num = 2;
	transPos(theX,theY);
	//if (screen_num == 2) theX +=1600;
	ar_tsh_rect[VideoChannel].start_x = theX;
	ar_tsh_rect[VideoChannel].start_y = theY;
	ar_tsh_rect[VideoChannel].width   = theWidth;
	ar_tsh_rect[VideoChannel].height  = theHeight;

	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	rc=mywidget->widgets[VideoChannel]->ML_GVID_OpenAr(VideoChannel,VideoChannel,1600-340,400,340,340);//AR在后页作图区域
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	SetWindowPos(radar_ar_hwnd[VideoChannel],NULL,theX,theY,theWidth,theHeight,SWP_SHOWWINDOW);
	ShowWindow(radar_ar_hwnd[VideoChannel], SW_SHOW);
	bInitAR[VideoChannel]  = true;
	return (rc==0 ? 1 : -1);
}
GVIDMW_API Status ML_GVID_CloseAr(char VideoChannel)
{		
	INIT_CHECK
	//INIT_PPI_CHECK
	INIT_AR_CHECK
	VC_CHECK
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	rc=mywidget->widgets[VideoChannel]->ML_GVID_CloseAr(-1,VideoChannel);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	bInitAR[VideoChannel]  = false;
	ShowWindow(radar_ar_hwnd[VideoChannel], SW_HIDE);
	return (rc==0 ? 1 : -1);
}
GVIDMW_API Status ML_GVID_SetArXY(char VideoChannel ,short theX ,short theY)
{	

	bool flag = false;
	INIT_CHECK
	//INIT_PPI_CHECK
	INIT_AR_CHECK
	VC_CHECK
	XY_CHECK
	transPos(theX,theY);
	
	ar_tsh_rect[VideoChannel].start_x = theX;
	ar_tsh_rect[VideoChannel].start_y = theY;
	flag = SetWindowPos(radar_ar_hwnd[VideoChannel],NULL,theX,theY,ar_tsh_rect[VideoChannel].width,ar_tsh_rect[VideoChannel].height,SWP_SHOWWINDOW);
	return (flag==true ? 1 : -1);
}
GVIDMW_API Status ML_GVID_SetArWH(char VideoChannel ,short theWidth, short theHeight)
{	
	bool flag = false;
	INIT_CHECK
	//INIT_PPI_CHECK
	INIT_AR_CHECK
	VC_CHECK
	VR_CHECK(theWidth,1,512)
	VR_CHECK(theHeight,1,512)
	ar_tsh_rect[VideoChannel].width   = theWidth;
	ar_tsh_rect[VideoChannel].height  = theHeight;
	
	flag = SetWindowPos(radar_ar_hwnd[VideoChannel],NULL,ar_tsh_rect[VideoChannel].start_x,ar_tsh_rect[VideoChannel].start_y,theWidth,theHeight,SWP_SHOWWINDOW);
	return (flag==true ? 1 : -1);
}
GVIDMW_API Status ML_GVID_SetArGate (char VideoChannel ,char theGate)
{		
		INIT_CHECK
		INIT_PPI_CHECK
		INIT_AR_CHECK
		VC_CHECK
		VR_CHECK(theGate,0,7)
		theGate -= 4;
		theGate *= 25;
		p_dm[VideoChannel]->LockAllRadarRawSurfaces();
		mywidget->widgets[VideoChannel]->ML_GVID_ChangArGate(-1,VideoChannel,theGate);
		p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
		return 1;
}
GVIDMW_API Status ML_GVID_SetArDist (char VideoChannel ,short theDist)
{		
	INIT_CHECK
	INIT_PPI_CHECK
	INIT_AR_CHECK
	VC_CHECK
	VR_CHECK(theDist,1,32767)
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_ChangArDist(-1,VideoChannel,theDist);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;

}

//------MiniWin------
GVIDMW_API Status ML_GVID_OpenMiniWin(char VideoChannel,short theX, short theY , short theWidth, short theHeight)
{
		//int screen_num = 1;
		bool flag = false;
		INIT_CHECK
		INIT_PPI_CHECK
		VC_CHECK //这里VideoChannel已经被减一了
		XY_CHECK
		VR_CHECK(theWidth,1,1200)
		VR_CHECK(theHeight,1,1200)
		//if(theY > 1199)screen_num = 2;
		transPos(theX,theY);
		//if (screen_num == 2) theX +=1600;
		//bInitMiniWin = true;
		//ML_GVID_SetMiniWinXY(VideoChannel+1,theX,theY);
		//ML_GVID_SetMiniWinWH(VideoChannel+1,theWidth,theHeight);
		miniwin_tsh_rect[VideoChannel].start_x = theX;
		miniwin_tsh_rect[VideoChannel].start_y = theY;
		miniwin_tsh_rect[VideoChannel].width   = theWidth;
		miniwin_tsh_rect[VideoChannel].height  = theHeight;
		//mywidget->widgets[VideoChannel]->ML_GVID_SetWinXY(VideoChannel,VideoChannel,1600-340,0);
		//mywidget->widgets[VideoChannel]->ML_GVID_SetWinWH(VideoChannel,VideoChannel,340,340);

		p_dm[VideoChannel]->LockAllRadarRawSurfaces();
		mywidget->widgets[VideoChannel]->ML_GVID_SetWinDisp(VideoChannel,VideoChannel,0);
		//mywidget->widgets[VideoChannel]->map->updateWindow();
		p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();

		flag=SetWindowPos(radar_miniwin_hwnd[VideoChannel],NULL,theX,theY,theWidth,theHeight,SWP_SHOWWINDOW);
		ShowWindow(radar_miniwin_hwnd[VideoChannel], SW_SHOW);
		bInitMiniWin[VideoChannel]  = true;
		return (flag==true ? 1 : -1);
	//wait for code
}
GVIDMW_API Status ML_GVID_CloseMiniWin(char VideoChannel)
{
	INIT_CHECK
	//INIT_PPI_CHECK
	INIT_MiniWin_CHECK
	VC_CHECK
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetWinDisp(-1,VideoChannel,1);
	mywidget->widgets[VideoChannel]->map->clearWindow();
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	ShowWindow(radar_miniwin_hwnd[VideoChannel], SW_HIDE);
	bInitMiniWin[VideoChannel]  = false;
	return 1;

}
GVIDMW_API Status ML_GVID_SetMiniWinXY(char VideoChannel ,short theX ,short theY)
{

	bool flag = false;
	INIT_CHECK
	//INIT_PPI_CHECK
	INIT_MiniWin_CHECK
	VC_CHECK
	XY_CHECK
	transPos(theX,theY);
	
	miniwin_tsh_rect[VideoChannel].start_x = theX;
	miniwin_tsh_rect[VideoChannel].start_y = theY;
	flag=SetWindowPos(radar_miniwin_hwnd[VideoChannel],NULL,theX,theY,miniwin_tsh_rect[VideoChannel].width,miniwin_tsh_rect[VideoChannel].height,SWP_SHOWWINDOW);
	return (flag==true ? 1 : -1);
}
GVIDMW_API Status ML_GVID_SetMiniWinWH(char VideoChannel ,short theWidth, short theHeight)
{	
	bool flag = false;
	INIT_CHECK
	//INIT_PPI_CHECK
	INIT_MiniWin_CHECK
	VC_CHECK
	VR_CHECK(theWidth,1,1200)
	VR_CHECK(theHeight,1,1200)
	miniwin_tsh_rect[VideoChannel].width   = theWidth;
	miniwin_tsh_rect[VideoChannel].height  = theHeight;
	//rc=mywidget->widgets[VideoChannel]->ML_GVID_SetWinWH(-1,VideoChannel,theWidth,theHeight);
	//return (rc==0 ? 1 : -1);
	flag=SetWindowPos(radar_miniwin_hwnd[VideoChannel],NULL,miniwin_tsh_rect[VideoChannel].start_x,miniwin_tsh_rect[VideoChannel].start_y,theWidth,theHeight,SWP_SHOWWINDOW);
	return (flag==true ? 1 : -1);
}
GVIDMW_API Status ML_GVID_SetMiniWinMode(char VideoChannel, short theMode)
{		
	INIT_CHECK
	INIT_PPI_CHECK
	INIT_MiniWin_CHECK
	VC_CHECK
	VR_CHECK(theMode,0,1)
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPIMode(-1,VideoChannel,theMode);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}
GVIDMW_API Status ML_GVID_SetMiniWinColor(char VideoChannel,unsigned int theColor)
{		
	INIT_CHECK
	INIT_PPI_CHECK
	INIT_MiniWin_CHECK
	VC_CHECK
	//wait for code
	//mywidget->widgets[VideoChannel]->ML_GVID_SetPPIColor(-1,VideoChannel,theColor);
	switch(VideoChannel)
	{
	case 0:
		mywidget->widget1->dp->setWinColor((theColor>>24) & 0x0ff,(theColor>>16) & 0x0ff,(theColor>>8) & 0x0ff);
		break;
	case 1:
		mywidget->widget2->dp->setWinColor((theColor>>24) & 0x0ff,(theColor>>16) & 0x0ff,(theColor>>8) & 0x0ff);
		break;
	}
	return 1;
}
GVIDMW_API Status ML_GVID_SetMiniWinDistDir(char VideoChannel ,short theDistance ,float theDirection)
{		
	INIT_CHECK
	INIT_PPI_CHECK
	INIT_MiniWin_CHECK
	VC_CHECK
	VR_CHECK(theDistance,0,300)
	while(theDirection<0)
	theDirection += 360.0;
	while(theDirection>=360)
	theDirection -= 360.0;
	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->ML_GVID_SetPPIWinDistDir(-1,VideoChannel,theDistance,theDirection);
	mywidget->widgets[VideoChannel]->map->updateWindow(
		(miniwin_src_rect[VideoChannel].right-miniwin_src_rect[VideoChannel].left+1)/2
		);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();
	return 1;
}
GVIDMW_API Status ML_GVID_SetMiniWinScale(char VideoChannel,int theScale)
{	
	bool flag = false;
	INIT_CHECK
	INIT_PPI_CHECK
	INIT_MiniWin_CHECK
	VR_CHECK(theScale,1,8)
	VC_CHECK
	//wait for code
	//int x = miniwin_tsh_rect[VideoChannel].start_x;
	//int y = miniwin_tsh_rect[VideoChannel].start_y;
	//int temp = miniwin_tsh_rect[VideoChannel].width*theScale;
	//int width = temp>1200?1200:temp;
	//temp = miniwin_tsh_rect[VideoChannel].height*theScale;
	//int height = temp>1200?1200:temp;
	////mywidget->ML_GVID_SetPPIWinScale(-1, VideoChannel, theScale);

	//flag=SetWindowPos(radar_miniwin_hwnd[VideoChannel],NULL,x,y,width,height,SWP_SHOWWINDOW);

	miniwin_src_rect[VideoChannel]=getScrRect(theScale);

	p_dm[VideoChannel]->LockAllRadarRawSurfaces();
	mywidget->widgets[VideoChannel]->map->updateWindow( 
		(miniwin_src_rect[VideoChannel].right-miniwin_src_rect[VideoChannel].left+1)/2
		);
	p_dm[VideoChannel]->UnLockAllRadarRawSurfaces();

	return 1;
}

//------TV------
GVIDMW_API Status ML_GVID_OpenTV (char VideoChannel ,short theX, short theY,short theWidth,short theHeight)
{
	//int screen_num = 1;
	bool flag = false;
	INIT_CHECK
	VC_CHECK
	XY_CHECK
	VR_CHECK(theWidth,1,720);
	VR_CHECK(theHeight,1,576);

	transPos(theX,theY);

	TVopenState[VideoChannel] = true;
	tv_tsh_rect[VideoChannel].start_x = theX;
	tv_tsh_rect[VideoChannel].start_y = theY;
	tv_tsh_rect[VideoChannel].width  = theWidth;
	tv_tsh_rect[VideoChannel].height = theHeight;

	//p_dm[VideoChannel]->LockSurface(E_TV_RAW);
	rc=mywidget->player->ML_GVID_OpenTv(-1,VideoChannel,theX,theY,theWidth,theHeight);
	//p_dm[VideoChannel]->UnLockSurface(E_TV_RAW);
	flag=SetWindowPos(tv_hwnd[VideoChannel],NULL,theX,theY,theWidth,theHeight,SWP_SHOWWINDOW);
	ShowWindow(tv_hwnd[VideoChannel], SW_SHOW);

	bInitTV[VideoChannel]  = true;	
	return (flag==true ? 1 : -1);


}
GVIDMW_API Status ML_GVID_CloseTV (char VideoChannel )
{		
	INIT_CHECK
	INIT_TV_CHECK
	VC_CHECK
	//p_dm[VideoChannel]->LockSurface(E_TV_RAW);
	rc=mywidget->player->ML_GVID_CloseTv(-1,VideoChannel);
	//p_dm[VideoChannel]->UnLockSurface(E_TV_RAW);

	bInitTV[VideoChannel]  = false;
	ShowWindow(tv_hwnd[VideoChannel], SW_HIDE);
	return (rc==0 ? 1 : -1);
}
GVIDMW_API Status ML_GVID_SetTVXY(char VideoChannel ,short theX, short theY)
{	

	bool flag = true;
	INIT_CHECK
	INIT_TV_CHECK
	VC_CHECK
	XY_CHECK
	transPos(theX,theY);
	
	tv_tsh_rect[VideoChannel].start_x = theX;
	tv_tsh_rect[VideoChannel].start_y = theY;

	flag=SetWindowPos(tv_hwnd[VideoChannel],NULL,theX,theY,tv_tsh_rect[VideoChannel].width,tv_tsh_rect[VideoChannel].height,SWP_SHOWWINDOW);
	return (flag==true ? 1 : -1);
	
}
GVIDMW_API Status ML_GVID_SetTVWH(char VideoChannel ,short theWidth, short theHeight)
{
	bool flag = true;
	INIT_CHECK
	INIT_TV_CHECK
	VC_CHECK
	VR_CHECK(theWidth,1,720)
	VR_CHECK(theHeight,1,576)
	tv_tsh_rect[VideoChannel].width  = theWidth;
	tv_tsh_rect[VideoChannel].height = theHeight;

	flag=SetWindowPos(tv_hwnd[VideoChannel],NULL,tv_tsh_rect[VideoChannel].start_x ,tv_tsh_rect[VideoChannel].start_y,theWidth,theHeight,SWP_SHOWWINDOW);
	return (flag==true ? 1 : -1);
	//rc=mywidget->players[VideoChannel]->ML_GVID_SetTvWH(-1,VideoChannel,theWidth,theHeight);
	//return (rc==0 ? 1 : -1);
}
GVIDMW_API Status ML_GVID_FrostTV(char VideoChannel,char theFrost)
{		
	INIT_CHECK
	INIT_TV_CHECK
	VC_CHECK

	//p_dm[VideoChannel]->LockSurface(E_TV_RAW);

	if(theFrost==0)
		mywidget->player->ML_GVID_FrostTV(-1,VideoChannel);
	else if(theFrost==1)
		mywidget->player->ML_GVID_UnFrostTV(-1,VideoChannel);
	else
	{
		//p_dm[VideoChannel]->UnLockSurface (E_TV_RAW);
		return -1;
	}

	//p_dm[VideoChannel]->UnLockSurface (E_TV_RAW);

	return 1;
}
GVIDMW_API Status ML_GVID_SetTVBright(char VideoChannel,unsigned char theBright)
{	
	INIT_CHECK
	INIT_TV_CHECK
	VC_CHECK

	//p_dm[VideoChannel]->LockSurface(E_TV_RAW);
	mywidget->player->ML_GVID_ChangTvBright(-1,VideoChannel,(int)theBright);
	//p_dm[VideoChannel]->UnLockSurface(E_TV_RAW);
	return 1;
}
GVIDMW_API Status ML_GVID_SetTVContrast(char VideoChannel,unsigned char theContrast)
{		
	INIT_CHECK
	INIT_TV_CHECK
	VC_CHECK
	//p_dm[VideoChannel]->LockSurface(E_TV_RAW);
	mywidget->player->ML_GVID_ChangTvContrast(-1,VideoChannel,(int)theContrast);
	//p_dm[VideoChannel]->UnLockSurface(E_TV_RAW);
	return 1;
}
GVIDMW_API Status ML_GVID_SetTVColor(char VideoChannel,unsigned char theColor)
{		
	INIT_CHECK
	INIT_TV_CHECK
	VC_CHECK
	//p_dm[VideoChannel]->LockSurface(E_TV_RAW);
	mywidget->player->ML_GVID_ChangTvColor(-1,VideoChannel,(int)theColor);
	//p_dm[VideoChannel]->UnLockSurface(E_TV_RAW);
	return 1;
}


//============= 系统函数及特殊接口 ==============

// channel 均为 0,1

 void tshTestYh(int channel)
{
	if(channel!=0 && channel!=1)return;
	
	p_dm[channel]->LockAllRadarRawSurfaces();
	mywidget->yh(channel);
	p_dm[channel]->UnLockAllRadarRawSurfaces();
	
}

 void tshTestDispUpdate(int channel,LPARAM lParam)
{
	if(channel!=0 && channel!=1)return;
	
	p_dm[channel]->LockAllRadarRawSurfaces();
	mywidget->dispUpdate(channel,(s_ppi_video_for_disp*)lParam);
	p_dm[channel]->UnLockAllRadarRawSurfaces();
	
}

// for secret use
 int __thu_makeSNFile()
{
	return -1;//SNFileMaker(__SN_ST_POS,__SN_JMP_NUM,__SN_CODE_LEN,snfilename);
}

//----------------------------------------------------------------

 LRESULT CALLBACK
	 MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
 {


	 return DefWindowProc(hWnd, msg, wParam, lParam);
 }
//////////////////////////////////////////////////////////////////////////
#define _TREADBUFNUM 3600*7
bool initPpiThread(void)
{
	bool isOK;

	if( (rpkgfp=fopen("./save/radar1","rb"))==NULL ){
		printf("error open pkg file\n");
		isOK=false;
	}
	else
		isOK=true;


	if( (hpkgfp=fopen("./save/radar2","rb"))==NULL ){
		printf("error open pkg file\n");
		isOK=false;
	}
	else
		isOK=true;

	if(isOK==false)return false;


	dispbuf1 = new s_ppi_video_for_disp[_TREADBUFNUM];

	if(dispbuf1==NULL)isOK=false;

	fread(dispbuf1,sizeof(s_ppi_video_for_disp),_TREADBUFNUM,rpkgfp);

	dispbuf2 = new s_ppi_video_for_disp[_TREADBUFNUM];

	if(dispbuf2==NULL)isOK=false;

	fread(dispbuf2,sizeof(s_ppi_video_for_disp),_TREADBUFNUM,hpkgfp);



	//	netCom.CreateServer();

	return isOK;

}

void freePpiThread(void)
{
	if(rpkgfp)fclose(rpkgfp);
	if(hpkgfp)fclose(hpkgfp);

	//if(dispbuf1)delete dispbuf1;
	//if(dispbuf2)delete dispbuf2;

}

#define NEWPKGNUM 20

void pkgTimerHandler(void)
{
	static int n1=0;
	static int n2=0;

	if(isOK)
	{
		if(radarOpenState[0])
		{

			for(int i=0;i<NEWPKGNUM;i++)tshTestDispUpdate(0,(LPARAM)(dispbuf1+n1+i));
			n1 += NEWPKGNUM;
			if(n1+NEWPKGNUM>_TREADBUFNUM)
			{
				n1=0;
				rewind(rpkgfp);
				fread(dispbuf1,sizeof(s_ppi_video_for_disp),_TREADBUFNUM,rpkgfp);
			}


		}

		if(radarOpenState[1] && screenMode!=2)
		{

			for(int i=0;i<NEWPKGNUM;i++)tshTestDispUpdate(1,(LPARAM)(dispbuf2+n2+i));
			n2 += NEWPKGNUM;
			if(n2+NEWPKGNUM>_TREADBUFNUM)
			{
				n2=0;
				rewind(hpkgfp);
				fread(dispbuf2,sizeof(s_ppi_video_for_disp),_TREADBUFNUM,hpkgfp);
			}

		}
	}

}

#define YHNUM 2

void yhTimerHandler(void)
{
	for(int i=0;i<YHNUM;i++){
		tshTestYh(0);
		tshTestYh(1);
		if(isOK && radarOpenState[0])tshTestYh(0);
		if(isOK && radarOpenState[1])tshTestYh(1);
	}
}

//---- create window ----

HWND tshCreateWindow(char * name,char VideoChannel,short theX, short theY , short theWidth, short theHeight)
{



	HWND                        hWnd;
	char s[40];

	sprintf(s,"%s%d",name,(int)VideoChannel);


	hWnd = CreateWindow(NAME,
		s,
		WS_OVERLAPPEDWINDOW ,  // WS_OVERLAPPED|WS_SIZEBOX|WS_OVERLAPPEDWINDOW |WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU
		theX,
		theY,
		theWidth,
		theHeight,
		NULL,
		NULL,
		NULL,
		NULL);
	/*HMENU sys_menu = ::GetSystemMenu(hWnd,false);
	::RemoveMenu(sys_menu,4,MF_BYPOSITION);*/

	return hWnd;

}

void tshInitRect()
{
	radar_ppi_rect.left  = 0;
	radar_ppi_rect.right = SURF_CM_WIDTH-1;
	radar_ppi_rect.top   = 0;
	radar_ppi_rect.bottom =SURF_CM_HEIGHT-1;

	radar_miniwin_rect.left  =0;
	radar_miniwin_rect.right =SURF_MINI_WIDTH-1;
	radar_miniwin_rect.top = 0;
	radar_miniwin_rect.bottom =SURF_MINI_HEIGHT-1;

	radar_ar_rect.left =0;
	radar_ar_rect.right =SURF_AR_WIDTH-1;
	radar_ar_rect.top = 0;
	radar_ar_rect.bottom=SURF_AR_HEIGHT-1;

	tv_rect.left =0;
	tv_rect.right=SURF_TV_WIDTH-1;
	tv_rect.top=0;
	tv_rect.bottom=SURF_TV_HEIGHT-1;

	miniwin_src_rect[0]=getScrRect(1.0);
	miniwin_src_rect[1]=getScrRect(1.0);
}

/*
背景图中的开窗大小为：340 X 340

                                                              left     top     right      bottom
这样如果是 2倍放大，翻屏的源位置就为：0        0      340-1     340-1
*/
RECT getScrRect(float b)
{// b:倍数，1~8
	RECT r;
	int t;

	b = b/2.0;
	if(b<1.0)b=1.0;
	else if(b>4.0)b=4.0;

	t=(SURF_MINI_WIDTH-SURF_MINI_WIDTH/b)/2;
	r.left=0+t;
	r.top=0+t;

	t=SURF_MINI_WIDTH/b;//区域边长
	r.right=r.left+t-1;
	r.bottom=r.top+t-1;

	return r;
}


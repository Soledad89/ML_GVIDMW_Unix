


#include "stdafx.h"
#include "debug.h"
#include "C_THU_ML_GVID.h"
#include "ML_GVIDMW_sys.h"

C_THU_ML_GVID::C_THU_ML_GVID( const s_in_param* p,CDrawingManager* dm1,CDrawingManager* dm2)
{
	widget1 = NULL;
	widget2 = NULL;
	player = NULL;
	//netclient = NULL;
	netradar = NULL;

	memcpy(initParam,p,sizeof(s_init_param)*2);
	_dm1=dm1;
	_dm2=dm2;
	//---------------------------------------------------
	state1.hasPPI=true;
	state1.hasWIN=true;
	state1.hasAR=false;
	state1.isFrost=false;
	state1.isOpen=true;

	state1.PPI_XSTART=0;
	state1.PPI_YSTART=0;
	state1.PPI_WIDTH=SURF_CM_WIDTH;
	state1.PPI_HEIGHT=SURF_CM_HEIGHT;

	state1.WIN_XSTART=0;
	state1.WIN_YSTART=0;
	state1.WIN_WIDTH=SURF_MINI_WIDTH;
	state1.WIN_HEIGHT=SURF_MINI_HEIGHT;

	state1.AR_XSTART=0;
	state1.AR_YSTART=0;
	state1.AR_WIDTH=SURF_AR_WIDTH;
	state1.AR_HEIGHT=SURF_AR_HEIGHT;
	//--------------------------------------------
	state2.hasPPI=true;
	state2.hasWIN=true;
	state2.hasAR=false;
	state2.isFrost=false;
	state2.isOpen=true;

	state2.PPI_XSTART=0;
	state2.PPI_YSTART=0;
	state2.PPI_WIDTH=SURF_CM_WIDTH;
	state2.PPI_HEIGHT=SURF_CM_HEIGHT;

	state2.WIN_XSTART=0;
	state2.WIN_YSTART=0;
	state2.WIN_WIDTH=SURF_MINI_WIDTH;
	state2.WIN_HEIGHT=SURF_MINI_HEIGHT;

	state2.AR_XSTART=0;
	state2.AR_YSTART=0;
	state2.AR_WIDTH=SURF_AR_WIDTH;
	state2.AR_HEIGHT=SURF_AR_HEIGHT;

	void * lplinear[3];
	int pitch[3];
	int bc;
	int ret;

	fprintf(debugfp,"-----my widget 1 is creating, ID=%d-----\n",initParam[0].ID);
	if(initParam[0].ID>=0)
	{
		ret=_dm1->getSurfaceInfoByLock(E_PPI_RAW,&lplinear[0],&pitch[0],&bc);
		if(ret!=DM_OK)fprintf(debugfp,"---- err get PPI surface info,  errno=%d\n",ret);

		ret=_dm1->getSurfaceInfoByLock(E_AR_RAW,&lplinear[1],&pitch[1],&bc);
		if(ret!=DM_OK)fprintf(debugfp,"---- err get AR surface info,  errno=%d\n",ret);

		ret=_dm1->getSurfaceInfoByLock(E_MINI_RAW,&lplinear[2],&pitch[2],&bc);
		if(ret!=DM_OK)fprintf(debugfp,"---- err get MINI surface info,  errno=%d\n",ret);

		widget1 = new MyWidget(lplinear[0],pitch[0],lplinear[1],pitch[1],lplinear[2],pitch[2]);
		MEMALLOCCHECK(widget1);
		//_dm1->UnLockSurface(E_PPI_RAW);
		//_dm1->UnLockSurface(E_AR_RAW);
		//_dm1->UnLockSurface(E_MINI_RAW);
	}
	fprintf(debugfp,"-----my widget 1 created, ID=%d-----\n",initParam[0].ID);

	fprintf(debugfp,"-----my widget 2 is creating, ID=%d-----\n",initParam[0].ID);
	if(initParam[1].ID>=0)
	{
		_dm2->getSurfaceInfoByLock(E_PPI_RAW,&lplinear[0],&pitch[0],&bc);
		_dm2->getSurfaceInfoByLock(E_AR_RAW,&lplinear[1],&pitch[1],&bc);
		_dm2->getSurfaceInfoByLock(E_MINI_RAW,&lplinear[2],&pitch[2],&bc);
		widget2 = new MyWidget(lplinear[0],pitch[0],lplinear[1],pitch[1],lplinear[2],pitch[2]);
		MEMALLOCCHECK(widget2);
		//_dm2->UnLockSurface(E_PPI_RAW);
		//_dm2->UnLockSurface(E_AR_RAW);
		//_dm2->UnLockSurface(E_MINI_RAW);
	}
	fprintf(debugfp,"-----my widget 2 created, ID=%d-----\n",initParam[2].ID);

	//---------------new -7.3-----
	widget1->ppi->bFirstInfo = true;
	widget2->ppi->bFirstInfo = true;
	//------------------end-----

	widgets[0]=widget1;
	widgets[1]=widget2;

	mx=0;
	my=0;
	proState=0;


	if(initParam[0].tvChannel.ID>=0 || initParam[1].tvChannel.ID>=0)
	{
		// Create TV
		player = new TVPlayer();
		if(initParam[0].tvChannel.ID>=0)player->TV_Init(
			0,
			initParam[0].tvChannel.hTVWnd,
			initParam[0].tvChannel.netConfig.address,
			initParam[0].tvChannel.netConfig.user,
			initParam[0].tvChannel.netConfig.pwd
			);
		if(initParam[1].tvChannel.ID>=0)player->TV_Init(
			1,
			initParam[1].tvChannel.hTVWnd,
			initParam[1].tvChannel.netConfig.address,
			initParam[1].tvChannel.netConfig.user,
			initParam[1].tvChannel.netConfig.pwd
			);
	
	}


	//fprintf(debugfp,"init param : 0x%x\n",initParam.mode);

	if(initParam[0].radarChannel.source==E_NET || initParam[1].radarChannel.source==E_NET)
	{
		netradar = new CNetRadar(this);

		if( !netradar->init() )fprintf(debugfp,"radar network init error!\n");
		else fprintf(debugfp,"radar network inited!\n");
	}

}

C_THU_ML_GVID::~C_THU_ML_GVID(void)
{

	if(widget1)delete widget1;

	if( widget2 )delete widget2;

	//delete player1;
	if(player)delete player;
	//if(netclient)delete netclient;
	if(netradar)delete netradar;

}

void C_THU_ML_GVID::close()//处理关闭前除delete外的动作，由外层应用程序调用
{
	//if(player2)player2->ML_GVID_CloseTv(1,2);
}

void C_THU_ML_GVID::sendPkg(int channel, unsigned char * datbuf)
{
	switch(channel)
	{
	case 0:
		widget1->ppiUpdate(datbuf);
		break;
	case 1:
		widget2->ppiUpdate(datbuf);
		break;

	}

}

void C_THU_ML_GVID::sendDispPkg(int channel,int n,s_ppi_video_for_disp * buf)
{
	switch(channel)
	{
	case 0:
		widget1->dp->sendDispPkg(n,buf);
		//widget1->ar ->ARDraw();
		widget1->ppi->ppiDraw();
		break;
	case 1:
		widget2->dp->sendDispPkg(n,buf);
		//widget2->ar ->ARDraw();
		widget2->ppi->ppiDraw();
		break;

	}

}
void C_THU_ML_GVID::yh(int channel)
{
	switch(channel)
	{
	case 0:
		if(widget1->ppi->bFirstInfo == true||widget1->ppi->benCloseFlag == true)
		widget1->yh->yh();
		break;
	case 1:
		if(widget2->ppi->bFirstInfo == true||widget2->ppi->benCloseFlag == true)
		widget2->yh->yh();
		break;

	}
}

void C_THU_ML_GVID::dispUpdate(int channel,s_ppi_video_for_disp *dispdat)
{
	int a = 0;
	int j = 0;

	MyWidget *wd = widgets[channel];

	if(wd->ppi->bFirstInfo == true)
	{
		wd->ppi->ppiDraw(dispdat);
		wd->ar ->ARDraw(dispdat);

		if(wd->ppi->isScanLine() && wd->ppi->scopeMode == 1)
		{
			for(j=0;j< dispdat->znum;j++)
			{
				a=(dispdat->azicnt[j])%4096;
				wd->ppi->ppiClearScanLine(a);
				wd->ppi->ppiDrawScanLine(a+1);
			}
		}
	}

}

void C_THU_ML_GVID::trackUpdate(int channel, s_arpa_trace info)
{
	switch(channel)
	{
	case 0:
		//widget1->track->trackUpdate(info);
		break;
	case 1:
		//widget2->track->trackUpdate(info);
		break;

	}
}
void C_THU_ML_GVID::initSurface(int channel)
{
	switch(channel)
	{
	case 0:
		widget1->initSurface();
		break;
	case 1:
		widget2->initSurface();
		break;

	}
}

void C_THU_ML_GVID::MouseEventHandler(unsigned int type,int x,int y)
{
	//static firstDraw = true;

	if(x>=1600)
	{
		widget2->MouseEventHandler(type,x%1600,y);
	}
	else
	{
		widget1->MouseEventHandler(type,x,y);
	}


}

void C_THU_ML_GVID::PlayTV(short VideoChannel,HWND hwnd)
{

	//player->PlayTV(VideoChannel,hwnd);

}

void C_THU_ML_GVID::clearScreen(short  theDisplay)
{
	switch(theDisplay)
	{
	case 0:
		widget1->dp->clear();
		break;
	case 1:
		widget2->dp->clear();
		break;
	}
}


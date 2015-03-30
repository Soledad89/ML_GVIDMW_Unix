
//  Version: 1.0

#include "stdafx.h"

#include "netRadar.h"
#include "debug.h"


CNetRadar::CNetRadar(C_THU_ML_GVID *mp_widget)
{
	p_widget = mp_widget;
	p_dm1=p_widget->_dm1;
	p_dm2=p_widget->_dm2;

	sem1 = NULL;
	sem2 = NULL;
	thread1 = NULL;
	thread2 = NULL;
}

CNetRadar::~CNetRadar()
{

}

bool CNetRadar::init()
{
	for(int i=0;i<DISP_BUF_NUM;i++)
	{
		deqDP1.push_back(&dispPkg1[i]);
		deqDP2.push_back(&dispPkg2[i]);
	}

	sem1 = CreateSemaphore(NULL,0,DISP_BUF_NUM*2,NULL);
	sem2 = CreateSemaphore(NULL,0,DISP_BUF_NUM*2,NULL);
	InitializeCriticalSection(&cs1);
	InitializeCriticalSection(&cs2);


	// create thread
	DWORD threadID;
	thread1 = CreateThread(NULL,0,RadarThread_1,this,0,&threadID);
	if(thread1==NULL)return false;

	fprintf(debugfp,"radar thread 1 created!\n");

	thread2 = CreateThread(NULL,0,RadarThread_2,this,0,&threadID);
	if(thread2==NULL)return false;

	fprintf(debugfp,"radar thread 2 created!\n");


	CreateServer();
	//CreateServer(port);

	return true;
}

/************************************************************************/
/* OnReceive当期望的数据体(编号为ID，长度为len)到达后的发生，pData为发来的数据的起始地址(不包含报文头)，可以拷贝直接使用
/*此函数为虚函数，由使用方重写                                          */
/************************************************************************/
void CNetRadar::OnReceive(unsigned char id, short len,char* pData,int pos)
{
	static int lastAzi1=0;
	static int lastAzi2=0;

	int lastAzi;
	int curAzi;
	int i;

	static bool post1=false;
	static bool post2=false;

	//fprintf(debugfp,"recvpkg,id=%d\n",id);

	if(id==10)
	{
		EnterCriticalSection(&cs1);
		if(deqDP1.size()>0)
		{
			dispbuf=*deqDP1.begin();
			deqDP1.pop_front();
			LeaveCriticalSection(&cs1);
		}
		else
		{
			LeaveCriticalSection(&cs1);
			//fprintf(debugfp,"buff 1 is full!\n");
			return;
		}
		lastAzi = lastAzi1;
	}
	else if(id==11)
	{
		EnterCriticalSection(&cs2);
		if(deqDP2.size()>0)
		{
			dispbuf=*deqDP2.begin();
			deqDP2.pop_front();
			LeaveCriticalSection(&cs2);
		}
		else
		{
			LeaveCriticalSection(&cs2);
			return;
		}
		lastAzi = lastAzi2;
	}
	else
		return;


	if(pos==0){  //大报文(XSMXP)起始
		post1=false;
		post2=false;
	}

	curAzi = *(unsigned short*)(pData+2);

	//fprintf(debugfp,"curAzi=%d\n",curAzi);

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

	memcpy(dispbuf->videodata,pData+4,len-4);//pData中前四个字节为其他信息
//TRACEN;
	if(id==10){//channel 1
		lastAzi1=lastAzi;
		EnterCriticalSection(&cs1);
		deqDispP1.push_back(dispbuf);
		LeaveCriticalSection(&cs1);
		post1=true;
		//ReleaseSemaphore(sem1,1,NULL);
	}
	else if(id==11){//channel 2
		lastAzi2=lastAzi;
		EnterCriticalSection(&cs2);
		deqDispP2.push_back(dispbuf);
		LeaveCriticalSection(&cs2);
		post2=true;
		//ReleaseSemaphore(sem2,1,NULL);
	}

	//fprintf(debugfp,"recv%d\n",pos);

	if(pos<0){//大报文(XSMXP)结束,绘制该报文传来的视频
		if(post1)
			ReleaseSemaphore(sem1,1,NULL);
		if(post2)
			ReleaseSemaphore(sem2,1,NULL);
	}

}



DWORD WINAPI CNetRadar::RadarThread_1(void* pParam)
{
	CNetRadar * pointer = (CNetRadar *) pParam;

	if (pointer)
	{
		pointer->RadarLoop_1();
		return 1;
	}
	return 0;
}

DWORD WINAPI CNetRadar::RadarThread_2(void* pParam)
{
	CNetRadar * pointer = (CNetRadar *) pParam;

	if (pointer)
	{
		pointer->RadarLoop_2();
		return 1;
	}
	return 0;
}

void CNetRadar::RadarLoop_1()// update
{
	s_ppi_video_for_disp * p;
	int num;
	static int a=0;
	bool drawScan=false;

	fprintf(debugfp,"running radar loop 1\n");

	while(true)
	{
		WaitForSingleObject(sem1,INFINITE);

		EnterCriticalSection(&cs1);
		num = deqDispP1.size();
		LeaveCriticalSection(&cs1);


		p_dm1->LockSurface(E_PPI_RAW);
		p_dm1->LockSurface(E_MINI_RAW);
		p_dm1->LockSurface(E_AR_RAW);

		//if(num>0){
		//	p_widget->widget1->ppi->ppiClearScanLine(a);
		//	drawScan = true;
		//}

		while(num-->0)
		{
			EnterCriticalSection(&cs1);
			p=*deqDispP1.begin();
			LeaveCriticalSection(&cs1);

			p_widget->dispUpdate(0,p);

			EnterCriticalSection(&cs1);
			deqDP1.push_back(p);
			deqDispP1.pop_front();
			LeaveCriticalSection(&cs1);
		}
		
		//if(drawScan){
		//	a=p->azicnt[p->znum-1]+1;
		//	p_widget->widget1->ppi->ppiDrawScanLine(a);
		//}


		p_dm1->UnLockSurface(E_AR_RAW);
		p_dm1->UnLockSurface(E_MINI_RAW);
		p_dm1->UnLockSurface(E_PPI_RAW);

	}
}
void CNetRadar::RadarLoop_2()
{
	s_ppi_video_for_disp * p;
	int num;
	static int a=0;
	bool drawScan=false;

	fprintf(debugfp,"running radar loop 2\n");

	while(true)
	{
		WaitForSingleObject(sem2,INFINITE);

		EnterCriticalSection(&cs2);
		num = deqDispP2.size();
		LeaveCriticalSection(&cs2);


		p_dm2->LockSurface(E_PPI_RAW);
		p_dm2->LockSurface(E_MINI_RAW);
		p_dm2->LockSurface(E_AR_RAW);

		//if(num>0){
		//	p_widget->widget2->ppi->ppiClearScanLine(a);
		//	drawScan = true;
		//}

		while(num-->0)
		{
			EnterCriticalSection(&cs2);
			p=*deqDispP2.begin();
			LeaveCriticalSection(&cs2);

			p_widget->dispUpdate(1,p);

			EnterCriticalSection(&cs2);
			deqDP2.push_back(p);
			deqDispP2.pop_front();
			LeaveCriticalSection(&cs2);
		}

		//if(drawScan){
		//	a=p->azicnt[p->znum-1]+1;
		//	p_widget->widget2->ppi->ppiDrawScanLine(a);
		//}


		p_dm2->UnLockSurface(E_AR_RAW);
		p_dm2->UnLockSurface(E_MINI_RAW);
		p_dm2->UnLockSurface(E_PPI_RAW);

	}
}
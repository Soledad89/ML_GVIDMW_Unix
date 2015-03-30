#include "stdafx.h"
#include "AR.h"

#include "debug.h"

#define DRAW_HANDLE if(!markEnable)return;

CAR::CAR(CDataPool * m_dp)
{
	dp = m_dp;
	updateRegion();

	markEnable = true;

	colortable = dp->colortable;
	fbp = dp->fbpAR;
	layer = dp->layerAR;

	distance = AR_WIDTH*2;
	range = 600;
	gate = 0; //-100;
	videoCount=0;
	bAR=false;
	bSurface=false;
	bRegionLine=false;
	hasLine1=false;
	hasLine2=false;

	ARcolor=MFB_YELLOW;
	foreColor=MFB_GREEN;
	meshColor=MFB_GREEN;
	markNum=5;

	video_enlarge = new int[SAMPLE_NUM];
	memset(video_enlarge,0,SAMPLE_NUM*sizeof(int));

	datbuf = new VIDEODATATYPE[SAMPLE_NUM];

	clear();

	layerS2=new CLayerBase(fbp,layer,2,true);
	layerR2=new CLayerBase(fbp,layer,2,false);

	painter2=new CPainter(dp->painterPool,layerS2);
	eraser2=new CPainter(dp->painterPool,layerR2);

	//
	layerS3=new CLayerBase(fbp,layer,3,true);
	layerR3=new CLayerBase(fbp,layer,3,false);

	painter3=new CPainter(dp->painterPool,layerS3);
	eraser3=new CPainter(dp->painterPool,layerR3);

	painter3->setColor(MFB_RED);

}

CAR::~CAR(void)
{
	delete video_enlarge;
	delete datbuf;
}

void CAR::setARGate(int g)
{
	clear();
	gate = g;
	updateSurface();
}

void CAR::setARDistance(int d)
{
	clear();
	distance = d;
	updateSurface();
}

bool CAR::isValid( int &x, int &y )
{
	//return ( (x>=AR_XSTART) && (x<AR_XSTART+AR_WIDTH) && (y>=AR_YSTART) && (y<AR_YSTART2+AR_HEIGHT) );
	return ( (x>=AR_XSTART) && (x<AR_XSTART+AR_WIDTH) && (y>=AR_YSTART) && (y<AR_YSTART+AR_HEIGHT) );
}

void CAR::setFirstPixel(int x, int y, DISPMEMTYPE color)
{

	if(isValid(x,y))
	{
		if(layer[y][x].curlayer<2)
		{
			
			fbp[y][x]=color;
			
		}
	
	}
}

void CAR::ARDraw(s_ppi_video_for_disp* dispdat)
	{
	VIDEODATATYPE * p;

	//fprintf(debugfp,"bAR=%s\n",(bAR? "true":"false"));
	//fprintf(debugfp,"gate=%d\n",gate);

	if(!bAR)return; //����AR�Ƿ���ʾ

	videoCount += 1;
	if(videoCount<oncevn)return;// ��Ƶ��Ŀ������ֱ�ӷ���
	videoCount %= oncevn;

	if(hasLine1)drawVideo(MFB_CLEAR);//������

	p = dispdat->videodata;
	for(int i=0;i<AR_WIDTH;i++)
	{
		video_data[i]=*p;
		p+=2;// ��ȡ
	}

	drawVideo(ARcolor);//����
	hasLine1 = true;

}

void CAR::ARDraw(unsigned char * dat,int len)
{
	if(!bAR)return; //����AR�Ƿ���ʾ
	if(len>1200)return;

	if(hasLine1)drawVideo(MFB_CLEAR);//������

	for(int i=0; i<AR_WIDTH && i<len;i++)
	{
		video_data[i]=dat[i];
	}

	drawVideo(ARcolor);//����
	hasLine1 = true;

}

void CAR::ARDrawEnlarge(unsigned char * dat,int len)
{
	//int st,ed;
	//int n;
	//int x1;
	//int delta;
	//unsigned char *p;

	////static int step=0;

	//if(!bAR)return; //����AR�Ƿ���ʾ

	//if(hasLine2)drawEnlargeVideo(MFB_CLEAR);//������

	//st=stR*len/range;
	//ed=edR*len/range;
	//n=ed-st+1;
	////step=AR_WIDTH/n;

	////fprintf(debugfp,"stR=%d,edR=%d,range=%d\n",stR,edR,range);
	////fprintf(debugfp,"st=%d,ed=%d,n=%d,step=%d\n",st,ed,n,step);

	//// ��������ֵ��������ʱЧ������
	////for(int i=0;i<n+1 && st+i+1<len && (i+1)*step<SAMPLE_NUM;i++)//ȷ�����鲻Խ��
	////{
	////	delta=dat[st+i+1]-dat[st+i];
	////	x1=i*step;
	////	for(int j=0;j<step;j++)
	////	{
	////		video_enlarge[x1+j]=j*delta/(step+1)+dat[st+i];
	////	}
	////}

	//// ȫ���Ȳ�ֵ
	//float f;
	//p=dat+st;
	//for(int i=0;i<AR_WIDTH;i++)
	//{
	//	f=((float)i)*n/((float)AR_WIDTH);
	//	x1=floorf(f);
	//	f-=x1;
	//	if(x1+1>=len)break;
	//	video_enlarge[i]=p[x1]+(p[x1+1]-p[x1])*f;
	//}

	//drawEnlargeVideo(ARcolor);//����
	//hasLine2 = true;

}

void CAR::drawVideo( DISPMEMTYPE color)
{
	int i,j;
	int n,n1;
	int x,y;

	for(i=0;i<AR_WIDTH;i++)
	{
		if(i==0)
		{
			n = video_data[i]>video_data[i+1]? video_data[i]-video_data[i+1]+1 : 1 ;
		}
		else if(i==AR_WIDTH-1)
		{
			n = video_data[i]>video_data[i-1]? video_data[i]-video_data[i-1]+1 : 1 ;
		}
		else
		{
			n = video_data[i]>video_data[i+1]? video_data[i]-video_data[i+1]+1 : 1 ;
			n1 = video_data[i]>video_data[i-1]? video_data[i]-video_data[i-1]+1 : 1 ;
			n = n>n1? n : n1;
		}

		for(j=0;j<n;j++)
		{
			y = AR_YSTART+AR_HEIGHT-(video_data[i]-j);
			x = i+AR_XSTART;

			setFirstPixel(x,y,color);
		}
	}

}


void CAR::drawEnlargeVideo( DISPMEMTYPE color)
{

	//int i,j;
	//int n,n1;
	//int x,y;

	//for(i=0;i<AR_WIDTH;i++)
	//{
	//	if(i==0)
	//	{
	//		n = video_enlarge[i]>video_enlarge[i+1]? video_enlarge[i]-video_enlarge[i+1]+1 : 1 ;
	//	}
	//	else if(i==AR_WIDTH-1)
	//	{
	//		n = video_enlarge[i]>video_enlarge[i-1]? video_enlarge[i]-video_enlarge[i-1]+1 : 1 ;
	//	}
	//	else
	//	{
	//		n = video_enlarge[i]>video_enlarge[i+1]? video_enlarge[i]-video_enlarge[i+1]+1 : 1 ;
	//		n1 = video_enlarge[i]>video_enlarge[i-1]? video_enlarge[i]-video_enlarge[i-1]+1 : 1 ;
	//		n = n>n1? n : n1;
	//	}

	//	for(j=0;j<n;j++)
	//	{
	//		y = AR_YSTART2+AR_HEIGHT-(video_enlarge[i]-j);
	//		x = i+AR_XSTART;

	//		setFirstPixel(x,y,color);
	//	}
	//}


	////---------------------------------------------------------
	////int x1,x2,y1,y2;

	////for(x1=0;x1<AR_WIDTH-step;x1+=step)
	////{
	////	x2=x1+step;
	////	y1=video_enlarge[x1];
	////	y2=video_enlarge[x2];
	////	drawLineP2P(x1+AR_XSTART,AR_YSTART2+AR_HEIGHT-y1,x2+AR_XSTART,AR_YSTART2+AR_HEIGHT-y2,color);
	////}

}
//���ߣ���Ե�
void CAR::drawLineP2P(int x1,int y1,int x2,int y2,DISPMEMTYPE color)
{
	int i;

	if(x1==x2)
	{
		//Ϊ����
		if(y1<=y2)
		{
			for(i=y1;i<=y2;i++)
				setFirstPixel(x1,i,color);
		}
		else
		{
			for(i=y2;i<=y1;i++)
				setFirstPixel(x1,i,color);
		}

		return;
	}

	//Ϊ����
	if(y1==y2)
	{
		if(x1<=x2)
		{
			for(i=x1;i<=x2;i++)
				setFirstPixel(i,y1,color);
		}
		else
		{
			for(i=x2;i<=x1;i++)
				setFirstPixel(i,y1,color);
		}

		return;
	}

	//Ϊб��
	float m=(y2-y1)*1.0/(x2-x1);
	float fTemp;

	if(abs(m)<=1)
	{
		if(x1<x2)
		{
			fTemp=y1-m;
			for(i=x1;i<=x2;i++)
				setFirstPixel(i,fTemp+=m,color);
		}
		else
		{
			fTemp=y2-m;
			for(i=x2;i<=x1;i++)
				setFirstPixel(i,fTemp+=m,color);
		}
		return;
	}

	if(y1<y2)
	{
		fTemp=x1-1/m;
		for(i=y1;i<=y2;i++)
			setFirstPixel(fTemp+=1/m,i,color);
	}
	else
	{
		fTemp=x2-1/m;
		for(i=y2;i<=y1;i++)
			setFirstPixel(fTemp+=1/m,i,color);
	}
}

void CAR::updateRegion(void)
{

	clear();

	AR_XSTART=dp->AR_XSTART + 10;
	AR_YSTART=dp->AR_YSTART + 10;
	//AR_WIDTH=dp->AR_WIDTH;
	AR_WIDTH = 300;
	//AR_HEIGHT=dp->AR_HEIGHT;
	AR_HEIGHT = 256;

	AR_YSTART2=AR_YSTART+AR_HEIGHT+20;

}


void CAR::clear()
{
	for(int i=AR_XSTART;i<AR_WIDTH+AR_XSTART;i++)
		for(int j=AR_YSTART;j<AR_HEIGHT+AR_YSTART;j++)
			setFirstPixel(i,j,MFB_CLEAR);

	if(bSurface)
	{
		clearSurface(AR_YSTART,0,range);
		//clearSurface(AR_YSTART2,stR,edR);
	}

}

void CAR::updateFbp(void) //���»�ͼ�Դ�����ַ
{
	fbp=dp->fbpAR;
	layerS2->setFbp(fbp);
	layerR2->setFbp(fbp);
}


void CAR::updateSurface()
{
	DRAW_HANDLE

	if(bSurface)clearSurface(AR_YSTART,0,range);
	drawSurface(AR_YSTART,0,range);

	//if(bSurface)clearSurface(AR_YSTART2,stR,edR);
	//drawSurface(AR_YSTART2,stR,edR);

	//updateRegionLine();
}

void CAR::clearSurface(int y,int r1,int r2)
{

	DRAW_HANDLE

	int i;
	int n,num,step;

	//eraser2->setColor(meshColor);
	for(i=32;i<256;i+=32)
	{
		eraser2->drawLine(AR_XSTART,y+i,1024,AR_WIDTH);
	}

	n=AR_WIDTH/markNum;

	for(i=0;i<AR_WIDTH;i+=n)
	{
		eraser2->drawLine(AR_XSTART+i,y,2048,AR_HEIGHT);
	}

	//eraser2->setColor(foreColor);
	eraser2->drawNoFillRect(AR_XSTART,y,AR_XSTART+AR_WIDTH-1,y+AR_HEIGHT-1);

	step=(r2-r1)/markNum;
	num=r1;
	for(i=0;i<=AR_WIDTH;i+=n)
	{
		if(num>10000)break;//��������λ��
		eraser2->drawNum(AR_XSTART+i,y+AR_HEIGHT+4,num);
		num+=step;
	}

	bSurface=false;

}
void CAR::drawSurface(int y,int r1,int r2)
{

	DRAW_HANDLE

	int i;
	int n,num,step;

	painter2->setColor(meshColor);
	for(i=32;i<256;i+=32)
	{
		painter2->drawLine(AR_XSTART,y+i,1024,AR_WIDTH);
	}

	n=AR_WIDTH/markNum;

	for(i=0;i<AR_WIDTH;i+=n)
	{
		painter2->drawLine(AR_XSTART+i,y,2048,AR_HEIGHT);
	}

	painter2->setColor(foreColor);
	painter2->drawNoFillRect(AR_XSTART,y,AR_XSTART+AR_WIDTH-1,y+AR_HEIGHT-1);

	step=(r2-r1)/markNum;
	num=r1;
	for(i=0;i<=AR_WIDTH;i+=n)
	{
		if(num>10000)break;//��������λ��
		painter2->drawNum(AR_XSTART+i,y+AR_HEIGHT+4,num);
		num+=step;
	}

	bSurface=true;

}

void CAR::updateRegionLine()
{
	DRAW_HANDLE

	//int x1,x2;

	//if(bRegionLine)clearRegionLine();

	//x1=stR*AR_WIDTH/range;
	//x2=edR*AR_WIDTH/range;
	//painter3->drawLine(x1+AR_XSTART,AR_YSTART+1,2048,AR_HEIGHT-1);
	//painter3->drawLine(x2+AR_XSTART,AR_YSTART+1,2048,AR_HEIGHT-1);

	//bRegionLine=true;

}
void CAR::clearRegionLine()
{
	DRAW_HANDLE

	//int x1,x2;

	//x1=stR*AR_WIDTH/range;
	//x2=edR*AR_WIDTH/range;
	//eraser3->drawLine(x1+AR_XSTART,AR_YSTART+1,2048,AR_HEIGHT-1);
	//eraser3->drawLine(x2+AR_XSTART,AR_YSTART+1,2048,AR_HEIGHT-1);

	//bRegionLine=false;
}

void CAR::setARColor(unsigned int c)
{
	ARcolor=csectocmem(c);
	drawVideo(MFB_CLEAR);
	drawVideo(ARcolor);
}
void CAR::setForeColor(unsigned int c)
{
	foreColor=csectocmem(c);
	setMeshBright(getBright(meshColor));
}
void CAR::setMeshBright(unsigned char b)
{
	meshColor=changeBright(cmemtocsec(foreColor),b);
	updateSurface();
}

void CAR::setARRange(int r)
{
	clearSurface(AR_YSTART,0,range);
	if(bRegionLine)clearRegionLine();
	range = r;
	drawSurface(AR_YSTART,0,range);

	updateRegionLine();
	if(hasLine1 && hasLine2)ARDrawEnlarge(datbuf,datbufLen);
}

void CAR::setEnlargeRegion(int st,int ed)//unit as range--km
{
	
	if(bSurface)clearSurface(AR_YSTART2,stR,edR);
	if(bRegionLine)clearRegionLine();

	stR=st;
	edR=ed;

	drawSurface(AR_YSTART2,stR,edR);

	updateRegionLine();
	if(hasLine2)ARDrawEnlarge(datbuf,datbufLen);

}
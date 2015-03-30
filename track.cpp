//******************************
//
//  author : lishidan
//
//  create time: 2007-10-10
//
//  last modify time:2008-2-19
//
//  Version: 1.0
//
//******************************

#include "StdAfx.h"

#include "track.h"
#include "math.h"


CTrack::CTrack(CDataPool *m_dp)
{

	dp=m_dp;
	vecTime=dp->dispState.vecTime;
	vecType=dp->dispState.vecType;
	traceDotsNum=1;
	traceNum=0;
	infoBatchNum=-1;

	range = 60000;

	dotColor=MFB_YELLOW;
	dotsNum=30;

    layerS4=new CLayerBase(dp->fbpPPI,dp->layerPPI,4,true);
    layerR4=new CLayerBase(dp->fbpPPI,dp->layerPPI,4,false);
    painter = new CPainter(dp->painterPool,layerS4);
    eraser = new CPainter(dp->painterPool,layerR4);

	layerS3=new CLayerBase(dp->fbpPPI,dp->layerPPI,3,true);
	layerR3=new CLayerBase(dp->fbpPPI,dp->layerPPI,3,false);
	dotPainter = new CPainter(dp->painterPool,layerS3);
	dotEraser = new CPainter(dp->painterPool,layerR3);

	dotPainter->setColor(dotColor);

  //  painter->setColor(MFB_WHITE);

}

CTrack::~CTrack()
{
delete eraser;
delete painter;
delete layerS4;
delete layerR4;

TRACE("CTrack::~CTrack()");
}

ListTargetsIte CTrack::findTarget(int batchNum)
{

	ListTargetsIte ite;

	for(ite=targets.begin();ite!=targets.end();ite++)
	{
		if(ite->batchNum==batchNum)return ite;
	}

	return targets.end();
}

ListTargetsIte CTrack::findTarget(int x,int y)
{
	ListTargetsIte ite;
	int td;

	for(ite=targets.begin();ite!=targets.end();ite++)
	{
		td= floor(sqrt(pow((float)(x-ite->newx),2)+pow((float)(y-ite->newy),2)));
		if(td<10)return ite;
	}

	return targets.end();
}

void CTrack::updateTraceData(ListTargetsIte ite)//根据当前显示状态更新Trace中的绘图数据
{
	DeqTraceIte tite;

	for(tite=ite->trace.begin();tite!=ite->trace.end();tite++)
	{
		updateTraceData(tite);
	}

	tite=ite->trace.begin();
	ite->newx=tite->x;
	ite->newy=tite->y;
}

void CTrack::updateTraceData(DeqTraceIte ite) //根据当前显示状态更新Trace中的trace dot航迹点数据
{
	float azi,dis;
	int x,y;
	int aziIndex,disIndex;// used for cal x,y

    float moveAzi,speed;

	azi=ite->azi;
	dis=ite->dis;

	aziIndex=(int)floorf(azi/AZIDIS);
	disIndex=(int)floorf(dis/range*600.0);

	if(disIndex<ADDR_NEAR_SMP_LEN)
		{
		x=dp->dispState.xorg + dp->addr_near_x[aziIndex][disIndex];
		y=dp->dispState.yorg - dp->addr_near_y[aziIndex][disIndex];
		}
	else
		{
        x=dp->dispState.xorg + dp->addr_far_x[aziIndex*2][disIndex-ADDR_NEAR_SMP_LEN];
        y=dp->dispState.yorg - dp->addr_far_y[aziIndex*2][disIndex-ADDR_NEAR_SMP_LEN];
		}

	ite->x=x;
	ite->y=y;

}

void CTrack::insertTraceNode(ListTargetsIte ite, int_trace &info)
{
	float azi,dis;//unit degree, meter
	DeqTraceIte tite;
	
	ite->moveAzi=info.moveAzi;
	ite->speed=info.speed;
	ite->attr=info.attr;
	ite->type=info.type;

    azi=info.azi;
 	dis=info.dis;

    ite->updated=true;

	ite->pushFront(0, 0, azi, dis);
	
	while(ite->trace.size()>traceDotsNum)ite->trace.pop_back();

	tite=ite->trace.begin();
	updateTraceData(tite);//运算绘图矢量线方位 距离

	ite->newx=tite->x;
	ite->newy=tite->y;

}

void CTrack::trackUpdate(int_trace info)
{
	ListTargetsIte targetite;
	CTrace tmptrace;

	int batchNum = info.batchNum;


    targetite=findTarget(batchNum);
	tmptrace.batchNum=batchNum;
	if(targetite==targets.end())  // if it cannot find this trace, build a new one
		{
			targets.push_back(tmptrace);
			targetite=targets.end();
			targetite--;
			insertTraceNode(targetite, info);
            drawSingleTrace(targetite);
            traceNum ++;
            if (traceNum == 50)
            {
                //emit TraceFullAlarm (1);
            }
		}
	else
		{
			targetite->updated=true;
			clearSingleTrace(targetite);
			insertTraceNode(targetite,info);
			drawSingleTrace(targetite);

		}

}



void CTrack::drawAllTraces()
{
	ListTargetsIte targetite;

	for(targetite=targets.begin();targetite!=targets.end();targetite++)
	{
		drawSingleTrace(targetite);
	}
}



void CTrack::clearAllTraces()
{
	ListTargetsIte targetite;

	for(targetite=targets.begin();targetite!=targets.end();targetite++)
	{
		clearSingleTrace(targetite);
	}

}

void CTrack::drawSingleTrace(ListTargetsIte ite)
{
	DeqTraceIte tite;

	drawTargetIcon(ite->newx, ite->newy, ite->attr, ite->type);	
	if(ite->isInfoDisp)
		drawInfoRect(ite->newx,ite->newy,ite->batchNum,ite->moveAzi,ite->speed);
    
}

void CTrack::clearSingleTrace(ListTargetsIte ite)
{
   	DeqTraceIte tite;

	clearTargetIcon(ite->newx, ite->newy, ite->attr, ite->type);	
	if(ite->isInfoDisp)
		clearInfoRect(ite->newx,ite->newy,ite->batchNum,ite->moveAzi,ite->speed);
}


void CTrack::frozeTrack()
{

}



void CTrack::targetsCheck()
{
	ListTargetsIte ite;
	for(ite=targets.begin();ite!=targets.end();ite++)
	{
		if(ite->updated){ite->updated=false;continue;}

		clearSingleTrace(ite);

	}

}

void CTrack::updateAllTraces()
{
	ListTargetsIte ite;
	for(ite=targets.begin();ite!=targets.end();ite++)
	{
		clearSingleTrace(ite);
		updateTraceData(ite);
		drawSingleTrace(ite);
	}

}

void CTrack::updateSomeTraces(int xleft,int yup,int xrtight,int ydown)
{
	ListTargetsIte ite;
	for(ite=targets.begin();ite!=targets.end();ite++)
	{
		if(ite->newx>=xleft && ite->newx <=xrtight && \
			ite->newy>=yup && ite->newy <= ydown)
		{
			clearSingleTrace(ite);
			updateTraceData(ite);
			drawSingleTrace(ite);
		}
	}

}

void CTrack::redrawAllTraces()
{
	ListTargetsIte ite;
	for(ite=targets.begin();ite!=targets.end();ite++)
	{
		drawSingleTrace(ite);
	}

}

void CTrack::redrawSomeTraces(int xleft,int yup,int xrtight,int ydown)
{
	ListTargetsIte ite;
	for(ite=targets.begin();ite!=targets.end();ite++)
	{
		if(ite->newx>=xleft && ite->newx <=xrtight && \
			ite->newy>=yup && ite->newy <= ydown)
		{
			drawSingleTrace(ite);
		}
	}

}

void CTrack::updateFbp()
{
	layerR4->setFbp(dp->fbpPPI);
	layerS4->setFbp(dp->fbpPPI);
	layerR3->setFbp(dp->fbpPPI);
	layerS3->setFbp(dp->fbpPPI);
}

int CTrack::targetWithdraw(int bn)
{
	ListTargetsIte ite;
	int x1,y1,x2,y2;
	ite=findTarget(bn);
	if(ite==targets.end())return -1;// target does not exist

	if(infoBatchNum==ite->batchNum)infoBatchNum=-1;
	clearSingleTrace(ite);

	x1=ite->newx-10;
	y1=ite->newy-10;
	x2=ite->newx+10;
	y2=ite->newy+10;
	
	targets.erase(ite);
	traceNum--;
	redrawSomeTraces(x1,y1,x2,y2);

	return 0;

}

int CTrack::targetWithdraw(int x,int y)
{
	ListTargetsIte ite;
	int bn;
	int x1,y1,x2,y2;
	ite=findTarget(x,y);
	if(ite==targets.end())return -1;// target does not exist

	if(infoBatchNum==ite->batchNum)infoBatchNum=-1;
	clearSingleTrace(ite);
	bn= ite->batchNum;

	x1=ite->newx-10;
	y1=ite->newy-10;
	x2=ite->newx+10;
	y2=ite->newy+10;

	targets.erase(ite);
	traceNum--;
	redrawSomeTraces(x1,y1,x2,y2);

	return bn;

}

int CTrack::cancelInfoDisplay()
{
	int bn;
	ListTargetsIte ite;
	if(infoBatchNum<0)return 0;

	bn = targetInfoDisplay(infoBatchNum,false);
	ite=findTarget(bn);
	if(ite==targets.end())return -1;
	redrawSomeTraces(ite->newx-10,ite->newy-10,ite->newx+10+INFO_WIDTH,ite->newy+10+INFO_HEIGHT);

	return bn;
}

int CTrack::targetInfoDisplay(int x,int y,bool flag)
{
	ListTargetsIte ite;
	ite=findTarget(x,y);
	if(ite==targets.end())return -1;

	targetInfoDisplay(ite,flag);
	return ite->batchNum;

}
int CTrack::targetInfoDisplay(int bn,bool flag)
{
	ListTargetsIte ite;
	ite=findTarget(bn);
	if(ite==targets.end())return -1;

	targetInfoDisplay(ite,flag);
	return ite->batchNum;
}

int CTrack::targetInfoDisplay(ListTargetsIte ite,bool flag)
{
	ListTargetsIte tmpite;
	if(flag)//画新的信息框
	{
		if(infoBatchNum>0)//清除已有的信息框
		{
			tmpite=findTarget(infoBatchNum);
			if(tmpite!=targets.end())
			{
				clearInfoRect(tmpite->newx,tmpite->newy,tmpite->batchNum,tmpite->moveAzi,tmpite->speed);
				tmpite->isInfoDisp=false;
				infoBatchNum=-1;
			}
		}

		//画新框
		painter->setColor(targetColor[ite->attr]);

		infoBatchNum=ite->batchNum;
		drawInfoRect(ite->newx,ite->newy,ite->batchNum,ite->moveAzi,ite->speed);
		ite->isInfoDisp=true;
	}
	else//清除指定信息框
	{
		clearInfoRect(ite->newx,ite->newy,ite->batchNum,ite->moveAzi,ite->speed);
		ite->isInfoDisp=false;
	}

	return 0;
}

void CTrack::dotUpdate(float d,float a)// 距离d，单位：米; 方位a，单位：度
{
	s_tracedot tmp;
	DeqTraceIte ite;

	tmp.azi=a;
	tmp.dis=d;

	dotTrace.push_back(tmp);
	ite=dotTrace.end();
	ite--;
	updateTraceData(ite);

	dotPainter->drawTrackDot (ite->x,ite->y);

	while(dotTrace.size()>dotsNum)
	{
		ite=dotTrace.begin();
		dotEraser->drawTrackDot (ite->x,ite->y);
		dotTrace.pop_front();
	}

}
void CTrack::updateAllDots()
{
	DeqTraceIte ite;

	for(ite=dotTrace.begin();ite!=dotTrace.end();ite++)
	{
		dotEraser->drawTrackDot(ite->x,ite->y);
		updateTraceData(ite);
		dotPainter->drawTrackDot (ite->x,ite->y);
	}
}
void CTrack::drawAllDots()
{
	DeqTraceIte ite;

	for(ite=dotTrace.begin();ite!=dotTrace.end();ite++)
	{
		dotPainter->drawTrackDot(ite->x,ite->y);
	}
}
void CTrack::clearAllDots()
{
	DeqTraceIte ite;

	for(ite=dotTrace.begin();ite!=dotTrace.end();ite++)
	{
		dotEraser->drawTrackDot(ite->x,ite->y);
	}
}

void CTrack::setRange(float r)
{
	if(r<0)return;

	range=r;
	updateAllTraces();
}

//------------------ drawing function -------------------------
void CTrack::drawTargetIcon(int x,int y,int attr,int type)
{
	int n=attr*3+type;

	painter->setColor(targetColor[attr]);

	switch(n)
	{
	case 0://三角形，内部有一点
		painter->drawTrackDot(x,y);
		painter->drawLine(x-10,y,512,14);
		painter->drawLine(x+10,y,4096-512,14);
		break;

	case 1://
		painter->drawTrackDot(x,y);
		painter->drawLine(x-10,y,512,14);
		painter->drawLine(x+10,y,4096-512,14);
		painter->drawLine(x-10,y,1024+512,14);
		painter->drawLine(x+10,y,2048+512,14);
		break;

	case 2:
		painter->drawTrackDot(x,y);
		painter->drawLine(x-10,y,1024+512,14);
		painter->drawLine(x+10,y,2048+512,14);	
		break;

	case 3:
		painter->drawTrackDot(x,y);
		painter->drawArc(x,y,3072,1024,10);
		break;

	case 4:
		painter->drawTrackDot(x,y);
		painter->drawCircle(x,y,10);
		break;

	case 5:
		painter->drawTrackDot(x,y);
		painter->drawArc(x,y,1024,3072,10);
		break;

	case 6:
		painter->drawTrackDot(x,y);
		painter->drawLine(x-10,y,0,10);
		painter->drawLine(x-10,y-10,1024,20);
		painter->drawLine(x+10,y,0,10);
		break;
		
	case 7:
		painter->drawTrackDot(x,y);
		painter->drawNoFillRect(x-10,y-10,x+10,y+10);
		break;

	case 8:
		painter->drawTrackDot(x,y);
		painter->drawLine(x-10,y,2048,10);
		painter->drawLine(x-10,y+10,1024,20);
		painter->drawLine(x+10,y,2048,10);
		break;

	case 9:
		painter->drawTrackDot(x,y);
		painter->drawLine(x-10,y,512,14);
		painter->drawLine(x+10,y,4096-512,14);
		painter->drawLine(x-10,y,1024,20);
		//painter->drawArc(x,y,3072,1024,10);
		//painter->drawLine(x,y,512,10);
		//painter->drawLine(x,y,4096-512,10);
		break;

	case 10:
		painter->drawTrackDot(x,y);
		painter->drawLine(x-10,y,512,14);
		painter->drawLine(x+10,y,4096-512,14);
		painter->drawLine(x-10,y,1024+512,14);
		painter->drawLine(x+10,y,2048+512,14);
		painter->drawLine(x-10,y,1024,20);
		//painter->drawCircle(x,y,10);
		//painter->drawLine(x,y,512,10);
		//painter->drawLine(x,y,4096-512,10);
		//painter->drawLine(x,y,1024+512,10);
		//painter->drawLine(x,y,2048+512,10);
		break;
		
	case 11:
		painter->drawTrackDot(x,y);
		painter->drawLine(x-10,y,1024+512,14);
		painter->drawLine(x+10,y,2048+512,14);	
		painter->drawLine(x-10,y,1024,20);
	/*	painter->drawArc(x,y,1024,3072,10);
		painter->drawLine(x,y,1024+512,10);
		painter->drawLine(x,y,2048+512,10);*/
		break;

	}

}


void CTrack::clearTargetIcon(int x,int y,int attr,int type)
{
	int n=attr*3+type;

	switch(n)
	{
	case 0:
		eraser->drawTrackDot(x,y);
		eraser->drawLine(x-10,y,512,14);
		eraser->drawLine(x+10,y,4096-512,14);
		break;

	case 1:
		eraser->drawTrackDot(x,y);
		eraser->drawLine(x-10,y,512,14);
		eraser->drawLine(x+10,y,4096-512,14);
		eraser->drawLine(x-10,y,1024+512,14);
		eraser->drawLine(x+10,y,2048+512,14);
		break;

	case 2:
		eraser->drawTrackDot(x,y);
		eraser->drawLine(x-10,y,1024+512,14);
		eraser->drawLine(x+10,y,2048+512,14);	
		break;

	case 3:
		eraser->drawTrackDot(x,y);
		eraser->drawArc(x,y,3072,1024,10);
		break;

	case 4:
		eraser->drawTrackDot(x,y);
		eraser->drawCircle(x,y,10);
		break;

	case 5:
		eraser->drawTrackDot(x,y);
		eraser->drawArc(x,y,1024,3072,10);
		break;

	case 6:
		eraser->drawTrackDot(x,y);
		eraser->drawLine(x-10,y,0,10);
		eraser->drawLine(x-10,y-10,1024,20);
		eraser->drawLine(x+10,y,0,10);
		break;

	case 7:
		eraser->drawTrackDot(x,y);
		eraser->drawNoFillRect(x-10,y-10,x+10,y+10);
		break;

	case 8:
		eraser->drawTrackDot(x,y);
		eraser->drawLine(x-10,y,2048,10);
		eraser->drawLine(x-10,y+10,1024,20);
		eraser->drawLine(x+10,y,2048,10);
		break;

	case 9:
		eraser->drawTrackDot(x,y);
		eraser->drawLine(x-10,y,512,14);
		eraser->drawLine(x+10,y,4096-512,14);
		eraser->drawLine(x-10,y,1024,20);
		//eraser->drawArc(x,y,3072,1024,10);
		//eraser->drawLine(x,y,512,10);
		//eraser->drawLine(x,y,4096-512,10);
		break;

	case 10:
		eraser->drawTrackDot(x,y);
		eraser->drawLine(x-10,y,512,14);
		eraser->drawLine(x+10,y,4096-512,14);
		eraser->drawLine(x-10,y,1024+512,14);
		eraser->drawLine(x+10,y,2048+512,14);
		eraser->drawLine(x-10,y,1024,20);
		//eraser->drawCircle(x,y,10);
		//eraser->drawLine(x,y,512,10);
		//eraser->drawLine(x,y,4096-512,10);
		//eraser->drawLine(x,y,1024+512,10);
		//eraser->drawLine(x,y,2048+512,10);
		break;

	case 11:
		eraser->drawTrackDot(x,y);
		eraser->drawTrackDot(x,y);
		eraser->drawLine(x-10,y,1024+512,14);
		eraser->drawLine(x+10,y,2048+512,14);	
		eraser->drawLine(x-10,y,1024,20);
		//eraser->drawArc(x,y,1024,3072,10);
		//eraser->drawLine(x,y,1024+512,10);
		//eraser->drawLine(x,y,2048+512,10);
		break;

	}

}

void CTrack::drawInfoRect(int x,int y,int bn,float f1,float f2)
{
	unsigned char *d;
	unsigned char dat;
	int ox,oy,k;

	d=(unsigned char*)infoBitmapDot;

	ox=x;
	oy=y;
	for(int i=0;i<INFO_HEIGHT;i++)
	{
		y=oy+i;
		x=ox;
		for(int j=0;j<INFO_WIDTH/8;j++)
		{
			k=i*INFO_WIDTH/8 + j;
			dat=d[k];

			for(int l=7;l>=0;l--)
			{
				if(picMask[l] & dat)
					painter->setPixel(x,y);
				x++;
			}
		}
	}

	painter->drawNum(ox+INFO_X,oy+INFO_Y1,bn);
	painter->drawFloat(ox+INFO_X,oy+INFO_Y2,f1,1);
	painter->drawFloat(ox+INFO_X,oy+INFO_Y3,f2,1);

}
void CTrack::clearInfoRect(int x,int y,int bn,float f1,float f2)
{
	unsigned char *d;
	unsigned char dat;
	int ox,oy,k;

	d=(unsigned char*)infoBitmapDot;

	ox=x;
	oy=y;
	for(int i=0;i<INFO_HEIGHT;i++)
	{
		y=oy+i;
		x=ox;
		for(int j=0;j<INFO_WIDTH/8;j++)
		{
			k=i*INFO_WIDTH/8 + j;
			dat=d[k];

			for(int l=7;l>=0;l--)
			{
				if(picMask[l] & dat)
					eraser->setPixel(x,y);
				x++;
			}
		}
	}

	eraser->drawNum(ox+INFO_X,oy+INFO_Y1,bn);
	eraser->drawFloat(ox+INFO_X,oy+INFO_Y2,f1,1);
	eraser->drawFloat(ox+INFO_X,oy+INFO_Y3,f2,1);

}
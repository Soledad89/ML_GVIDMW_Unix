#pragma once


#include "datapool.h"
#include "fbinclude.h"
#include "ppidef.h"
#include "mycolor.h"


class CAR
{
public:
	CAR(CDataPool * m_dp);
public:
	~CAR(void);

public:
	void setMarkEnable(bool b){markEnable = b;}

	void ARDraw(s_ppi_video_for_disp*);
	void ARDraw(unsigned char * dat,int len);
	void ARDrawEnlarge(unsigned char * dat,int len);

	void updateRegion(void);//更新作图区位置，即从DataPool中获取区域位置数据
	void clear();
	void setAR(bool b){bAR = b;}
	bool isAR(){return bAR;}
	void setEnlarge(bool b){bEnlarge = b;}
	bool isEnlarge(){return bEnlarge;}
	void setARGate(int g);
	int getARGate(){return gate;}
	void setARDistance(int d);
	int getARDistance(){return distance;}

	void updateSurface();
	void clearSurface(int y,int r1,int r2);
	void drawSurface(int y,int r1,int r2);
	void updateRegionLine();
	void clearRegionLine();
	void setARColor(unsigned int c);
	void setForeColor(unsigned int c);
	void setMeshBright(unsigned char b);
	void setARRange(int r);
	void setEnlargeRegion(int st,int ed);//unit as range--km

	void updateFbp(void);//更新绘图显存区地址

private:
	inline bool isValid( int &x, int &y );
	inline void setFirstPixel(int x, int y, DISPMEMTYPE color);
	void drawVideo( DISPMEMTYPE color);
	void drawEnlargeVideo( DISPMEMTYPE color);
	void drawLineP2P(int x1,int y1,int x2,int y2,DISPMEMTYPE color);


private:
	int AR_XSTART;//上半区（完整回波）尺寸
	int AR_YSTART;
	int AR_WIDTH;
	int AR_HEIGHT;

	int AR_XSTART2;//下半区（放大回波）尺寸
	int AR_YSTART2;
	int AR_WIDTH2;
	int AR_HEIGHT2;

	DISPMEMTYPE ARcolor;

	bool markEnable;

	bool bAR;
	bool bEnlarge;
	static const int oncevn=20;// once every video number,每多少条视频画一次AR
	int videoCount;// count for video passed

	int gate;
	int distance;//显示的像素点范围

	CDataPool *dp;
	MFB_COLORTYPE *colortable;
	DISPMEMTYPE** fbp;//pointer to framebuffer mem
	s_layer_unit ** layer;//frame buffer layer

	int video_data[SAMPLE_NUM];//合并后的视频数据	
	int *video_enlarge;//局部区域放大后的视频
	VIDEODATATYPE *datbuf;//缓存当前显示图形的雷达数据
	int datbufLen;

	//for surface
	CLayerBase *layerS2;
	CLayerBase *layerR2;

	CPainter *painter2;
	CPainter *eraser2;

	// for enlarge line
	CLayerBase *layerS3;
	CLayerBase *layerR3;

	CPainter *painter3;
	CPainter *eraser3;

	bool bSurface;
	bool bRegionLine;
	bool hasLine1;
	bool hasLine2;
	int range;// unit km
	int stR,edR;// unit km

	DISPMEMTYPE foreColor;
	DISPMEMTYPE meshColor;
	int markNum;

};

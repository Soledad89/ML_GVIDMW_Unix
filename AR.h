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

	void updateRegion(void);//������ͼ��λ�ã�����DataPool�л�ȡ����λ������
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

	void updateFbp(void);//���»�ͼ�Դ�����ַ

private:
	inline bool isValid( int &x, int &y );
	inline void setFirstPixel(int x, int y, DISPMEMTYPE color);
	void drawVideo( DISPMEMTYPE color);
	void drawEnlargeVideo( DISPMEMTYPE color);
	void drawLineP2P(int x1,int y1,int x2,int y2,DISPMEMTYPE color);


private:
	int AR_XSTART;//�ϰ����������ز����ߴ�
	int AR_YSTART;
	int AR_WIDTH;
	int AR_HEIGHT;

	int AR_XSTART2;//�°������Ŵ�ز����ߴ�
	int AR_YSTART2;
	int AR_WIDTH2;
	int AR_HEIGHT2;

	DISPMEMTYPE ARcolor;

	bool markEnable;

	bool bAR;
	bool bEnlarge;
	static const int oncevn=20;// once every video number,ÿ��������Ƶ��һ��AR
	int videoCount;// count for video passed

	int gate;
	int distance;//��ʾ�����ص㷶Χ

	CDataPool *dp;
	MFB_COLORTYPE *colortable;
	DISPMEMTYPE** fbp;//pointer to framebuffer mem
	s_layer_unit ** layer;//frame buffer layer

	int video_data[SAMPLE_NUM];//�ϲ������Ƶ����	
	int *video_enlarge;//�ֲ�����Ŵ�����Ƶ
	VIDEODATATYPE *datbuf;//���浱ǰ��ʾͼ�ε��״�����
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
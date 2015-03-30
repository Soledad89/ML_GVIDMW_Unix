//******************************
//
//  author : lishidan
//
//  create time: 2007-10-23
//
//  last modify time:2007-10-23
//
//  Version: 0.1
//
//******************************

#ifndef _YH_H_
#define _YH_H_

//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "debug.h"
#include "ppidef.h"
#include "fbinclude.h"
#include "datapool.h"

//#include <qtimer.h>

const int YH_NUM=600*50;
const int CNUM = 4; //������

class CYh //: public QObject
{
    //Q_OBJECT

public:
    CYh( CDataPool *dp );
    ~CYh();

public:
    void setYhWeight( int yhmul );
	void setYhLen(int len){yhLen=len;}
	int getYhWeight(){return yhDecWeight;}

    void enableYh(){ bYh = 1; }
    void disableYh(){ bYh = 0; }
    bool isYh(){ return bYh; }

	void setYhWin(bool b){bWin=b;}

public: // slots:
    void yh();
	void updateRegion(void);//������ͼ��λ�ã�����DataPool�л�ȡ����λ������
	void updateFbp(void);//���»�ͼ�Դ�����ַ

	void updateTransTable();


public:
	int PPI_XSTART;
	int PPI_YSTART;
	int PPI_WIDTH;
	int PPI_HEIGHT;

	int WIN_XSTART;
	int WIN_YSTART;
	int WIN_WIDTH;
	int WIN_HEIGHT;

	int AR_XSTART;
	int AR_YSTART;
	int AR_WIDTH;
	int AR_HEIGHT;

private:
	DISPMEMTYPE *winColorTable;

public:
    //QTimer *timer;

    int yhDecWeight;
    int yhLen;
    int yh_index;
    bool bYh;

	bool bWin;

    CDataPool *m_dp;
    int * randomx;
    int * randomy;
    MFB_COLORTYPE *colortable;
    DISPMEMTYPE ** fbpPPI;//pointer to framebuffer mem
    s_layer_unit ** layerPPI;//frame buffer layer
	DISPMEMTYPE ** fbpMINI;//pointer to framebuffer mem
	s_layer_unit ** layerMINI;//frame buffer layer
	
	//DISPMEMTYPE fbpbuf[CNUM];//�������������ʱ����������

	VIDEODATATYPE transTable[256];
};

#endif

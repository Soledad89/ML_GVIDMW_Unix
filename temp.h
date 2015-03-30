#pragma once

#include "DrawingManager_DD.h"
#include "mywidget.h"
//
#include "ppidef.h"
#include "tv.h"
#include "fbinclude.h"
#include "painter.h"
//
#include "layers.h"
//#include "datapool.h"

#include "ML_GVIDMW.h"
#include "ARPAInfo.h"





typedef struct s_TV_channel{
	int x;
	int y;
	int width;
	int height;
}s_TV_channel;

class C_THU_ML_GVID
{


public:
	C_THU_ML_GVID( );
public:
	~C_THU_ML_GVID(void);


public:
	
	int arStep1;
	int arStep2;
	//--- for mouse painte
	int mx;
	int my;
	int proState;
	//-------------------------
	//对外函数接口
public:

	void sendPkg(int, unsigned char *);
	

};

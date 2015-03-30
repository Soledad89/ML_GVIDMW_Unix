#pragma once

#include "DrawingManager_DD.h"
#include "mywidget.h"

#include "ppidef.h"
#include "TVlib/tv.h"
#include "fbinclude.h"
#include "painter.h"

#include "ML_GVIDMW.h"
#include "ARPAInfo.h"
#include "netRadar.h"


class CNetRadar;

typedef struct s_TV_channel{
	int x;
	int y;
	int width;
	int height;
}s_TV_channel;

class C_THU_ML_GVID
{
	friend class CNetRadar;

public:
	C_THU_ML_GVID( const s_in_param* p,CDrawingManager* dm1,CDrawingManager* dm2);
public:
	~C_THU_ML_GVID(void);



public:
	MyWidget *widgets[2];
	TVPlayer *player;

public:
	MyWidget * widget1;
	MyWidget * widget2;

	//TVPlayer * player1;
	//TVPlayer * player2;

	//Client * netclient;
	CNetRadar * netradar;

	s_channel state1;
	s_channel state2;

	s_TV_channel stv1;
	s_TV_channel stv2;

	s_init_param initParam[2];

//--- for mouse painte
	int mx;
	int my;
	int proState;
//-------------------------
private:
	CDrawingManager *_dm1;
	CDrawingManager *_dm2;

//对外函数接口
public:

	void sendPkg(int, unsigned char *);
	void sendDispPkg(int,int,s_ppi_video_for_disp *);
	void yh(int);
	void trackUpdate(int, s_arpa_trace info);
	void dispUpdate(int ,s_ppi_video_for_disp *);
	void initSurface(int);

	void close();

	void MouseEventHandler(unsigned int type,int x,int y);
	void clearScreen(short  theDisplay);

	void PlayTV(short VideoChannel,HWND hwnd);

};

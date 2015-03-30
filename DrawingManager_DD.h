

/********************************************
Author: Li Shidan
Create time: 2009.4
Verified: partial
********************************************/



#ifndef _DRAWING_MANAGER_DD__A91AC6F5C1DC
#define _DRAWING_MANAGER_DD__A91AC6F5C1DC


#include <ddraw.h>
#include <Windows.h>

#pragma  comment(lib,"ddraw.lib")
#pragma  comment(lib,"dxguid.lib")

#include "ML_GVIDMW.h"
#include "ML_GVIDMW_sys.h"

/*
enum e_source_type{E_CARD,E_NET,E_SWITCH};
enum e_access_type{E_BROADCAST,E_P2P};

struct s_net_config{
	char address[16];
	unsigned int port;
	e_access_type accessType;
};

struct s_radar_channel{
	int ID;// ID<0 indicates invalid channel, ID should be 0 or 1 for valid channel
	HWND hPPIWnd;
	HWND hARWnd;
	HWND hMiniWnd;
	int PPIRadius;
	int PPIWidth;
	int PPIHeight;

	BOOL hasBottom;
	BOOL hasTop;

	e_source_type source;

	unsigned int hwCode;
	s_net_config netConfig;

};

struct s_tv_channel{
	int ID;// ID<0 indicates invalid channel, ID should be 0 or 1 for valid channel
	HWND hTVWnd;
	BOOL hasTop;

	e_source_type source;

	unsigned int hwCode;
	s_net_config netConfig;

};

struct s_init_param{
	int ID;// ID<0 indicates invalid channel, ID should be 0 or 1 for valid channel
	s_radar_channel radarChannel;
	s_tv_channel tvChannel;
	int refreshRate;
};
*/

//struct s_in_param{
//	s_init_param _param[2];
//};

//==============================================
//   !!! 此处大小的改动，移动到了ML_GVID_sys.h中！！！
//#define SURF_MINI_WIDTH 340
//#define SURF_MINI_HEIGHT 340
//#define SURF_AR_WIDTH 360
//#define SURF_AR_HEIGHT 300
//#define SURF_TV_WIDTH 720
//#define SURF_TV_HEIGHT 576
//#define SURF_CM_WIDTH 1200
//#define SURF_CM_HEIGHT 1200

#define DM_OK 1
#define DMERR_NOSUCHSURFACE -1
#define DMERR_INVALIDPARAM -2
#define DMERR_SYSTEM -3

enum e_disp_type{
	E_PPI=0,
	E_AR,
	E_TV,
	E_MINI
};

enum e_surface_type{
	E_COMBINATION=0, // this must be '0', for the last item is the total number
	E_PPI_RAW,
	E_PPI_BOTTOM,
	E_PPI_TOP,
	E_AR_RAW,
	E_AR_TOP,
	E_TV_RAW,
	E_TV_TOP,
	E_MINI_RAW,
	E_MINI_TOP,
	// the number below is the number of surfaces defined in the enum 'e_surface_type'
	_E_SERFACE_NUM 
};// the order and index cannot be changed

struct s_surface{
	LPDIRECTDRAWSURFACE7 lpsurface;// NULL for invalid surface
	int width;
	int height;
	e_surface_type type;
	BOOL isVisible;
	COLORREF rgbColorKey;
	DWORD dwColorKey;
};


//==============================================

class CDrawingManager{
// for singleton pattern
public:
	CDrawingManager();
	virtual ~CDrawingManager();
private:
	static LPDIRECTDRAW7 _pDD;   //DirectDraw Object
	static BOOL _count;

// interface
public:
	int init(const s_init_param& initParam);
	LPDIRECTDRAW7 getDirectDrawObject(){return _pDD;}

	int WindowDisplay_DC(HDC dc,e_disp_type type ,RECT &destRect);
	int WindowDisplay_DC(HDC dc,e_disp_type type ,RECT &destRect,RECT &srcRect);

	int LockSurface(e_surface_type surfType);
	int UnLockSurface(e_surface_type surfType);
	int LockAllRadarRawSurfaces();
	int UnLockAllRadarRawSurfaces();

	int GetSurfaceDC(HDC* dc, e_surface_type surfType);
	int ReleaseSurfaceDC(HDC dc, e_surface_type surfType);

	int SetSurfaceColorKey(e_surface_type surfType,COLORREF colorkey);
	int SetSurfaceVisible(e_surface_type surfType,BOOL visible);
	int EraseSurface(e_surface_type surfType);

	LPDIRECTDRAWSURFACE7 getPSurface(e_surface_type surfType);
	int getSurfaceInfo(e_surface_type surfType,void ** lplinear,int* pitch,int* rgbBitCount);
	int getSurfaceInfoByLock(e_surface_type surfType,void ** lplinear,int* pitch,int* rgbBitCount);

	//--------------------------------
	RECT getPPIRect(){return _PPIRect;}
	RECT getARRect(){return _ARRect;}
	RECT getTVRect(){return _TVRect;}

protected:
	LPDIRECTDRAW7 getDD(){return _pDD;}
	DWORD DDColorMatch(LPDIRECTDRAWSURFACE7  pdds, COLORREF rgb);

private:
	s_surface _surface[_E_SERFACE_NUM];//index using e_surface_type
	DDSURFACEDESC2 _surfDesc[_E_SERFACE_NUM];//index using e_surface_type

	// for efficient using
	LPDIRECTDRAWSURFACE7 _lpCombSurf;
	RECT _PPIRect;
	RECT _ARRect;
	RECT _TVRect;
	RECT _MINIRect;

};


#endif
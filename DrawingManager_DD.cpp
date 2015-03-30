
#include "StdAfx.h"
#include "DrawingManager_DD.h"


LPDIRECTDRAW7 CDrawingManager::_pDD=NULL;
BOOL CDrawingManager::_count=0;


int CDrawingManager::init(const s_init_param& initParam)
{
	DDSURFACEDESC2 ddsd; 

	if(_pDD==NULL)
	{
		if(DD_OK != DirectDrawCreateEx(NULL,(void**)&_pDD,IID_IDirectDraw7,NULL))   
		{ 
			return -1;
		}   

		if(DD_OK != _pDD->SetCooperativeLevel(NULL, DDSCL_NORMAL))   
		{   
			return -2;
		}   

		//DDCAPS ddCaps;   
		//DDCAPS ddHelCaps;   
		//_pDD->GetCaps(&ddCaps, &ddHelCaps);   

		//if (!(ddCaps.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER))    
		//{  
		//	return -3;    
		//}   

		++_count;
	}
	else
	{
		++_count;
	}
	

	// create surface for the channel
	
	//enum e_surface_type{
	//	E_COMBINATION=0,
	//	E_PPI_RAW=1,
	//	E_PPI_BOTTOM=2,
	//	E_PPI_TOP=3,
	//	E_AR_RAW=4,
	//	E_AR_TOP=5,
	//	E_TV_RAW=6,
	//	E_TV_TOP=7
	// E_MINI_RAW=8,
	//	E_MINI_TOP=9,
	//_E_SERFACE_NUM=10
	//};
	memset( (void*)_surface,0,sizeof(s_surface)*_E_SERFACE_NUM);
	//memset( (void*)&_PPIRect,0,sizeof(RECT));
	_lpCombSurf = NULL;

	if(initParam.ID>=0)
	{
		for(int k=0;k<_E_SERFACE_NUM;k++)
		{	// the hWnd in initParam is just used for identifying the validation of the display function,
			// such as PPI, AR, TV.
			if(initParam.radarChannel.hPPIWnd==0)
				if(k==1 || k==2 || k==3)continue;
			if(initParam.radarChannel.hARWnd==0)
				if(k==4 || k==5)continue;
			//if(initParam.tvChannel.hTVWnd==0)
			//	if(k==6 || k==7)continue;
			if(initParam.radarChannel.hMiniWnd==0)
				if(k==8|| k==9)continue;

			if(initParam.radarChannel.hasTop==FALSE)
				if(k==3 || k==5 || k==7 || k==9)continue;
			if(initParam.radarChannel.hasBottom==FALSE)
				if(k==2)continue;

			ZeroMemory(&ddsd,sizeof(ddsd));
			ddsd.dwSize         = sizeof( ddsd );
			ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;//DDSCAPS_SYSTEMMEMORY;//DDSCAPS_VIDEOMEMORY;

			if(k==0)// this 'if' should consider every surface's creation
			{
				ddsd.dwWidth        = SURF_CM_WIDTH;
				ddsd.dwHeight       = SURF_CM_HEIGHT;
			}
			else if(k==1 || k==2 || k==3)
			{
				ddsd.dwWidth        = SURF_CM_WIDTH;//initParam.radarChannel.PPIWidth;
				ddsd.dwHeight       = SURF_CM_HEIGHT;//initParam.radarChannel.PPIHeight;
			}
			else if(k==4 || k==5)
			{
				ddsd.dwWidth        = SURF_AR_WIDTH;
				ddsd.dwHeight       = SURF_AR_HEIGHT;
			}
			else if(k==6 || k==7)
			{
				ddsd.dwWidth        = SURF_TV_WIDTH;
				ddsd.dwHeight       = SURF_TV_HEIGHT;
			}
			else
			{
				ddsd.dwWidth        = SURF_MINI_WIDTH;
				ddsd.dwHeight       = SURF_MINI_HEIGHT;
			}

			_surface[k].width=ddsd.dwWidth;
			_surface[k].height=ddsd.dwHeight;
			_surface[k].isVisible=TRUE;// the initial visibilities are all true
			
			if(DD_OK != _pDD->CreateSurface(&ddsd, &_surface[k].lpsurface, NULL))
			{
				return -10-k;	
			}
		}
	}

	_lpCombSurf=_surface[E_COMBINATION].lpsurface;
	_PPIRect.left = 0;
	_PPIRect.top = 0;
	_PPIRect.right=_surface[E_PPI_RAW].width-1;
	_PPIRect.bottom=_surface[E_PPI_RAW].height-1;

	SetSurfaceColorKey(e_surface_type::E_PPI_RAW,RGB(0,0,0));

	for(int k=0;k<_E_SERFACE_NUM;k++)
	{
		SetSurfaceColorKey(e_surface_type(k),RGB(0,0,0));
		EraseSurface(e_surface_type(k));
	}

	return DM_OK;
}

CDrawingManager::CDrawingManager()
{
	//memset((void*)&_ARRect,0,sizeof(RECT));//如此清零有问题
	//memset((void*)&_TVRect,0,sizeof(RECT));
	//memset((void*)&_MINIRect,0,sizeof(RECT));
	_ARRect.left = 0;
	_ARRect.top = 0;
	_ARRect.right=SURF_AR_WIDTH-1;
	_ARRect.bottom=SURF_AR_HEIGHT-1;
	_TVRect.left = 0;
	_TVRect.top = 0;
	_TVRect.right=SURF_TV_WIDTH-1;
	_TVRect.bottom=SURF_TV_HEIGHT-1;
	_MINIRect.left = 0;
	_MINIRect.top = 0;
	_MINIRect.right=SURF_MINI_WIDTH-1;
	_MINIRect.bottom=SURF_MINI_HEIGHT-1;

	memset((void*)_surfDesc,0,sizeof(DDSURFACEDESC2)*_E_SERFACE_NUM);
	for(int i=0;i<_E_SERFACE_NUM;++i)
		_surfDesc[i].dwSize = sizeof(DDSURFACEDESC2);
}

CDrawingManager::~CDrawingManager()
{

	for(int k=0;k<_E_SERFACE_NUM;k++)
	{
		if(_surface[k].lpsurface != NULL)
			_surface[k].lpsurface->Release();
	}

	if(_pDD!=NULL)
		if(--_count==0){_pDD->Release();_pDD=NULL;}

	//memset((void*)_surface,0,sizeof(s_surface)*_E_SERFACE_NUM);
	//memset((void*)_surfDesc,0,sizeof(DDSURFACEDESC2)*_E_SERFACE_NUM);

}

int CDrawingManager::WindowDisplay_DC(HDC dc,e_disp_type type ,RECT &destRect)
{
	LPDIRECTDRAWSURFACE7 lpsurf;
	bool updated=false;

	if(destRect.right-destRect.left+1>SURF_CM_WIDTH)destRect.right = destRect.left +SURF_CM_WIDTH -1;
	if(destRect.bottom-destRect.top+1>SURF_CM_HEIGHT)destRect.bottom = destRect.top +SURF_CM_HEIGHT -1;
	
	if(type==E_PPI)// drawing radar
	{
		bool bBottom=false;

		lpsurf=_surface[E_PPI_BOTTOM].lpsurface;
		if( lpsurf && _surface[E_PPI_BOTTOM].isVisible)
		{
			_lpCombSurf->Blt(&destRect,  lpsurf, &_PPIRect, DDBLT_WAIT, NULL);
			bBottom=true;
			updated=true;
		}

		lpsurf=_surface[E_PPI_RAW].lpsurface;
		if( lpsurf && _surface[E_PPI_RAW].isVisible)
		{
			if(bBottom)
			{
				_lpCombSurf->Blt(&destRect, lpsurf, &_PPIRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); 
			}
			else
			{
				if(_lpCombSurf->Blt(&destRect, lpsurf, &_PPIRect, DDBLT_WAIT, NULL)==DDERR_SURFACEBUSY)
					return DMERR_SYSTEM;
			}
			updated=true;
		}

		lpsurf=_surface[E_PPI_TOP].lpsurface;
		if( lpsurf && _surface[E_PPI_TOP].isVisible)
		{
			_lpCombSurf->Blt(&destRect, lpsurf, &_PPIRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); 
			updated=true;
		}
	}
	else if(type==E_AR)// drawing AR
	{
		lpsurf=_surface[E_AR_RAW].lpsurface;
		if( lpsurf && _surface[E_AR_RAW].isVisible)
		{

			_lpCombSurf->Blt(&destRect, lpsurf, &_ARRect, DDBLT_WAIT, NULL);
			updated=true;
		}

		lpsurf=_surface[E_AR_TOP].lpsurface;
		if( lpsurf && _surface[E_AR_TOP].isVisible)
		{
			_lpCombSurf->Blt(&destRect, lpsurf, &_ARRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); 
			updated=true;
		}

	}
	else if(type==E_TV)// drawing TV
	{
		lpsurf=_surface[E_TV_RAW].lpsurface;
		if( lpsurf && _surface[E_TV_RAW].isVisible)
		{

			_lpCombSurf->Blt(&destRect, lpsurf, &_TVRect, DDBLT_WAIT, NULL);
			updated=true;
		}

		lpsurf=_surface[E_TV_TOP].lpsurface;
		if( lpsurf && _surface[E_TV_TOP].isVisible)
		{
			_lpCombSurf->Blt(&destRect, lpsurf, &_TVRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); 
			updated=true;
		}

	}
	else if(type==E_MINI)
	{
		lpsurf=_surface[E_MINI_RAW].lpsurface;
		if( lpsurf && _surface[E_MINI_RAW].isVisible)
		{

			if(_lpCombSurf->Blt(&destRect, lpsurf, &_MINIRect, DDBLT_WAIT, NULL)!=DD_OK)
				return DMERR_SYSTEM;
			updated=true;
		}

		lpsurf=_surface[E_MINI_TOP].lpsurface;
		if( lpsurf && _surface[E_MINI_TOP].isVisible)
		{
			_lpCombSurf->Blt(&destRect, lpsurf, &_MINIRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); 
			updated=true;
		}

	}
	else // no such display type
	{
		return DMERR_SYSTEM;
	}

	// blt combination surface to the user's DC
	if(updated)
	{
		HDC srcDC;
		if(_lpCombSurf->GetDC(&srcDC)!=DD_OK)return DMERR_SYSTEM;

		BitBlt(dc,destRect.left,destRect.top,destRect.right-destRect.left+1,destRect.bottom-destRect.top+1, 
			srcDC,destRect.left,destRect.top,SRCCOPY );

		_lpCombSurf->ReleaseDC(srcDC);
	}

	return DM_OK;
}
int CDrawingManager::WindowDisplay_DC(HDC dc,e_disp_type type ,RECT &destRect,RECT &srcRect)
{
	// this operation is the same as the above, except for having another parameter "srcRect" which 
	// is used for changing the source surface region for blting.

	LPDIRECTDRAWSURFACE7 lpsurf;
	bool updated=false;

	if(destRect.right-destRect.left+1>SURF_CM_WIDTH)destRect.right = destRect.left +SURF_CM_WIDTH -1;
	if(destRect.bottom-destRect.top+1>SURF_CM_HEIGHT)destRect.bottom = destRect.top +SURF_CM_HEIGHT -1;

	if(type==E_PPI)// drawing radar
	{
		bool bBottom=false;

		lpsurf=_surface[E_PPI_BOTTOM].lpsurface;
		if( lpsurf && _surface[E_PPI_BOTTOM].isVisible)
		{
			_lpCombSurf->Blt(&destRect,  lpsurf, &srcRect, DDBLT_WAIT, NULL);
			bBottom=true;
			updated=true;
		}

		lpsurf=_surface[E_PPI_RAW].lpsurface;
		if( lpsurf && _surface[E_PPI_RAW].isVisible)
		{
			if(bBottom)
			{
				_lpCombSurf->Blt(&destRect, lpsurf, &srcRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); 
			}
			else
			{
				_lpCombSurf->Blt(&destRect, lpsurf, &srcRect, DDBLT_WAIT, NULL);
			}
			updated=true;
		}

		lpsurf=_surface[E_PPI_TOP].lpsurface;
		if( lpsurf && _surface[E_PPI_TOP].isVisible)
		{
			_lpCombSurf->Blt(&destRect, lpsurf, &srcRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); 
			updated=true;
		}
	}
	else if(type==E_AR)// drawing AR
	{
		lpsurf=_surface[E_AR_RAW].lpsurface;
		if( lpsurf && _surface[E_AR_RAW].isVisible)
		{

			_lpCombSurf->Blt(&destRect, lpsurf, &srcRect, DDBLT_WAIT, NULL);
			updated=true;
		}

		lpsurf=_surface[E_AR_TOP].lpsurface;
		if( lpsurf && _surface[E_AR_TOP].isVisible)
		{
			_lpCombSurf->Blt(&destRect, lpsurf, &srcRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); 
			updated=true;
		}

	}
	else if(type==E_TV)// drawing TV
	{
		lpsurf=_surface[E_TV_RAW].lpsurface;
		if( lpsurf && _surface[E_TV_RAW].isVisible)
		{

			_lpCombSurf->Blt(&destRect, lpsurf, &srcRect, DDBLT_WAIT, NULL);
			updated=true;
		}

		lpsurf=_surface[E_TV_TOP].lpsurface;
		if( lpsurf && _surface[E_TV_TOP].isVisible)
		{
			_lpCombSurf->Blt(&destRect, lpsurf, &srcRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); 
			updated=true;
		}

	}
	else if(type==E_MINI)
	{
		lpsurf=_surface[E_MINI_RAW].lpsurface;
		if( lpsurf && _surface[E_MINI_RAW].isVisible)
		{

			_lpCombSurf->Blt(&destRect, lpsurf, &srcRect, DDBLT_WAIT, NULL);
			updated=true;
		}

		lpsurf=_surface[E_MINI_TOP].lpsurface;
		if( lpsurf && _surface[E_MINI_TOP].isVisible)
		{
			_lpCombSurf->Blt(&destRect, lpsurf, &srcRect, DDBLT_WAIT|DDBLT_KEYSRC, NULL); 
			updated=true;
		}
	}
	else // no such display type
	{
		return DMERR_SYSTEM;
	}

	// blt combination surface to the user's DC
	if(updated)
	{
		HDC srcDC;
		if(_lpCombSurf->GetDC(&srcDC)!=DD_OK)return DMERR_SYSTEM;

		BitBlt(dc,destRect.left,destRect.top,destRect.right-destRect.left+1,destRect.bottom-destRect.top+1, 
			srcDC,destRect.left,destRect.top,SRCCOPY );

		_lpCombSurf->ReleaseDC(srcDC);
	}

	return DM_OK;
}

int CDrawingManager::LockSurface(e_surface_type surfType)
{
	if(_surface[surfType].lpsurface==NULL)return DMERR_NOSUCHSURFACE;

	if(_surface[surfType].lpsurface->Lock(NULL, &_surfDesc[surfType], DDLOCK_WAIT, NULL)!= DD_OK)  
		return DMERR_SYSTEM;
	else
		return DM_OK;
}
int CDrawingManager::UnLockSurface(e_surface_type surfType)
{
	if(_surface[surfType].lpsurface==NULL)return DMERR_NOSUCHSURFACE;

	_surface[surfType].lpsurface->Unlock(NULL);
	return DM_OK;
}

int CDrawingManager::LockAllRadarRawSurfaces()
{
	int ret;
	ret=LockSurface(e_surface_type::E_PPI_RAW);
	if(ret==DMERR_SYSTEM)return DMERR_SYSTEM;
	ret=LockSurface(e_surface_type::E_AR_RAW);
	if(ret==DMERR_SYSTEM)return DMERR_SYSTEM;
	ret=LockSurface(e_surface_type::E_MINI_RAW);
	if(ret==DMERR_SYSTEM)return DMERR_SYSTEM;

	return DM_OK;
}
int CDrawingManager::UnLockAllRadarRawSurfaces()
{
	int ret;
	ret=UnLockSurface(e_surface_type::E_MINI_RAW);
	//if(ret==DMERR_SYSTEM)return DMERR_SYSTEM;
	ret=UnLockSurface(e_surface_type::E_AR_RAW);
	//if(ret==DMERR_SYSTEM)return DMERR_SYSTEM;
	ret=UnLockSurface(e_surface_type::E_PPI_RAW);
	//if(ret==DMERR_SYSTEM)return DMERR_SYSTEM;

	return DM_OK;
}

int CDrawingManager::GetSurfaceDC(HDC *dc, e_surface_type surfType)
{
	if(_surface[surfType].lpsurface==NULL)return DMERR_NOSUCHSURFACE;

	if(_surface[surfType].lpsurface->GetDC(dc)!=DD_OK)return DMERR_SYSTEM;

	return DM_OK;
}
int CDrawingManager::ReleaseSurfaceDC(HDC dc, e_surface_type surfType)
{
	if(_surface[surfType].lpsurface==NULL)return DMERR_NOSUCHSURFACE;

	if(_surface[surfType].lpsurface->ReleaseDC(dc)!=DD_OK)return DMERR_SYSTEM;
	return DM_OK;
}

int CDrawingManager::SetSurfaceColorKey(e_surface_type surfType,COLORREF colorkey)
{
	DDCOLORKEY ddck;   

	if(_surface[surfType].lpsurface==NULL)return DMERR_NOSUCHSURFACE;

	ddck.dwColorSpaceLowValue  = DDColorMatch(_surface[surfType].lpsurface,colorkey);
	ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;

	_surface[surfType].dwColorKey=ddck.dwColorSpaceLowValue;
	_surface[surfType].rgbColorKey=colorkey;

	if(_surface[surfType].lpsurface->SetColorKey(DDCKEY_SRCBLT, &ddck)!=DD_OK)return DMERR_SYSTEM; 

	return DM_OK;
}
int CDrawingManager::SetSurfaceVisible(e_surface_type surfType,BOOL visible)
{
	if(_surface[surfType].lpsurface==NULL)return DMERR_NOSUCHSURFACE;

	_surface[surfType].isVisible=visible;
	return DM_OK;
}

int CDrawingManager::EraseSurface(e_surface_type surfType)
{
	if(_surface[surfType].lpsurface==NULL)return DMERR_NOSUCHSURFACE;

	/*
	 the following codes are using direct draw Blt to fill a surface, it is hardware accelerated,
	 but in this using, it is not always working right and has different appearances.
	 thinking it may have some dependences on hardware which I do not know.
	*/

	//DDBLTFX ddbltfx; 
	//ddbltfx.dwSize = sizeof(ddbltfx); 
	//ddbltfx.dwFillColor = _surface[surfType].dwColorKey;
	//if(_surface[surfType].lpsurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&ddbltfx)!=DD_OK)
	//{
	//	return DMERR_SYSTEM;
	//}


	/*
	the following codes are using Window GDI to fill a rectangle with colorkey, it may not be hardware accelerated,
	however, it can always work right.
	I will temporarily use this code before I know what is wrong for above codes.
	*/

	HDC dc;
	GetSurfaceDC(&dc,surfType);
	HBRUSH brsh=CreateSolidBrush(_surface[surfType].rgbColorKey);
	FillRect(dc,&_PPIRect,brsh);
	ReleaseSurfaceDC(dc,surfType);
	DeleteObject(brsh);

	return DM_OK;
}

LPDIRECTDRAWSURFACE7 CDrawingManager::getPSurface(e_surface_type surfType)
{
	return _surface[surfType].lpsurface;
}

int CDrawingManager::getSurfaceInfo(e_surface_type surfType,void ** lplinear,int* pitch,int* rgbBitCount)
{
	if(_surface[surfType].lpsurface==NULL)return DMERR_NOSUCHSURFACE;

	*lplinear=_surfDesc[surfType].lpSurface;
	*pitch=_surfDesc[surfType].lPitch;

	if(_surfDesc[surfType].ddpfPixelFormat.dwFlags & DDPF_RGB)
		*rgbBitCount=_surfDesc[surfType].ddpfPixelFormat.dwRGBBitCount;
	else
		*rgbBitCount=0;

	return DM_OK;
}
int CDrawingManager::getSurfaceInfoByLock(e_surface_type surfType,void ** lplinear,int* pitch,int* rgbBitCount)
{
	int ret;
	ret=LockSurface(surfType);
	if(ret!=DM_OK)
	{
		*lplinear=NULL;
		*pitch=0;
		*rgbBitCount=0;
		return ret;
	}
	else
		return getSurfaceInfo(surfType,lplinear,pitch,rgbBitCount);
}

DWORD CDrawingManager::DDColorMatch(LPDIRECTDRAWSURFACE7  pdds, COLORREF rgb)
{
	COLORREF                rgbT;
	HDC                     hdc;
	DWORD                   dw = CLR_INVALID;
	DDSURFACEDESC2          ddsd;
	HRESULT                 hres;

	//
	//  Use GDI SetPixel to color match for us
	//
	if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
	{
		rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
		SetPixel(hdc, 0, 0, rgb);       // Set our value
		pdds->ReleaseDC(hdc);
	}
	//
	// Now lock the surface so we can read back the converted color
	//
	ddsd.dwSize = sizeof(ddsd);
	while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
		;
	if (hres == DD_OK)
	{
		dw = *(DWORD *) ddsd.lpSurface;                 // Get DWORD
		if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
			dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;  // Mask it to bpp
		pdds->Unlock(NULL);
	}
	//
	//  Now put the color that was there back.
	//
	if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
	{
		SetPixel(hdc, 0, 0, rgbT);
		pdds->ReleaseDC(hdc);
	}
	return dw;
}


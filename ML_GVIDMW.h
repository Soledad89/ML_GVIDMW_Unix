
/****************************************************
中间件名称：综合图形图像显示中间件（网络版）
版本：3.1（对外版本号1.0）
出品单位：清华大学电子工程系，信号采集、处理与显控实验室
文件描述：中间件对外接口定义
****************************************************/


#ifndef __THU_6BE7D83F_50C1_4007_804D_525118CDCC34__
#define __THU_6BE7D83F_50C1_4007_804D_525118CDCC34__


#ifdef _WINDLL
	#define GVIDMW_API  extern "C" __declspec(dllexport) 
#else 
	#define GVIDMW_API  extern "C" __declspec(dllimport) 
#endif

typedef int Status;


//======标准中规定的对外接口======
// VideoChannel 取值1、2
// theColor的格式为：由高到低，R、G、B、保留  ( 0xRRGGBB00 )

//------init------
GVIDMW_API Status ML_GVID_Init();
//初始化函数返回值错误信息：
//  -1  --  重复调用初始化函数
//  -2  --  日志文件建立出错，可能没有save文件夹
//  -3  --  配置文件出错
//  -4  --  图形系统创建出错，可能没启动DirectDraw加速
//  -5  --  系统数据出错，检查dat文件夹下文件
//  -6  --  雷达信息源设为local方式，save文件夹下缺少名为radar1和radar2的视频文件

//------PPI------
GVIDMW_API Status ML_GVID_OpenPPI(char VideoChannel,short theX, short theY , short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_ClosePPI(char VideoChannel);
GVIDMW_API Status ML_GVID_SetPPIXY(char VideoChannel ,short theX ,short theY);
GVIDMW_API Status ML_GVID_SetPPIWH(char VideoChannel ,short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_SetPPIMode(char VideoChannel, short theMode);
GVIDMW_API Status ML_GVID_SetPPICenter(char VideoChannel ,short theX ,short theY);
GVIDMW_API Status ML_GVID_SetPPIShipDir(char VideoChannel,float theShipDir);
GVIDMW_API Status ML_GVID_SetPPIColor(char VideoChannel,unsigned int theColor);
GVIDMW_API Status ML_GVID_SetPPIScan(char VideoChannel,int theScan);
GVIDMW_API Status ML_GVID_SetPPILineColor(char VideoChannel,unsigned int theColor);
GVIDMW_API Status ML_GVID_FrostPPI(char VideoChannel,char theFrost);
GVIDMW_API Status ML_GVID_SetPPIDist(char VideoChannel,short theDist);
GVIDMW_API Status ML_GVID_SetPPISwitch(char VideoChannel,short theSwitch);
GVIDMW_API Status ML_GVID_SetPPIGate(char VideoChannel,char theGate);
GVIDMW_API Status ML_GVID_SetPPITail(char VideoChannel,int theTail);

//------AR------
GVIDMW_API Status ML_GVID_OpenAr(char VideoChannel,short theX, short theY , short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_CloseAr(char VideoChannel);
GVIDMW_API Status ML_GVID_SetArXY(char VideoChannel ,short theX ,short theY);
GVIDMW_API Status ML_GVID_SetArWH(char VideoChannel ,short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_SetArGate (char VideoChannel ,char theGate);
GVIDMW_API Status ML_GVID_SetArDist (char VideoChannel ,short theDist);

//------MiniWin------
GVIDMW_API Status ML_GVID_OpenMiniWin(char VideoChannel,short theX, short theY , short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_CloseMiniWin(char VideoChannel);
GVIDMW_API Status ML_GVID_SetMiniWinXY(char VideoChannel ,short theX ,short theY);
GVIDMW_API Status ML_GVID_SetMiniWinWH(char VideoChannel ,short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_SetMiniWinMode(char VideoChannel, short theMode);
GVIDMW_API Status ML_GVID_SetMiniWinColor(char VideoChannel,unsigned int theColor);
GVIDMW_API Status ML_GVID_SetMiniWinDistDir(char VideoChannel ,short theDistance ,float theDirection);
GVIDMW_API Status ML_GVID_SetMiniWinScale(char VideoChannel,int theScale);

//------TV------
GVIDMW_API Status ML_GVID_OpenTV (char VideoChannel ,short theX, short theY,short theWidth,short theHeight);
GVIDMW_API Status ML_GVID_CloseTV (char VideoChannel );
GVIDMW_API Status ML_GVID_SetTVXY(char VideoChannel ,short theX, short theY);
GVIDMW_API Status ML_GVID_SetTVWH(char VideoChannel ,short theWidth, short theHeight);
GVIDMW_API Status ML_GVID_FrostTV(char VideoChannel,char theFrost);
GVIDMW_API Status ML_GVID_SetTVBright(char VideoChannel,unsigned char theBright);
GVIDMW_API Status ML_GVID_SetTVContrast(char VideoChannel,unsigned char theContrast);
GVIDMW_API Status ML_GVID_SetTVColor(char VideoChannel,unsigned char theColor);


//========== 标准外的扩展接口==========
// VideoChannel 取值1、2

// 设置PPI和AR的标绘层是否显示，disp：true显示；false不显示
GVIDMW_API int tshSetArMarkDisplay(char VideoChannel,bool disp);
GVIDMW_API int tshSetRadarMarkDisplay(char VideoChannel,bool disp);

// ========== 绘图扩展接口 ==========

 // 绘图层类型定义(surfType)
#define SURF_PPI_BOTTOM 0
#define SURF_PPI_TOP 1
#define SURF_AR_TOP 2
#define SURF_MINI_TOP 3

GVIDMW_API int tshGetSurfaceDC(HDC* dc, char VideoChannel,int surfType);
GVIDMW_API int tshReleaseSurfaceDC(HDC dc, char VideoChannel,int surfType);

// !! 注意，colorKey的类型是COLORREF，不同于标准接口中的颜色格式，
// 使用宏RGB(r,g,b)生成，格式为：由高到低，保留、B、G、R   ( 0x00BBGGRR )
GVIDMW_API int tshSetSurfaceColorKey(char VideoChannel,int surfType,COLORREF colorkey);
GVIDMW_API int tshSetSurfaceVisible(char VideoChannel,int surfType,BOOL visible);
GVIDMW_API int tshEraseSurface(char VideoChannel,int surfType);

// ========== 电视相关扩展接口 ==========

// 设置OSD层显示字符或时间
// XY坐标为字符串“左上角”相对视频“左上角”的坐标,字符最大长度32
GVIDMW_API int tshSetOSDStr(char VideoChannel,char* pstr,short x,short y);
GVIDMW_API int tshSetOSDTime(char VideoChannel,short x,short y);

// 发送透明数据至设备端串口，pData 数据; nLen 数据长度; nChnNum ---- 0为485接口,1为232接口
GVIDMW_API int tshSendTransparentData(char VideoChannel,char* pData,int nLen,int nChnNum);
// 校时,将系统时间发送到设备端
GVIDMW_API int tshVerifyTime(char VideoChannel);


// ========== 记录回放接口 ==========
//typedef UINT (__stdcall *FN_RADAR_REC_CALLBACK)(LPBYTE pData,DWORD pkgLen);
//typedef UINT (__stdcall *FN_RADAR_PLY_CALLBACK)(LPBYTE pBuff,DWORD bufLen,DWORD PkgNum);
//
//GVIDMW_API int tshRadarStartRecord( char VideoChannel );
//GVIDMW_API int tshRadarStopRecord( char VideoChannel );
//GVIDMW_API int tshRadarSetRecordCallback( char VideoChannel,  FN_RADAR_REC_CALLBACK lpFun);
//
//GVIDMW_API int tshRadarStartReplay( char VideoChannel, int timeStep );
//GVIDMW_API int tshRadarStopReplay( char VideoChannel );
//GVIDMW_API int tshRadarSetReplayCallback( char VideoChannel,  FN_RADAR_PLY_CALLBACK lpFun);
//
//
//GVIDMW_API int tshTVStartRecord( char VideoChannel ,char * filename);
//GVIDMW_API int tshTVStopRecord( char VideoChannel );
//
//GVIDMW_API int tshTVStartReplay( char VideoChannel,char * filename, int timeStep );
//GVIDMW_API int tshTVStopReplay( char VideoChannel );

// !!!!!!   **** 自用扩展接口，最终版不提供 ****

GVIDMW_API Status tshPluginInterface1(void *param1,void *param2,void *param3,void *param4);
GVIDMW_API Status tshPluginInterface2(void *param1,void *param2,void *param3,void *param4);
GVIDMW_API Status tshPluginInterface3(void *param1,void *param2,void *param3,void *param4);
GVIDMW_API Status tshPluginInterface4(void *param1,void *param2,void *param3,void *param4);


#endif


//******************************
//
//  author : lishidan
//
//  create time: 2007-10-10
//
//  last modify time:2007-10-16
//
//  Version: 1.1
//
//******************************

#ifndef __TRACE_H__
#define __TRACE_H__

#include <deque>
#include <math.h>
#include "datapool.h"

#define m_PI  3.14159265/180
#define SPEED_DIF 0.8
#define COURSE_DIF 3
#define SPEED_DIF2 0.5
#define COURSE_DIF2 2


typedef struct s_tracedot
{
int x; //航迹点x坐标
int y; //航迹点y坐标
float azi; //航迹点真方位
float dis; //航迹点真距离
}s_tracedot;

typedef std::deque<s_tracedot> DeqTrace;
typedef DeqTrace::iterator DeqTraceIte;

class CTrace
{
public:
	CTrace();
	~CTrace();


public:

	int batchNum;// 目标批号
	int attr;// 目标属性：敌1、我2、友3、中立4、不明5，五类
	int type;// 目标类型：空中1、水面2、水下3，三类

	
	bool updated;//表明该航迹在某段时间内是否更新过
	bool isInfoDisp;//该航迹的信息框是否为显示状态

	float moveAzi;//目标当前运动真方位
	float speed;//目标当前运动速度

	int newx;
	int newy;

	DeqTrace trace;//存储目标航迹的双端队列，Front 方向对应最新点迹

	float TargetCPA; //目标的CPA
	float TargetTCPA; //目标TCPA

public:
	void pushFront(int x, int y, float azi, float dis);
	
	void getFrontPos(int &x, int &y);
		
	void getBackPos(int &x, int &y);

};


#endif



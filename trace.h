

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
int x; //������x����
int y; //������y����
float azi; //�������淽λ
float dis; //�����������
}s_tracedot;

typedef std::deque<s_tracedot> DeqTrace;
typedef DeqTrace::iterator DeqTraceIte;

class CTrace
{
public:
	CTrace();
	~CTrace();


public:

	int batchNum;// Ŀ������
	int attr;// Ŀ�����ԣ���1����2����3������4������5������
	int type;// Ŀ�����ͣ�����1��ˮ��2��ˮ��3������

	
	bool updated;//�����ú�����ĳ��ʱ�����Ƿ���¹�
	bool isInfoDisp;//�ú�������Ϣ���Ƿ�Ϊ��ʾ״̬

	float moveAzi;//Ŀ�굱ǰ�˶��淽λ
	float speed;//Ŀ�굱ǰ�˶��ٶ�

	int newx;
	int newy;

	DeqTrace trace;//�洢Ŀ�꺽����˫�˶��У�Front �����Ӧ���µ㼣

	float TargetCPA; //Ŀ���CPA
	float TargetTCPA; //Ŀ��TCPA

public:
	void pushFront(int x, int y, float azi, float dis);
	
	void getFrontPos(int &x, int &y);
		
	void getBackPos(int &x, int &y);

};


#endif



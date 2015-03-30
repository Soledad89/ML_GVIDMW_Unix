//******************************
//
//  author : lishidan
//
//  create time: 2007-10-10
//
//  last modify time:2007-10-10
//
//  Version: 0.1
//
//******************************

#include "StdAfx.h"

#include "trace.h"


CTrace::CTrace()
{
	s_tracedot tmp={0,0,0,0};

    trace.resize(10,tmp);
    trace.clear();

    updated=false;
	isInfoDisp=false;
}

CTrace::~CTrace()
{

}

void CTrace::pushFront(int x, int y, float azi, float dis)
{
	s_tracedot tmp;
	tmp.x=x;tmp.y=y;
	tmp.azi=azi;tmp.dis=dis;
	trace.push_front(tmp);
}

void CTrace::getFrontPos(int &x, int &y)
{
x=trace.front().x;
y=trace.front().y;

}

void CTrace::getBackPos(int &x, int &y)
{
x=trace.back().x;
y=trace.back().y;

}


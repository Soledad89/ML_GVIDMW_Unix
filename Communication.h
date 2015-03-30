// Communication.h: interface for the CCommunication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMUNICATION_H__BBF3FC8F_5DD1_4888_9544_8E802062D831__INCLUDED_)
#define AFX_COMMUNICATION_H__BBF3FC8F_5DD1_4888_9544_8E802062D831__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//数据报头结构
#include <WINSOCK2.H>

#pragma pack (push,4)

typedef struct  
{
	unsigned short TotalLength;	// 报文长度	2字节	本次数据报文的全部长度
	unsigned short Reserved;	// 备用	2字节	
	long SourceIP;				// 报文源地址	4字节	发送方计算机IP地址
	long DestinationIP;			// 报文目地地址	4字节	接收方计算机IP地址
	unsigned char cSN;			// 序列号	1字节	协议序列号，顺序加1
	unsigned char cACKNo;		// 确认号	1字节	协议确认号，默认为0
	unsigned char cFlag;		// 报文标志	1字节	默认为0
	unsigned char cSumofUnits;	// 信息单元个数	1字节	同类信息单元的个数，默认为1
	
}CSMXP_GRAMHEAD;

//数据报单元头结构
typedef struct  
{
	unsigned char cUnitSN;		// 信息单元序号	1字节	信息单元在应用层报文中的序号，默认为1
	unsigned char cUnitID;		// 信息单元标识	1字节	报文标识，由本测试评估系统定义
	unsigned short UnitLength;	// 信息单元长度	2字节	有效信息单元的实际长度
	unsigned long TimeStamp;	// 时戳	4字节	报文发送时刻
}GRAM_UNITHEAD;

typedef struct  
{
	unsigned short	 pkgNum;//包序号,表示数据包之间的前后关系，取值范围 0~65535
	unsigned short	 azi;//方位,相对正北的方位值，取值范围0~4095，对应0~360度
	char Video[];//视频,合并后用于显示的雷达视频，数据量根据网络速度设为固定值
};
#pragma pack (pop)


/************************************************************************/
/* CCommunication可以做为服务器接收以太网上的广播数据，同时也可以做为客户端发送点到点的数据或是广播数据*/
/* 做服务器要调用CreateServer，退出时调CloseServer关闭
/*         服务器端，一般情况下重写OnReceive函数即可处理到到达的数据实体。如果一包数据中有多个数据单元，此函数会被调用多次，每次处理一个单元的数据*/
/*					IsValidate用来判定所到的报文要不要处理，如果要处理，交给OnReceive函数，否则返回
/* 客户端 有两种：1是每次都要指定地址的SendData，每次可以灵活向不同的地址发送
/*                2是要调用CreateClient，先指定地址，再调用没有地地址参数的SendData，向固定地址发送有较高的效率。closeclient 关闭  
/*  广播数据：调用CreateBroadcastlient创建广播，调用 BroadcastData广播数据 closeclient 关闭                                               */
/************************************************************************/
class CCommunication  
{
public:
	BOOL SetServerThreadPriority(int nPriority);
	int BroadcastData(unsigned char id, short len, char *pSendData);
	int CloseBroadcastClient();
#define PORTNUM 8000
#define GRAM_LENTH 0x10000
	int CreateBroadCastClient( const char *RecGropIp="192.168.0.255", unsigned short nPort=PORTNUM);
	int SendData(unsigned char id,short len,char *SendData);
	int CloseClient();
	int CreateClient(const char* SendIp,const char* RecIp,unsigned short nPort = PORTNUM);
	int CloseServer();
	int SendData(unsigned char id,short len,const char* SendIp,const char* RecIp,char *SendData,unsigned short nPort = PORTNUM);
	int CreateServer(unsigned short nPort = PORTNUM);
	
	CCommunication();
	virtual ~CCommunication();
protected:
	virtual int GramProHandle(int nGramlen,char* gram,struct sockaddr_in* pAddr);
	virtual BOOL OnRcvError(int lError);
	static DWORD WINAPI Recvthread(LPVOID lpparam);
	virtual void OnReceive(unsigned char id,short len,char* pData,int pos);// pos=0 is begin;pos=-i is end;other positive is position
	virtual BOOL IsValidate(unsigned char  Id,long SourceIP);
	
	//接收广播数据报文的
	SOCKET m_hSocket;//
	SOCKADDR_IN m_addr;
	HANDLE m_hsrvThread;

	//为对点发送的
	SOCKET m_hSocketSend;
	SOCKADDR_IN m_addrSend;
	char *m_buffSend;

	//为广播发送的
	SOCKET m_hSocketBroadcast;
	SOCKADDR_IN m_addrLocalBroadcast;
	SOCKADDR_IN m_addrBroadcast;
	char *m_buffSendBroadcast;

	static int InitFlag;//Socket启动标识，启动一个加一，结束一个减一

};


#endif // !defined(AFX_COMMUNICATION_H__BBF3FC8F_5DD1_4888_9544_8E802062D831__INCLUDED_)

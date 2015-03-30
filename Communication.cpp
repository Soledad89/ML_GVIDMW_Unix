// Communication.cpp: implementation of the CCommunication class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "testCom.h"
#include "Communication.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif


#pragma comment( lib, "WS2_32.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int CCommunication::InitFlag = 0;
// INTERFACECFG CCommunication::interfaceConfig[]={
// 	{
// 		1,1,1
// 	},
// 	{
// 			2,sizeof(double),2
// 		}
// };
CCommunication::CCommunication()
{
	if (InitFlag==0)
	{
		WSADATA wsaData;
		WORD version = MAKEWORD(2, 0);
		int ret = WSAStartup(version, &wsaData);//用于加载WinSock Dll动态库
		if(ret != 0)
		{
//			TRACE("WSAStartup failed!\n");
			return;
		};
		InitFlag++;
	}
	m_buffSend=NULL;
	m_hSocket=NULL;
	m_hSocketSend = NULL;

	m_hSocketBroadcast = NULL;
	m_buffSendBroadcast = NULL;
		

}

CCommunication::~CCommunication()
{
	InitFlag--;
	if (InitFlag==0)
	{
		if (WSACleanup() != 0)//用于卸载WinSock Dll动态库
		{
//			TRACE("WSACleanup failed!\n");
			InitFlag++;
		}
	}
	closesocket(m_hSocket);

}
/************************************************************************/
/* 当数据由源SourceIP发来的编号为ID的数据到达后，是否要处理，SourceIP已经是网络字节顺序
/* 如要处理(返回TURE)，则数据到达后由OnReceive通知
/*此函数为虚函数，由使用方重写                                          */
/************************************************************************/
BOOL CCommunication::IsValidate(unsigned char Id,long SourceIP)
{
	
	return TRUE;
}
/************************************************************************/
/* OnReceive当期望的数据体(编号为ID，长度为len)到达后的发生，pData为发来的数据的起始地址(不包含报文头)，可以拷贝直接使用
/*此函数为虚函数，由使用方重写                                          */
/************************************************************************/
void CCommunication::OnReceive(unsigned char id, short len,char* pData,int pos)
{

}

/************************************************************************/
/* Recvthread用来以太网报文有没有到达，到达交由GramProHandle来处理      */
/************************************************************************/
DWORD WINAPI CCommunication::Recvthread(LPVOID lpparam)
{
	CCommunication* pCom = (CCommunication*)lpparam;
	SOCKET  m_socket=(SOCKET)pCom->m_hSocket;
	char buff[GRAM_LENTH];
	struct sockaddr_in sourceAddr;
    int gram_len;
	int len;
	memset(&sourceAddr,0,sizeof(sourceAddr));

	while(1)
	{
		len = sizeof(sourceAddr);
		gram_len = recvfrom(m_socket,buff,GRAM_LENTH,0,(struct sockaddr *)&sourceAddr,&len);

		if (gram_len<=0 || gram_len>GRAM_LENTH)//Error
			if (pCom->OnRcvError(WSAGetLastError()))
				continue;
 			else
 				break;
		if (pCom->GramProHandle(gram_len,buff,&sourceAddr)==0)
		{
			break;
		}

	}
	return 1;
}

/************************************************************************/
/* CreateServer   创建一个接收报文的线程，让它工作在THREAD_PRIORITY_TIME_CRITICAL优先级*/
/************************************************************************/
int CCommunication::CreateServer(unsigned short nPort)
{

	memset(&m_addr,0,sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(nPort);
	m_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	m_hSocket = socket(AF_INET,SOCK_DGRAM,0);
	if (m_hSocket<0)
	{
		return 0;
	}
	char tmp1=1,tmp2=1;
	if (setsockopt(m_hSocket,SOL_SOCKET,SO_REUSEADDR,&tmp1,1)==0
		&& setsockopt(m_hSocket,SOL_SOCKET,SO_REUSEADDR,&tmp2,1)==0
		&&bind(m_hSocket,(struct sockaddr *)&m_addr,sizeof(m_addr))==0)
	{
		unsigned long dwthread;
		m_hsrvThread = ::CreateThread(NULL,0x4000,Recvthread,(LPVOID)this,0 ,&dwthread);
		if (m_hsrvThread){
			::SetThreadPriority(m_hsrvThread,THREAD_PRIORITY_TIME_CRITICAL);
			return 1;
		}
	}
	OnRcvError(WSAGetLastError());



	closesocket(m_hSocket);
	m_hsrvThread = NULL;
	return 0;
}

int CCommunication::SendData(unsigned char id,short len,const char* SendIp,const char* RecIp,char *pSendData,unsigned short nPort)
{
	if(len>GRAM_LENTH)
		return 0;
	static unsigned char No=0;

	short nHeadLen = sizeof(CSMXP_GRAMHEAD) + sizeof(GRAM_UNITHEAD);

	char* buff = new char[len + nHeadLen];
	CSMXP_GRAMHEAD * pGramHead;
	GRAM_UNITHEAD * pGramUnitHead;
	pGramHead = (CSMXP_GRAMHEAD *)buff;
	pGramUnitHead = (GRAM_UNITHEAD *)(buff + sizeof(CSMXP_GRAMHEAD));
	
	pGramHead->TotalLength = /*htons*/(len+nHeadLen);
	pGramHead->Reserved = 0;
	pGramHead->SourceIP = inet_addr(SendIp);
	pGramHead->DestinationIP = inet_addr(RecIp);
	pGramHead->cSN = No++;
	pGramHead->cSN = 0;
	pGramHead->cSumofUnits = 1;

	pGramUnitHead->cUnitSN = 0;
	pGramUnitHead->cUnitID = id;
	pGramUnitHead->UnitLength = /*htons*/(len+sizeof(GRAM_UNITHEAD));
	SYSTEMTIME time;
	GetSystemTime(&time);
	pGramUnitHead->TimeStamp = /*htonl*/((time.wHour*3600 + time.wMinute*60 + time.wSecond)*1000 + time.wMilliseconds );
//	infohead.time=ntohl((t.GetHour()*3600+t.GetMinute()*60+t.GetSecond())*10000);


	SOCKET hSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	struct sockaddr_in sndAddr;
	sndAddr.sin_family = AF_INET;
	sndAddr.sin_port = htons(nPort);
	sndAddr.sin_addr.S_un.S_addr = inet_addr(RecIp);

	memcpy(buff+nHeadLen,pSendData,len);

	sendto(hSocket,buff,len+nHeadLen,0,(struct sockaddr *)&sndAddr,sizeof(sndAddr));

	delete[] buff;
	return 1;
}
/************************************************************************/
/* 接收错误处理，用户重写                                               */
/* lError   返回错误号，参见MSDN的recvfrom的error code                  */
/* 返回：TRUE将继续接收报文                                             */
/*		 FALSE将终止接收报文                                            */
/************************************************************************/
BOOL CCommunication::OnRcvError(int lError)
{
	return FALSE;
}
/************************************************************************/
/* 对收到的报文进行处理                                             */
/* 继承类可以重写此函数，以处理不同的报文头                             */
/* 返回0将终止报文接收						                            */
/************************************************************************/
int CCommunication::GramProHandle(int nGramlen,char *gram,struct sockaddr_in* pAddr)
{
	CSMXP_GRAMHEAD * pGramHead;
	GRAM_UNITHEAD * pGramUnitHead;
	pGramHead = (CSMXP_GRAMHEAD *)gram;
	pGramUnitHead = (GRAM_UNITHEAD *)(gram + sizeof(CSMXP_GRAMHEAD));
	int nDataOffset = sizeof(CSMXP_GRAMHEAD) + sizeof(GRAM_UNITHEAD);

	int pos;


	for (unsigned char i = 0;i<pGramHead->cSumofUnits;i++)
	{
		if (IsValidate(pGramUnitHead->cUnitID,/*ntohl*/(pGramHead->SourceIP) ))
		{
			//传递去掉数据单元头的数据体指针
			if(i==pGramHead->cSumofUnits-1)
				pos=-i;
			else
				pos = i;
			OnReceive(pGramUnitHead->cUnitID, (pGramUnitHead->UnitLength)-sizeof(GRAM_UNITHEAD),(char*)&pGramUnitHead[1],pos);
		}
		pGramUnitHead = (GRAM_UNITHEAD *)((char*)pGramUnitHead + pGramUnitHead->UnitLength);
	}

	return 1;

}

int CCommunication::CloseServer()
{
		if(m_hSocket==NULL)
			return 1;
		int retval = closesocket(m_hSocket);//关闭socket可以令接收线程结束
		m_hSocket = NULL;
		return retval;
}


int CCommunication::CreateClient(const char *SendIp, const char *RecIp, unsigned short nPort)
{
	m_buffSend = new char[GRAM_LENTH];
	if (!m_buffSend)
	{
		return 0;//内存不足
	}
	m_hSocketSend = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	m_addrSend.sin_family = AF_INET;
	m_addrSend.sin_port = htons(nPort);
	m_addrSend.sin_addr.S_un.S_addr = inet_addr(RecIp);

	//对报文头不变部分进行填充
	short nHeadLen = sizeof(CSMXP_GRAMHEAD) + sizeof(GRAM_UNITHEAD);

	CSMXP_GRAMHEAD * pGramHead;
	GRAM_UNITHEAD * pGramUnitHead;
	pGramHead = (CSMXP_GRAMHEAD *)m_buffSend;
	pGramUnitHead = (GRAM_UNITHEAD *)(m_buffSend + sizeof(CSMXP_GRAMHEAD));
	
//	pGramHead->TotalLength = /*htons*/(nHeadLen);
	pGramHead->Reserved = 0;
	pGramHead->SourceIP = inet_addr(SendIp);
	pGramHead->DestinationIP = inet_addr(RecIp);
//	pGramHead->cSN = 0;
	pGramHead->cSN = 0;
	pGramHead->cSumofUnits = 1;

	pGramUnitHead->cUnitSN = 0;
//	pGramUnitHead->cUnitID = 0;
//	pGramUnitHead->UnitLength = /*htons*/(sizeof(GRAM_UNITHEAD));
// 	SYSTEMTIME time;
// 	GetSystemTime(&time);
// 	pGramUnitHead->TimeStamp = /*htonl*/((time.wHour*3600 + time.wMinute*60 + time.wSecond)*1000 + time.wMilliseconds );
//	infohead.time=ntohl((t.GetHour()*3600+t.GetMinute()*60+t.GetSecond())*10000);

	return 1;
}

int CCommunication::CloseClient()
{
	if (m_buffSend)
	{
		delete[] m_buffSend;
	}
	return closesocket(m_hSocketSend);//关闭socket
}

int CCommunication::SendData(unsigned char id, short len, char *pSendData)
{
	short nHeadLen = sizeof(CSMXP_GRAMHEAD) + sizeof(GRAM_UNITHEAD);
	if(len>GRAM_LENTH-nHeadLen)
		return 0;
	static unsigned char No=0;


	CSMXP_GRAMHEAD * pGramHead;
	GRAM_UNITHEAD * pGramUnitHead;
	pGramHead = (CSMXP_GRAMHEAD *)m_buffSend;
	pGramUnitHead = (GRAM_UNITHEAD *)(m_buffSend + sizeof(CSMXP_GRAMHEAD));
	
	pGramHead->TotalLength = /*htons*/(len+nHeadLen);
// 	pGramHead->Reserved = 0;
// 	pGramHead->SourceIP = inet_addr(SendIp);
// 	pGramHead->DestinationIP = inet_addr(RecIp);
	pGramHead->cSN = No++;
// 	pGramHead->cSN = 0;
// 	pGramHead->cSumofUnits = 1;

	pGramUnitHead->cUnitSN = 0;
	pGramUnitHead->cUnitID = id;
	pGramUnitHead->UnitLength = /*htons*/(len+sizeof(GRAM_UNITHEAD));
	SYSTEMTIME time;
	GetLocalTime(&time);
	pGramUnitHead->TimeStamp = /*htonl*/((time.wHour*3600 + time.wMinute*60 + time.wSecond)*1000 + time.wMilliseconds );



	memcpy(m_buffSend+nHeadLen,pSendData,len);

	sendto(m_hSocketSend,m_buffSend,len+nHeadLen,0,(struct sockaddr *)&m_addrSend,sizeof(m_addrSend));

	return 1;
}

int CCommunication::CreateBroadCastClient(const char *RecGropIp, unsigned short nPort)
{
	m_buffSendBroadcast = new char[GRAM_LENTH];
	if (!m_buffSendBroadcast)
	{
		return 0;//内存不足
	}
	//对报文头不变部分进行填充
	short nHeadLen = sizeof(CSMXP_GRAMHEAD) + sizeof(GRAM_UNITHEAD);

	CSMXP_GRAMHEAD * pGramHead;
	GRAM_UNITHEAD * pGramUnitHead;
	pGramHead = (CSMXP_GRAMHEAD *)m_buffSendBroadcast;
	pGramUnitHead = (GRAM_UNITHEAD *)(m_buffSendBroadcast + sizeof(CSMXP_GRAMHEAD));
	
//	pGramHead->TotalLength = /*htons*/(nHeadLen);
	pGramHead->Reserved = 0;
	pGramHead->SourceIP = (INADDR_ANY);
	pGramHead->DestinationIP = inet_addr(RecGropIp);
//	pGramHead->cSN = 0;
	pGramHead->cSN = 0;
	pGramHead->cSumofUnits = 1;

	pGramUnitHead->cUnitSN = 0;
//	pGramUnitHead->cUnitID = 0;
//	pGramUnitHead->UnitLength = /*htons*/(sizeof(GRAM_UNITHEAD));
// 	SYSTEMTIME time;
// 	GetLocalTime(&time);
// 	pGramUnitHead->TimeStamp = /*htonl*/((time.wHour*3600 + time.wMinute*60 + time.wSecond)*1000 + time.wMilliseconds );
	

	//设置广播IP
	memset(&m_addrBroadcast,0,sizeof(m_addrBroadcast));
	m_addrBroadcast.sin_family = AF_INET;
	m_addrBroadcast.sin_port = htons(nPort);
	m_addrBroadcast.sin_addr.s_addr = inet_addr(RecGropIp);

// 		//生成m_hSocketBroadcast
	m_hSocketBroadcast = socket(AF_INET,SOCK_DGRAM,0);
	if (m_hSocketBroadcast<0)
	{
		return 0;
	}
	//设置本机端口
	memset(&m_addrLocalBroadcast,0,sizeof(m_addrLocalBroadcast));
	m_addrLocalBroadcast.sin_family = AF_INET;
	m_addrLocalBroadcast.sin_port = htons(nPort);
	m_addrLocalBroadcast.sin_addr.s_addr = INADDR_ANY;
	//绑定socket和本机端口
	char tmp1=1,tmp2=1;
	if (setsockopt(m_hSocketBroadcast,SOL_SOCKET,SO_REUSEADDR,&tmp1,1)==0
		&& setsockopt(m_hSocketBroadcast,SOL_SOCKET,SO_BROADCAST,&tmp2,1)==0
		&&bind(m_hSocketBroadcast,(struct sockaddr *)&m_addrLocalBroadcast,sizeof(m_addrLocalBroadcast))==0)
	{
		return 1;
	}
		OnRcvError(WSAGetLastError());
		closesocket(m_hSocketBroadcast);
		delete m_buffSendBroadcast;
		m_buffSendBroadcast = NULL;
		return 0;


}

int CCommunication::CloseBroadcastClient()
{

	if (m_hSocketBroadcast == NULL)
		return 1;
	int retval = closesocket(m_hSocketBroadcast);


	m_hSocketBroadcast = NULL;
	if (m_buffSendBroadcast)
	{
		delete m_buffSendBroadcast;
		m_buffSendBroadcast = NULL;
	}
	return retval;
}

int CCommunication::BroadcastData(unsigned char id, short len, char *pSendData)
{
	short nHeadLen = sizeof(CSMXP_GRAMHEAD) + sizeof(GRAM_UNITHEAD);
	if(len>GRAM_LENTH-nHeadLen)
		return 0;
	static unsigned char No=0;


	CSMXP_GRAMHEAD * pGramHead;
	GRAM_UNITHEAD * pGramUnitHead;
	pGramHead = (CSMXP_GRAMHEAD *)m_buffSendBroadcast;
	pGramUnitHead = (GRAM_UNITHEAD *)(m_buffSendBroadcast + sizeof(CSMXP_GRAMHEAD));
	
	pGramHead->TotalLength = /*htons*/(len+nHeadLen);
// 	pGramHead->Reserved = 0;
// 	pGramHead->SourceIP = inet_addr(SendIp);
// 	pGramHead->DestinationIP = inet_addr(RecIp);
	pGramHead->cSN = No++;
// 	pGramHead->cSN = 0;
// 	pGramHead->cSumofUnits = 1;

	pGramUnitHead->cUnitSN = 0;
	pGramUnitHead->cUnitID = id;
	pGramUnitHead->UnitLength = /*htons*/(len+sizeof(GRAM_UNITHEAD));
	SYSTEMTIME time;
	GetSystemTime(&time);
	pGramUnitHead->TimeStamp = /*htonl*/((time.wHour*3600 + time.wMinute*60 + time.wSecond)*1000 + time.wMilliseconds );
//	pGramUnitHead->TimeStamp=ntohl((t.GetHour()*3600+t.GetMinute()*60+t.GetSecond())*10000);


	memcpy(m_buffSendBroadcast+nHeadLen,pSendData,len);

	sendto(m_hSocketBroadcast,m_buffSendBroadcast,len+nHeadLen,0,(struct sockaddr *)&m_addrBroadcast,sizeof(m_addrBroadcast));

	return 1;
}
/************************************************************************/
/* Priority Meaning 
THREAD_PRIORITY_ABOVE_NORMAL 1 Priority 1 point above the priority class. 
THREAD_PRIORITY_BELOW_NORMAL 1 Priority 1 point below the priority class. 
THREAD_PRIORITY_HIGHEST      2 Priority 2 points above the priority class. 
THREAD_PRIORITY_IDLE		-15 Base priority of 1 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority of 16 for REALTIME_PRIORITY_CLASS processes. 
THREAD_PRIORITY_LOWEST		-2 Priority 2 points below the priority class. 
THREAD_PRIORITY_NORMAL		0 Normal priority for the priority class. 
THREAD_PRIORITY_TIME_CRITICAL	15 Base priority of 15 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS 
                                                                     */
/************************************************************************/

BOOL CCommunication::SetServerThreadPriority(int nPriority)
{
	return SetThreadPriority(m_hsrvThread,nPriority);
}




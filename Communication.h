// Communication.h: interface for the CCommunication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMUNICATION_H__BBF3FC8F_5DD1_4888_9544_8E802062D831__INCLUDED_)
#define AFX_COMMUNICATION_H__BBF3FC8F_5DD1_4888_9544_8E802062D831__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//���ݱ�ͷ�ṹ
#include <WINSOCK2.H>

#pragma pack (push,4)

typedef struct  
{
	unsigned short TotalLength;	// ���ĳ���	2�ֽ�	�������ݱ��ĵ�ȫ������
	unsigned short Reserved;	// ����	2�ֽ�	
	long SourceIP;				// ����Դ��ַ	4�ֽ�	���ͷ������IP��ַ
	long DestinationIP;			// ����Ŀ�ص�ַ	4�ֽ�	���շ������IP��ַ
	unsigned char cSN;			// ���к�	1�ֽ�	Э�����кţ�˳���1
	unsigned char cACKNo;		// ȷ�Ϻ�	1�ֽ�	Э��ȷ�Ϻţ�Ĭ��Ϊ0
	unsigned char cFlag;		// ���ı�־	1�ֽ�	Ĭ��Ϊ0
	unsigned char cSumofUnits;	// ��Ϣ��Ԫ����	1�ֽ�	ͬ����Ϣ��Ԫ�ĸ�����Ĭ��Ϊ1
	
}CSMXP_GRAMHEAD;

//���ݱ���Ԫͷ�ṹ
typedef struct  
{
	unsigned char cUnitSN;		// ��Ϣ��Ԫ���	1�ֽ�	��Ϣ��Ԫ��Ӧ�ò㱨���е���ţ�Ĭ��Ϊ1
	unsigned char cUnitID;		// ��Ϣ��Ԫ��ʶ	1�ֽ�	���ı�ʶ���ɱ���������ϵͳ����
	unsigned short UnitLength;	// ��Ϣ��Ԫ����	2�ֽ�	��Ч��Ϣ��Ԫ��ʵ�ʳ���
	unsigned long TimeStamp;	// ʱ��	4�ֽ�	���ķ���ʱ��
}GRAM_UNITHEAD;

typedef struct  
{
	unsigned short	 pkgNum;//�����,��ʾ���ݰ�֮���ǰ���ϵ��ȡֵ��Χ 0~65535
	unsigned short	 azi;//��λ,��������ķ�λֵ��ȡֵ��Χ0~4095����Ӧ0~360��
	char Video[];//��Ƶ,�ϲ���������ʾ���״���Ƶ�����������������ٶ���Ϊ�̶�ֵ
};
#pragma pack (pop)


/************************************************************************/
/* CCommunication������Ϊ������������̫���ϵĹ㲥���ݣ�ͬʱҲ������Ϊ�ͻ��˷��͵㵽������ݻ��ǹ㲥����*/
/* ��������Ҫ����CreateServer���˳�ʱ��CloseServer�ر�
/*         �������ˣ�һ���������дOnReceive�������ɴ������������ʵ�塣���һ���������ж�����ݵ�Ԫ���˺����ᱻ���ö�Σ�ÿ�δ���һ����Ԫ������*/
/*					IsValidate�����ж������ı���Ҫ��Ҫ�������Ҫ��������OnReceive���������򷵻�
/* �ͻ��� �����֣�1��ÿ�ζ�Ҫָ����ַ��SendData��ÿ�ο��������ͬ�ĵ�ַ����
/*                2��Ҫ����CreateClient����ָ����ַ���ٵ���û�еص�ַ������SendData����̶���ַ�����нϸߵ�Ч�ʡ�closeclient �ر�  
/*  �㲥���ݣ�����CreateBroadcastlient�����㲥������ BroadcastData�㲥���� closeclient �ر�                                               */
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
	
	//���չ㲥���ݱ��ĵ�
	SOCKET m_hSocket;//
	SOCKADDR_IN m_addr;
	HANDLE m_hsrvThread;

	//Ϊ�Ե㷢�͵�
	SOCKET m_hSocketSend;
	SOCKADDR_IN m_addrSend;
	char *m_buffSend;

	//Ϊ�㲥���͵�
	SOCKET m_hSocketBroadcast;
	SOCKADDR_IN m_addrLocalBroadcast;
	SOCKADDR_IN m_addrBroadcast;
	char *m_buffSendBroadcast;

	static int InitFlag;//Socket������ʶ������һ����һ������һ����һ

};


#endif // !defined(AFX_COMMUNICATION_H__BBF3FC8F_5DD1_4888_9544_8E802062D831__INCLUDED_)

#include "winsock2.h"  
#include <iostream>
#include <thread>
#pragma comment(lib, "ws2_32.lib")  
using namespace std;

BOOL RecvLine(SOCKET s, char* buf); //��ȡһ������  
const int BUF_SIZE = 64;
WSADATA wsd; //WSADATA����  
SOCKET sHost; //�������׽���  
SOCKADDR_IN servAddr; //��������ַ  
char buf[BUF_SIZE]; //�������ݻ�����  
char bufRecv[BUF_SIZE];
int retVal; //����ֵ
volatile int connetFlag;//�ж�����״̬

//����DOS�ڴ���
#pragma comment(linker,"/subsystem:\"windows\"  /entry:\"mainCRTStartup\"" )
//����·�����̶�
#define MAX_PATH_NUM 4096
//�����ػ���������
#define PROCCESS_NAME "socket�����.exe"
//����д���ע���·��
#define SELFSTART_REGEDIT_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"
//���ñ�����������
BOOL SetSelfStart()
{
	//��ȡ������������
	char pName[MAX_PATH_NUM] = { 0 };
	GetModuleFileNameA(NULL, pName, MAX_PATH_NUM);

	//��ע�����д��������Ϣ
	HKEY hKey = NULL;
	LONG lRet = 0;
	lRet = RegOpenKeyExA(HKEY_CURRENT_USER, SELFSTART_REGEDIT_PATH, 0, KEY_ALL_ACCESS, &hKey);

	//�ж��Ƿ�ɹ�
	if (lRet != ERROR_SUCCESS)
	{
		return FALSE;
	}
	//testSocket��ʾ����������
	lRet = RegSetValueExA(hKey, "testSocket", 0, REG_SZ, (const unsigned char*)pName, strlen(pName) + sizeof(char));

	//�ж��Ƿ�ɹ�
	if (lRet != ERROR_SUCCESS)
	{
		return FALSE;
	}

	//�ر�ע���
	RegCloseKey(hKey);
	return TRUE;
}

void receive_info_thread()
{
	while (true)
	{
		ZeroMemory(bufRecv, BUF_SIZE);
		int ret = recv(sHost, bufRecv, BUF_SIZE, 0); // ���շ������˵����ݣ�retVal��ʾ����ֵ�����ַ�����
		if (0 == ret)
		{
			cout << "send failed!" << endl;
			closesocket(sHost); //�ر��׽���  
			WSACleanup(); //�ͷ��׽�����Դ
			connetFlag = 0;
			//break;
			return;
		}
		cout << endl << "�ӷ������������ݣ�" << bufRecv;
		cout << "\n";
	}
}

void send_info_thread()
{
	while (true)
	{
		//���������������  
		ZeroMemory(buf, BUF_SIZE);
		cout << " ���������������:  ";
		cin >> buf;
		int ret = send(sHost, buf, strlen(buf), 0);
		if (SOCKET_ERROR == ret)
		{
			cout << "send failed!" << endl;
			closesocket(sHost); //�ر��׽���  
			WSACleanup(); //�ͷ��׽�����Դ
			connetFlag = 0;
			//break;
			return ;
		}
	}
}

int main(int argc, char* argv[])
{
	//���ó��򿪻�������
	if (!SetSelfStart())
	{
		cout << "�ػ����̿���������ʧ��" << endl;
		return -1;
	}
lable1:
	//��ʼ���׽��ֶ�̬��  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	//�����׽���  
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost)
	{
		cout << "socket failed!" << endl;
		WSACleanup();//�ͷ��׽�����Դ  
		return  -1;
	}

	//���÷�������ַ  
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");   
	servAddr.sin_port = htons((short)3009);   //4999
	int nServAddlen = sizeof(servAddr);

	//���ӷ�����  
	do
	{
		retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
		cout << "connecting����" << endl;
		Sleep(3000);
	} while (SOCKET_ERROR == retVal);
	connetFlag = 1;
	if (SOCKET_ERROR == retVal)
	{
		cout << "connect failed!" << endl;
		closesocket(sHost); //�ر��׽���  
		WSACleanup(); //�ͷ��׽�����Դ  
		system("pause");
		return -1;
	}

	thread th1(receive_info_thread);
	th1.detach();

	thread th2(send_info_thread);
	th2.detach();

	while (true)
	{
		if (connetFlag == 0)
		{
			goto lable1;
		}
		//���������������  
		//ZeroMemory(buf, BUF_SIZE);
		//cout << " ���������������:  ";
		//cin >> buf;
		//retVal = send(sHost, buf, strlen(buf), 0);
		//if (SOCKET_ERROR == retVal)
		//{
		//	cout << "send failed!" << endl;
		//	closesocket(sHost); //�ر��׽���  
		//	WSACleanup(); //�ͷ��׽�����Դ  
		//	return -1;
		//}
		//RecvLine(sHost, bufRecv);  
		//ZeroMemory(bufRecv, BUF_SIZE);
		//recv(sHost, bufRecv, BUF_SIZE, 0); // ���շ������˵����ݣ� ֻ����5���ַ�  
		//cout << endl << "�ӷ������������ݣ�" << bufRecv;
		//cout << "\n";
		Sleep(10);
	}
	//�˳�  
	closesocket(sHost); //�ر��׽���  
	WSACleanup(); //�ͷ��׽�����Դ  
	return 0;
}
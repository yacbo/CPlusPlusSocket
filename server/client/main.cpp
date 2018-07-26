#include "winsock2.h"  
#include <iostream>
#include <thread>
#pragma comment(lib, "ws2_32.lib")  
using namespace std;

BOOL RecvLine(SOCKET s, char* buf); //读取一行数据  
const int BUF_SIZE = 64;
WSADATA wsd; //WSADATA变量  
SOCKET sHost; //服务器套接字  
SOCKADDR_IN servAddr; //服务器地址  
char buf[BUF_SIZE]; //接收数据缓冲区  
char bufRecv[BUF_SIZE];
int retVal; //返回值
volatile int connetFlag;//判断连接状态

//隐藏DOS黑窗口
#pragma comment(linker,"/subsystem:\"windows\"  /entry:\"mainCRTStartup\"" )
//定义路径最大程度
#define MAX_PATH_NUM 4096
//定义守护进程名称
#define PROCCESS_NAME "socket服务端.exe"
//定义写入的注册表路径
#define SELFSTART_REGEDIT_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"
//设置本身开机自启动
BOOL SetSelfStart()
{
	//获取程序完整名称
	char pName[MAX_PATH_NUM] = { 0 };
	GetModuleFileNameA(NULL, pName, MAX_PATH_NUM);

	//在注册表中写入启动信息
	HKEY hKey = NULL;
	LONG lRet = 0;
	lRet = RegOpenKeyExA(HKEY_CURRENT_USER, SELFSTART_REGEDIT_PATH, 0, KEY_ALL_ACCESS, &hKey);

	//判断是否成功
	if (lRet != ERROR_SUCCESS)
	{
		return FALSE;
	}
	//testSocket表示本程序名称
	lRet = RegSetValueExA(hKey, "testSocket", 0, REG_SZ, (const unsigned char*)pName, strlen(pName) + sizeof(char));

	//判断是否成功
	if (lRet != ERROR_SUCCESS)
	{
		return FALSE;
	}

	//关闭注册表
	RegCloseKey(hKey);
	return TRUE;
}

void receive_info_thread()
{
	while (true)
	{
		ZeroMemory(bufRecv, BUF_SIZE);
		int ret = recv(sHost, bufRecv, BUF_SIZE, 0); // 接收服务器端的数据，retVal表示还回值接受字符个数
		if (0 == ret)
		{
			cout << "send failed!" << endl;
			closesocket(sHost); //关闭套接字  
			WSACleanup(); //释放套接字资源
			connetFlag = 0;
			//break;
			return;
		}
		cout << endl << "从服务器接收数据：" << bufRecv;
		cout << "\n";
	}
}

void send_info_thread()
{
	while (true)
	{
		//向服务器发送数据  
		ZeroMemory(buf, BUF_SIZE);
		cout << " 向服务器发送数据:  ";
		cin >> buf;
		int ret = send(sHost, buf, strlen(buf), 0);
		if (SOCKET_ERROR == ret)
		{
			cout << "send failed!" << endl;
			closesocket(sHost); //关闭套接字  
			WSACleanup(); //释放套接字资源
			connetFlag = 0;
			//break;
			return ;
		}
	}
}

int main(int argc, char* argv[])
{
	//设置程序开机自启动
	if (!SetSelfStart())
	{
		cout << "守护进程开机自启动失败" << endl;
		return -1;
	}
lable1:
	//初始化套结字动态库  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	//创建套接字  
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost)
	{
		cout << "socket failed!" << endl;
		WSACleanup();//释放套接字资源  
		return  -1;
	}

	//设置服务器地址  
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");   
	servAddr.sin_port = htons((short)3009);   //4999
	int nServAddlen = sizeof(servAddr);

	//连接服务器  
	do
	{
		retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
		cout << "connecting……" << endl;
		Sleep(3000);
	} while (SOCKET_ERROR == retVal);
	connetFlag = 1;
	if (SOCKET_ERROR == retVal)
	{
		cout << "connect failed!" << endl;
		closesocket(sHost); //关闭套接字  
		WSACleanup(); //释放套接字资源  
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
		//向服务器发送数据  
		//ZeroMemory(buf, BUF_SIZE);
		//cout << " 向服务器发送数据:  ";
		//cin >> buf;
		//retVal = send(sHost, buf, strlen(buf), 0);
		//if (SOCKET_ERROR == retVal)
		//{
		//	cout << "send failed!" << endl;
		//	closesocket(sHost); //关闭套接字  
		//	WSACleanup(); //释放套接字资源  
		//	return -1;
		//}
		//RecvLine(sHost, bufRecv);  
		//ZeroMemory(bufRecv, BUF_SIZE);
		//recv(sHost, bufRecv, BUF_SIZE, 0); // 接收服务器端的数据， 只接收5个字符  
		//cout << endl << "从服务器接收数据：" << bufRecv;
		//cout << "\n";
		Sleep(10);
	}
	//退出  
	closesocket(sHost); //关闭套接字  
	WSACleanup(); //释放套接字资源  
	return 0;
}
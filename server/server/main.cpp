// Server.cpp : Defines the entry point for the console application.  
//  
#include "winsock2.h"   
#include <iostream>
#include <thread>
#include <string>
#pragma comment(lib, "ws2_32.lib") 
using namespace std;

const int BUF_SIZE = 64;
WSADATA         wsd;            //WSADATA变量  
SOCKET          sServer;        //服务器套接字  
SOCKET          sClient;        //客户端套接字  
SOCKADDR_IN     addrServ;;      //服务器地址  
char            buf[BUF_SIZE];  //接收数据缓冲区  
char            sendBuf[BUF_SIZE];//返回给客户端得数据  
int             retVal;         //返回值  

void receive_info_thread()
{
	while (true)
	{
		//接收客户端数据  
		ZeroMemory(buf, BUF_SIZE);
		int ret = recv(sClient, buf, BUF_SIZE, 0);
		if (0 == ret)
		{
			cout << "send failed!" << endl;
			closesocket(sServer); //关闭套接字  
			WSACleanup(); //释放套接字资源
			//break;
			return;
		}
		cout << endl << "从服务器接收数据：" << buf;
		cout << "\n";
	}
}

void send_info_thread()
{
	while (true)
	{
		cout << "向客户端发送数据: ";
		cin >> sendBuf;
		send(sClient, sendBuf, strlen(sendBuf), 0);
	}
}

string ip_to_str(ULONG ipInt)
{
	int ipStr[4];
	ipStr[0] = ipInt % 256;
	ipStr[3] = ipInt / 256 / 256/ 256;
	ipStr[2] = (ipInt - ipStr[3] * 256 * 256 * 256)/256/256;
	ipStr[1] = (ipInt - ipStr[3] * 256 * 256 * 256 - ipStr[2]*256*256) / 256;

	return to_string(ipStr[0]) + "." + to_string(ipStr[1]) + "." + to_string(ipStr[2]) + "." + to_string(ipStr[3]);
}



int main(int argc, char* argv[])
{
	//初始化套结字动态库  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return 1;
	}

	//创建套接字  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket failed!" << endl;
		WSACleanup();//释放套接字资源;  
		return  -1;
	}

	//服务器套接字地址   
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(4999);
	addrServ.sin_addr.s_addr = INADDR_ANY;
	//绑定套接字  
	retVal = ::bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);   //关闭套接字  
		WSACleanup();           //释放套接字资源;  
		return -1;
	}

	//开始监听   
	retVal = listen(sServer, 1);
	if (SOCKET_ERROR == retVal)
	{
		cout << "listen failed!" << endl;
		closesocket(sServer);   //关闭套接字  
		WSACleanup();           //释放套接字资源;  
		return -1;
	}

	//接受客户端请求  
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
	cout << "Client Address:" << ip_to_str(addrClient.sin_addr.S_un.S_addr) << endl;
	cout<< "Port:"<<addrClient.sin_port << endl;

	if (INVALID_SOCKET == sClient)
	{
		cout << "accept failed!" << endl;
		closesocket(sServer);   //关闭套接字  
		WSACleanup();           //释放套接字资源;  
		return -1;
	}

	thread th1(receive_info_thread);
	th1.detach();

	thread th2(send_info_thread);
	th2.detach();

	while (true)
	{
		////接收客户端数据  
		//ZeroMemory(buf, BUF_SIZE);
		//retVal = recv(sClient, buf, BUF_SIZE, 0);
		//if (SOCKET_ERROR == retVal)
		//{
		//	cout << "recv failed!" << endl;
		//	closesocket(sServer);   //关闭套接字  
		//	closesocket(sClient);   //关闭套接字       
		//	WSACleanup();           //释放套接字资源;  
		//	return -1;
		//}
		//if (buf[0] == '0')
		//	break;
		//cout << "客户端发送的数据: " << buf << endl;

		//cout << "向客户端发送数据: ";
		//cin >> sendBuf;

		//send(sClient, sendBuf, strlen(sendBuf), 0);
		Sleep(3000);
	}

	//退出  
	closesocket(sServer);   //关闭套接字  
	closesocket(sClient);   //关闭套接字  
	WSACleanup();           //释放套接字资源;  

	return 0;
}
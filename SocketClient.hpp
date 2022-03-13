#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS



#include <winsock2.h>
#include <windows.h>

#include <iostream>
#include <thread>
#include <string>

#include "RingbufferLock.hpp"

#pragma comment(lib, "wsock32.lib")


class SocketClient
{
public:
	SocketClient()
	{
		StartClient();
	}
	virtual ~SocketClient()
	{
		DistroyClient();
	}

	bool Init(const char* ip, const USHORT port)
	{
		bool bResult = false;
		do
		{
			WSADATA wsaData;
			int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0)
			{
				mLastError = "WSAStartup failed with error: "+ iResult;
				break;
			}
			mSocket = socket(PF_INET, SOCK_STREAM, 0);
			if (mSocket == INVALID_SOCKET)
			{
				mLastError = "socket failed with error: " +  WSAGetLastError();
				break;
			}
			memset(&mAddr, 0, sizeof(mAddr));
			mAddr.sin_family = AF_INET;
			mAddr.sin_addr.s_addr = inet_addr(ip);
			mAddr.sin_port = htons(port);
			 
			bResult = true;
		} while (false);

		return bResult;
	}

	bool Connect()
	{
		bool bResult = false;
		do
		{
			if (connect(mSocket, (SOCKADDR*)&mAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
			{
				closesocket(mSocket);
				mSocket = INVALID_SOCKET;
				mLastError = "Socket Connect fail";
				break;
			}
			mIsConnect = true;
			std::cout << "Socket Connect\r\n";
			OnConnect();
			bResult = true;
		} while (false);
		return bResult;
	}

	void StartClient()
	{
		mIsReceiveRun = true;
		mIsSendRun = true;

		mReceiveThread = std::thread([this]() {this->ReceiveThread();});
		mSendThread = std::thread([this]() {this->SendThread(); });
		std::cout << "Thread Start\r\n";
	}

	void Disconnect()
	{
		int iResult = shutdown(mSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) 
		{
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(mSocket);
			WSACleanup();
		}
		mIsConnect = false;
		//std::cout << "Socket DisConnect\r\n";
		OnDisConnect();
	}
	bool IsConnect()
	{
		return mIsConnect;
	}

	void DistroyClient()
	{
		closesocket(mSocket);
		WSACleanup();
		mIsConnect = false;
		mIsReceiveRun = false;
		mIsSendRun = false;

		if (mReceiveThread.joinable())
		{
			mReceiveThread.join();
		}
		if (mSendThread.joinable())
		{
			mSendThread.join();
		}
	}

	virtual void OnRecive(RingbufferLock* receive, size_t size) {};
	virtual void OnConnect() {};
	virtual void OnDisConnect() {};

	bool SendToData(const void* pData, size_t size)
	{
		bool bResult = false;
		if (IsConnect())
		{
			mSendBuffer.PutData(pData, size);
			bResult = true;
		}
		return bResult;
	}
	short GetPort()
	{
		sockaddr_in stAddr;
		int client_len = sizeof(stAddr);
		getsockname(mSocket, (struct sockaddr*)&stAddr, &client_len);
		return stAddr.sin_port;
	}
	IN_ADDR GetIP()
	{
		sockaddr_in stAddr;
		int client_len = sizeof(stAddr);
		getsockname(mSocket, (struct sockaddr*)&stAddr, &client_len);
		return stAddr.sin_addr;
	}

protected:

	void ReceiveThread()
	{
		char buf[4096] = { 0, };
		int receiveSize = 0;
		while (true)
		{
			if (mIsReceiveRun == false)
				break;

			if (mSocket == INVALID_SOCKET)
			{
				mLastError = "Socket is INVALID_SOCKET";
			}
			receiveSize = recv(mSocket, &buf[0], sizeof(buf), 0);
			if (receiveSize > 0)
			{
				//Receive Data
				bool bSuccess = false;
				bSuccess = mReceiveBuffer.PutData(buf, receiveSize);
				OnRecive(&mReceiveBuffer, receiveSize);
				if (bSuccess == false)
				{
					mLastError = "Socket buffer full";
				}
			}
			else if(receiveSize == 0)
			{
				//Connection closed;
				Disconnect();
			}
			else
			{
				mLastError = "recv failed with error:" + WSAGetLastError();
			}
		}
	}
	void SendThread()
	{
		const unsigned int bufferSize = 32768;
		char buf[32768] = { 0, };
		size_t sendSize = 0;
		size_t sendBufferSize = 0;

		while (true)
		{
			if (mIsSendRun == false)
				break;

			if (mSocket == INVALID_SOCKET)
			{
				mLastError = "INVALID_SOCKET";
			}
			if (!mSendBuffer.IsEmpty() && IsConnect())
			{
				if (mSendBuffer.GetSize() > bufferSize)
					sendBufferSize = bufferSize;
				else
					sendBufferSize = mSendBuffer.GetSize();

				mSendBuffer.GetData(buf, sendBufferSize);

				if (send(mSocket, buf, sendBufferSize, 0) == SOCKET_ERROR)
				{
					mLastError = "send failed with error : " + WSAGetLastError();
				}
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(16));
			}
		}
	}

	bool mIsConnect = false;
	SOCKET mSocket = INVALID_SOCKET;
	SOCKADDR_IN mAddr = {0,};

	std::thread mReceiveThread;
	std::thread mSendThread;

	bool mIsReceiveRun = true;
	bool mIsSendRun = true;


	RingbufferLock mReceiveBuffer;
	RingbufferLock mSendBuffer;

	std::string mLastError;
};

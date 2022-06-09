#pragma once
#include <thread>
#include <vector>
#include <chrono>
#include <string>
#include <queue>

#include "zlib.h"
#include "SocketClient.hpp"
#include "RingBufferMutexLock.hpp"
#include "PacketDefine.h"

#define SINGLE 1

constexpr uint16_t USER_BUFFER_SIZE = 1024;

class ChatClient : public SocketClient
{
public:
	void OnRecive(RingbufferMutexLock* receive, size_t size) override;
	void OnConnect() override;
	void OnDisConnect() override;

	void SetName(std::string name);
	void Start();
	void End();

private:
	void sendProc();
	void receiveProc();
	void mainProc();

	ALL_USER_CHAT_REQUEST mChattingPacket;
	char mSendCompressBuffer[USER_BUFFER_SIZE] = { '\0', };
	char mReceiveCompressBuffer[USER_BUFFER_SIZE] = { '\0', };


	std::thread mSendThread;
	bool mIsSendThreadRun = false;
	RingbufferMutexLock mSendQueue;
	byte mSendBuffer[MAX_BUFFER_SIZE] = {0,};

	std::thread mReceiveThread;
	bool mIsReceiveThreadRun = false;
	RingbufferMutexLock mReceiveQueue;
	byte mReceiveBuffer[MAX_BUFFER_SIZE] = { 0, };

	std::thread mMainThread;
	bool mIsMainThreadRun = false;
};


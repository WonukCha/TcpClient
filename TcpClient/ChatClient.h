#pragma once
#include <thread>
#include <vector>
#include <chrono>
#include <string>
#include <queue>


#include "SocketClient.hpp"
#include "RingBufferMutexLock.hpp"
#include "PacketDefine.h"

class ChatClient : public SocketClient
{
public:
	void OnRecive(RingbufferMutexLock* receive, size_t size) override;
	void OnConnect() override;
	void OnDisConnect() override;

	void SetPacket(std::string name);
	void Start();
	void End();

private:
	void sendProc();
	void receiveProc();
	void mainProc();

	ChattingPacket mChattingPacket;

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


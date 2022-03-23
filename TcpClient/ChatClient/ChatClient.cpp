#include "ChatClient.h"

void ChatClient::OnRecive(RingbufferMutexLock* receive, size_t size)
{
	receive->GetData(mReceiveBuffer, size);
	mReceiveQueue.PutData(mReceiveBuffer, size);
	//std::cout << "OnReceive : " << size << "\r\n";
}
void ChatClient::OnConnect()
{
	std::cout << "OnConnect \r\n";

}
void ChatClient::OnDisConnect()
{
	std::cout << "OnDisconnect \r\n";
}
void ChatClient::SetName(std::string name)
{
	ZeroMemory(&mChattingPacket, sizeof(mChattingPacket));

	if (name.size() > NAME_SIZE)
	{
		strcpy_s(mChattingPacket.cName, NAME_SIZE - 2 ,name.c_str());
	}
	else
	{
		strcpy_s(mChattingPacket.cName, name.c_str());
	}
}
void ChatClient::Start()
{
	mIsSendThreadRun = true;
	mIsReceiveThreadRun = true;
	mIsMainThreadRun = true;

	mSendThread = std::thread([this](){this->sendProc();});
	mReceiveThread = std::thread([this]() {this->receiveProc(); });
	mMainThread = std::thread([this]() {this->mainProc(); });
}
void ChatClient::End()
{
	mIsMainThreadRun = false;
	mIsReceiveThreadRun = false;
	mIsSendThreadRun = false;

	if (mMainThread.joinable())
		mMainThread.join();
	if (mReceiveThread.joinable())
		mReceiveThread.join();
	if (mSendThread.joinable())
		mSendThread.join();
}
void ChatClient::sendProc()
{
	while(true)
	{
		if (mIsSendThreadRun == false)
			break;
		if (IsConnect() == false)
			break;

		if (mSendQueue.IsEmpty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
		else
		{
			size_t sendSize = mSendQueue.GetSize();
			mSendQueue.GetData(mSendBuffer,sendSize);
			SendToData(mSendBuffer, sendSize);
			std::cout << "SendToData: " << sendSize << "\r\n";
		}
	}
}
void ChatClient::receiveProc()
{
	UINT64 count = 0;
	bool IsIdle = false;
	while (true)
	{
		IsIdle = true;

		if (mIsReceiveThreadRun == false)
			break;

		do
		{
			UINT16	packetSize = mReceiveQueue.GetSize();
			if (packetSize < sizeof(PacketHeader))
				break;
			PacketHeader header;
			mReceiveQueue.GetData(&header,sizeof(header),rbuf_opt_e::RBUF_NO_CLEAR);

			if (header.pakcetID != PACKET_ID::SERVER_TO_CLIENT_CHATTING
				|| header.packetSize != sizeof(ChattingPacket))
			{
				mReceiveQueue.GetData(&header, sizeof(header));
				std::cout <<"BUR SIZE : " << packetSize << "\r\n";
			}

			if (header.packetSize > packetSize)
				break;
			if (header.pakcetID != PACKET_ID::SERVER_TO_CLIENT_CHATTING)
				break;

			ChattingPacket cp;
			mReceiveQueue.GetData(&cp, sizeof(cp));

			if (strcmp(cp.cName, mChattingPacket.cName) == 0)
			{
				ULONGLONG cur = GetTickCount64();
				ULONGLONG result = cur - cp.ulTickCount;
				//if(result > 1000)
				count++;
				std::cout << "Deley : " << result << " GET : " <<count << "\r\n";
			}

			IsIdle = false;
		} while (false);


		if(IsIdle)
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

void ChatClient::mainProc()
{
	UINT64 count = 1;
	while (true)
	{
		if (mIsMainThreadRun == false)
			break;

		mChattingPacket.ulTickCount = GetTickCount64();
		std::string strCount = std::to_string(count);
		strcpy_s(mChattingPacket.cChat, strCount.c_str());
		mChattingPacket.pakcetID = PACKET_ID::CLIENT_TO_SERVER_CHATTING;
		mChattingPacket.compressType = COMPRESS_TYPE::ZLIB;
		uLongf destLen = sizeof(mCompressBuffer);
		int result = compress((Bytef*)(mCompressBuffer + sizeof(PacketHeader)), &destLen, 
			(Bytef*)(&mChattingPacket.cName), (sizeof(mChattingPacket) - sizeof(PacketHeader)));

		mChattingPacket.packetSize = sizeof(PacketHeader) + destLen;
		memcpy_s(&mCompressBuffer[0], sizeof(mCompressBuffer), &mChattingPacket, sizeof(PacketHeader));

		std::cout << "PUT : " << strCount << "\r\n";
		mSendQueue.PutData(&mCompressBuffer, mChattingPacket.packetSize);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		count++;
	}
}



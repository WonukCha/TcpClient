#include <iostream>
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>


#include "ChatClient.h"

//constexpr int port = 6000;
//const char* ip = "192.168.219.101";

constexpr int clientCount = 100;
const char* name = "client";


class Test
{
private:
	std::string mIp;
	int mPort = 0;
public:
	Test()
	{

	}
	~Test()
	{
		for (unsigned int i = 0; i < pool.size(); i++)
		{
			delete pool[i];
		}
	}
	void Init(unsigned int clientCount)
	{

		auto path = std::filesystem::current_path();
		path += ("\\config.ini");

		wchar_t* cBuf = NULL;
		cBuf = (wchar_t*)malloc(sizeof(wchar_t) * 256);
		memset(cBuf, 0x00, sizeof(cBuf));

		if (!std::filesystem::exists(path))
		{
			WritePrivateProfileString(L"ServerInfo", L"IP", L"127.0.0.1", path.c_str());
			WritePrivateProfileString(L"ServerInfo", L"PORT", L"10000", path.c_str());
		}
		memset(cBuf, 0x00, sizeof(cBuf));
		GetPrivateProfileString(L"ServerInfo", L"PORT", L"6000", cBuf, 256, path.c_str());
		mPort = _wtoi(cBuf);

		memset(cBuf, 0x00, sizeof(cBuf));
		GetPrivateProfileString(L"ServerInfo", L"IP", L"127.0.0.1", cBuf, 256, path.c_str());
		std::wstring ws(cBuf);
		std::string str(ws.begin(), ws.end());
		mIp = str;

		pool.resize(clientCount);
		for (unsigned int i = 0; i < pool.size(); i++)
		{
			pool[i] = new ChatClient();
		}
	}
	void start()
	{
		for (unsigned int i = 0; i < pool.size(); i++)
		{
			pool[i]->Init(mIp.c_str(), mPort);
			if (pool[i]->Connect())
			{
				std::string index;
#ifdef SINGLE
				index = "-1";
#else
				index = i;
#endif
				pool[i]->SetName(index);
				pool[i]->Start();
			}
			else
			{
				std::cout << "엥~? Index : "<<i<< "번 연결실패~;; \r\n";
			}
		}
	}
	void end()
	{
		for (unsigned int i = 0; i < pool.size(); i++)
		{
			pool[i]->End();
		}
	}
private:
	std::vector<ChatClient*> pool;
};

int main(void)
{
	Test t;
#ifdef SINGLE
	t.Init(1);
#else
	t.Init(clientCount);
#endif
	
	t.start();
	
	printf("아무 키나 누를 때까지 대기합니다\n");
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);
	
		if (inputCmd == "quit")
		{
			break;
		}
	}
	
	t.end();
	return 0;
}
#include <iostream>
#include <vector>

#include "ChatClient.h"

constexpr int port = 6000;
const char* ip = "127.0.0.1";

constexpr int clientCount = 100;
const char* name = "client";


class Test
{
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
			pool[i]->Init(ip, port);
			if (pool[i]->Connect())
			{
				std::string index;
				index = i;
				pool[i]->SetName(index);
				pool[i]->Start();
			}
			else
			{
				std::cout << "��~? Index : "<<i<< "�� �������~;; \r\n";
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
#ifdef MULTI

	Test t;
	t.Init(clientCount);
	t.start();
	
	printf("�ƹ� Ű�� ���� ������ ����մϴ�\n");
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

#else
	ChatClient chatClient;
	chatClient.Init(ip, port);
	while (true)
	{
		if (chatClient.IsConnect())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
		else
		{
			chatClient.Connect();
			chatClient.SetName(name);
			chatClient.Start();
		}
	}
	chatClient.End();

#endif // MUITI
	return 0;
}
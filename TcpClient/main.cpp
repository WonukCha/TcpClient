#include <iostream>
#include <vector>

#include "ChatClient.h"

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
			pool[i]->Init("127.0.0.1", 10000);
			if (pool[i]->Connect())
			{
				std::string index;
				index = i;
				pool[i]->SetPacket(index);
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
	t.Init(99);
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

	//ChatClient chatClient;
	//chatClient.Init("127.0.0.1", 10000);
	//if (chatClient.Connect())
	//{
	//	chatClient.SetPacket("1");
	//	chatClient.Start();
	//	printf("아무 키나 누를 때까지 대기합니다\n");
	//	while (true)
	//	{
	//		std::string inputCmd;
	//		std::getline(std::cin, inputCmd);
	//
	//		if (inputCmd == "quit")
	//		{
	//			break;
	//		}
	//	}
	//	chatClient.End();
	//}

	
	
	

	return 0;
}
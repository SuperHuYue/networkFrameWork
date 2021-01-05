#include <iostream>
#include "william_net.hpp"
enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
	ServerPong
};



class CustomClient : public william::net::client_interface<CustomMsgTypes>
{
public:
	void PingServer()	
	{
		william::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		// Caution with this...
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();		

		msg << timeNow;
		Send(msg);
	}
	void PongServer() {
		william::net::message<CustomMsgTypes>msg;
		msg.header.id = CustomMsgTypes::ServerPong;
		// Caution with this...
		Send(msg);
	}

	void MessageAll()
	{
		william::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;		
		Send(msg);
	}
};

int main()
{
	CustomClient c;
	c.Connect("127.0.0.1", 60000);
	bool bQuit = false;
	int a = 1;
	while (!bQuit)
	{
		c.PongServer();//在段话在release中会卡顿，原因是超速的push_msg导致tsqueue一直被锁住，消息 无法被write出去，下次找时间进行优化处理
		if (c.IsConnected())
		{
			if (!c.Incoming().empty())
			{
				auto msg = c.Incoming().pop_front().msg;

				switch (msg.header.id)
				{
				case CustomMsgTypes::ServerAccept:
				{
					// Server has responded to a ping request				
					std::cout << "Server Accepted Connection\n";
				}
				break;


				case CustomMsgTypes::ServerPing:
				{
					// Server has responded to a ping request
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;
					std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
				}
				break;

				case CustomMsgTypes::ServerMessage:
				{
					// Server has responded to a ping request	
					uint32_t clientID;
					msg >> clientID;
					std::cout << "Hello from [" << clientID << "]\n";
				}
				case CustomMsgTypes::ServerPong:
				{
					std::cout << "Server Pong...\n";
				}

                break;
                default:
				break;
				}
			}
		}
		else
		{
			std::cout << "Server Down\n";
			bQuit = true;
		}

	}

	return 0;
}
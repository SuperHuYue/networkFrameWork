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
		std::string introduce_msg = "hellow everybody";
		msg.body.assign(introduce_msg.begin(), introduce_msg.end());
		msg.header.size = msg.size();
		Send(msg);
	}
};

int main()
{
	CustomClient c;
	c.Connect("127.0.0.1", 60000);
	bool bQuit = false;
	int a = 1;
	bool key[3] = { false, false, false };
	bool old_key[3] = { false, false, false };

	while (!bQuit)
	{

#ifdef _MSC_VER
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
		}

		if (key[0] && !old_key[0])
			c.PingServer();
		if (key[1] && !old_key[1]) 
			c.MessageAll();
		if (key[2] && !old_key[2]) bQuit = true;
		for (int i = 0; i < 3; i++) old_key[i] = key[i];
#else
		c.PoingServer();
#endif
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
				case CustomMsgTypes::MessageAll:
				{
					std::string out;
					out.assign(msg.body.begin(), msg.body.end());
					std::cout << out << "\n";
				}
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
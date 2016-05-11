#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <deque>

using namespace std;

class IRCClient
{
public:
	void connect();
	enum class ConnectionState
	{
		NOT_CONNECTED, //not connected 
		PINGED,		   //received first ping
		CONNECTED,	   //received first ping and connected
	};
private:
	sf::TcpSocket socket;
	sf::Socket::Status status;
	void receive();
	void send(string message);
	void handleMsgs(); // handles how to respond to the received messages
	deque<string> receivedMsgs;
	string leftovers; // what was left of the last received message; will be prepended to the next message received
	ConnectionState connectionState;
};
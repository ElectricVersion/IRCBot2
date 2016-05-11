#include "ircclient.hpp"
#include <vector>

void IRCClient::send(string message)
{
	message+="\r\n";
	size_t totalSent = 0;
	size_t amountSent = 0;
	while (totalSent < message.length()) // this loop is to make sure the whole message sends
	{
		socket.send(message.substr(totalSent, string::npos).c_str(), message.length(), amountSent);
		if (amountSent>0)
		{
			if (amountSent < message.length())
			{
				cout << "Partial Message sent: " <<  message.substr(totalSent, amountSent) << endl;
			}
			else
			{
				cout << "Message sent: " <<  message << endl;
			}
			totalSent+= amountSent;
		}
	}
}

void IRCClient::receive()
{
	char output[512] = "";
	std::size_t amount_received;
	socket.receive(output, 512, amount_received);
	string strOutput = output;
	strOutput = leftovers + strOutput; // prepend the what's left from the last receive to the current one
	size_t nextReturn = 0;
	size_t prevReturn = 0;
	while (nextReturn!=string::npos) // while there are still returns left
	{
		nextReturn = strOutput.find("\r\n", prevReturn); // search for a return
		if (nextReturn!=string::npos) // if there is another return...
		{
			string currentLine = strOutput.substr(prevReturn, nextReturn-prevReturn); // make currentLine from the last return to the current one
			cout << "Message received: " << currentLine << endl;
			receivedMsgs.push_back(currentLine); // add it to the end of the queue
			prevReturn = nextReturn + 2; // the +2 is so that it doesnt include the return characters
		}
		else // if there's not another return...
		{
			leftovers = strOutput.substr(prevReturn, string::npos); // update leftovers to be whatever doesnt have a newline at the end
		}
	}
}

void IRCClient::handleMsgs()
{
	while (!receivedMsgs.empty())
	{
		string msg = receivedMsgs.front();
		/* HANDLE PINGS */
		if (msg.find("PING")==0) // if the first thing in the line is ping, then we should pong
		{
			send("PONG" + msg.substr(4, string::npos));
			if (connectionState==ConnectionState::NOT_CONNECTED)
			{
				connectionState = ConnectionState::PINGED;
			}
		}
		else
		{
			if (connectionState==ConnectionState::PINGED)
			{
				connectionState = ConnectionState::CONNECTED;
			}
		}
		/* HANDLE COMMANDS */
		
		
		/* Remove this from the deque once it's been processed */
		receivedMsgs.erase(receivedMsgs.begin());
	}
}

void IRCClient::connect()
{
	connectionState = ConnectionState::NOT_CONNECTED; // we haven't receieved the first ping yet
	socket.setBlocking(false);
	string server = "irc.darkmyst.org";
    string channel = "#petrocelli";
	status = socket.connect(server.c_str(), 6667);
	send("USER Sparkbot 0 * :Spark Bot");
	send("NICK Sparkbot");
	while (connectionState!=ConnectionState::CONNECTED) // until I get the first ping...
	{
		receive(); // ... just keep receiving
		handleMsgs();
	}
	send("JOIN " + channel);
	while (true)
	{
		receive();
		handleMsgs();
	}
}
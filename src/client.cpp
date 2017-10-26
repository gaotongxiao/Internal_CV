#include <fcntl.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
using namespace std;

class Client
{
  public:
	Client();
    ~Client();
    bool config(const string &ip, const int &port);
    bool c();
    bool sendAns(int i);

  private:
    int sock;
    bool connected;
    sockaddr_in serv_addr;
    char str[256];
};

Client::Client()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
	connected = false;
}

bool Client::config(const string& ip, const int& port)
{
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    //serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());  
	if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
		return false;
    serv_addr.sin_port = htons(port);  
	return true;
}

bool Client::c()
{
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		cerr << errno <<" connect error.\n";
		return false;
	}
    connected = true;
    return true;
}

bool Client::sendAns(int i)
{
    if (!connected && !c()) return false;
    if (i < 0 || i > 9)
    {
        cerr << "Invalid input! Please check you code." << endl;
        return false;
    }
    str[0] = i + '0';
    str[1] = '\0';
    if (send(sock, str, strlen(str) * sizeof(char), 0) < 0)
    {
        cerr << "Sending error" << endl;
		connected = false;
        return false;
    }
    return true;
}

Client::~Client()
{
    close(sock);
}


Client test;

void exit_handler(int a)
{
	test.~Client();
	exit(1);
}

int main(int argc, char* argv[]){
	signal(SIGINT, exit_handler);
	if (argc != 3)
	{
		cerr << "Please enter ip and port number.\n";
		exit(1);
	}
	if (test.config(argv[1], atoi(argv[2])) == false)
	{
		cerr << "Invalid ip or port number\n";
		exit(1);
	}
	while (!test.c())sleep(1);
	cout << "connect success\n";
    int a;
	while(1)
	{
		cin >> a;
		if (a == -1) break;
		cout << test.sendAns(a) << endl;
	}
    return 0;
}

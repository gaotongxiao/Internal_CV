#include <fcntl.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

class Client
{
  public:
    Client(const string &ip, const int &port);
    ~Client();
    bool c();
    bool sendAns(int i);

  private:
    int sock;
    bool connected;
    sockaddr_in serv_addr;
    char str[256];
    int replyLen;
    fd_set read_flags, write_flags;
    timeval waitd;
};

Client::Client(const string &ip, const int &port) : connected(false)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    int x = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, x | O_NONBLOCK);
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());  
    serv_addr.sin_port = htons(port);  
    waitd.tv_sec = 0;
    waitd.tv_usec = 100;
    c();
}

bool Client::c()
{
    connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));
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
    int stat;
    FD_ZERO(&read_flags);
    FD_SET(sock, &write_flags);
    for (int j = 0; j < 5; j++)
    {
        stat = select(sock, &read_flags, &write_flags, (fd_set*)0, &waitd);
        if (stat >= 0)
            break;
    }
    if (stat < 0) return false;
    str[0] = i + '0';
    str[1] = '\0';
    write(sock, str, strlen(str));
    return true;
}

Client::~Client()
{
    close(sock);
}

int main(){
    Client test("127.0.0.1", 4331);
    /*
    int a;
    cin >> a;
    test.sendAns(a);
    */


    return 0;
}
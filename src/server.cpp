#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
using namespace std;

class Judge
{
  public:
    Judge(const string &ip, const int &port);
    ~Judge();
    bool sendAns(int i);
    int recAns();

  private:
    int sock;
    int clnt_sock;
    sockaddr_in serv_addr;
    char str[256];
    struct sockaddr_in clnt_addr;
    fd_set read_flags, write_flags;
    timeval waitd;
};

Judge::Judge(const string &ip, const int &port) 
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
    bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(sock, 20);
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
}

int Judge::recAns()
{
    FD_ZERO(&write_flags);
    FD_SET(sock, &read_flags);
    int stat;
    for (int j = 0; j < 5; j++)
    {
        stat = select(sock, &read_flags, &write_flags, (fd_set*)0, &waitd);
        if (stat >= 0)
            break;
    }
    if (stat < 0) return -1;
    read(clnt_sock, str, sizeof(str-1));
    if (strlen(str) > 1)
        return -1;
    /*
    if (read(clnt_sock, str, sizeof(str-1)) < 0 || strlen(str) > 1)
    {
        cerr << "Receiving error\n";
        return -1;
    }
    */
    return str[0] - '0';
}

Judge::~Judge()
{
    close(clnt_sock);
    close(sock);
}
int main(){
    Judge j("127.0.0.1", 4331);
    while (1)
    {
        if (j.recAns() != -1)cout << j.recAns();
    }

    return 0;
}
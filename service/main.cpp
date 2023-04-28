
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;

int main()
{
    cout << "Hello,VScode!" << endl;

    cout << "Hello,VScode!" << endl;
    cout << "Hello,VScode!" << endl;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0); // 打开套接字
    cout << socket_fd << endl;
    
    return 0;
}
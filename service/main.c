#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

int multi_process(unsigned int port);
void handle_client(int c_socket, struct sockaddr_in *client_add);

int main()
{

    printf("aaaaaa\n");
    multi_process(12333);
    
    return 0;
}
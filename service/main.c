#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

int multi_process(unsigned int port);

int main()
{

    multi_process(12333);
    
    return 0;
}
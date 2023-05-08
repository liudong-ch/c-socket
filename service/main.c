#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

int multi_process(unsigned int port);
int multi_thread(unsigned int port);
int io_select(unsigned int port);

int main()
{

    // multi_process(12333);
    
    // multi_thread(12333);

    io_select(12333);

    return 0;
}
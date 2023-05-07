#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>


void io_select(unsigned int port) {
    int s_socket, c_socket_fd;
    struct sockaddr_in server_add, client_add2;
    int s_add_len = sizeof(server_add);
    socklen_t c_add_len = sizeof(client_add2);
    fd_set readfds, testfds;


    s_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (s_socket == -1) {
        printf("socket 创建失败\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_add, 0, s_add_len);
    server_add.sin_family = AF_INET;
    server_add.sin_addr.s_addr = htonl(INADDR_ANY);
    server_add.sin_port = htons(port);

    int r = bind(s_socket, (struct sockaddr *)&server_add, s_add_len);
    if (r == -1) {
        printf("绑定socket失败--%s\n", strerror(errno));
        close(s_socket);
        exit(EXIT_FAILURE);
    }

    r = listen(s_socket, 20);
    if (r == -1) {
        printf("监听失败\n");
        exit(EXIT_FAILURE);
    }
    
    char cli_ip[INET_ADDRSTRLEN];
    FD_ZERO(&readfds);
    FD_SET(s_socket, &readfds);

    printf("服务正在运行...\n");
    while(1) {

        testfds = readfds;
        int ret = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        if (ret < 1) {
            printf("select 失败\n");
            exit(EXIT_FAILURE);
        }
        

        struct sockaddr_in client_add;
        c_socket_fd = accept(s_socket, (struct sockaddr *)&client_add, &c_add_len);
        if (c_socket_fd == -1) {
            printf("接受连接失败\n");
            continue;
        }

        inet_ntop(AF_INET, &client_add.sin_addr, cli_ip, INET_ADDRSTRLEN);
        printf("连接成功...主机地址：%s:%d\n", cli_ip, ntohs(client_add.sin_port));



    }


    return 0;
}
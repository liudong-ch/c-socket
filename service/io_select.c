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


int io_select(unsigned int port) {
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
    
    FD_ZERO(&readfds);
    FD_SET(s_socket, &readfds);

    char buffer[5*1024];
    int recv_size;
    char msg[2*2014];

    printf("服务正在运行...\n");

    while(1) {
        int fd;

        testfds = readfds;
        int ret = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        if (ret < 1) {
            printf("select 失败\n");
            exit(EXIT_FAILURE);
        }

        for (fd = 0; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &testfds)) {
                if (fd == s_socket) {   //服务监听的socket，有新连接
                    struct sockaddr_in client_add;
                    c_socket_fd = accept(s_socket, (struct sockaddr *)&client_add, &c_add_len);
                    if (c_socket_fd == -1) {
                        printf("接受连接失败\n");
                        continue;
                    }
                    FD_SET(c_socket_fd, &readfds);

                    printf("连接成功...主机地址：%s:%d\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port));

                } else {    //客户端的socket有数据到达
                    struct sockaddr_in client_add;
                    int c_add_len = sizeof(client_add);
                    getpeername(fd, (struct sockaddr *)&client_add, &c_add_len);
                    recv_size = recv(fd, buffer, 5*1024, 0);
                    if (recv_size <= 0) {
                        FD_CLR(fd, &readfds);
                        printf("断开连接...主机地址：%s:%d\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port));
                        break;
                    }
                    buffer[recv_size] = '\0';
                    if (strcmp(buffer, "end") == 0) {
                        FD_CLR(fd, &readfds);
                        printf("断开连接...主机地址：%s:%d\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port));
                        close(fd);
                        break;
                    }

                    printf("正在处理...主机地址：%s:%d\n数据:%s\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port), buffer);
                    sleep(5);
                    printf("正在返回数据...主机地址：%s:%d\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port));
                    sleep(1);
                    int msg_size = sprintf(msg, "收到数据，长度：%d", recv_size);

                    send(fd, msg, msg_size, 0);
                }
            }
        }
        
    }


    return 0;
}
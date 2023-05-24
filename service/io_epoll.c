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
#include <sys/epoll.h>
#include <sys/resource.h>


int io_epoll(unsigned int port) {
    int s_socket, c_socket_fd, epollfd, events;
    struct sockaddr_in server_add, client_add2;
    int s_add_len = sizeof(server_add);
    socklen_t c_add_len = sizeof(client_add2);
    struct epoll_event event, evlist[512];


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

    // 创建epoll
    if ((epollfd = epoll_create(512)) < 0) {
        printf("epoll creat 失败\n");
        exit(EXIT_FAILURE);
    }
    
    event.events = EPOLLIN | EPOLLET | EPOLLHUP;
    event.data.fd = s_socket;
    // 添加epoll的fd的事件监听类型
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, s_socket, &event) < 0) {
        printf("epoll ctl 失败\n");
        exit(EXIT_FAILURE);
    }

    char buffer[5*1024];
    int recv_size;
    char msg[2*2014];

    printf("服务正在运行...\n");

    while(1) {
        // 等待监听的事件发生
        events = epoll_wait(epollfd, evlist, 512, -1);
        if (events <= 0) {
            printf("epoll wait 失败\n");
            continue;
        }

        int i;
        for (i = 0; i < events; i++) {
            // 连接断开、报错
            if (evlist[i].events & EPOLLHUP || evlist[i].events & EPOLLERR) {
                struct sockaddr_in client_add;
                int c_add_len = sizeof(client_add);
                getpeername(evlist[i].data.fd, (struct sockaddr *)&client_add, &c_add_len);
                printf("断开连接...主机地址：%s:%d\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port));
                // 删除fd的事件监听，关闭fd
                epoll_ctl(epollfd, EPOLL_CTL_DEL, evlist[i].data.fd, NULL);
                close(evlist[i].data.fd);
                continue;

            } else if (evlist[i].data.fd == s_socket) {    // 新连接到达
                struct sockaddr_in client_add;
                c_socket_fd = accept(s_socket, (struct sockaddr *)&client_add, &c_add_len);
                if (c_socket_fd == -1) {
                    printf("接受连接失败\n");
                    continue;
                }

                event.data.fd = c_socket_fd;
                event.events = EPOLLIN | EPOLLET | EPOLLHUP;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, c_socket_fd, &event) < 0) {
                    printf("epoll ctl 失败\n");
                    close(c_socket_fd);
                    continue;
                }

                printf("连接成功...主机地址：%s:%d\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port));

            } else if (evlist[i].events & EPOLLIN) {    // 客户端，连接有可读事件
                struct sockaddr_in client_add;
                int c_add_len = sizeof(client_add);
                getpeername(evlist->data.fd, (struct sockaddr *)&client_add, &c_add_len);

                recv_size = recv(evlist->data.fd, buffer, 5*1024, 0);    // 读数据
                if (recv_size <= 0) {    // 删除fd的事件监听，关闭fd
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, evlist[i].data.fd, NULL);
                    close(evlist[i].data.fd);
                    printf("断开连接...主机地址：%s:%d\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port));
                    break;
                }

                buffer[recv_size] = '\0';
                if (strcmp(buffer, "end") == 0) {
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, evlist[i].data.fd, NULL);
                    close(evlist[i].data.fd);
                    printf("断开连接...主机地址：%s:%d\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port));
                    break;
                }

                printf("正在处理...主机地址：%s:%d\n数据:%s\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port), buffer);
                sleep(5);
                printf("正在返回数据...主机地址：%s:%d\n", inet_ntoa(client_add.sin_addr), ntohs(client_add.sin_port));
                sleep(1);
                int msg_size = sprintf(msg, "收到数据，长度：%d", recv_size);

                send(evlist[i].data.fd, msg, msg_size, 0);    // 发送数据
            }


        }
        
    }


    return 0;
}
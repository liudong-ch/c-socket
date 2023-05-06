#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>



void *thread_handle(void*);

int multi_thread(unsigned int port) {
    int s_socket, c_socket;
    struct sockaddr_in server_add, client_add2;
    int s_add_len = sizeof(server_add);
    socklen_t c_add_len = sizeof(client_add2);
    pthread_t tid;


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
    

    printf("服务正在运行...\n");
    while(1) {
        struct sockaddr_in client_add;
        c_socket = accept(s_socket, (struct sockaddr *)&client_add, &c_add_len);
        if (c_socket == -1) {
            printf("接受连接失败\n");
            continue;
        }

        int ret = pthread_create(&tid, NULL, thread_handle, (void *)&c_socket);
        if (ret) {
            printf("创建线程失败%s\n", strerror(errno));
            close(c_socket);
            continue;
        }
        
        inet_ntop(AF_INET, &client_add.sin_addr, cli_ip, INET_ADDRSTRLEN);
        printf("连接成功...主机地址：%s:%d\n", cli_ip, ntohs(client_add.sin_port));

        pthread_detach(tid);

    }
    return 0;
}

void *thread_handle(void *arg) {
    int c_socket = *((int *)arg);
    char buffer[5*1024];
    int recv_size;
    char msg[2*2014];

    struct sockaddr_in client_add;
    int c_add_len = sizeof(client_add);
    getpeername(c_socket, (struct sockaddr *)&client_add, &c_add_len);
    char cli_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_add.sin_addr, cli_ip, INET_ADDRSTRLEN);

    while (1) {
        recv_size = recv(c_socket, buffer, 5*1024, 0);
        if (recv_size <= 0) {
            printf("断开连接...主机地址：%s:%d\n", cli_ip, ntohs(client_add.sin_port));
            break;
        }
        buffer[recv_size] = '\0';
        if (strcmp(buffer, "end") == 0) {
            printf("断开连接...主机地址：%s:%d\n", cli_ip, ntohs(client_add.sin_port));
            close(c_socket);
            break;
        }

        printf("正在处理...主机地址：%s:%d\n数据:%s\n", cli_ip, ntohs(client_add.sin_port), buffer);
        sleep(5);
        printf("正在返回数据...主机地址：%s:%d\n", cli_ip, ntohs(client_add.sin_port));
        sleep(1);
        int msg_size = sprintf(msg, "收到数据，长度：%d", recv_size);

        send(c_socket, msg, msg_size, 0);
    }
    

    close(c_socket);

}

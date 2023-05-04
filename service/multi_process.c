#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

void handle_client(int c_socket, struct sockaddr_in *client_add);


int client_sockets[20];
int num_client = 0;

int multi_process(unsigned int port) {
    printf("服务正在运行...\n");
    int s_socket, c_socket;
    struct sockaddr_in server_add, client_add;
    int s_add_len = sizeof(server_add);
    socklen_t c_add_len = sizeof(client_add);
    pid_t pid;


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
        printf("绑定socket失败\n");
        exit(EXIT_FAILURE);
    }

    r = listen(s_socket, 20);
    if (r == -1) {
        printf("监听失败\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        c_socket = accept(s_socket, (struct sockaddr *)&client_add, &c_add_len);
        if (c_socket == -1) {
            printf("接受连接失败\n");
            continue;
        }

        // client_sockets[num_client++] = c_socket;

        pid = fork();
        if (pid == -1) {
            printf("创建进程失败\n");
            continue;
        }

        if (pid == 0) {
            handle_client(c_socket, &client_add);
        }

    }
    return 0;
}

void handle_client(int c_socket, struct sockaddr_in *client_add) {
    char buffer[5*1024];
    int recv_size;
    char msg[2*2014];

    while((recv_size = recv(c_socket, buffer, 5*1024, 0)) > 0) {
        buffer[recv_size] = '\0';
    }
    printf("正在处理...主机地址：%d\n", client_add->sin_addr.s_addr);
    sleep(3);
    printf("正在返回数据...主机地址：%d\n", client_add->sin_addr.s_addr);
    sleep(3);
    int msg_size = sprintf(msg, "收到数据，长度：%d\n", recv_size);

    send(c_socket, msg, msg_size, 0);

    close(c_socket);

}


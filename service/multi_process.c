#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

void handle_client(int c_socket);


int multi_process(unsigned int port) {
    int s_socket, c_socket;
    struct sockaddr_in server_add, client_add2;
    int s_add_len = sizeof(server_add);
    socklen_t c_add_len = sizeof(client_add2);
    pid_t pid, pp;

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
        // close(s_socket);
        exit(EXIT_FAILURE);
    }

    r = listen(s_socket, 20);
    if (r == -1) {
        printf("监听失败\n");
        exit(EXIT_FAILURE);
    }
    
    printf("服务正在运行...\n");

    while(1) {
        struct sockaddr_in client_add;
        c_socket = accept(s_socket, (struct sockaddr *)&client_add, &c_add_len);
        if (c_socket == -1) {
            printf("接受连接失败\n");
            continue;
        }
        // 创建进程，会复制父进程的所有代码和文件描述符，
        // 子进程的代码执行和父进程完全一致。子进程中 pid返回 0
        pid = fork();
        if (pid == -1) {
            printf("创建进程失败\n");
            continue;
        }
        if (pid == 0) {
            // 子进程 处理逻辑
            close(s_socket);
            printf("正在准备接收数据\n");
            handle_client(c_socket);
            break;
        } else {
            // 父进程 处理逻辑
            close(c_socket);
        }

    }
    return 0;
}
// 连接处理函数
void handle_client(int c_socket) {
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


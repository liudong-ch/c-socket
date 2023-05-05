#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int start(char *ip, int port) {
    int c_sock ;
    struct sockaddr_in s_add;

    c_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (c_sock == -1) {
        printf("socket 创建失败\n");
        exit(EXIT_FAILURE);
    }

    memset(&s_add, 0, sizeof(s_add));
    s_add.sin_family = AF_INET;
    s_add.sin_addr.s_addr = inet_addr(ip);
    s_add.sin_port = htons(port);

    if(connect(c_sock, (struct sockaddr *)&s_add, sizeof(s_add)) == -1) {
        printf("连接失败\n");
        exit(EXIT_FAILURE);
    }
    printf("连接成功\n");

    while(1) {
        char msg[2*1024];
        scanf("%s", &msg[0]);
        int msg_len = strlen(msg);
        int recv_size;

        if (strcmp(msg, "exit") == 0) {
            break;
        }
        if (send(c_sock, msg, msg_len, 0) == -1) {
            printf("发送失败...\n");
            continue;
        }
        printf("发送成功\n");
        if (strcmp(msg, "end") == 0) {
            break;
        }

        recv_size = recv(c_sock, msg, 2*1024, 0);
        if (recv_size == -1) {
            printf("接收失败...\n");
            continue;
        }
        if (recv_size == 0) {
            printf("连接断开...\n");
            break;
        }

        msg[recv_size] = '\0';
        printf("接收成功：%s\n", msg);

    }

    close(c_sock);
    return 0;
}

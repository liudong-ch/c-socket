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

    menset(&s_add, 0, sizeof(s_add));
    s_add.sin_family = AF_INET;
    s_add.sin_addr.s_addr = inet_addr(ip);
    s_add.sin_port = htons(port);

    if(connect(c_sock, (struct sockaddr *)&s_add, sizeof(s_add)) == -1) {
        printf("连接失败\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        char msg[2*1024];
        scanf("%s", &msg);
        
    }

    return 0;
}

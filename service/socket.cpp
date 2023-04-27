#include <sys/types.h>
#include <sys/socket.h>

int single_process() {

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0); // 打开套接字
}
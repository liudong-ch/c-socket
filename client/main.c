#include <stdio.h>

int start(char *ip, int port);

int main(int argc, char const *argv[])
{
    start("127.0.0.1", 12333);
    return 0;
}

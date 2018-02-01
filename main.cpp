#include <iostream>
#include "socket_helper.h"

using namespace std;
void * fagt(void * arg);
int main()
{
    ServerSocket lol(8000);
    lol.registerConnectCallback(fagt);
    lol.startListening();
}

void * fagt(void * connectionInfoStruct)
{
    struct connectionInfoStruct * lel = connectionInfoStruct;
    write(lel, "hello", 5, 0);

}

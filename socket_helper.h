#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>

#define DEFAULT_PORT 8000

struct connectionInfo
{
    int * sock;
    struct sockaddr_in ipInfo;
};

class ServerSocket
{
private:
    struct _status
    {
        enum{NOT_CONNECTED = -1,
            CONNECTED,
            DISCONNECTED,
            LISTENING} Status = NOT_CONNECTED;
    }status;
    int listenerSocket;
    int serverPort = 8000;
    struct sockaddr_in listenerSocketInfo;
    struct sockaddr_in remoteSocketInfo;
    pthread_t threadId_listenerRoutine;
    pthread_attr_t threadAttr_listenerRoutine;
    void * routine_listener(void * arg);
    void * (*routine_connectCallback)(void * connectionInfoStruct);
public:
    ServerSocket(int port = DEFAULT_PORT);
    typedef void *(*connectCallback)(void * connectionInfoStruct);
    void registerConnectCallback(connectCallback cb);
    int getPort();
    int getStatus();
    void startListening();
};

void * ServerSocket::routine_listener(void * arg)
{
    int * sock = (int *)arg;
    while(1)
    {
        int * acceptorSocket = new int;
        listen(*sock, 10);
        struct connectionInfo * tempConnectionInfo = new struct connectionInfo;
        *acceptorSocket = accept(*sock,
                                (struct sockaddr*)&tempConnectionInfo->ipInfo,
                                (socklen_t)sizeof(struct sockaddr));
        pthread_t * newThreadId = new pthread_t;
        pthread_attr_t * newThreadAttr = new pthread_attr_t;
        pthread_attr_init(newThreadAttr);
        pthread_create(newThreadId,
                       newThreadAttr,
                       routine_connectCallback,
                       tempConnectionInfo);
        pthread_join(*newThreadId,
                     NULL);
    }
}

ServerSocket::ServerSocket(int port)
{
    serverPort = port;
    listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    listenerSocketInfo.sin_addr.s_addr = INADDR_ANY;
    listenerSocketInfo.sin_family = AF_INET;
    listenerSocketInfo.sin_port = htons(port);
    bind(listenerSocket,
         (struct sockaddr*)&listenerSocketInfo,
         sizeof(listenerSocketInfo));
}

void ServerSocket::registerConnectCallback(connectCallback cb)
{
    routine_connectCallback = cb;
}

int ServerSocket::getPort()
{
    return serverPort;
}

int ServerSocket::getStatus()
{
    return status.Status;
}

void ServerSocket::startListening()
{
    pthread_attr_init(&threadAttr_listenerRoutine);
    pthread_create(&threadId_listenerRoutine,
                   &threadAttr_listenerRoutine,
                   routine_listener,
                   &acceptorSocket);
    pthread_join(threadId_listenerRoutine,
                 NULL);
}

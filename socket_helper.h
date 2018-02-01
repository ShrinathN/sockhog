#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>

#define DEFAULT_PORT 8000
class ServerSocket
{
private:
    struct _status
    {
        enum{NOT_CONNECTED = -1,
            CONNECTED,
            DISCONNECTED,
            LISTENING} connectionStatus = NOT_CONNECTED;
    }status;
    int listenerSocket;
    int acceptorSocket;
    int serverPort = 8000;
    struct sockaddr_in listenerSocketInfo;
    struct sockaddr_in remoteSocketInfo;
    pthread_t threadId_listenerSocket;
    pthread_attr_t threadAttr_listenerSocket;
public:
    ServerSocket(int port = DEFAULT_PORT);
    typedef void *(*connectCallback)(void * arg);
    void registerConnectCallback(connectCallback cb);
    void * (*routine_listenerSocket)(void * arg);
    int getPort();
    int getStatus();
    void startListening();
};

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

int ServerSocket::getPort()
{
    return serverPort;
}

int ServerSocket::getStatus()
{
    return status.connectionStatus;
}

void ServerSocket::registerConnectCallback(connectCallback cb)
{
    routine_listenerSocket = cb;
}

void ServerSocket::startListening()
{
    pthread_attr_init(&threadAttr_listenerSocket);
    pthread_create(&threadId_listenerSocket,
                   &threadAttr_listenerSocket,
                   routine_listenerSocket,
                   &acceptorSocket);
    pthread_join(threadId_listenerSocket,
                 NULL);
}

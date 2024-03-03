#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>



//Creates a sockaddr_in struct pointer, with relevant information inside
struct sockaddr_in* CreateIPv4Address(char *ip, int port) {

    //ip4 address struct

    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    //address port
    address.sin_port = htons(port);
    //address family
    address.sin_family = AF_INET;
    //IP address
    inet_pton(AF_INET, ip, &address.sin_addr.s_addr);

    if (strlen(ip) == 0){
        address.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, ip, &address.sin_addr.s_addr);
    }
    return address;
}

//Creates a TCP/IPv4 socket for use
int CreateTCPIPv4Socket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct AcceptedSocket {
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccess;

};

/*
Creates the necessary file descriptor (for a client socket) for use in connecting a client, connects to a socket
*/
struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD) {

// basic info for struct
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, &clientAddress, &clientAddressSize);

// uses AcceptedSocket struct to populate information into return struct
    struct AcceptedSocket* acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket.address = clientAddress;
    acceptedSocket.acceptedSocketFD = clientSocketFD;
    acceptedSocket.acceptedSuccess = clientSocketFD>0;

// if the socket didn't connect, then set the error to the FD value, for debug
    if (!acceptedSocket.acceptedSuccess){
        acceptedSocket.error = clientSocketFD;
    }
    return acceptedSocket;
}

//recieves incoming messages
void recieveIncoming(int socketFD){
char buffer[1024];   
    while (true) {
        
    // recieves message
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);

        if(amountRecieved > 0) {

        // if it recieved something, output it into the server
            buffer[amountRecieved] = 0;
            printf("Response was: %s\n", buffer);

        // if the recieved message is empty, kill the server
        } else if(amountRecieved == 0) {

            break;
        }
    }

        // tear down client's socket if
    close(socketFD);
}

// recieves incoming on its own thread, for the while(true) loop
void recieveIncomingThread(struct AcceptedSocket *pSocket){

    pthread_t id;
    pthread_create(&id, NULL, recieveIncoming, pSocket->acceptedSocketFD);

}


//handles socket and recieves messages, waits until a new one connects
void handleClient(int serverSocketFD){

    while(true){

    //struct for holding client info
    struct AcceptedSocket* clientSocket = acceptIncomingConnection(serverSocketFD);

        recieveIncomingThread(clientSocket);
    }
}

// creates thread to run handleClient- unused as of now
/*void StartAcceptIncomingConnection(int serverSocketFD) {
    
    pthread_t id;
    pthread_create(&id,NULL, handleClient, serverSocketFD);
    
}*/




/* SERVER WORKFLOW PROCESS
*
* 1. Set up socket for server to communicate on, bind server to the socket.
*
* 2. Listen for new connections to the server.
*
* 3. Spins up a new thread to connect new connections
*
* 4. When a client connects, spin up a thread to recieve their messages
*
*/

int main() {

// generate relevant socket and addresses for server    
    int serverSocketFD = CreateTCPIPv4Socket();

    struct sockaddr_in *serverAddress = CreateIPv4Address("", 2000);

// bind server to the socket
    int result = bind(serverSocketFD, serverAddress, sizeof(*serverAddress));
    if (result == 0) {
        printf("Socket was bound successfully!");
    }

// listen for new connections
    int listenResult = listen(serverSocketFD, 10);
    
// accept the new connection, spin up thread to handle it
    handleClient(serverSocketFD);

// tears down the sockets serverside - unnecessary for the moment
    shutdown(ServerSocketFD, SHUT_RDWR);

    return 0
}
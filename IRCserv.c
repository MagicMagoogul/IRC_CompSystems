/////////////////////////////////////
//
//	COMMETNS ADDED BY CHATGPT
//
/////////////////////////////////////
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

// #1 Start by accepting a prot number for the listening service via command line arg
// Creates a sockaddr_in struct pointer with the given IP and port
struct sockaddr_in* createIPv4Address(char *ip, int port) {
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_port = htons(port);  // Convert port to network byte order
    address->sin_family = AF_INET;     // Set address family to IPv4

    // If the IP is empty, bind to any available local IP address
    if (strlen(ip) == 0){
        address->sin_addr.s_addr = INADDR_ANY;  
    } else {
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);  // Convert IP address to network format
    }

    return address;
}


// Creates a TCP/IPv4 socket
int createTCPIpv4Socket(){
    return socket(AF_INET, SOCK_STREAM, 0);  // Create socket (IPv4, TCP)
}

// Represents accepted socket information
struct AcceptedSocket {
    int acceptedSocketFD;          // Accepted socket file descriptor
    struct sockaddr_in address;    // Client address information
    int error;                     // Error code (if any)
    bool acceptedSuccessfully;     // Indicates if the socket was accepted successfully
};

// #2 Accept incomming client connections via Berkely sockets for an indeterminate amount of time
// Accepts incoming connection on the given server socket
struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, (socklen_t*)&clientAddressSize);
    
    // Allocate memory for AcceptedSocket struct
    struct AcceptedSocket* acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = clientSocketFD > 0;

    // Set error code if socket wasn't accepted successfully
    if (!acceptedSocket->acceptedSuccessfully){
        acceptedSocket->error = clientSocketFD;
    }

    return acceptedSocket;
}

// Array to store accepted sockets
struct AcceptedSocket acceptedSockets[10];
int acceptedSocketsCount = 0;

// #5 - Multicast received messages to other server tasks
// #6 - Transmit messages from other clients to the connected client
// Sends received message to other clients
void sendReceivedMessageToTheOtherClients(char* message, int socketFD){
    for(int i = 0; i < acceptedSocketsCount; i++){
        if(acceptedSockets[i].acceptedSocketFD != socketFD){
            send(acceptedSockets[i].acceptedSocketFD, message, strlen(message), 0);
        }
    }
}

// #4 - Receive messages from the client
// Thread function to receive and print incoming data
void *receiveAndPrintIncomingData(void *socketFD){
    int sockfd = *((int *)socketFD);
    char buffer[1024];   

    while (true) {
        ssize_t amountReceived = recv(sockfd, buffer, 1024, 0);

        if(amountReceived > 0) {
            buffer[amountReceived] = 0;  // Null-terminate the received message
            printf("%s\n", buffer);
            sendReceivedMessageToTheOtherClients(buffer, sockfd);  // Send message to other clients
        } else if(amountReceived == 0) {
            break;  // Break the loop if no data received (connection closed)
        }
    }

    close(sockfd);  // Close the socket
    return NULL;
}

// Starts receiving and printing incoming data on a separate thread
void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket){
    pthread_t id;  // Thread ID
    int *socketFD = malloc(sizeof(*socketFD));
    *socketFD = pSocket->acceptedSocketFD;
    pthread_create(&id, NULL, receiveAndPrintIncomingData, socketFD);  // Create thread
}
// #3 - Map a single task on the server to a single connected client in order to manage a client session
// Starts accepting incoming connections
void startAcceptingIncomingConnections(int serverSocketFD){
    while(true){
        struct AcceptedSocket* clientSocket = acceptIncomingConnection(serverSocketFD);
        acceptedSockets[acceptedSocketsCount++] = *clientSocket;  // Add accepted socket to array
        receiveAndPrintIncomingDataOnSeparateThread(clientSocket);  // Start thread for handling the accepted connection
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);  // Print usage information if arguments are incorrect
        exit(1);  // Exit with error
    }

    int port = atoi(argv[1]);  // Convert port from string to integer
    int serverSocketFD = createTCPIpv4Socket();  // Create TCP/IPv4 socket

    struct sockaddr_in *serverAddress = createIPv4Address("", port);  // Create sockaddr_in struct for server address
    int result = bind(serverSocketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress));  // Bind socket to address

    if (result == 0) {
        printf("Socket was bound successfully!\n");  // Print success message if socket binding was successful
    }

    int listenResult = listen(serverSocketFD, 10);  // Listen for incoming connections
    startAcceptingIncomingConnections(serverSocketFD);  // Start accepting incoming connections

    shutdown(serverSocketFD, SHUT_RDWR);  // Shutdown the server socket
    return 0;  // Return success
}


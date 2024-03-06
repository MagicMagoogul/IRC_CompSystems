#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

// Requirement 1 - Start by accepting an IP address and a port number for the server's listening service
// Creates a sockaddr_in struct pointer with the given IP and port
struct sockaddr_in* createIPv4Address(char *ip, int port) {
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_port = htons(port);  // Convert port to network byte order
    address->sin_family = AF_INET;     // Set address family to IPv4
    inet_pton(AF_INET, ip, &address->sin_addr.s_addr);  // Convert IP address to network format
    return address;
}

// Requirement 4 - Recieve and Display messages from the server's assigned session task
// Thread function to receive messages from the server
void *receiveMessages(void *socketFD){
    int sockfd = *((int *)socketFD);  // Socket file descriptor
    char buffer[1024];  // Buffer for incoming messages
    
    while (true) {
        ssize_t amountReceived = recv(sockfd, buffer, 1024, 0);  // Receive message
        
        if(amountReceived > 0) {
            buffer[amountReceived] = 0;  // Null-terminate the received message
            printf("%s\n", buffer);  // Print received message
        } else if(amountReceived == 0) {
            break;  // Break the loop if no data received (connection closed)
        }
    }
    
    close(sockfd);  // Close the socket when done receiving
    return NULL;
}

  // Starts a thread for receiving messages
  void startIncomingThread(int socketFD){
    pthread_t id;  // Thread ID
    int *socketFDPtr = malloc(sizeof(*socketFDPtr));  // Allocate memory for socket file descriptor
    *socketFDPtr = socketFD;  // Assign socket file descriptor
    pthread_create(&id, NULL, receiveMessages, socketFDPtr);  // Create thread for receiving messages
  } 

  // Creates a TCP/IPv4 socket
  int createTCPIPv4Socket(){
    return socket(AF_INET, SOCK_STREAM, 0);  // Create socket (IPv4, TCP)
  }

  int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);  // Print usage information if arguments are incorrect
        exit(1);  // Exit with error
    }
    
    // Requirement 1 - Start by accepting an IP address and a port number for the server's listening service (also mentioned above)
    char *ip = argv[1];  // Server IP address
    int port = atoi(argv[2]);  // Server port
    int socketFD = createTCPIPv4Socket();  // Create TCP/IPv4 socket
    struct sockaddr_in *address = createIPv4Address(ip, port);  // Create sockaddr_in struct for server address
    int result = connect(socketFD, (struct sockaddr*)address, sizeof(*address));  // Connect to server
    
    if (result == 0){
        printf("Connection was successful!\n");  // Connection successful message
    }
    
    // Requirement 2 - Accept input messages from the user
    char* user = NULL;  // User input for username
    size_t userSize = 0;  // Size of user input

    printf("Enter your username:\n");
    getline(&user, &userSize, stdin);  // Get username from user input
    user[strcspn(user, "\n")] = 0;  // Remove newline character from username
    
    char* line = NULL;  // User input for message
    size_t lineSize = 0;  // Size of user input
    printf("Type your message below, hit Enter to send (type 'exit' to close the application):\n");
    
    startIncomingThread(socketFD);  // Start thread for receiving messages
    
    char message[1024];  // Buffer for outgoing message
    
    while(true) {
        printf("%s: ", user);
        getline(&line, &lineSize, stdin);  // Get message from user input
        line[strcspn(line, "\n")] = 0;  // Remove newline character from message
        sprintf(message, "%s: %s\n", user, line);  // Format message with username
        
        if(strcmp(line, "exit") == 0){  // Check if exit command entered
            break;  // Break the loop if exit command entered
        } 
        // Requirement 3 - Transmit messages to the server's assigned session task
        ssize_t amountSent = send(socketFD, message, strlen(message),  0);  // Send message to server
        
        if(amountSent == -1) {
            printf("Message failed to send.\n");  // Print message failure if send fails
        }
    }
    
    close(socketFD);  // Close the socket
    return 0;  // Return success
}


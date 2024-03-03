#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <unistd.h>

struct sockaddr_in* CreateIPv4Address(char *ip, int port);

int CreateTCPIPv4Socket();

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
    return address;
}

//Creates a TCP/IPv4 socket for use
int CreateTCPIPv4Socket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

int main() {

//create socket (protocol(ip4), type(tcp), protocol layer beneath transport)
    int socketFD = CreateTCPIPv4Socket();
//create sockaddr_in struct to keep relevant info
    struct sockaddr_in *address = CreateIPv4Address(ADDRESS, 443);

    //connect to a server: (socket to use, pointer to address struct, size of address struct)
    int result = connect(socketFD, address, sizeof(*address));

    //debug lets you know if it worked
    if (result == 0){
        printf("connection was successful!");
    }


// set up message to send
    char* line = NULL;
    size_t lineSize = 0;
    printf("Type your message below, hit Enter to send (type 'exit' to close the application):\n");

    while(true) {
    // fills line with the info from stdin, set to linesize size, also counts the characters
        ssize_t charCount = getline(&line, &lineSize, stdin);

        if (charCount > 0){

        // checks if exit command entered
            if(strcmp(line, "exit")){
                break;
            } 

        // Sends the message. If -1, it failed to sent.
        // (socket to send through, message to send, length, extra options)
            ssize_t amountSent = send(socketFD, line, charCount,  0);
        }
    }



// send a hardcoded message to the server, for testing
   /* char* outgoing[1024];
    outgoing = "Test message";

    send(socketFD, outgoing, strlen(buffer), 0); 

    char* incoming[1024];
    recv(socketFD, incoming, 1024, 0);

    printf("response was: %s\n", incoming);*/

    close(socketFD);


    return 0;
}
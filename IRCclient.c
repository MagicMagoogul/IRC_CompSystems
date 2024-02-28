#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1024

/* -IRC PROJECT: Client Side-
 *
 * 1. Create a socket for the client
 * 2. Find the server address
 * 3. Connect to the server 
 * 4. Create a loop to send and recieve messages from the server for as long as the user is connected.
 * 5. Close loop when client wishes to disconnect
 *
 * -ADDITIONAL CONNSIDERATIONS-
 * 
 *
 *
 *
 * */

int main(int argc, char *argv[]) {

    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 1. Create socket
    // This socket allows for 
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Initialize server address structure 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) 
    {
        perror("Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }

    // 3. Connect to server
    // Uses the key word connect to link the client socket that we create in step 1 to the server address.
    // On fail prints connection failed. 
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)  // if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) { break; }
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");//Message confirms to the user that the connection is successful.

    
    
    // 4. Main loop to send and receive messages
    // This is the main function of the server to actually be able to communicate.
    // This the chat box in discord.
    while (1) 
    {

        // Clear buffer before reading input
        memset(buffer, 0, BUFFER_SIZE);

        printf("Enter message: ");

        // Read input from user
        // This is the main input from the error 
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) 
	{
            perror("fgets error");
            break;
        }

        // Remove newline character from input
        buffer[strcspn(buffer, "\n")] = '\0';
	
	    // Debugging: Print the contents of the buffer
        printf("DEBUG: Input buffer contains: %s\n", buffer);

        // Check for quit command
        if (strcmp(buffer, "quit\n") == 0) 
	{
            printf("Quitting...\n");
            break;
        }

        // Send message to server
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive and display message from server
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) 
	{
            buffer[bytes_received] = '\0';
            printf("Server says: %s", buffer);
        } 
	else 
	{
            printf("Server disconnected.\n");
            break;
        }

    }// End of while loop.

    // 5. Close client socket
    close(client_socket);

    return 0;
}

/* -IRC PROJECT: Server Side-
 *
 * 1. Create a struct to hold all of a clients info. 
 *
 * 2. Create a singular socket for 
 * the server to use for the multiple client sockets 
 *
 * 3. 
 *
 * 4.
 *
 * 5. 
 *
 * */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Structure to hold client information
typedef struct {
	int socket;
	struct sockaddr_in address;
	pthread_t thread_id;
} client_t;

// Function to handle client session
void *handle_client(void *arg) {
	client_t *client = (client_t *)arg;
	char buffer[BUFFER_SIZE];
	int bytes_received;

	while ((bytes_received = recv(client->socket, buffer, BUFFER_SIZE, 0)) > 0) {
		// Broadcast received message to other clients
		// (Implementation omitted for brevity)
	}

	// Handle client disconnection
	close(client->socket);
	printf("Client disconnected.\n");

	return NULL;
}

int main(int argc, char *argv[]) {
	int server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	pthread_t thread_id;
	client_t clients[MAX_CLIENTS];

	// Create socket
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Initialize server address structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	// Bind socket to address and port
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) 
	{
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(server_socket, MAX_CLIENTS) == -1) 
	{
		perror("Listen failed");
		exit(EXIT_FAILURE);
	}

	printf("Server started. Listening on port %d...\n", PORT);

	//Prints out read input to the server back to the user.  

	// Accept incoming connections and handle each in a new thread
	while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len))) 
	{
		printf("New connection accepted.\n");

		// Find a free slot for the client
		int i;
		for (i = 0; i < MAX_CLIENTS; i++) 
		{
			if (clients[i].socket == 0) 
			{
				clients[i].socket = client_socket;
				clients[i].address = client_addr;
				pthread_create(&clients[i].thread_id, NULL, handle_client, (void *)&clients[i]);
				break;
			}
		}

		// If too many clients, reject the connection
		if (i == MAX_CLIENTS) 
		{
			printf("Too many clients. Connection rejected.\n");
			close(client_socket);
		}
	}

	// Close server socket
	close(server_socket);

	return 0;
}

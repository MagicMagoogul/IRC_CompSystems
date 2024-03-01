/* -IRC PROJECT: Server Side-
 *
 * 1. Create a struct to hold all of a clients info. 
 *
 * 2. Create a singular socket for the server to use to recieve from and send to clients
 *
 * 3. Detect when a client connects to the server
 * 
 * 4. Spin up a thread to listen to ALL client threads, detecting when they send a message, and signaling the server to
 * 	  read from the client's buffer
 *
 * 5. Spin up two (2) threads for each client: one to recieve messages from the client, one to send messages to the client
 *
 * 6. Recieve incoming messages from a client, send them to all other clients
 * 
 * 7. Shut down 
 *
 * (josh note- okay where the *fuck* is the documentation on this shit I have no clue what the 
 *  imported methods do (not local methods those make enough sense))
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
struct client_info {
	int socket;
	// may not work as is, but the thread ids need to be stored alongside the client's address so the server and listener thread
	// can know which threads belong to who.
	pthread_t[] thread_ids = *malloc(sizeof(pthread_t)*2);
	struct sockaddr_in address;
	pthread_t thread_id;
	char *username[] = (char *)malloc(32);
};


// Function to handle client session
void handle_client(void *arg) {
	struct client_info *client = (client_t *)arg;
	char buffer[BUFFER_SIZE];
	int bytes_received;

	while ((bytes_received = recv(client->socket, buffer, BUFFER_SIZE, 0)) > 0) {
		*buffer[0] = bytes_received; 
	}

	// Handle client disconnection
	close(client->socket);
	printf("Client disconnected.\n");

	return NULL;
}

/* Function to listen to other client handling threads- needs to know what threads belong to which clients,
*  needs to be able to loop over and over checking everybody's in/out buffers to see when they've been updated- maybe have each thread flag it down?
*  needs to notify the server who exactly needs to have their out buffers read and sent to all others
*/
void listen_to_client(pthread_t[] threadList, )

int main(int argc, char *argv[]) {
	int server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	pthread_t thread_id;
	client_t clients[MAX_CLIENTS];
	int first_connect_flag = 0;

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
		if (first_connect_flag = 0){
			first_connect_flag = 1;

		}
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
	free()


	return 0;
}

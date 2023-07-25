#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <process.h>
#include <string.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

typedef struct
{
    SOCKET socket;
    char email[50];
} ClientInfo;

// Function to handle each client's communication in a separate thread
void handleClient(void *client_info_ptr)
{
    ClientInfo *client_info = (ClientInfo *)client_info_ptr;
    SOCKET client_socket = client_info->socket;
    char buffer[MAX_BUFFER_SIZE] = {0};
    int score;
    int bytes_received;

    printf("Connection established with %s\n", client_info->email);
/*
    // Receive data from client
    bytes_received = recv(client_socket, score, sizeof(score), 0);
    if (bytes_received == SOCKET_ERROR || bytes_received == 0)
    {
        printf("%s disconnected\n", client_info->email);
        return 1;
    }

    // Print received message from client
    printf("Total Score of %s is %s\n", client_info->email, score);

    // Send data back to client
    // send(client_socket, buffer, bytes_received, 0);

    // Clear the buffer
    // memset(buffer, 0, sizeof(buffer));
 */
    while (1)
    {
        // Receive data from client
        bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_received == SOCKET_ERROR || bytes_received == 0)
        {
            printf("%s disconnected\n", client_info->email);
            break;
        }

        // Print received message from client
        printf("%s: %s\n", client_info->email, buffer);

        // Send data back to client
        send(client_socket, buffer, bytes_received, 0);

        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));
    }

    // Close the client socket and cleanup
    closesocket(client_socket);
    free(client_info);
    _endthread();
}

int main()
{
    WSADATA wsa;
    SOCKET server_socket, new_socket;
    struct sockaddr_in server, client;
    int client_size;
    HANDLE thread;
    unsigned threadID;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed to initialize Winsock");
        return 1;
    }

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Failed to create socket");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    // server.sin_addr.s_addr = inet_addr("192.168.1.7");
    server.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed");
        return 1;
    }

    // Listen for incoming connections
    listen(server_socket, 3);

    printf("Waiting for incoming connections...\n");

    while (1)
    {
        // Accept an incoming connection
        client_size = sizeof(client);
        new_socket = accept(server_socket, (struct sockaddr *)&client, &client_size);
        if (new_socket == INVALID_SOCKET)
        {
            printf("Accept failed");
            return 1;
        }

        // Get the client's email
        char client_email[50] = {'\0'};
        int bytes_received = recv(new_socket, client_email, sizeof(client_email), 0);
        if (bytes_received == SOCKET_ERROR || bytes_received == 0)
        {
            printf("Client email not received\n");
            closesocket(new_socket);
            continue;
        }
        client_email[strcspn(client_email, "\n")] = '\0'; // Remove the newline character from the email

        // Prompt the server user to enter the string to send to the client
        char server_message[MAX_BUFFER_SIZE];
        printf("Enter the string to send to the client: ");
        fflush(stdin);
        fgets(server_message, MAX_BUFFER_SIZE, stdin);
        server_message[strcspn(server_message, "\n")] = '\0'; // Remove the newline character

        // Send the message to the client before creating the communication thread
        send(new_socket, server_message, strlen(server_message), 0);

        // Create a new thread to handle the client's communication
        ClientInfo *client_info = (ClientInfo *)malloc(sizeof(ClientInfo));
        client_info->socket = new_socket;
        strcpy(client_info->email, client_email);

        memset(client_email, 0, sizeof(client_email));
        thread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)handleClient, (void *)client_info, 0, &threadID);

        // Check if thread creation failed
        if (thread == 0)
        {
            printf("Thread creation failed");
            closesocket(new_socket);
        }
    }

    // Close the server socket and cleanup
    closesocket(server_socket);
    WSACleanup();
    return 0;
}

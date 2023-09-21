// build command: gcc -o server.exe main.c quiz_select.c print_center.c -I include/ -lws2_32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <pthread.h>
#include <quiz_select.h>
#include <print_center.h>

#define PORT 12345
#define MAX_CLIENTS 5
#define MAX_BUFFER_SIZE 1024

extern yCoord;
int mIndx = 1;
int tIndx = 0;
char *file_name = NULL;

// Function to get the current time in seconds
double get_current_time()
{
    return (double)clock() / CLOCKS_PER_SEC;
}

double start_time;

void *handle_client(void *client_socket_void);

int main()
{
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    pthread_t threads[MAX_CLIENTS];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        perror("WSAStartup failed");
        return 1;
    }

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        perror("Bind failed");
        return 1;
    }

    // Listen
    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR)
    {
        perror("Listen failed");
        return 1;
    }

    printCenter("+==================================+", &yCoord);
    printCenter("|                                  |", &yCoord);
    printCenter("|                                  |", &yCoord);
    gotoxy(20 + 1, yCoord - 1);
    printf("Server is listening on port %d\n", PORT);
    printCenter("|                                  |", &yCoord);
    printCenter("+==================================+", &yCoord);

    Sleep(3000);
    system("cls");
    yCoord = 3;

    file_name = quiz_select();
    system("cls");
    yCoord = 3;

    // Set current time as start time
    start_time = get_current_time();

    while (mIndx)
    {
        mIndx += 2;
        // Accept connection from an incoming client
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) == INVALID_SOCKET)
        {
            perror("Accept failed");
            return 1;
        }

        gotoxy(20, mIndx+2);
        printf("+==========================================+");
        gotoxy(20, mIndx+3);
        printf("| connected with                           |");
        gotoxy(20, mIndx+4);
        printf("+==========================================+");

        // Create a new thread to handle the client
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, (void *)&client_socket) != 0)
        {
            perror("Thread creation failed");
            return 1;
        }

        // Detach the thread so that it can clean up its resources on exit
        pthread_detach(thread);
    }

    // Close the socket and cleanup
    closesocket(server_socket);
    WSACleanup();

    return 0;
}

void *handle_client(void *client_socket_void)
{
    SOCKET client_socket = *((SOCKET *)client_socket_void);
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;

    int tCoord = tIndx;
    tIndx += 2;

    // Receive email from client
    char email[50];
    bytes_received = recv(client_socket, email, sizeof(email), 0);
    if (bytes_received == SOCKET_ERROR || bytes_received == 0)
    {
        perror("Email receiving failed");
        closesocket(client_socket);
        return NULL;
    }

    // Print the received email
    gotoxy(20+17, mIndx + 1);
    printf("%s\n", email);

    // Send a file destination to the client
    // char filename[MAX_BUFFER_SIZE] = "database\\question_bank\\primary\\primary_level_1.txt";
    // printf("Enter the file destination: ");
    // fgets(filename, sizeof(filename), stdin);
    // filename[strcspn(filename, "\n")] = '\0'; // Remove the newline character

    if (send(client_socket, file_name, strlen(file_name), 0) == SOCKET_ERROR)
    {
        perror("File destination sending failed");
        closesocket(client_socket);
        return NULL;
    }

    // Send time limit to the client
    int time_limit = 90;
    // int remaining_time = time_limit - (get_current_time() - start_time);

    sprintf(buffer, "%d", /* remaining_time */ time_limit);
    if (send(client_socket, buffer, strlen(buffer), 0) == SOCKET_ERROR)
    {
        perror("Time limit sending failed");
        closesocket(client_socket);
        return NULL;
    }

    // continuously receive updated score from client
    int no_mcq = 10;
    while (no_mcq--)
    {
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == SOCKET_ERROR || bytes_received == 0)
        {
            perror("uScore receiving failed");
            closesocket(client_socket);
            return NULL;
        }
        buffer[bytes_received] = '\0';
        int uScore = atoi(buffer);
        if (uScore == -1)
        {
            break;
        }
        gotoxy(20, tCoord + 17);
        printf("+----------------------------------+");
        gotoxy(20, tCoord + 18);
        printf("|%-18s: scored %-7d|", email, uScore);
        gotoxy(20, tCoord + 19);
        printf("+----------------------------------+");
    }

    // Receive the total score from the client
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == SOCKET_ERROR || bytes_received == 0)
    {
        perror("Score receiving failed");
        closesocket(client_socket);
        return NULL;
    }
    buffer[bytes_received] = '\0';
    int received_score = atoi(buffer);

    // Process the received number (you can implement your logic here)
    gotoxy(20, tCoord + 17);
    printf("+----------------------------------+");
    gotoxy(20, tCoord + 18);
    printf("|%-18s: total score %-2d|", email, received_score);
    gotoxy(20, tCoord + 19);
    printf("+----------------------------------+");
    // gotoxy(20, tCoord + 8);
    /*
        // Send postion to the client
        int position = -1;
        sprintf(buffer, "%d", position);
        if (send(client_socket, buffer, strlen(buffer), 0) == SOCKET_ERROR)
        {
            perror("Position sending failed");
            closesocket(client_socket);
            return NULL;
        }
     */
    // Close the client socket and return
    closesocket(client_socket);
    Sleep(2000);
    gotoxy(20, tCoord + 17);
    printf("+----------------------------------+");
    gotoxy(20, tCoord + 18);
    printf("|%-18s: disconnect   ", email);
    gotoxy(20, tCoord + 19);
    printf("+----------------------------------+");
    return NULL;
}
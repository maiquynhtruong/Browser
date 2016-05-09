#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

static const int MAXPENDING = 5; // Maximum outstanding connection requests

void read_and_send(int socket) {
    ///reading input from stdin
        char buffSend[BUFSIZE];
        if (fgets(buffSend, BUFSIZE, stdin) != NULL) {
            ///send messages to peer
            send(socket, buffSend, BUFSIZE, 0);
        } else {
            close(socket);
            exit(0);
        }
}

void receive_and_print(int socket) {
    ///receive messages from peer
        char recvBuff[BUFSIZE];
        ssize_t bytesRecv = 0;

            bytesRecv = recv(socket, recvBuff, BUFSIZE, 0);
            if (bytesRecv == 0) {
                close(socket);
                exit(0);
            }
            ///and print the messages
            printf("%s", recvBuff);

}

void callServer(char **argv){

    in_port_t servPort = atoi(argv[1]); // First arg:  local port

    ssize_t serverNameLen = strlen(argv[2]);
    char serverName[serverNameLen + 1];
    strcpy(serverName, argv[2]);
    serverName[serverNameLen] = '\0';
    serverNameLen++;

  // Create socket for incoming connections
  int servSock; // Socket descriptor for server
  if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithSystemMessage("socket() failed");

  // Construct local address structure
  struct sockaddr_in servAddr;                  // Local address
  memset(&servAddr, 0, sizeof(servAddr));       // Zero out structure
  servAddr.sin_family = AF_INET;                // IPv4 address family
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
  servAddr.sin_port = htons(servPort);          // Local port

  // Bind to the local address
  if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
    DieWithSystemMessage("bind() failed");

  // Mark the socket so it will listen for incoming connections
  if (listen(servSock, MAXPENDING) < 0)
    DieWithSystemMessage("listen() failed");

    struct sockaddr_in clntAddr; // Client address
    // Set length of client address structure (in-out parameter)
    socklen_t clntAddrLen = sizeof(clntAddr);

    // Wait for a client to connect
    int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
    if (clntSock < 0)
      DieWithSystemMessage("accept() failed");

    // clntSock is connected to a client!

    char clntName[INET_ADDRSTRLEN]; // String to contain client address
    if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName,
        sizeof(clntName)) != NULL)
      printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));
    else
      puts("Unable to get client address");

    ///receiving client's handle
    int bytesReceived = 0;
    char clientName[BUFSIZE];
    bytesReceived = recv(clntSock, clientName, BUFSIZE, 0);
    if (bytesReceived < 0)
        DieWithSystemMessage("recv() failed");

    printf("Name of client: %s\n", clientName);


    /// sending the handle to the client

    if (send(clntSock, serverName, serverNameLen, 0) < 0){
        DieWithSystemMessage("send() failed");
    }

    while(true){
        printf("%s: ", clientName);
        receive_and_print(clntSock);
        printf("%s: ", serverName);
        read_and_send(clntSock);
    }

}



void callClient(int argc, char **argv){
    char *servIP = argv[3];     // First arg: server IP address (dotted quad)
    ssize_t clientNameLen = strlen(argv[2]);
    char clientName[clientNameLen + 1];
    strcpy(clientName, argv[2]);
    clientName[clientNameLen] = '\0';            //append the NULL terminating char
    clientNameLen++;


      // First arg (optional): server port (numeric)
      in_port_t servPort = atoi(argv[1]);

      // Create a reliable, stream socket using TCP
      int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (sock < 0)
        DieWithSystemMessage("socket() failed");
      // Construct the server address structure
      struct sockaddr_in servAddr;            // Server address
      memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
      servAddr.sin_family = AF_INET;          // IPv4 address family
      // Convert address
      int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
      if (rtnVal == 0){
        DieWithUserMessage("inet_pton() failed", "invalid address string");
      }
      else if (rtnVal < 0)
        DieWithSystemMessage("inet_pton() failed");
      servAddr.sin_port = htons(servPort);    // Server port

      // Establish the connection to the echo server

        if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithSystemMessage("connect() failed");

    ///sending the handle to the server
    int byteSent = send(sock, clientName, clientNameLen, 0);
    if (byteSent < 0) {
        DieWithSystemMessage("send() failed");
    }
    ///receiving the handle from the server

    char serverName[BUFSIZE];
    int bytesReceived = 0;
    bytesReceived = recv(sock, serverName, BUFSIZE, 0);
    if (bytesReceived < 0)
        DieWithSystemMessage("recv() failed");

    printf("Name of server: %s\n", serverName);

    while(true){

        printf("%s: ", clientName);
        read_and_send(sock);
        printf("%s: ", serverName);
        receive_and_print(sock);
    }
    exit(0);
}
int main(int argc, char **argv){
	if (argc == 3) {
        callServer(argv);
	} else if (argc == 4) {
        callClient(argc, argv);
	} else {
        DieWithUserMessage("Command line arguments", "Wrong number of arguments");
	}

}


/* 
TCP server
*/

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
	//char buffer[256];
	WSADATA wsa; // Свойства WinSock (результат функции WSAStartup)
	SOCKET serverSocket, clientSock; // Серверный и клиентский сокеты
	struct sockaddr_in server, client;
	char *answer = "I've got your message!";
	if (WSAStartup(0x0202, &wsa) != 0)
	{
		printf("failed with error: : %d", WSAGetLastError());
		return 1;
	}

	// Create a SOCKET for connecting to server

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("socket failed with error: %d", WSAGetLastError());
	}

	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons((UINT16)atoi(argv[1]));

	// Setup the TCP listening socket
	if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("bind failed with error code : %d", WSAGetLastError());
	}
	//Listen to incoming connections
	listen(serverSocket, 5);
	printf("Waiting for incoming connections...");
	int c = sizeof(struct sockaddr_in);
	clientSock = accept(serverSocket, (struct sockaddr *)&client, &c);
	if (clientSock == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
	}

	/* Read msg */
	int l = readn(clientSock, buffer);
	printf("Message length:%d\n", l);

	/* Write a response to the client */
	int n = send(clientSock, answer, strlen(answer), 0); // send on Windows
	if (n < 0) {
		printf("ERROR writing to socket");
		exit(1);
	}
	closesocket(serverSocket);
	WSACleanup();
}
int readn(SOCKET socket, char *buffer){
//	char buffer[256];
	memset(buffer, 0, strlen(buffer));
	int n;
	int length = 0;
	int first_msg = 1;
	while (1){
		n = recv(socket, buffer, 255, 0);
		if (n < 0) return -1;
		else
		{
			if (first_msg) { 
				printf("\nReceived message:"); 
				first_msg = 0; 
			}
			if (n < 255){ 
				length += n; 
				printf("%s\n", buffer);
				return length; 
						}
			else { 
				length += n; 
				printf("%s", buffer);
				}
		}
		memset(buffer, 0, strlen(buffer));
	}
	return -1;
}
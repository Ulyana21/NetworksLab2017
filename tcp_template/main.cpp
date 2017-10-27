/*
TCP client
*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _GNU_SOURCE
#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[])
{
	WSADATA wsa;
	SOCKET client;
	struct sockaddr_in server;

	if (argc < 3) {
		printf("Params ERROR.\nExample:client-win ""server_ip"" ""server_port""\n");
		exit(0);
	}

	if (WSAStartup(0x0202, &wsa) != 0)
	{
		printf("failed with error: %d", WSAGetLastError());
		return 1;
	}
	//Create a socket
	if ((client = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons((UINT16)atoi(argv[2]));

	//Connect to remote server
	if (connect(client, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("connect error");
		return 1;
	}
	printf("Please enter the message: ");
	char buf[2048];
	scanf("%[^\n]", buf);
	printf("Message length:%d\n", strlen(buf));

	/* Send message to the server */
	int n = send(client, buf, strlen(buf), 0);
	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
	/* Now read server response */
	int l = readn(client,buf);
	printf("Message length:%d\n", l);
	closesocket(client);
	WSACleanup();
}

int readn(SOCKET socket, char* buf){
	char buffer[256];
	memset(buffer, 0, strlen(buffer));
	int n;
	int length = 0;
	int first_msg = 1;
	while (1){
		n = recv(socket, buffer, 255, 0);
		if (n < 0) return -1;
		else
		{
			if (first_msg) { printf("\nReceived message:"); first_msg = 0; }
			if (n < 255){ length += n; printf("%s\n", buffer); return length; }
			else{ length += n; printf("%s", buffer); }
		}
		memset(buffer, 0, strlen(buffer));
	}
	return -1;
}
#define _CRT_SECURE_NO_WARNING
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#pragma warning(disable : 4996)
#pragma comment (lib, "Ws2_32.lib")

#define PORT 666
#define SERVERADDR "127.0.0.1"

int main(int argc, char* argv[])
{
	//setlocale(LC_ALL, "Russian");

	char buff[10 * 1014];
	printf("UDP DEMO Client\nType quit to quit\nEnter two integers with a space\n");

	// Шаг 1 - иницилизация библиотеки Winsocks
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStartup error: %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 2 - открытие сокета
	SOCKET my_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (my_sock == INVALID_SOCKET)
	{
		printf("socket() error: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// Шаг 3 - обмен сообщений с сервером
	HOSTENT *hst;
	sockaddr_in dest_addr;

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);

	// определение IP-адреса узла
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
	{
		if (hst = gethostbyname(SERVERADDR))
			dest_addr.sin_addr.s_addr = ((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Unknown host: %d\n", WSAGetLastError());
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}
	}
	while (1)
	{
		printf("\nS<=C:");
		// чтение введенного сообщенич в buff
		fgets(&buff[0], sizeof(buff) - 1, stdin);
		// проверка на quit
		if (!strcmp(&buff[0], "quit\n")) break;

		// Передача датаграмы
		sendto(my_sock, &buff[0], strlen(&buff[0]), 0, \
			(sockaddr *)&dest_addr, sizeof(dest_addr));

		// структура sockaddr_in и ее размер
		sockaddr_in server_addr;
		int server_addr_size = sizeof(server_addr);

		// Прием датаграмы
		int n = recvfrom(my_sock, &buff[0], sizeof(buff) - 1, 0, \
			(sockaddr *)&server_addr, &server_addr_size);

		if (n == SOCKET_ERROR)
		{
			printf("recvfrom() error: %d\n", WSAGetLastError());
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}

		buff[n] = 0;

		// Вывод принятого с сервера сообщения на экран
		printf("\nS=>C:%s", &buff[0]);
	} // end while

	// Шаг последний - выход
	closesocket(my_sock);
	WSACleanup();
	return 0;
}

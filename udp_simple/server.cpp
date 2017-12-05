#define _CRT_SECURE_NO_WARNING
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <locale.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include "windows.h"
//#include <string.h>
#include <stdio.h>
#include <math.h>
//#include <iostream>
//#include <tchar.h> 
//#include <strsafe.h>
#include <conio.h>
#include <direct.h>

#pragma warning(disable : 4996)
#pragma comment (lib, "Ws2_32.lib")

#define PORT 666

// функция по варианту
int myfunc(int a, int b)
{
	return a + b;
}

int main(int argc, char* argv[])
{
	//setlocale(LC_ALL, "Russian");

	char buff[1024];
	printf("UDP DEMO Server\n");
	// Шаг 1 - подключение библиотеки
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("Error WSAStartup : %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 2 - создание сокета
	SOCKET mysocket;
	if ((mysocket = socket(AF_INET, SOCK_DGRAM, 0))<0)
	{
		printf("Socket() error: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// Шаг 3 - связывание сокета с локальным адресом
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(PORT);
	local_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr)))
	{
		printf("bind error: %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	// Шаг 4 обработка пакетов, присланных клиентами
	// переменные для функции по варианту (-1 - значение не установлено)
	int a = -1;
	int b = -1;
	while (1)
	{
		sockaddr_in client_addr;
		int client_addr_size = sizeof(client_addr);
		int bytes_recv = recvfrom(mysocket, &buff[0], sizeof(buff) - 1, 0,
			(sockaddr *)&client_addr, &client_addr_size);
		if (bytes_recv == SOCKET_ERROR)
			printf("recvfrom() error: %d\n", WSAGetLastError());

		// Определяем IP-адрес клиента и прочие атрибуты
		HOSTENT *hst;
		hst = gethostbyaddr((char *)&client_addr.sin_addr, 4, AF_INET);
		printf("+%s [%s:%d] new DATAGRAM!\n",
			(hst) ? hst->h_name : "Unknown host",
			inet_ntoa(client_addr.sin_addr),
			ntohs(client_addr.sin_port));

		// добавление завершающего нуля
		buff[bytes_recv] = 0;

		// Вывод на экран
		printf("C=>S:%s\n", &buff[0]);

		// установка значений параметров
		if (a == -1) // установка a
			a =atoi(buff);
		else if (b == -1) // установка b
			b =atoi(buff);

		if (a != -1 && b != -1)
		{
			// вызов функции(если a и b != -1 )
			a = myfunc(a, b);
			_itoa_s(a, buff, 10);
			buff[sizeof(buff)] = '\n';
			a = -1;
			b = -1;
		}

		// посылка датаграммы клиенту
		sendto(mysocket, &buff[0], sizeof(buff), 0,
			(sockaddr *)&client_addr, sizeof(client_addr));

	}//end while

}

#define _CRT_SECURE_NO_WARNING
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 2048
#define DEFAULT_PORT 27015
#define SERVERADDR "127.0.0.1"
#define MSG_SIZE 1500
#define FROM_SIZE 127
#define TO_SIZE 127
#define SUBJ_SIZE 127
#define LOGIN_SIZE 30
#define FNAME_SIZE 30
#define SNAME_SIZE 3
#define MAX_FID 30

#pragma warning(disable : 4996)

//char ID[4];

bool ROK(int my_sock) //для записи строки о финансах
{
	char recvbuf[2];
	int iResult;
	iResult = recv(my_sock, recvbuf, sizeof(recvbuf), 0);
	if (iResult == SOCKET_ERROR)
	{
		return false;
	}
	else
	{
		switch (recvbuf[0])
		{
		case 'Y':
		{
			return true;
			break;
		}
		default:
		{
			return false;
			break;
		}
		}
	}
}

bool SOK(int my_sock, bool OK)
{
	char sendbuf[2];
	int iResult;
	if (OK == true)
	{
		strcpy_s(sendbuf, "Y");
		iResult = send(my_sock, sendbuf, sizeof(sendbuf), 0);
		if (iResult == SOCKET_ERROR)
		{
			return false;
		}
		else
		{
			return true;
		}

	}
	else
	{
		strcpy_s(sendbuf, "N");
		iResult = send(my_sock, sendbuf, sizeof(sendbuf), 0);
		if (iResult == SOCKET_ERROR)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

int __cdecl main(int argc, char **argv)
{
	WSADATA wsaData;

	char sendbuf[DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN];
	//	char commandbuf[DEFAULT_BUFLEN];
	memset(recvbuf, 0, DEFAULT_BUFLEN);
	memset(sendbuf, 0, DEFAULT_BUFLEN);
	int inputsize = 0;
	//	char tmpbuf[1];
	int iResult;


	sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = htons(DEFAULT_PORT);
	HOSTENT *hst;




	SecureZeroMemory(&recvbuf, sizeof(recvbuf));

	//================Connection Manager=========================//
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	printf("WSAS starting... \n");
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	SOCKET my_sock;
	my_sock = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
	printf("Opening socket... \n");
	if (my_sock == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	printf("Success! \n");

	//Preobrazovanie IP Adresa iz simvolnogo v setevoi format
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		client.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		// Poluchenie Adresa hosta po DNS
		if (hst = gethostbyname(SERVERADDR))
		{
			// hst->h_addr_list soderjit massiv adresov i ukazatelei na adresa
			((unsigned long *)&client.sin_addr)[0] = ((unsigned long **)hst->h_addr_list)[0][0];
		}
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}


	iResult = connect(my_sock, (SOCKADDR *)& client, sizeof(client));
	printf("Connecting... \n");
	if (iResult == SOCKET_ERROR)
	{
		closesocket(my_sock);
		my_sock = INVALID_SOCKET;
	}
	if (my_sock == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	printf("Connected to %s \n", SERVERADDR);

	//=====================End Connection Manafer===============//


	//=====================Client Menu===========================//
	int end = 0;
	//=====================Login Section=========================//

	bool r;
	r = false;

	//=================End of login section=====================//


	//=================Main Menu===============================//
	printf("Welcome to the Finance Cource programm client! \n \n");
	printf("Main menu: \n \n");
	printf("|=====Commands:==============|=====Semantic:===========================|\n");
	printf("| Add New Finance Position:  | A <ID> <NAME> <SHORT NAME> <COURCE>     |\n");
	printf("| Delete Finance Position:   | D <ID>                                  |\n");
	printf("| Read All Finance Courcess: | R                                       |\n");
	printf("| Add Finance Cource:        | C <ID> <COURCE>                         |\n");
	printf("| Exit Programm:             | O                                       |\n");
	printf("| Show Menu:                 | M                                       |\n");
	printf("| Show History               | H <ID>                                  |\n");
	printf("|============================|=========================================|\n");
	printf("|               Enter your command... And PRAY!....                    |\n");
	printf("|============================|=========================================|\n");
	fflush(stdout);

	int endMenu = 0;
	do
	{
		memset(sendbuf, 0, DEFAULT_BUFLEN);

		gets(sendbuf);


		//Мнемоники команд:
		//А - добавить валюту+
		//D - удалить валюту+
		//С - добавить курс валюты+
		//R - прочитать текущие котировки
		//М - вызвать меню+

		switch (sendbuf[0])
		{
		case 'A':
		case 'a':
		{
			int tempID;
			tempID = 0;
			int tempCource;
			tempCource = 0;
			char tempIDC[5];
			memset(tempIDC, 0x00, 5);
			char tempCourceC[15];
			memset(tempCourceC, 0x00, 15);
			int i = 2;
			int j = 0;
			while (sendbuf[i] != 0x20) //Начинаем проверять условия. Первым пунктом - соответствие ID
			{
				if (sendbuf[i] == 0x00)//Если ВНЕЗАПНО достигли конца строки - значит аргументов слишком мало.
				{
					printf("|================= Error! To few arguments! ===========================|\n");
					goto br1;
				}
				if (j > 5)
				{

					printf("|====================== ID is too long! ===============================|\n");//Слишком много цифр в ID.
					goto br1;
				}
				tempIDC[j] = sendbuf[i];
				i++;
				j++;
			}
			tempID = atoi(tempIDC);//Переводим строковый ID в Целый вот так.
			if (tempID<0 || tempID>MAX_FID) //Теперь проверяем, не выходит ли ID за границы нужного.
			{
				printf("|==================== ID Is Incorrect! ================================|\n");
				goto br1;
			}

			i = i + 1;
			j = 0;
			while (sendbuf[i] != 0x20) //Проверяем имя валюты
			{
				if (sendbuf[i] == 0x00)//Опять-же, если ВНЕЗАПНО достигли конца строки - значит аргументов слишком мало.
				{
					printf("|======================= Error! To few arguments! =====================|\n");//Обрабатываем
					goto br1;
				}
				if (j > FNAME_SIZE)
				{

					printf("|======================== Finance Name is too long! ===================|\n");//Слишком длинное название валюты.
					goto br1;
				}
				i++;
				j++;
			}
			i = i + 1;
			j = 0;
			while (sendbuf[i] != 0x20) //Проверяем короткое имя валюты
			{
				if (sendbuf[i] == 0x00)//Если достигли конца - неполная строчка
				{
					printf("|====================== Error! To few arguments! ======================|\n");//Обрабатываем
					goto br1;
				}
				if (j > SNAME_SIZE)
				{

					printf("|===================== Finance Short Name is too long! ================|\n");//Слишком длинное короткое название валюты. 
					goto br1;
				}
				i++;
				j++;
			}
			i = i + 1;
			j = 0;
			while (sendbuf[i] != 0x00)//Последний аргумент, поэтому его считываем уже до конца
			{
				if (j > 15)
				{
					printf("|====================== Cource is too big! ============================|\n");//Слишком большой курс
					goto br1;
				}
				tempCourceC[j] = sendbuf[i];
				i++;
				j++;
			}
			tempCource = atoi(tempCourceC);
			if (tempCource < 0)//Проверка на положительность курса. Он ведь не может быть отрицательным!
			{
				printf("|================ Finance Cource Can't Be a Negative Value! ===========|\n");
				goto br1;
			}
			//Вот теперь, если всё норм - отсылаем данные!
			iResult = send(my_sock, sendbuf, DEFAULT_BUFLEN, 0);
			if (iResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d \n", WSAGetLastError());
				goto br1;
			}
			printf("sended \n");
			if (ROK(my_sock) == true)
			{
				printf("|================ Finance Cource Successfully Added! ==================|\n");
				goto br1;
			}
			else
			{
				printf("|=========== Error! Cource With This ID Is Alredy Added! ==============|\n");
				goto br1;
			}
			goto br1;

		}
		case 'D':
		case 'd':
		{
			
		}

		case 'R':
		case 'r':
		{
			iResult = send(my_sock, sendbuf, DEFAULT_BUFLEN, 0);
			
		}
		case 'C':
		case 'c':
		{
			
					iResult = send(my_sock, sendbuf, DEFAULT_BUFLEN, 0);
			
		}
		case 'M':
		case'm':
		{
			//То есть если написана строка формата: A <ID> <NAME> <SHORT NAME> <COURCE>  
			//ТО в консоли нужно ввести: A 1 RUB RU 12.34 - это пример
			//А если: R
			//То в консоли вводишь: r или R
			//Вводить первую букву как строчную или заглавную - не имеет значения
			printf("Main menu: \n \n");
			printf("|=====Commands:==============|=====Semantic:===========================|\n");
			printf("| Add New Finance Position:  | A <ID> <NAME> <SHORT NAME> <COURCE>     |\n");
			printf("| Delete Finance Position:   | D <ID>                                  |\n");
			printf("| Read All Finance Courcess: | R                                       |\n");
			printf("| Add Finance Cource:        | C <ID> <COURCE>                         |\n");
			printf("| Exit Programm:             | O                                       |\n");
			printf("| Show Menu:                 | M                                       |\n");
			printf("| Show History               | H <ID>                                  |\n");
			printf("|============================|=========================================|\n");
			printf("|               Enter your command... And PRAY!....                    |\n");
			printf("|============================|=========================================|\n");
			fflush(stdout);
			goto br1;
		}
		case 'H':
		case 'h':
		{
			iResult = send(my_sock, sendbuf, DEFAULT_BUFLEN, 0);
			
		}
		case 'O':
		case 'o':
		{
			strcpy_s(sendbuf, "O");
			//strcat_s(sendbuf, ID);
			send(my_sock, sendbuf, DEFAULT_BUFLEN, 0);
			end = 1;
			endMenu = 1;
			break;
		}
		default:
		{
			printf("|==================== Command ERROR! Try once again! ==================|\n");
		br1:			continue;

		}
		}while (end == 0);


		iResult = recv(my_sock, recvbuf, sizeof(recvbuf), 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
		}



	} while (endMenu == 0);
	//=====================End Client Menu=============================//


	iResult = shutdown(my_sock, SD_SEND);
	printf("Shutdown... \n");
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		return 1;
	}
	printf("Success! \n");


	closesocket(my_sock);
	WSACleanup();

	printf("Complited! \n");

	return 0;
}

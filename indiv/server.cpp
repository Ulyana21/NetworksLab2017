//Based On Simple TCP Client of MSDN: http://msdn.microsoft.com/en-us/library/ms737593

#undef UNICODE
#define _CRT_SECURE_NO_WARNING
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN


#include <winsock2.h>
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


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 2048 // Размер буффера
#define DEFAULT_PORT 27015
#define FNAME_SIZE 30
#define SNAME_SIZE 3
#define MAX_FID 30

#define PRINTNUSERS 
#pragma warning(disable : 4996)

//char const_path[MAX_PATH];
char const_path[MAX_PATH] = "C:/VS2010_test/File_sys/Debug/";
int CCount = 0;
SOCKET client_socket;    // Сокет для клиента
sockaddr_in client_addr;    // Адрес клиента
int ID = 0;

// Макрос для печати подключенных пользователей
#define PRINTNUSERS if (CCount)\
printf("%d user on-line\n",CCount);\
else printf("No User on line\n");

char UserInfo[256][40];


// Прототип функции, обслуживающей пользователя
DWORD WINAPI ServToClient(LPVOID client_socket);
DWORD WINAPI Console(LPVOID IpParam);

int FDataID = 0;

struct Finance_data
{
	int FID; //ID
	char name[FNAME_SIZE];//имя валюты
	char sname[SNAME_SIZE];//сокращ имя валюты
	float Course; //курс
	float conCourse; //абслолютное значение изменения курса
	float absCourse; //значение изменения курса
	int HistoryCounter;
	char FHistory[10][21];
};



struct Finance_data FData[MAX_FID];//строчка Finance_data


bool ROK(int my_sock)
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
	switch(recvbuf[0])
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
	if(OK == true)
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

bool DirectoryExistsServer(const char *path)
{
	int Code = GetFileAttributes(path);
	if( Code == DWORD(-1) )
	{
        DWORD dLastError = GetLastError();

        if(    ERROR_SHARING_VIOLATION == dLastError 
            || ERROR_LOCK_VIOLATION == dLastError 
            || ERROR_SHARING_BUFFER_EXCEEDED == dLastError )
        {
            return true;
        }
		else
        {
            return false;
        }
	}
}



int main(int argc, char* argv[]) 
{
	DWORD thID;
	
    WSADATA wsaData;
    int iResult; 
	//Счетчик кол-ва пользователей
	int length=0;
	

    struct addrinfo *result = NULL;
    struct addrinfo hints;

	printf("Mail Server Started...\n");
    
	// Инициализация Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	printf("WSAS starting... \n");
    if (iResult != 0) 
	{
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
	
    ZeroMemory(&hints, sizeof(hints));
	
	
	sockaddr_in Server; 
	Server.sin_family = AF_INET;
	Server.sin_addr.s_addr = inet_addr("127.0.0.1"); //local_addr.sin_addr.s_addr=0; Для приема со всех адресов
	Server.sin_port = htons(DEFAULT_PORT); //Адрес порта подключения

	SOCKET MySocket; //Создание сокета для сервера

    MySocket = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
	printf("Listening starting... \n");
    if (MySocket == INVALID_SOCKET) 
	{
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
	printf("Success! \n");

    // Bind для связки 
    iResult = bind( MySocket, (SOCKADDR *) & Server, sizeof (Server));
	printf("Bind starting... \n");
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(MySocket);
        WSACleanup();
        return 1;
    }

	HANDLE Ct;
	Ct = CreateThread(NULL, 0, Console, 0, 0, &thID);
	if(Ct==NULL)
		printf("Oshibka sozdaniya potoka konsoli! \n");


    freeaddrinfo(result);

    iResult = listen(MySocket, 0x100); // 0x100 Размер очереди
	printf("Listening... \n");
    if (iResult == SOCKET_ERROR) 
	{
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(MySocket);
        WSACleanup();
        return 1;
    }
	printf("Success! \n");

	printf("Waiting for connections...\n");

	
	printf("Accepting... \n");
	int client_addr_size=sizeof(client_addr);
    while(client_socket = accept(MySocket, (sockaddr *) &client_addr, &client_addr_size))
	{
	if (client_socket == INVALID_SOCKET) 
		{
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(MySocket);
        WSACleanup();
        return 1;
		}
	CCount = CCount + 1;
	ID = ID + 1;
	if(ID > 128)
	{
		ID = 0;
	}

	//Получение сведений о клиенте
	HOSTENT *hst;
    hst=gethostbyaddr((char *) &client_addr.sin_addr.s_addr,4, AF_INET);

	// Вывод сведений о клиенте
    printf("+%s [%s] new connect!\n",(hst)?hst->h_name:"",inet_ntoa(client_addr.sin_addr));
    PRINTNUSERS //Вывод инфы
		
		//Вызов нового потока для обслуживания клиента
    CreateThread(NULL,NULL,ServToClient,&client_socket,NULL,&thID);
    
	}
	printf("Success! \n");
	return 0;
}
 
//Ф-ия обслуживания очередного подключенного клиента

 DWORD WINAPI ServToClient(LPVOID client_socket)
  {
	  SOCKET my_sock;
	  my_sock=((SOCKET *) client_socket)[0];
	  int MyID = ID;
	  char ID[8];
	  char SockBuf[8];
	  _itoa_s(my_sock, SockBuf, 10);
	  _itoa_s(MyID, ID, 10);
	  strcpy_s(UserInfo[MyID], "ID: ");
	  strcat_s(UserInfo[MyID], ID);
	  strcat_s(UserInfo[MyID], " Addr: ");
	  strcat_s(UserInfo[MyID], inet_ntoa(client_addr.sin_addr));
	  strcat_s(UserInfo[MyID], " S: ");
	  strcat_s(UserInfo[MyID], SockBuf);
	  strcat_s(UserInfo[MyID], "\n");
	  printf("%s", UserInfo[MyID]);
	  fflush(stdout);
	  //Задание длин
	  //Определение переменных
   	  char recvbuf[DEFAULT_BUFLEN]; //Входной buffe
	  int iResult;
	  char user[30];

	  //Инициализация буфферов
	  memset(recvbuf, 0, DEFAULT_BUFLEN);
	  int end = 0;
	  memset(user, 0, 30);
	
	

	  //Регулятор остановки приложения
	bool r = false;

	do
	{
br2:	printf("Waiting for commands... \n");
	fflush(stdout);//сброс буффера

	memset(recvbuf, 0, DEFAULT_BUFLEN);
	iResult = recv(my_sock, recvbuf, sizeof(recvbuf), 0);
	if (iResult == SOCKET_ERROR) 
	{
        printf("recv failed with error: %d\n", WSAGetLastError());
		goto br1;
	}


	switch(recvbuf[0])
	{
	case 'A':
	case 'a':
		{
			//Добавление финансовых данных: <ID> <Name> <Short_Name> <Course>

			FDataID++;
			int i = 2;
			int j = 0;
			float tempCourse;
			char tempAddID[5];
			memset(tempAddID, 0x00, 5);
			char tempAddname[FNAME_SIZE];
			memset(tempAddname, 0x00, FNAME_SIZE);
			char tempAddsname[SNAME_SIZE];
			memset(tempAddsname, 0x00, SNAME_SIZE);
			char tempAddCourse[21];
			memset(tempAddCourse, 0x00, 21);
			char tempCourseC[21];
			memset(tempCourseC, 0x00, 21);
			while (recvbuf[i] != 0x20) //Заполнение ID
			{
				tempAddID[j] = recvbuf[i];
				i++;
				j++;
			}
			i = i+1;
			j = 0;
			while (recvbuf[i] != 0x20) //Заполнение Name
			{
				tempAddname[j] = recvbuf[i];
				i++;
				j++;
			}
			i = i+1;
			j = 0;
			while (recvbuf[i] != 0x20)// Заполнение Short_Name
			{
				tempAddsname[j] = recvbuf[i];
				i++;
				j++;
			}
			i = i+1;
			j = 0;
			while (recvbuf[i] != 0x00) //Заполнение Course
			{
				tempAddCourse[j] = recvbuf[i];
				i++;
				j++;
			}
			int tempID;
			tempID = atoi(tempAddID);
			for(int x = 0; x<MAX_FID; x++)
			{
				if(FData[x].FID == tempID)
				{
					SOK(my_sock, false);//Ошибка. Валюты в таком ID уже присутсвует!
					goto br2;
				}
			}

			//Вывод информации в консоль
			
			FData[tempID].FID = tempID;
			strcpy_s(FData[tempID].name, tempAddname);
			strcpy_s(FData[tempID].sname, tempAddsname);
			tempCourse = atof(tempAddCourse);
			FData[tempID].Course = tempCourse;
			FData[tempID].absCourse = 0;
			FData[tempID].conCourse = 0;
			FData[tempID].HistoryCounter = 0;
			sprintf_s(tempCourseC, "%.2f", tempCourse);
			strcpy_s(FData[tempID].FHistory[FData[tempID].HistoryCounter], tempCourseC);
			printf("Added Finance Data: ID: %d, Name: %s Short name: %s, Course: %.2f, absCource: %.2f, conCource %.2f \n \n", FData[tempID].FID, FData[tempID].name, FData[tempID].sname, FData[tempID].Course, FData[tempID].absCourse, FData[tempID].conCourse);
			SOK(my_sock, true);
			break;
		}
	case 'D':
	case 'd':
		{
			int tempID;
			tempID = 0;
			char ID[5];
			memset(ID, 0, 5);
			int i = 2;
			int j = 0;
			while(recvbuf[i] != 0x00)
			{
				ID[j] = recvbuf[i];
				i++;
				j++;
			}
			tempID = atoi(ID);
			//Удаление конкретной валюты:
			for(int x = 0; x < MAX_FID; x++)
			{
				if (FData[x].FID == tempID)
				{
					FData[tempID].FID = 0x00;
					memset(FData[tempID].name, 0x00, FNAME_SIZE);
					memset(FData[tempID].sname, 0x00, SNAME_SIZE);
					FData[tempID].Course = 0x00;
					FData[tempID].absCourse = 0x00;
					FData[tempID].conCourse = 0x00;
					printf("Finance Data: ID: %d, Name: %s Short name: %s, Course: %.2f, absCource: %.2f, conCource %.2f \n \n", FData[tempID].FID, FData[tempID].name, FData[tempID].sname, FData[tempID].Course, FData[tempID].absCourse, FData[tempID].conCourse);
					SOK(my_sock, true);
					goto br2;
				}
			}
			SOK(my_sock, false);
			goto br2;

		}
	case 'C':
	case'c':

		{
			int tempID = 0;
			float tempCource = 0;
			float oldCource = 0;
			char ID[5];
			memset(ID, 0, 5);
			char tempCourceC[15];
			memset(tempCourceC, 0x00, 15);
			char tempCourceD[21];
			memset(tempCourceD, 0x00, 21);
			int i = 2;
			int j = 0;
			while(recvbuf[i] != 0x20)
			{
				ID[j] = recvbuf[i];
				i++;
				j++;
			}
			tempID = atoi(ID);
			i = i+1;
			j = 0;
			while(recvbuf[i] != 0x00)
			{
				tempCourceC[j] = recvbuf[i];
				i++;
				j++;
			}
			tempCource = atof(tempCourceC);
			//Изменени курса валют
			for(int x = 0; x < MAX_FID; x++)
			{
				if (FData[x].FID == tempID)
				{
					oldCource = FData[tempID].Course;
					FData[tempID].Course = tempCource;
					FData[tempID].conCourse = oldCource - tempCource;
					FData[tempID].absCourse = fabs(FData[tempID].conCourse);
					if(FData[tempID].HistoryCounter > 9)
					{
						FData[tempID].HistoryCounter = 0;
					}
					else
					{
						FData[tempID].HistoryCounter ++;
					}
					

					//char buf[18]; // Все равно максимальн точность на Intel платформе 17 знаков

					//sprintf_s(buf,"%17.2lf",x);

					sprintf_s(tempCourceD, "%.2f", FData[tempID].Course);
					strcpy_s(FData[tempID].FHistory[FData[tempID].HistoryCounter], tempCourceD);
					printf("Finance Data: ID: %d, Name: %s Short name: %s, Course: %.2f, absCource: %.2f, conCource %.2f \n \n", FData[tempID].FID, FData[tempID].name, FData[tempID].sname, FData[tempID].Course, FData[tempID].absCourse, FData[tempID].conCourse);
					SOK(my_sock, true);
					goto br2;
				}
			}
			SOK(my_sock, false);
			goto br2;

		}
	case 'R':
	case'r':
		{
			char sendbuf[DEFAULT_BUFLEN];
			memset(sendbuf, 0x00, DEFAULT_BUFLEN);
			char tempIDC[3];
			memset(tempIDC, 0x00, 3);
			char tempCName[FNAME_SIZE];
			memset(tempCName, 0x00, FNAME_SIZE);
			char tempCSName[SNAME_SIZE];
			memset(tempCSName, 0x00, SNAME_SIZE);
			char tempCource[15];
			memset(tempCource, 0x00, 15);
			char tempConCource[15];
			memset(tempConCource, 0x00, 15);
			char tempAbsCource[15];
			memset(tempAbsCource, 0x00, 15);
			for (int i = 0; i < MAX_FID; i++)
			{
				if (FData[i].FID == 0x00 || FData[i].FID == 0)
				{
					continue;
				}
				else
				{
					//Подготовка строки для отправки клиенту
					//sprintf_s(buf,"%17.2lf",x);
					//Перевод всех значений в строковые с определенным форматом
					sprintf_s(tempIDC, "%d", FData[i].FID);
					sprintf_s(tempCName, "%s", FData[i].name);
					sprintf_s(tempCSName, "%s", FData[i].sname);
					sprintf_s(tempCource, "%.2f", FData[i].Course);
					sprintf_s(tempConCource, "%.2f", FData[i].conCourse);
					sprintf_s(tempAbsCource, "%.2f", FData[i].absCourse);
					//Заполнение массива для отправки
					strcpy_s(sendbuf, "Cource ID: ");
					strcat_s(sendbuf, tempIDC);
					strcat_s(sendbuf, " Cource Name: ");
					strcat_s(sendbuf, tempCName);
					strcat_s(sendbuf, " Cource: ");
					strcat_s(sendbuf, tempCource);
					strcat_s(sendbuf, " Concerning Cource: ");
					strcat_s(sendbuf, tempConCource);
					strcat_s(sendbuf, " Absolute Cource: ");
					strcat_s(sendbuf, tempAbsCource);
					strcat_s(sendbuf, "\n");
					//Отправка клиенту!
					iResult = send(my_sock, sendbuf, DEFAULT_BUFLEN, 0);
					if (iResult == SOCKET_ERROR) 
						{
							printf("Send error! \n");
							goto br2;
						}
				}
			}
			memset(sendbuf, 0x00, DEFAULT_BUFLEN);
			strcpy_s(sendbuf,".");//Сигнал о том, что передача закончена
			iResult = send(my_sock, sendbuf, DEFAULT_BUFLEN, 0);
			if (iResult == SOCKET_ERROR) 
				{
					printf("Send error! \n");
					goto br2;
				}
			break;
		}
	case 'O':
	case 'o':
		{
			end = 1;
			break; 
		}
	case 'H':
	case 'h':
		{
			char HIDC[4];
			memset(HIDC, 0x00, 4);
			int HID = 0;
			int i = 2;
			int j = 0;
			while(recvbuf[i] != 0x00)
			{
				HIDC[j] = recvbuf[i];
				i++;
				j++;
			}
			HID = atoi(HIDC);
			char sendbuf[DEFAULT_BUFLEN];
			memset(sendbuf, 0x00, DEFAULT_BUFLEN);
			for (int i = 0; i<10; i++)
			{
				strcpy_s(sendbuf, FData[HID].FHistory[i]);
				iResult = send(my_sock, sendbuf, DEFAULT_BUFLEN, 0);
				if (iResult == SOCKET_ERROR) 
					{
						printf("Send error! \n");
						goto br2;
					}
			}
			break;
		}

	//}while(!(recvbuf[0] == 'O'));
	};
	}while(end == 0);

	
    
br1:
	ZeroMemory(UserInfo[MyID], 30);
	closesocket(my_sock);
   
    
	
	CCount = CCount -1; // Umenshaem schetchik klientov
    printf("-disconnect\n"); PRINTNUSERS

   // cleanup
    
   // WSACleanup();

	printf("Complited! \n");

    return 0;
}

DWORD WINAPI Console(LPVOID IpParam)
{
	int endConsole = 0;
	int endsw = 0;
	char Command[30];
	ZeroMemory(Command, sizeof(Command));
	do
	{
		printf("Server console \n C - Client List \n k <Socket number> - Kill klient \n \n");
		gets(Command);
		switch(Command[0])
		{
		case 'c':
		case 'C':
			{
				for (int i = 0; i<128; i++)
				{
					printf("%s", UserInfo[i]);
				}
				printf("Sucessfully \n");
				break;
			}
		case 'k':
		case 'K':
			{
			char num[4];
			memset(num, 0, 4);
			int i = 2;
			int j = 0;
			while(Command[i] != 0x00)
				{
					num[j] = Command[i];
					i++;
					j++;
				}
				shutdown(atoi(num), SD_BOTH); 
				closesocket(atoi(num));
				break;
			}
		default:
			{
				printf("Command error! \n");
				break;
			}
		};
	}while (endConsole == 0);
	return 0;
}

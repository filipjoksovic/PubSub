#pragma once

bool InitWinSock();
bool InitListenSock();
bool BindListenSocket();
void SetNonBlocking();
int Init();
DWORD WINAPI Listen(LPVOID);
void InvalidateSockets();
void ProcessMessages();
void ProcessMessage(Message*);
void Quit();
void NotifySub(Message*, Node*);
void SendToNewSub(SOCKET, Node*);
bool InitCritSec();


#define PORT "55000"
#define MAX_CLIENTS 10
#define TIMEVAL_SEC 0
#define TIMEVAL_USEC 0

fd_set readfds;
SOCKET listenSocket = INVALID_SOCKET;
SOCKET availableSockets[MAX_CLIENTS];
addrinfo* resultingAddress = NULL;
timeval timeVal;



CRITICAL_SECTION CSSAData;
CRITICAL_SECTION CSSVData;
CRITICAL_SECTION CSFuseData;
CRITICAL_SECTION CSBreakerData;
CRITICAL_SECTION CSSASubs;
CRITICAL_SECTION CSSVSubs;
CRITICAL_SECTION CSFuseSubs;
CRITICAL_SECTION CSBreakerSubs;

Node* secasubs = NULL;
Node* secvsubs = NULL;
Node* fusesubs = NULL;
Node* breakersubs = NULL;
Node* fusedata = NULL;
Node* breakerdata = NULL;
Node* secadata = NULL;
Node* secvdata = NULL;



HANDLE listenHandle;

#define SAFE_DELETE_HANDLE(a)  if(a){CloseHandle(a);}

int Init() {
	//proveri svaku komponentu koja moze da prouzrokuje nepokretanje servera i vrati broj zbog debugginga
	if (InitWinSock() == false)
	{
		return -1;
	}
	if (InitListenSock() == false) {
		return -2;
	}
	if (BindListenSocket() == false) {
		return -3;
	}
	if (InitCritSec() == false) {
		return -4;
	}
	//oslobodi adresu i pokreni TCP u neblokirajucem rezimu
	freeaddrinfo(resultingAddress);
	SetNonBlocking();
	//postavi sve sockete na invalid za dalju upotrebu
	InvalidateSockets();
	//ako sve prodje vrati 1 za uspeh
	return 1;
}
//inicijalizacija kriticnih oblasti kako bi se sprecilo pucanje programa
//stitimo analogne podatke i pretplatnike, kao i statusne
bool InitCritSec() {

	InitializeCriticalSection(&CSSAData);
	InitializeCriticalSection(&CSSVData);
	InitializeCriticalSection(&CSFuseData);
	InitializeCriticalSection(&CSBreakerData);
	InitializeCriticalSection(&CSSASubs);
	InitializeCriticalSection(&CSSVSubs);
	InitializeCriticalSection(&CSFuseSubs);
	InitializeCriticalSection(&CSBreakerSubs);

	//ako moze vrati true;
	return true;
}
//aktiviraj slusanje na socketu
DWORD WINAPI Listen(LPVOID param) {
	//slusaj zadati socket
	int res = listen(listenSocket, SOMAXCONN);
	//ako dodje do kreske pri slusanju, zavrsi program, zatvori socket i izbaci gresku
	if (res == SOCKET_ERROR)
	{
		cout << "> Cannot listen to the socket, error encountered: " << WSAGetLastError();
		closesocket(listenSocket);
		Quit();
		return 0;
	}
	//u suprotnom, slusaj na zadatom socketu
	cout << "> Listening initiated" << endl;

	while (1)
	{
		FD_ZERO(&readfds);
		FD_SET(listenSocket, &readfds);

		for (int i = 0; i < MAX_CLIENTS; i++) {
			if (availableSockets[i] != INVALID_SOCKET)
				FD_SET(availableSockets[i], &readfds);
		}

		int value = select(0, &readfds, NULL, NULL, &timeVal);

		if (value == SOCKET_ERROR) {
			// ukoliko dodje do greske prilikom selecta, izbaci gresku 
			cout << "> Select failed with error:" << WSAGetLastError() << endl;
			for (int i = 0; i < MAX_CLIENTS; i++) {
				//zatvori svaki socket i postavi ga kao nevalidan radi kasnije upotrebe
				if (FD_ISSET(availableSockets[i], &readfds)) {
					closesocket(availableSockets[i]);
					availableSockets[i] = INVALID_SOCKET;
				}
			}
		}
		else {
			if (FD_ISSET(listenSocket, &readfds)) {
				int  i;
				for (i = 0; i < MAX_CLIENTS; i++) {
					if (availableSockets[i] == INVALID_SOCKET) {
						availableSockets[i] = accept(listenSocket, NULL, NULL);
						if (availableSockets[i] == INVALID_SOCKET)
						{
							cout << "> Accept failed with error: " << WSAGetLastError() << endl;
							closesocket(availableSockets[i]);
							availableSockets[i] = INVALID_SOCKET;
							return 0;
						}
						break;
					}
				}
				if (i == MAX_CLIENTS) {
					//ukoliko nema praznih mesta, izbaci obavestenje
					cout << "> Max users reached." << endl;
				}
			}
			else {
				//ako nema greske, obradi poruke
				ProcessMessages();
			}
		}
	}
}

//Postavi socket da koristi non-blocking TCP rezim
void SetNonBlocking() {
	unsigned long mode = 1;
	int res = ioctlsocket(listenSocket, FIONBIO, &mode);
	FD_ZERO(&readfds);
	FD_SET(listenSocket, &readfds);
	timeVal.tv_sec = TIMEVAL_SEC;
	timeVal.tv_usec = TIMEVAL_USEC;
}
//Inicijalizacija WinSock-a
bool InitWinSock()
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		//Ako dodje do greske prilikom pokretanja servisa, izbaci gresku i vrati false
		cout << "> WSAStartup failed with error: " << WSAGetLastError() << endl;
		return false;
	}
	//u suprotnom vrati true
	return true;
}

//Inicijalizacija listening socketa
bool InitListenSock() {

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	//osiguraj koriscenje tcp protokola
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	int res = getaddrinfo(NULL, PORT, &hints, &resultingAddress);
	//ako se adresa ne moze naci, izbaci gresku
	if (res != 0)
	{
		cout << "> getaddrinfo failed with error: " << res << endl;
		WSACleanup();
		return false;
	}

	//u suprotnom inicijalizuj socket
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//ako je socket nevalidan izbaci gresku korisniku
	if (listenSocket == INVALID_SOCKET)
	{
		cout << "> Socket failed : " << WSAGetLastError() << endl;
		//oslobodi adresu, odradi cleanup, vrati false
		freeaddrinfo(resultingAddress);
		WSACleanup();
		return false;
	}
	return true;
}

//Binduj listening socket
bool BindListenSocket() {
	int res = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
	if (res == SOCKET_ERROR)
	{
		//ako dodje do greske, odradi ciscenje i vrati servisu false
		cout << "Binding failed: " << WSAGetLastError() << endl;
		freeaddrinfo(resultingAddress);
		closesocket(listenSocket);
		WSACleanup();
		return false;
	}
	return true;
}

//Podesi sockete na invalid vrednost radi upotrebe
void InvalidateSockets() {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		availableSockets[i] = INVALID_SOCKET;
	}
}
//Koristi TCP datoteku za preuzimanje poruka i puni liste sa podacima
void ProcessMessages() {
	char* data = (char*)malloc(sizeof(Message));
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (FD_ISSET(availableSockets[i], &readfds)) {

			bool res = Receive(availableSockets[i], data, sizeof(Message));

			if (!res) {
				//ako dodje do greske ukloni podatke koji postoje
				EnterCriticalSection(&CSSASubs);
				DeleteNode(&secasubs, &availableSockets[i], sizeof(SOCKET));
				LeaveCriticalSection(&CSSASubs);

				EnterCriticalSection(&CSSVSubs);
				DeleteNode(&secvsubs, &availableSockets[i], sizeof(SOCKET));
				LeaveCriticalSection(&CSSVSubs);

				EnterCriticalSection(&CSFuseSubs);
				DeleteNode(&fusesubs, &availableSockets[i], sizeof(SOCKET));
				LeaveCriticalSection(&CSFuseSubs);

				EnterCriticalSection(&CSBreakerSubs);
				DeleteNode(&breakersubs, &availableSockets[i], sizeof(SOCKET));
				LeaveCriticalSection(&CSBreakerSubs);

				closesocket(availableSockets[i]);
				availableSockets[i] = INVALID_SOCKET;
				continue;
			}
			SOCKET* ptr = &availableSockets[i];
			//da li je introduction ili poruka sa podacima?
			if (data[0] == 'p') {
				cout << "> Connection established : publisher" << endl;
			}
			else if (data[0] == 'd') {
				cout << "> Connection established: subscriber" << endl;
			}
			// f - status-fuse
			// b - status-breaker
			// v - analog-sec_v
			// a - analog-sec_a
			//ako su podaci u pitanju dodaj ih na pocetak liste koristeci Prepend() funkciju
			else if (data[0] == 'f') {
				EnterCriticalSection(&CSFuseData);
				Prepend(&fusesubs, ptr, sizeof(SOCKET));
				SendToNewSub(availableSockets[i], fusedata);
				LeaveCriticalSection(&CSFuseData);
			}
			else if (data[0] == 'b') {
				EnterCriticalSection(&CSBreakerData);
				Prepend(&breakersubs, ptr, sizeof(SOCKET));
				SendToNewSub(availableSockets[i], breakerdata);
				LeaveCriticalSection(&CSBreakerData);
			}
			
			else if (data[0] == 'v') {
				EnterCriticalSection(&CSSVData);
				Prepend(&secvsubs, ptr, sizeof(SOCKET));
				SendToNewSub(availableSockets[i], secvdata);
				LeaveCriticalSection(&CSSVData);
			}
			else if (data[0] == 'a') {
				EnterCriticalSection(&CSSAData);
				Prepend(&secasubs, ptr, sizeof(SOCKET));
				SendToNewSub(availableSockets[i], secadata);
				LeaveCriticalSection(&CSSAData);
			}
			//ako nije ni to onda su podaci o merenju koji se treba procesovati
			else {
				Message* newMessage = (Message*)malloc(sizeof(Message));
				memcpy(newMessage, data, sizeof(Message));
				ProcessMessage(newMessage);
				free(newMessage);
			}
		}
	}
	free(data);
}

//Procesovanje merenja
void ProcessMessage(Message* m) {
	cout << "Service received: ";
	PrintMessage(m);
	//proveri sta se salje

	if (m->signal == Analog) {
		if (m->type == sec_A) {
			EnterCriticalSection(&CSSAData);
			Prepend(&secadata, m, sizeof(Message));
			NotifySub(m, secasubs);
			LeaveCriticalSection(&CSSAData);
		}
		if (m->type == sec_V) {
			EnterCriticalSection(&CSSVData);
			Prepend(&secvdata, m, sizeof(Message));
			NotifySub(m, secvsubs);
			LeaveCriticalSection(&CSSVData);
		}
	}
	if (m->signal == Status) {
		if (m->type == fuse) {
			EnterCriticalSection(&CSFuseData);
			Prepend(&fusedata, m, sizeof(Message));
			NotifySub(m, fusesubs);
			LeaveCriticalSection(&CSFuseData);
		}
		if (m->type == breaker) {
			EnterCriticalSection(&CSBreakerData);
			Prepend(&breakerdata, m, sizeof(Message));
			NotifySub(m, breakersubs);
			LeaveCriticalSection(&CSBreakerData);
		}
	}
	else {
		//Ako nije ni status ni analog onda ne postoji
		cout << "> Topic not supported" << endl;
	}
}
//Zavrsi program i odradi cleanup
void Quit() {
	closesocket(listenSocket);
	for (int i = 0; i < MAX_CLIENTS; i++) {
		//zatvori svaki socket
		closesocket(availableSockets[i]);
	}
	WSACleanup();
	//isprazni sve liste
	Destroy(&secasubs);
	Destroy(&secvsubs);
	Destroy(&fusesubs);
	Destroy(&breakersubs);
	Destroy(&secadata);
	Destroy(&secvdata);
	Destroy(&fusedata);
	Destroy(&breakerdata);

	//bezbedno ukloni thread handler
	SAFE_DELETE_HANDLE(listenHandle);
	//ukloni kriticne oblasti
	DeleteCriticalSection(&CSSAData);
	DeleteCriticalSection(&CSSVData);
	DeleteCriticalSection(&CSFuseData);
	DeleteCriticalSection(&CSBreakerData);
	DeleteCriticalSection(&CSFuseSubs);
	DeleteCriticalSection(&CSBreakerSubs);
	DeleteCriticalSection(&CSSASubs);
	DeleteCriticalSection(&CSSVSubs);
	getchar();
}

//Posalji obavestenja pretplatnicima
void NotifySub(Message* m, Node* list) {
	Node* temp = list;
	if (temp == NULL) {
		return;
	}
	while (temp != NULL) {
		SOCKET s;
		memcpy(&s, temp->data, sizeof(SOCKET));
		Send(s, *m);
		temp = temp->next;
	}
}


//Posalji vec postojece podatke i postaraj se da mu stignu novi
void SendToNewSub(SOCKET sub, Node* dataHead) {
	Node* temp = dataHead;
	if (temp == NULL) {
		return;
	}
	Message* data = (Message*)malloc(sizeof(Message));
	while (temp != NULL) {
		memcpy(data, temp->data, sizeof(Message));
		Send(sub, *data);
		temp = temp->next;
	}
	free(data);
}

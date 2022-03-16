#pragma once

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 55000
#define STR_LEN 10


SOCKET connectSocket;
sockaddr_in serverAddress;
string ADDRESS = "127.0.0.1";

int Init();
bool InitWinSock();
bool Connect();
bool CreateSock();
bool Introduction();
void SendMessage(int, int);
Message* GenerateMessage();
Message* CreateMessage();


//inicijalizuj sve komponente aplikacije, vrati negativan broj ako nesto ne valja vrati 1 ako je sve ok
int Init() {
	if (!InitWinSock())
		return -1;
	if (!CreateSock())
		return -2;
	if (!Connect())
		return -3;
	if (!Introduction())
		return -4;
	return 1;
}
bool InitWinSock() {
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		cout << "> Socket startup failed: " << WSAGetLastError();
		return false;
	}
	return true;
}
bool Connect() {
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, ADDRESS.c_str(), &serverAddress.sin_addr);
	serverAddress.sin_port = htons(DEFAULT_PORT);
	if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		cout << "> Error while connecting to the server: " << WSAGetLastError() << endl;
		closesocket(connectSocket);
		WSACleanup();
		return false;
	}
	return true;
}
bool CreateSock() {
	connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connectSocket == INVALID_SOCKET) {
		cout << "> Socket creation failed: " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}
	return true;
}
//predstavi se servisu kao publisher
bool Introduction() {
	char greeting[2] = "p";
	return Send(connectSocket, greeting);
}
//Kreiranje poruke
void SendMessage(int pubType, int interval) {
	while (true) {
		Message* m = (Message*)malloc(sizeof(Message));
		// Osiguraj validan unos
		if (pubType == 1) {
			m = GenerateMessage();
		}
		else if (pubType == 2) {
			m = CreateMessage();
		}
		else {
			break;
		}
		//stampaj sta je pub poslao
		if (Send(connectSocket, *m)) {
			cout << "> Message sent successfully. Message content: ";
			PrintMessage(m);
		}
		free(m);
		Sleep(interval);
	}
}
//Nasumicno kreiraj poruku 
Message* GenerateMessage() {
	Message* msg = (Message*)malloc(sizeof(Message));
	int signal = (rand() % 2);
	//ako je 0 ide analog
	if (signal == 0) {
		msg->signal = Analog;
	}
	//ako je 1 ide status
	else if (signal == 1) {
		msg->signal = Status;
	}
	if (signal == 0) {
		int type = GetRandomTypeAnalog();
		if (type == 0) {
			msg->type = sec_A;
		}
		if (type == 1) {
			msg->type = sec_V;
		}
		msg->value = GetRandomAnalogValue();

	}
	if (signal == 1) {
		int type = GetRandomTypeDigital();
		if (type == 2) {
			msg->type = fuse;
		}
		if (type == 3) {
			msg->type = breaker;
		}
		msg->value = GetRandomStatusValue();
	}
	return msg;
}
//Kreiraj poruku
Message* CreateMessage() {
	Message* msg = (Message*)malloc(sizeof(Message));
	string signal;
	string type;
	int value;
	//osiguraj validan unos
	
	while (signal != "Analog" && signal != "Status") {
		cout << "> State your signal (Analog, Status): " << endl;
		cin >> signal;
		cout << signal << endl;
	}
	if (signal == "Analog") {
		while (type != "sec_A" && type != "sec_V") {
			cout << "> State the signal type(sec_A, sec_V): " << endl;
			cin >> type;
		}
	}
	if (signal == "Status") {
		while (type != "fuse" && type != "breaker") {
			cout << "> State the signal type(fuse, breaker): " << endl;
			cin >> type;
		}
	}
	cout << "> State your value" << endl;
	cin >> value;
	
	if (signal == "Analog") {
		msg->signal = Analog;
	}
	else if (signal == "Status") {
		msg->signal = Status;
	}
	if (type == "sec_A") {
		msg->type = sec_A;
	}
	else if (type == "sec_V") {
		msg->type = sec_V;
	}
	else if (type == "fuse") {
		msg->type = fuse;
	}
	else if (type == "breaker") {
		msg->type = breaker;
	}
	return msg;
}
#pragma once

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 55000
#define ADDRESS "127.0.0.1"
#define TYPE_STRING_LENGHT 10
#define DEMOTESTCOUNT 20
SOCKET connectSocket = INVALID_SOCKET;
sockaddr_in serverAddress;

bool InitWinSock();
bool CreateSock();
bool Connect();
bool Introduction();
void Subscribe();
void StartReceiveThread();
DWORD WINAPI Receive(LPVOID param);
bool Validate(Message*);

//Inicijalizacija klijenta
int Init() {

    if (InitWinSock() == false)
    {
        return 1;
    }
    if (CreateSock() == false) {
        return 2;
    }
    if (Connect() == false) {
        return 3;
    }
    if (Introduction() == false) {
        return 4;
    }
    return 0;
}
bool InitWinSock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "> WinSock initialization failed: " << WSAGetLastError();
        return false;
    }
    return true;
}
//kreiranje socketa
bool CreateSock() {
    connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET)
    {
        cout << "> Socket failed: " << WSAGetLastError();
        WSACleanup();
        return false;
    }
    return true;
}
//konektuj se na servis
bool Connect() {
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(ADDRESS);
    serverAddress.sin_port = htons(DEFAULT_PORT);

    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        cout << "> Connection failed." << endl;
        closesocket(connectSocket);
        WSACleanup();
        return false;
    }
    return true;
}
//Predstavi se servisu kao subscriber
bool Introduction() {
    char introduction[2] = "d";
    return Send(connectSocket, introduction);
}

// f - status-fuse
// b - status-breaker
// x - status-both
// v - analog-sec_v
// a - analog-sec_a
// t - analog-both

//Vrati korisniku meni za pretplatu
void Subscribe() {
    cout << "> 1)Analog \t 2)Status\t 3) Both" << endl;
    char c;
    cin >> c;
    char c1[2] = "f";
    char c2[2] = "b";
    char c3[2] = "x";
    char c4[2] = "v";
    char c5[2] = "a";
    char c6[2] = "t";
    if (c == '1') {
        cout << "1) Sec_A \t 2) Sec_V \t 3) Both" << endl;
        char option;
        cin >> option;
        if (option == '1') {
            Send(connectSocket, c5);

        }
        else if (option == '2') {
            Send(connectSocket, c4);

        }
        else if (option == '3') {
            Send(connectSocket, c4);
            Send(connectSocket, c5);
        }
        else {
            cout << "Invalid data. Try again" << endl;
            Subscribe();
        }
    }
    else if (c == '2'){
        cout << "1) Fuse\t 2) Breaker\t 3) Both" << endl;
        char option;
        cin >> option;
        if (option == '1') {
            Send(connectSocket, c1);

        }
        else if (option == '2') {
            Send(connectSocket, c2);

        }
        else if (option == '3') {
            Send(connectSocket, c1);
            Send(connectSocket, c2);
        }
        else {
            cout << "Invalid data. Try again." << endl;
            Subscribe();
        }
    }
    else if (c == '3') {
        Send(connectSocket, c1);
        Send(connectSocket, c2);
        Send(connectSocket, c4);
        Send(connectSocket, c5);
    }
    else {
        cout << "Invalid data." << endl;
        Subscribe();
    }
}
//Kreiranje thread-a za primanje paketa
void StartRecieveThread() {
    DWORD id, param = 1;
    HANDLE handle;
    handle = CreateThread(
        NULL, // default security
        0, // default velicina stacka
        Receive, // callback funkcije koja se izvrsava
        &param, // argument koji se daje funkciji
        0, // default creation flags
        &id); // povratni podatak
    int liI = getchar();
    CloseHandle(handle);
}

//primi paket i proveri da li je validan ili ne
DWORD WINAPI Receive(LPVOID param) {
    char* data = (char*)malloc(sizeof(Message));
    while (true) {
        Receive(connectSocket, data, sizeof(Message));
        Message* message = (Message*)malloc(sizeof(Message));
        memcpy(message, data, sizeof(Message));
        if (Validate(message)) {
            cout << "> Valid: ";
        }
        else {
            cout << "> Invalid: ";
        }
        PrintMessage(message);
        free(message);
        Sleep(10);
    }
}
//provera validnosti paketa
bool Validate(Message* m) {
    if (m->signal == Analog && m->value >= 0) {
        return true;
    }
    else if (m->signal == Status && m->value == 1) {
        return true;
    }
    return false;
}






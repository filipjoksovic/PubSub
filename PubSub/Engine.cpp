#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <ws2tcpip.h>
#include <time.h>
#include <iostream>

using namespace std; 

#include "../Utility/GenericListTS.h";
#include "../Utility/Message.h";
#include "../TCPHandler/TCPHandler.cpp";
#include "Engine.h";

#pragma comment(lib,"WS2_32")

int main()
{
    DWORD listenID;
    int res = Init();
    if (!res) {
        cout << "> Error encountered while starting the server, press any key to exit: ";
        getchar();
        return res;
    }
    cout << "> Server alive and listening" << endl;
    listenHandle = CreateThread(NULL, 0, &Listen, (LPVOID)0, 0, &listenID);
    if (listenHandle) {
        WaitForSingleObject(listenHandle, INFINITE);
    }
    getchar();
    Quit();
}


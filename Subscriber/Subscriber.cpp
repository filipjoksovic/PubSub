#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

using namespace std;

#include "../Utility/Message.h";
#include "../TCPHandler/TCPHandler.h";
#include "../TCPHandler/TCPHandler.cpp";
#include "Subscriber.h";
#pragma comment(lib,"WS2_32")
int main()
{
    int result = Init();
    if (result) {
        cout << "> Error while connecting to the server" << endl;
        getchar();
        return result;
    }
    cout << "> Connection established" << endl;
    Subscribe();
    cout << "> Successfully subscribed" << endl;
    StartRecieveThread();
    getchar();

}
